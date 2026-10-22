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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#অন্তর্ভাব <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#অন্তর্ভাব <sys/un.h>
#endif

#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব <stddef.h>  /* for offsetof() */

#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "fake_addrinfo.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#যদি defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier চঞ্চল
#অন্যথা
#  define vqualifier
#endif

শূন্য Curl_freeaddrinfo(সংরচনা Curl_addrinfo *cahead)
{
  সংরচনা Curl_addrinfo *vqualifier canext;
  সংরচনা Curl_addrinfo *ca;

  প্রতি(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

সংরচনা Curl_addrinfo *Curl_addrinfo_get(সংরচনা Curl_addrinfo *ai,
                                        পূর্ণাঙ্ক ai_family,
                                        unsigned পূর্ণাঙ্ক n)
{
  unsigned পূর্ণাঙ্ক i;
  প্রতি(i = 0; ai; ai = ai->ai_next) {
    যদি(ai->ai_family == ai_family) {
      যদি(i == n)
        প্রত্যাগম ai;
      ++i;
    }
  }
  প্রত্যাগম রিক্ত;
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
পূর্ণাঙ্ক Curl_getaddrinfo_ex(ধ্রুব বর্ণ *nodename,
                        ধ্রুব বর্ণ *servname,
                        ধ্রুব সংরচনা addrinfo *hints,
                        সংরচনা Curl_addrinfo **result)
{
  ধ্রুব সংরচনা addrinfo *ai;
  সংরচনা addrinfo *aihead;
  সংরচনা Curl_addrinfo *cafirst = রিক্ত;
  সংরচনা Curl_addrinfo *calast = রিক্ত;
  সংরচনা Curl_addrinfo *ca;
  size_t ss_size;
  পূর্ণাঙ্ক error;

  *result = রিক্ত; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  যদি(error)
    প্রত্যাগম error;

  /* traverse the addrinfo list */

  প্রতি(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    যদি(ai->ai_family == AF_INET)
      ss_size = পরিমাণ(সংরচনা sockaddr_in);
#ifdef USE_IPV6
    অন্যথা যদি(ai->ai_family == AF_INET6)
      ss_size = পরিমাণ(সংরচনা sockaddr_in6);
#endif
    অন্যথা
      অনুবর্ত;

    /* ignore elements without required address info */
    যদি(!ai->ai_addr || !(ai->ai_addrlen > 0))
      অনুবর্ত;

    /* ignore elements with bogus address size */
    যদি((size_t)ai->ai_addrlen < ss_size)
      অনুবর্ত;

    ca = curlx_malloc(পরিমাণ(সংরচনা Curl_addrinfo) + ss_size + namelen);
    যদি(!ca) {
      error = EAI_MEMORY;
      ভঙ্গ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = রিক্ত;
    ca->ai_canonname = রিক্ত;
    ca->ai_next      = রিক্ত;

    ca->ai_addr = (শূন্য *)((বর্ণ *)ca + পরিমাণ(সংরচনা Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    যদি(namelen) {
      ca->ai_canonname = (শূন্য *)((বর্ণ *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    যদি(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    যদি(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  যদি(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  যদি(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = রিক্ত;
  }
  অন্যথা যদি(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#অন্যথা
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  প্রত্যাগম error;
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
#যদি !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
সংরচনা Curl_addrinfo *Curl_he2ai(ধ্রুব সংরচনা hostent *he, পূর্ণাঙ্ক port)
{
  সংরচনা Curl_addrinfo *ai;
  সংরচনা Curl_addrinfo *prevai = রিক্ত;
  সংরচনা Curl_addrinfo *firstai = রিক্ত;
  সংরচনা sockaddr_in *addr;
#ifdef USE_IPV6
  সংরচনা sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক i;
  বর্ণ *curr;

  যদি(!he)
    /* no input == no output! */
    প্রত্যাগম রিক্ত;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  প্রতি(i = 0; (curr = he->h_addr_list[i]) != রিক্ত; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    যদি(he->h_addrtype == AF_INET6)
      ss_size = পরিমাণ(সংরচনা sockaddr_in6);
    অন্যথা
#endif
      ss_size = পরিমাণ(সংরচনা sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, পরিমাণ(সংরচনা Curl_addrinfo) + ss_size + namelen);
    যদি(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      ভঙ্গ;
    }
    /* put the address after the struct */
    ai->ai_addr = (শূন্য *)((বর্ণ *)ai + পরিমাণ(সংরচনা Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (বর্ণ *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    যদি(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    যদি(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    বিকল্প(ai->ai_family) {
    পক্ষ AF_INET:
      addr = (শূন্য *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, পরিমাণ(সংরচনা in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      ভঙ্গ;

#ifdef USE_IPV6
    পক্ষ AF_INET6:
      addr6 = (শূন্য *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, পরিমাণ(সংরচনা in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      ভঙ্গ;
#endif
    }

    prevai = ai;
  }

  যদি(result) {
    Curl_freeaddrinfo(firstai);
    firstai = রিক্ত;
  }

  প্রত্যাগম firstai;
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
স্থির CURLcode ip2addr(সংরচনা Curl_addrinfo **addrp, পূর্ণাঙ্ক af,
                        ধ্রুব শূন্য *inaddr, ধ্রুব বর্ণ *hostname, পূর্ণাঙ্ক port)
{
  সংরচনা Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  সংরচনা sockaddr_in *addr;
#ifdef USE_IPV6
  সংরচনা sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = রিক্ত;

  যদি(af == AF_INET)
    addrsize = পরিমাণ(সংরচনা sockaddr_in);
#ifdef USE_IPV6
  অন্যথা যদি(af == AF_INET6)
    addrsize = পরিমাণ(সংরচনা sockaddr_in6);
#endif
  অন্যথা
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, পরিমাণ(সংরচনা Curl_addrinfo) + addrsize + namelen);
  যদি(!ai)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (শূন্য *)((বর্ণ *)ai + পরিমাণ(সংরচনা Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (বর্ণ *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  বিকল্প(af) {
  পক্ষ AF_INET:
    addr = (শূন্য *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, পরিমাণ(সংরচনা in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    ভঙ্গ;

#ifdef USE_IPV6
  পক্ষ AF_INET6:
    addr6 = (শূন্য *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, পরিমাণ(সংরচনা in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    ভঙ্গ;
#endif
  }
  *addrp = ai;
  প্রত্যাগম CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ধ্রুব বর্ণ *dotted, uint16_t port,
                       সংরচনা Curl_addrinfo **addrp)
{
  সংরচনা in_addr in;
  যদি(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    প্রত্যাগম ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    সংরচনা in6_addr in6;
    যদি(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      প্রত্যাগম ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(ধ্রুব বর্ণ *address)
{
  সংরচনা in_addr in;
  প্রত্যাগম (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(ধ্রুব বর্ণ *address)
{
  যদি(Curl_is_ipv4addr(address))
    প্রত্যাগম TRUE;
#ifdef USE_IPV6
  {
    সংরচনা in6_addr in6;
    যদি(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      প্রত্যাগম TRUE;
  }
#endif
  প্রত্যাগম FALSE;
}

bool Curl_looks_like_ipv6(ধ্রুব বর্ণ *s, size_t len, bool maybe_url_encoded,
                          সংরচনা Curl_str *host, সংরচনা Curl_str *zone)
{
  ধ্রুব বর্ণ *zonep = রিক্ত;
  size_t i = 0, hlen = 0, zlen = 0;

  যদি(host)
    memset(host, 0, পরিমাণ(*host));
  যদি(zone)
    memset(zone, 0, পরিমাণ(*zone));

  প্রতি(i = 0; i < len; ++i, ++hlen) {
    যদি(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      ভঙ্গ;
  }

  যদি((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    যদি(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    প্রতি(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      যদি(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        ভঙ্গ;
    }
  }

  যদি(i != len)
    প্রত্যাগম FALSE; /* invalid chars in zone */
  যদি(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  যদি(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  প্রত্যাগম TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ধ্রুব বর্ণ *path, bool abstract,
                        সংরচনা Curl_addrinfo **paddr)
{
  সংরচনা Curl_addrinfo *ai;
  সংরচনা sockaddr_un *sa_un;
  size_t path_len;

  *paddr = রিক্ত;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  যদি(path_len > পরিমাণ(sa_un->sun_path))
    প্রত্যাগম CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    পরিমাণ(সংরচনা Curl_addrinfo) + পরিমাণ(সংরচনা sockaddr_un));
  যদি(!ai)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (শূন্য *)((বর্ণ *)ai + পরিমাণ(সংরচনা Curl_addrinfo));
  sa_un = (শূন্য *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(সংরচনা sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  যদি(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  অন্যথা
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  প্রত্যাগম CURLE_OK;
}
#endif

#যদি defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
শূন্য curl_dbg_freeaddrinfo(সংরচনা addrinfo *freethis,
                           পূর্ণাঙ্ক সারি, ধ্রুব বর্ণ *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, সারি, (শূন্য *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#নোচেদ্যদি defined(USE_FAKE_GETADDRINFO)
  {
    ধ্রুব বর্ণ *env = getenv("CURL_DNS_SERVER");
    যদি(env)
      r_freeaddrinfo(freethis);
    অন্যথা
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#অন্যথা
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#যদি defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
পূর্ণাঙ্ক curl_dbg_getaddrinfo(ধ্রুব বর্ণ *hostname,
                         ধ্রুব বর্ণ *service,
                         ধ্রুব সংরচনা addrinfo *hints,
                         সংরচনা addrinfo **result,
                         পূর্ণাঙ্ক সারি, ধ্রুব বর্ণ *source)
{
#ifdef USE_LWIPSOCK
  পূর্ণাঙ্ক res = lwip_getaddrinfo(hostname, service, hints, result);
#নোচেদ্যদি defined(USE_FAKE_GETADDRINFO)
  পূর্ণাঙ্ক res;
  ধ্রুব বর্ণ *env = getenv("CURL_DNS_SERVER");
  যদি(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  অন্যথা
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#অন্যথা
  /* !checksrc! disable BANNEDFUNC 1 */
  পূর্ণাঙ্ক res = getaddrinfo(hostname, service, hints, result);
#endif
  যদি(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, সারি,
                 (শূন্য *)*result);
  অন্যথা
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, সারি);
  প্রত্যাগম res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#যদি defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
শূন্য Curl_addrinfo_set_port(সংরচনা Curl_addrinfo *addrinfo, পূর্ণাঙ্ক port)
{
  সংরচনা Curl_addrinfo *ca;
  সংরচনা sockaddr_in *addr;
#ifdef USE_IPV6
  সংরচনা sockaddr_in6 *addr6;
#endif
  প্রতি(ca = addrinfo; ca; ca = ca->ai_next) {
    বিকল্প(ca->ai_family) {
    পক্ষ AF_INET:
      addr = (শূন্য *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      ভঙ্গ;

#ifdef USE_IPV6
    পক্ষ AF_INET6:
      addr6 = (শূন্য *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      ভঙ্গ;
#endif
    }
  }
}
#endif
