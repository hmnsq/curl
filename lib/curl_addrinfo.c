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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_IN6_H
#అన్తర్భావ <netinet/in6.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_UN_H
#అన్తర్భావ <sys/un.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ <stddef.h>  /* for offsetof() */

#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "fake_addrinfo.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#ఒకవేళ defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier అస్థిర
#లేకపోతే
#  define vqualifier
#అయితే_ముగింపు

శూన్య Curl_freeaddrinfo(నిర్మాణం Curl_addrinfo *cahead)
{
  నిర్మాణం Curl_addrinfo *vqualifier canext;
  నిర్మాణం Curl_addrinfo *ca;

  కొరకు(ca = cahead; ca; ca = canext) {
    canext = ca->ai_తదుపరి;
    curlx_free(ca);
  }
}

నిర్మాణం Curl_addrinfo *Curl_addrinfo_get(నిర్మాణం Curl_addrinfo *ai,
                                        పూర్ణాంకం ai_family,
                                        గుర్తులేని పూర్ణాంకం n)
{
  గుర్తులేని పూర్ణాంకం i;
  కొరకు(i = 0; ai; ai = ai->ai_తదుపరి) {
    ఒకవేళ(ai->ai_family == ai_family) {
      ఒకవేళ(i == n)
        తిరిగి ai;
      ++i;
    }
  }
  తిరిగి ఖాళీ;
}

#నిర్వచితమైతే HAVE_GETADDRINFO
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
పూర్ణాంకం Curl_getaddrinfo_ex(స్థిరరాశి అక్షరం *nodename,
                        స్థిరరాశి అక్షరం *servname,
                        స్థిరరాశి నిర్మాణం addrinfo *hints,
                        నిర్మాణం Curl_addrinfo **ఫలితం)
{
  స్థిరరాశి నిర్మాణం addrinfo *ai;
  నిర్మాణం addrinfo *aihead;
  నిర్మాణం Curl_addrinfo *cafirst = ఖాళీ;
  నిర్మాణం Curl_addrinfo *calast = ఖాళీ;
  నిర్మాణం Curl_addrinfo *ca;
  size_t ss_కొలత;
  పూర్ణాంకం error;

  *ఫలితం = ఖాళీ; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  ఒకవేళ(error)
    తిరిగి error;

  /* traverse the addrinfo list */

  కొరకు(ai = aihead; ai; ai = ai->ai_తదుపరి) {
    size_t namelen = ai->ai_canonname ? అక్షరశ్రేణి_పొడవు(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ఒకవేళ(ai->ai_family == AF_INET)
      ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in);
#నిర్వచితమైతే USE_IPV6
    లేకపోతే ఒకవేళ(ai->ai_family == AF_INET6)
      ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in6);
#అయితే_ముగింపు
    లేకపోతే
      కొనసాగు;

    /* ignore elements without required address info */
    ఒకవేళ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      కొనసాగు;

    /* ignore elements with bogus address size */
    ఒకవేళ((size_t)ai->ai_addrlen < ss_కొలత)
      కొనసాగు;

    ca = curlx_malloc(పరిమాణం(నిర్మాణం Curl_addrinfo) + ss_కొలత + namelen);
    ఒకవేళ(!ca) {
      error = EAI_MEMORY;
      ఆపు;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_కొలత;
    ca->ai_addr      = ఖాళీ;
    ca->ai_canonname = ఖాళీ;
    ca->ai_తదుపరి      = ఖాళీ;

    ca->ai_addr = (శూన్య *)((అక్షరం *)ca + పరిమాణం(నిర్మాణం Curl_addrinfo));
    జ్ఞాపకం_నకలు(ca->ai_addr, ai->ai_addr, ss_కొలత);

    ఒకవేళ(namelen) {
      ca->ai_canonname = (శూన్య *)((అక్షరం *)ca->ai_addr + ss_కొలత);
      జ్ఞాపకం_నకలు(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    ఒకవేళ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ఒకవేళ(calast)
      calast->ai_తదుపరి = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  ఒకవేళ(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  ఒకవేళ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ఖాళీ;
  }
  లేకపోతే ఒకవేళ(!cafirst) {
#నిర్వచితమైతే EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#లేకపోతే
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_WINSOCK
    సమితి_SOCKERRNO(error);
#అయితే_ముగింపు
  }

  *ఫలితం = cafirst;

  /* This is not a CURLcode */
  తిరిగి error;
}
#అయితే_ముగింపు /* HAVE_GETADDRINFO */

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
#ఒకవేళ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
నిర్మాణం Curl_addrinfo *Curl_he2ai(స్థిరరాశి నిర్మాణం hostent *he, పూర్ణాంకం పోర్టు)
{
  నిర్మాణం Curl_addrinfo *ai;
  నిర్మాణం Curl_addrinfo *prevai = ఖాళీ;
  నిర్మాణం Curl_addrinfo *firstai = ఖాళీ;
  నిర్మాణం sockaddr_in *addr;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం sockaddr_in6 *addr6;
#అయితే_ముగింపు
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం i;
  అక్షరం *curr;

  ఒకవేళ(!he)
    /* no input == no output! */
    తిరిగి ఖాళీ;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  కొరకు(i = 0; (curr = he->h_addr_list[i]) != ఖాళీ; i++) {
    size_t ss_కొలత;
    size_t namelen = అక్షరశ్రేణి_పొడవు(he->h_name) + 1; /* include null-terminator */
#నిర్వచితమైతే USE_IPV6
    ఒకవేళ(he->h_addrtype == AF_INET6)
      ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in6);
    లేకపోతే
#అయితే_ముగింపు
      ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_addrinfo) + ss_కొలత + namelen);
    ఒకవేళ(!ai) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      ఆపు;
    }
    /* put the address after the struct */
    ai->ai_addr = (శూన్య *)((అక్షరం *)ai + పరిమాణం(నిర్మాణం Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (అక్షరం *)ai->ai_addr + ss_కొలత;
    జ్ఞాపకం_నకలు(ai->ai_canonname, he->h_name, namelen);

    ఒకవేళ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ఒకవేళ(prevai)
      /* make the previous entry point to this */
      prevai->ai_తదుపరి = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_కొలత;

    /* leave the rest of the struct filled with zero */

    ఎంపిక(ai->ai_family) {
    స్థితి AF_INET:
      addr = (శూన్య *)ai->ai_addr; /* storage area for this info */

      జ్ఞాపకం_నకలు(&addr->sin_addr, curr, పరిమాణం(నిర్మాణం in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((గుర్తులేని పొట్టి)పోర్టు);
      ఆపు;

#నిర్వచితమైతే USE_IPV6
    స్థితి AF_INET6:
      addr6 = (శూన్య *)ai->ai_addr; /* storage area for this info */

      జ్ఞాపకం_నకలు(&addr6->sin6_addr, curr, పరిమాణం(నిర్మాణం in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((గుర్తులేని పొట్టి)పోర్టు);
      ఆపు;
#అయితే_ముగింపు
    }

    prevai = ai;
  }

  ఒకవేళ(ఫలితం) {
    Curl_freeaddrinfo(firstai);
    firstai = ఖాళీ;
  }

  తిరిగి firstai;
}
#అయితే_ముగింపు

/*
 * ip2addr()
 *
 * This function takes an Internet address, in binary form, as input parameter
 * along with its address family and the string version of the address, and it
 * returns a Curl_addrinfo chain filled in correctly with information for the
 * given address/host
 */
స్థిర CURLcode ip2addr(నిర్మాణం Curl_addrinfo **addrp, పూర్ణాంకం af,
                        స్థిరరాశి శూన్య *inaddr, స్థిరరాశి అక్షరం *hostname, పూర్ణాంకం పోర్టు)
{
  నిర్మాణం Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  నిర్మాణం sockaddr_in *addr;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం sockaddr_in6 *addr6;
#అయితే_ముగింపు

  DEBUGASSERT(inaddr && hostname);

  namelen = అక్షరశ్రేణి_పొడవు(hostname) + 1;
  *addrp = ఖాళీ;

  ఒకవేళ(af == AF_INET)
    addrsize = పరిమాణం(నిర్మాణం sockaddr_in);
#నిర్వచితమైతే USE_IPV6
  లేకపోతే ఒకవేళ(af == AF_INET6)
    addrsize = పరిమాణం(నిర్మాణం sockaddr_in6);
#అయితే_ముగింపు
  లేకపోతే
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_addrinfo) + addrsize + namelen);
  ఒకవేళ(!ai)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  /* put the address after the struct */
  ai->ai_addr = (శూన్య *)((అక్షరం *)ai + పరిమాణం(నిర్మాణం Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (అక్షరం *)ai->ai_addr + addrsize;
  జ్ఞాపకం_నకలు(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  ఎంపిక(af) {
  స్థితి AF_INET:
    addr = (శూన్య *)ai->ai_addr; /* storage area for this info */

    జ్ఞాపకం_నకలు(&addr->sin_addr, inaddr, పరిమాణం(నిర్మాణం in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((గుర్తులేని పొట్టి)పోర్టు);
    ఆపు;

#నిర్వచితమైతే USE_IPV6
  స్థితి AF_INET6:
    addr6 = (శూన్య *)ai->ai_addr; /* storage area for this info */

    జ్ఞాపకం_నకలు(&addr6->sin6_addr, inaddr, పరిమాణం(నిర్మాణం in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((గుర్తులేని పొట్టి)పోర్టు);
    ఆపు;
#అయితే_ముగింపు
  }
  *addrp = ai;
  తిరిగి CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(స్థిరరాశి అక్షరం *dotted, uint16_t పోర్టు,
                       నిర్మాణం Curl_addrinfo **addrp)
{
  నిర్మాణం in_addr లోపల;
  ఒకవేళ(curlx_inet_pton(AF_INET, dotted, &లోపల) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    తిరిగి ip2addr(addrp, AF_INET, &లోపల, dotted, పోర్టు);
#నిర్వచితమైతే USE_IPV6
  {
    నిర్మాణం in6_addr in6;
    ఒకవేళ(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      తిరిగి ip2addr(addrp, AF_INET6, &in6, dotted, పోర్టు);
  }
#అయితే_ముగింపు
  తిరిగి CURLE_BAD_FUNCTION_వాదం; /* bad input format */
}

bool Curl_is_ipv4addr(స్థిరరాశి అక్షరం *చిరునామా)
{
  నిర్మాణం in_addr లోపల;
  తిరిగి (curlx_inet_pton(AF_INET, చిరునామా, &లోపల) > 0);
}

bool Curl_is_ipaddr(స్థిరరాశి అక్షరం *చిరునామా)
{
  ఒకవేళ(Curl_is_ipv4addr(చిరునామా))
    తిరిగి TRUE;
#నిర్వచితమైతే USE_IPV6
  {
    నిర్మాణం in6_addr in6;
    ఒకవేళ(curlx_inet_pton(AF_INET6, చిరునామా, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      తిరిగి TRUE;
  }
#అయితే_ముగింపు
  తిరిగి FALSE;
}

bool Curl_looks_like_ipv6(స్థిరరాశి అక్షరం *s, size_t len, bool maybe_url_encoded,
                          నిర్మాణం Curl_str *host, నిర్మాణం Curl_str *zone)
{
  స్థిరరాశి అక్షరం *zonep = ఖాళీ;
  size_t i = 0, hlen = 0, zlen = 0;

  ఒకవేళ(host)
    memset(host, 0, పరిమాణం(*host));
  ఒకవేళ(zone)
    memset(zone, 0, పరిమాణం(*zone));

  కొరకు(i = 0; i < len; ++i, ++hlen) {
    ఒకవేళ(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      ఆపు;
  }

  ఒకవేళ((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    ఒకవేళ(maybe_url_encoded && !అక్షరశ్రేణి_పోలిక("25", s + i, 2))
      i += 2;
    zonep = s + i;
    కొరకు(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      ఒకవేళ(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        ఆపు;
    }
  }

  ఒకవేళ(i != len)
    తిరిగి FALSE; /* invalid chars in zone */
  ఒకవేళ(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  ఒకవేళ(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  తిరిగి TRUE;
}

#నిర్వచితమైతే USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(స్థిరరాశి అక్షరం *మార్గం, bool abstract,
                        నిర్మాణం Curl_addrinfo **paddr)
{
  నిర్మాణం Curl_addrinfo *ai;
  నిర్మాణం sockaddr_un *sa_un;
  size_t మార్గం_len;

  *paddr = ఖాళీ;

  /* sun_path must be able to store the null-terminated path */
  మార్గం_len = అక్షరశ్రేణి_పొడవు(మార్గం) + 1;
  ఒకవేళ(మార్గం_len > పరిమాణం(sa_un->sun_మార్గం))
    తిరిగి CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    పరిమాణం(నిర్మాణం Curl_addrinfo) + పరిమాణం(నిర్మాణం sockaddr_un));
  ఒకవేళ(!ai)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ai->ai_addr = (శూన్య *)((అక్షరం *)ai + పరిమాణం(నిర్మాణం Curl_addrinfo));
  sa_un = (శూన్య *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(నిర్మాణం sockaddr_un, sun_మార్గం) + మార్గం_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  ఒకవేళ(abstract)
    జ్ఞాపకం_నకలు(sa_un->sun_మార్గం + 1, మార్గం, మార్గం_len - 1);
  లేకపోతే
    జ్ఞాపకం_నకలు(sa_un->sun_మార్గం, మార్గం, మార్గం_len); /* copy NUL byte */

  *paddr = ai;
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

#ఒకవేళ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
శూన్య curl_dbg_freeaddrinfo(నిర్మాణం addrinfo *freethis,
                           పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               ఆధారం, పంక్తి, (శూన్య *)freethis);
#నిర్వచితమైతే USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#లేదావేళ defined(USE_FAKE_GETADDRINFO)
  {
    స్థిరరాశి అక్షరం *env = getenv("CURL_DNS_SERVER");
    ఒకవేళ(env)
      r_freeaddrinfo(freethis);
    లేకపోతే
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#లేకపోతే
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#అయితే_ముగింపు
}
#అయితే_ముగింపు /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#ఒకవేళ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
పూర్ణాంకం curl_dbg_getaddrinfo(స్థిరరాశి అక్షరం *hostname,
                         స్థిరరాశి అక్షరం *service,
                         స్థిరరాశి నిర్మాణం addrinfo *hints,
                         నిర్మాణం addrinfo **ఫలితం,
                         పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
#నిర్వచితమైతే USE_LWIPSOCK
  పూర్ణాంకం res = lwip_getaddrinfo(hostname, service, hints, ఫలితం);
#లేదావేళ defined(USE_FAKE_GETADDRINFO)
  పూర్ణాంకం res;
  స్థిరరాశి అక్షరం *env = getenv("CURL_DNS_SERVER");
  ఒకవేళ(env)
    res = r_getaddrinfo(hostname, service, hints, ఫలితం);
  లేకపోతే
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, ఫలితం);
#లేకపోతే
  /* !checksrc! disable BANNEDFUNC 1 */
  పూర్ణాంకం res = getaddrinfo(hostname, service, hints, ఫలితం);
#అయితే_ముగింపు
  ఒకవేళ(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", ఆధారం, పంక్తి,
                 (శూన్య *)*ఫలితం);
  లేకపోతే
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", ఆధారం, పంక్తి);
  తిరిగి res;
}
#అయితే_ముగింపు /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#ఒకవేళ defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
శూన్య Curl_addrinfo_సమితి_పోర్టు(నిర్మాణం Curl_addrinfo *addrinfo, పూర్ణాంకం పోర్టు)
{
  నిర్మాణం Curl_addrinfo *ca;
  నిర్మాణం sockaddr_in *addr;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం sockaddr_in6 *addr6;
#అయితే_ముగింపు
  కొరకు(ca = addrinfo; ca; ca = ca->ai_తదుపరి) {
    ఎంపిక(ca->ai_family) {
    స్థితి AF_INET:
      addr = (శూన్య *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((గుర్తులేని పొట్టి)పోర్టు);
      ఆపు;

#నిర్వచితమైతే USE_IPV6
    స్థితి AF_INET6:
      addr6 = (శూన్య *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((గుర్తులేని పొట్టి)పోర్టు);
      ఆపు;
#అయితే_ముగింపు
    }
  }
}
#అయితే_ముగింపు
