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
#  define vqualifier অস্থির
#অন্যথা
#  define vqualifier
#endif

শূন্য Curl_freeaddrinfo(কাঠামো Curl_addrinfo *cahead)
{
  কাঠামো Curl_addrinfo *vqualifier canext;
  কাঠামো Curl_addrinfo *ca;

  জন্য(ca = cahead; ca; ca = canext) {
    canext = ca->ai_পরবর্তী;
    curlx_free(ca);
  }
}

কাঠামো Curl_addrinfo *Curl_addrinfo_get(কাঠামো Curl_addrinfo *ai,
                                        পূর্ণসংখ্যা ai_family,
                                        unsigned পূর্ণসংখ্যা n)
{
  unsigned পূর্ণসংখ্যা i;
  জন্য(i = 0; ai; ai = ai->ai_পরবর্তী) {
    যদি(ai->ai_family == ai_family) {
      যদি(i == n)
        ফেরত ai;
      ++i;
    }
  }
  ফেরত ফাঁকা;
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
পূর্ণসংখ্যা Curl_getaddrinfo_ex(ধ্রুবক অক্ষর *nodename,
                        ধ্রুবক অক্ষর *servname,
                        ধ্রুবক কাঠামো addrinfo *hints,
                        কাঠামো Curl_addrinfo **ফলাফল)
{
  ধ্রুবক কাঠামো addrinfo *ai;
  কাঠামো addrinfo *aihead;
  কাঠামো Curl_addrinfo *cafirst = ফাঁকা;
  কাঠামো Curl_addrinfo *calast = ফাঁকা;
  কাঠামো Curl_addrinfo *ca;
  size_t ss_আকার;
  পূর্ণসংখ্যা error;

  *ফলাফল = ফাঁকা; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  যদি(error)
    ফেরত error;

  /* traverse the addrinfo list */

  জন্য(ai = aihead; ai; ai = ai->ai_পরবর্তী) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    যদি(ai->ai_family == AF_INET)
      ss_আকার = আকার(কাঠামো sockaddr_in);
#ifdef USE_IPV6
    অন্যথা যদি(ai->ai_family == AF_INET6)
      ss_আকার = আকার(কাঠামো sockaddr_in6);
#endif
    অন্যথা
      চালু;

    /* ignore elements without required address info */
    যদি(!ai->ai_addr || !(ai->ai_addrlen > 0))
      চালু;

    /* ignore elements with bogus address size */
    যদি((size_t)ai->ai_addrlen < ss_আকার)
      চালু;

    ca = curlx_malloc(আকার(কাঠামো Curl_addrinfo) + ss_আকার + namelen);
    যদি(!ca) {
      error = EAI_MEMORY;
      বিরতি;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_আকার;
    ca->ai_addr      = ফাঁকা;
    ca->ai_canonname = ফাঁকা;
    ca->ai_পরবর্তী      = ফাঁকা;

    ca->ai_addr = (শূন্য *)((অক্ষর *)ca + আকার(কাঠামো Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_আকার);

    যদি(namelen) {
      ca->ai_canonname = (শূন্য *)((অক্ষর *)ca->ai_addr + ss_আকার);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    যদি(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    যদি(calast)
      calast->ai_পরবর্তী = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  যদি(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  যদি(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ফাঁকা;
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

  *ফলাফল = cafirst;

  /* This is not a CURLcode */
  ফেরত error;
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
কাঠামো Curl_addrinfo *Curl_he2ai(ধ্রুবক কাঠামো hostent *he, পূর্ণসংখ্যা port)
{
  কাঠামো Curl_addrinfo *ai;
  কাঠামো Curl_addrinfo *prevai = ফাঁকা;
  কাঠামো Curl_addrinfo *firstai = ফাঁকা;
  কাঠামো sockaddr_in *addr;
#ifdef USE_IPV6
  কাঠামো sockaddr_in6 *addr6;
#endif
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা i;
  অক্ষর *curr;

  যদি(!he)
    /* no input == no output! */
    ফেরত ফাঁকা;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  জন্য(i = 0; (curr = he->h_addr_list[i]) != ফাঁকা; i++) {
    size_t ss_আকার;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    যদি(he->h_addrtype == AF_INET6)
      ss_আকার = আকার(কাঠামো sockaddr_in6);
    অন্যথা
#endif
      ss_আকার = আকার(কাঠামো sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, আকার(কাঠামো Curl_addrinfo) + ss_আকার + namelen);
    যদি(!ai) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      বিরতি;
    }
    /* put the address after the struct */
    ai->ai_addr = (শূন্য *)((অক্ষর *)ai + আকার(কাঠামো Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (অক্ষর *)ai->ai_addr + ss_আকার;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    যদি(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    যদি(prevai)
      /* make the previous entry point to this */
      prevai->ai_পরবর্তী = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_আকার;

    /* leave the rest of the struct filled with zero */

    নির্বাচন(ai->ai_family) {
    ক্ষেত্র AF_INET:
      addr = (শূন্য *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, আকার(কাঠামো in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      বিরতি;

#ifdef USE_IPV6
    ক্ষেত্র AF_INET6:
      addr6 = (শূন্য *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, আকার(কাঠামো in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      বিরতি;
#endif
    }

    prevai = ai;
  }

  যদি(ফলাফল) {
    Curl_freeaddrinfo(firstai);
    firstai = ফাঁকা;
  }

  ফেরত firstai;
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
স্থির CURLcode ip2addr(কাঠামো Curl_addrinfo **addrp, পূর্ণসংখ্যা af,
                        ধ্রুবক শূন্য *inaddr, ধ্রুবক অক্ষর *hostname, পূর্ণসংখ্যা port)
{
  কাঠামো Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  কাঠামো sockaddr_in *addr;
#ifdef USE_IPV6
  কাঠামো sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = ফাঁকা;

  যদি(af == AF_INET)
    addrsize = আকার(কাঠামো sockaddr_in);
#ifdef USE_IPV6
  অন্যথা যদি(af == AF_INET6)
    addrsize = আকার(কাঠামো sockaddr_in6);
#endif
  অন্যথা
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, আকার(কাঠামো Curl_addrinfo) + addrsize + namelen);
  যদি(!ai)
    ফেরত CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (শূন্য *)((অক্ষর *)ai + আকার(কাঠামো Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (অক্ষর *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  নির্বাচন(af) {
  ক্ষেত্র AF_INET:
    addr = (শূন্য *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, আকার(কাঠামো in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    বিরতি;

#ifdef USE_IPV6
  ক্ষেত্র AF_INET6:
    addr6 = (শূন্য *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, আকার(কাঠামো in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    বিরতি;
#endif
  }
  *addrp = ai;
  ফেরত CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ধ্রুবক অক্ষর *dotted, uint16_t port,
                       কাঠামো Curl_addrinfo **addrp)
{
  কাঠামো in_addr in;
  যদি(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    ফেরত ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    কাঠামো in6_addr in6;
    যদি(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ফেরত ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  ফেরত CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(ধ্রুবক অক্ষর *address)
{
  কাঠামো in_addr in;
  ফেরত (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(ধ্রুবক অক্ষর *address)
{
  যদি(Curl_is_ipv4addr(address))
    ফেরত TRUE;
#ifdef USE_IPV6
  {
    কাঠামো in6_addr in6;
    যদি(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ফেরত TRUE;
  }
#endif
  ফেরত FALSE;
}

bool Curl_looks_like_ipv6(ধ্রুবক অক্ষর *s, size_t len, bool maybe_url_encoded,
                          কাঠামো Curl_str *host, কাঠামো Curl_str *zone)
{
  ধ্রুবক অক্ষর *zonep = ফাঁকা;
  size_t i = 0, hlen = 0, zlen = 0;

  যদি(host)
    memset(host, 0, আকার(*host));
  যদি(zone)
    memset(zone, 0, আকার(*zone));

  জন্য(i = 0; i < len; ++i, ++hlen) {
    যদি(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      বিরতি;
  }

  যদি((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    যদি(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    জন্য(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      যদি(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        বিরতি;
    }
  }

  যদি(i != len)
    ফেরত FALSE; /* invalid chars in zone */
  যদি(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  যদি(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  ফেরত TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ধ্রুবক অক্ষর *path, bool abstract,
                        কাঠামো Curl_addrinfo **paddr)
{
  কাঠামো Curl_addrinfo *ai;
  কাঠামো sockaddr_un *sa_un;
  size_t path_len;

  *paddr = ফাঁকা;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  যদি(path_len > আকার(sa_un->sun_path))
    ফেরত CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    আকার(কাঠামো Curl_addrinfo) + আকার(কাঠামো sockaddr_un));
  যদি(!ai)
    ফেরত CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (শূন্য *)((অক্ষর *)ai + আকার(কাঠামো Curl_addrinfo));
  sa_un = (শূন্য *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(কাঠামো sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  যদি(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  অন্যথা
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  ফেরত CURLE_OK;
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
শূন্য curl_dbg_freeaddrinfo(কাঠামো addrinfo *freethis,
                           পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, সারি, (শূন্য *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#নতুবা_যদি defined(USE_FAKE_GETADDRINFO)
  {
    ধ্রুবক অক্ষর *env = getenv("CURL_DNS_SERVER");
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
পূর্ণসংখ্যা curl_dbg_getaddrinfo(ধ্রুবক অক্ষর *hostname,
                         ধ্রুবক অক্ষর *service,
                         ধ্রুবক কাঠামো addrinfo *hints,
                         কাঠামো addrinfo **ফলাফল,
                         পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
#ifdef USE_LWIPSOCK
  পূর্ণসংখ্যা res = lwip_getaddrinfo(hostname, service, hints, ফলাফল);
#নতুবা_যদি defined(USE_FAKE_GETADDRINFO)
  পূর্ণসংখ্যা res;
  ধ্রুবক অক্ষর *env = getenv("CURL_DNS_SERVER");
  যদি(env)
    res = r_getaddrinfo(hostname, service, hints, ফলাফল);
  অন্যথা
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, ফলাফল);
#অন্যথা
  /* !checksrc! disable BANNEDFUNC 1 */
  পূর্ণসংখ্যা res = getaddrinfo(hostname, service, hints, ফলাফল);
#endif
  যদি(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, সারি,
                 (শূন্য *)*ফলাফল);
  অন্যথা
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, সারি);
  ফেরত res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#যদি defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
শূন্য Curl_addrinfo_set_port(কাঠামো Curl_addrinfo *addrinfo, পূর্ণসংখ্যা port)
{
  কাঠামো Curl_addrinfo *ca;
  কাঠামো sockaddr_in *addr;
#ifdef USE_IPV6
  কাঠামো sockaddr_in6 *addr6;
#endif
  জন্য(ca = addrinfo; ca; ca = ca->ai_পরবর্তী) {
    নির্বাচন(ca->ai_family) {
    ক্ষেত্র AF_INET:
      addr = (শূন্য *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      বিরতি;

#ifdef USE_IPV6
    ক্ষেত্র AF_INET6:
      addr6 = (শূন্য *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      বিরতি;
#endif
    }
  }
}
#endif
