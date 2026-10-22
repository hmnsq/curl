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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#ଅନ୍ତର୍ଭାଵ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#ଅନ୍ତର୍ଭାଵ <sys/un.h>
#endif

#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ <stddef.h>  /* for offsetof() */

#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "fake_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#ଯଦି defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier ଅସ୍ଥିର
#ଅନ୍ୟଥା
#  define vqualifier
#endif

ଶୂନ୍ୟ Curl_freeaddrinfo(ସଂରଚନା Curl_addrinfo *cahead)
{
  ସଂରଚନା Curl_addrinfo *vqualifier canext;
  ସଂରଚନା Curl_addrinfo *ca;

  ପାଇଁ(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

ସଂରଚନା Curl_addrinfo *Curl_addrinfo_get(ସଂରଚନା Curl_addrinfo *ai,
                                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family,
                                        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ପାଇଁ(i = 0; ai; ai = ai->ai_next) {
    ଯଦି(ai->ai_family == ai_family) {
      ଯଦି(i == n)
        ଫେରନ୍ତୁ ai;
      ++i;
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_getaddrinfo_ex(ଅଚଳ ଅକ୍ଷର *nodename,
                        ଅଚଳ ଅକ୍ଷର *servname,
                        ଅଚଳ ସଂରଚନା addrinfo *hints,
                        ସଂରଚନା Curl_addrinfo **result)
{
  ଅଚଳ ସଂରଚନା addrinfo *ai;
  ସଂରଚନା addrinfo *aihead;
  ସଂରଚନା Curl_addrinfo *cafirst = ଖାଲି;
  ସଂରଚନା Curl_addrinfo *calast = ଖାଲି;
  ସଂରଚନା Curl_addrinfo *ca;
  size_t ss_size;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା error;

  *result = ଖାଲି; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  ଯଦି(error)
    ଫେରନ୍ତୁ error;

  /* traverse the addrinfo list */

  ପାଇଁ(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ଯଦି(ai->ai_family == AF_INET)
      ss_size = ଆକାର(ସଂରଚନା sockaddr_in);
#ifdef USE_IPV6
    ଅନ୍ୟଥା ଯଦି(ai->ai_family == AF_INET6)
      ss_size = ଆକାର(ସଂରଚନା sockaddr_in6);
#endif
    ଅନ୍ୟଥା
      ଜାରି;

    /* ignore elements without required address info */
    ଯଦି(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ଜାରି;

    /* ignore elements with bogus address size */
    ଯଦି((size_t)ai->ai_addrlen < ss_size)
      ଜାରି;

    ca = curlx_malloc(ଆକାର(ସଂରଚନା Curl_addrinfo) + ss_size + namelen);
    ଯଦି(!ca) {
      error = EAI_MEMORY;
      ଭାଙ୍ଗ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ଖାଲି;
    ca->ai_canonname = ଖାଲି;
    ca->ai_next      = ଖାଲି;

    ca->ai_addr = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ca + ଆକାର(ସଂରଚନା Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    ଯଦି(namelen) {
      ca->ai_canonname = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    ଯଦି(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ଯଦି(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  ଯଦି(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  ଯଦି(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ଖାଲି;
  }
  ଅନ୍ୟଥା ଯଦି(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#ଅନ୍ୟଥା
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  ଫେରନ୍ତୁ error;
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
#ଯଦି !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ସଂରଚନା Curl_addrinfo *Curl_he2ai(ଅଚଳ ସଂରଚନା hostent *he, ପୂର୍ଣ୍ଣସଂଖ୍ୟା port)
{
  ସଂରଚନା Curl_addrinfo *ai;
  ସଂରଚନା Curl_addrinfo *prevai = ଖାଲି;
  ସଂରଚନା Curl_addrinfo *firstai = ଖାଲି;
  ସଂରଚନା sockaddr_in *addr;
#ifdef USE_IPV6
  ସଂରଚନା sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ଅକ୍ଷର *curr;

  ଯଦି(!he)
    /* no input == no output! */
    ଫେରନ୍ତୁ ଖାଲି;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  ପାଇଁ(i = 0; (curr = he->h_addr_list[i]) != ଖାଲି; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    ଯଦି(he->h_addrtype == AF_INET6)
      ss_size = ଆକାର(ସଂରଚନା sockaddr_in6);
    ଅନ୍ୟଥା
#endif
      ss_size = ଆକାର(ସଂରଚନା sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, ଆକାର(ସଂରଚନା Curl_addrinfo) + ss_size + namelen);
    ଯଦି(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      ଭାଙ୍ଗ;
    }
    /* put the address after the struct */
    ai->ai_addr = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ai + ଆକାର(ସଂରଚନା Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (ଅକ୍ଷର *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    ଯଦି(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ଯଦି(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    ବାଛ(ai->ai_family) {
    ସ୍ଥିତି AF_INET:
      addr = (ଶୂନ୍ୟ *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, ଆକାର(ସଂରଚନା in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      ଭାଙ୍ଗ;

#ifdef USE_IPV6
    ସ୍ଥିତି AF_INET6:
      addr6 = (ଶୂନ୍ୟ *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, ଆକାର(ସଂରଚନା in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      ଭାଙ୍ଗ;
#endif
    }

    prevai = ai;
  }

  ଯଦି(result) {
    Curl_freeaddrinfo(firstai);
    firstai = ଖାଲି;
  }

  ଫେରନ୍ତୁ firstai;
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
ସ୍ଥିର CURLcode ip2addr(ସଂରଚନା Curl_addrinfo **addrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af,
                        ଅଚଳ ଶୂନ୍ୟ *inaddr, ଅଚଳ ଅକ୍ଷର *hostname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା port)
{
  ସଂରଚନା Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  ସଂରଚନା sockaddr_in *addr;
#ifdef USE_IPV6
  ସଂରଚନା sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = ଖାଲି;

  ଯଦି(af == AF_INET)
    addrsize = ଆକାର(ସଂରଚନା sockaddr_in);
#ifdef USE_IPV6
  ଅନ୍ୟଥା ଯଦି(af == AF_INET6)
    addrsize = ଆକାର(ସଂରଚନା sockaddr_in6);
#endif
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, ଆକାର(ସଂରଚନା Curl_addrinfo) + addrsize + namelen);
  ଯଦି(!ai)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ai + ଆକାର(ସଂରଚନା Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (ଅକ୍ଷର *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  ବାଛ(af) {
  ସ୍ଥିତି AF_INET:
    addr = (ଶୂନ୍ୟ *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, ଆକାର(ସଂରଚନା in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    ଭାଙ୍ଗ;

#ifdef USE_IPV6
  ସ୍ଥିତି AF_INET6:
    addr6 = (ଶୂନ୍ୟ *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, ଆକାର(ସଂରଚନା in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    ଭାଙ୍ଗ;
#endif
  }
  *addrp = ai;
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ଅଚଳ ଅକ୍ଷର *dotted, uint16_t port,
                       ସଂରଚନା Curl_addrinfo **addrp)
{
  ସଂରଚନା in_addr in;
  ଯଦି(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    ଫେରନ୍ତୁ ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    ସଂରଚନା in6_addr in6;
    ଯଦି(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ଫେରନ୍ତୁ ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(ଅଚଳ ଅକ୍ଷର *address)
{
  ସଂରଚନା in_addr in;
  ଫେରନ୍ତୁ (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(ଅଚଳ ଅକ୍ଷର *address)
{
  ଯଦି(Curl_is_ipv4addr(address))
    ଫେରନ୍ତୁ TRUE;
#ifdef USE_IPV6
  {
    ସଂରଚନା in6_addr in6;
    ଯଦି(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ଫେରନ୍ତୁ TRUE;
  }
#endif
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_looks_like_ipv6(ଅଚଳ ଅକ୍ଷର *s, size_t len, bool maybe_url_encoded,
                          ସଂରଚନା Curl_str *host, ସଂରଚନା Curl_str *zone)
{
  ଅଚଳ ଅକ୍ଷର *zonep = ଖାଲି;
  size_t i = 0, hlen = 0, zlen = 0;

  ଯଦି(host)
    memset(host, 0, ଆକାର(*host));
  ଯଦି(zone)
    memset(zone, 0, ଆକାର(*zone));

  ପାଇଁ(i = 0; i < len; ++i, ++hlen) {
    ଯଦି(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      ଭାଙ୍ଗ;
  }

  ଯଦି((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    ଯଦି(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    ପାଇଁ(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      ଯଦି(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        ଭାଙ୍ଗ;
    }
  }

  ଯଦି(i != len)
    ଫେରନ୍ତୁ FALSE; /* invalid chars in zone */
  ଯଦି(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  ଯଦି(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  ଫେରନ୍ତୁ TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ଅଚଳ ଅକ୍ଷର *path, bool abstract,
                        ସଂରଚନା Curl_addrinfo **paddr)
{
  ସଂରଚନା Curl_addrinfo *ai;
  ସଂରଚନା sockaddr_un *sa_un;
  size_t path_len;

  *paddr = ଖାଲି;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  ଯଦି(path_len > ଆକାର(sa_un->sun_path))
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    ଆକାର(ସଂରଚନା Curl_addrinfo) + ଆକାର(ସଂରଚନା sockaddr_un));
  ଯଦି(!ai)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ai + ଆକାର(ସଂରଚନା Curl_addrinfo));
  sa_un = (ଶୂନ୍ୟ *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(ସଂରଚନା sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  ଯଦି(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  ଅନ୍ୟଥା
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

#ଯଦି defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ଶୂନ୍ୟ curl_dbg_freeaddrinfo(ସଂରଚନା addrinfo *freethis,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, ଧାଡ଼ି, (ଶୂନ୍ୟ *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#ନଚେତଯଦି defined(USE_FAKE_GETADDRINFO)
  {
    ଅଚଳ ଅକ୍ଷର *env = getenv("CURL_DNS_SERVER");
    ଯଦି(env)
      r_freeaddrinfo(freethis);
    ଅନ୍ୟଥା
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#ଅନ୍ୟଥା
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#ଯଦି defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_dbg_getaddrinfo(ଅଚଳ ଅକ୍ଷର *hostname,
                         ଅଚଳ ଅକ୍ଷର *service,
                         ଅଚଳ ସଂରଚନା addrinfo *hints,
                         ସଂରଚନା addrinfo **result,
                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
#ifdef USE_LWIPSOCK
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = lwip_getaddrinfo(hostname, service, hints, result);
#ନଚେତଯଦି defined(USE_FAKE_GETADDRINFO)
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;
  ଅଚଳ ଅକ୍ଷର *env = getenv("CURL_DNS_SERVER");
  ଯଦି(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  ଅନ୍ୟଥା
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#ଅନ୍ୟଥା
  /* !checksrc! disable BANNEDFUNC 1 */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = getaddrinfo(hostname, service, hints, result);
#endif
  ଯଦି(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, ଧାଡ଼ି,
                 (ଶୂନ୍ୟ *)*result);
  ଅନ୍ୟଥା
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, ଧାଡ଼ି);
  ଫେରନ୍ତୁ res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#ଯଦି defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
ଶୂନ୍ୟ Curl_addrinfo_set_port(ସଂରଚନା Curl_addrinfo *addrinfo, ପୂର୍ଣ୍ଣସଂଖ୍ୟା port)
{
  ସଂରଚନା Curl_addrinfo *ca;
  ସଂରଚନା sockaddr_in *addr;
#ifdef USE_IPV6
  ସଂରଚନା sockaddr_in6 *addr6;
#endif
  ପାଇଁ(ca = addrinfo; ca; ca = ca->ai_next) {
    ବାଛ(ca->ai_family) {
    ସ୍ଥିତି AF_INET:
      addr = (ଶୂନ୍ୟ *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      ଭାଙ୍ଗ;

#ifdef USE_IPV6
    ସ୍ଥିତି AF_INET6:
      addr6 = (ଶୂନ୍ୟ *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      ଭାଙ୍ଗ;
#endif
    }
  }
}
#endif
