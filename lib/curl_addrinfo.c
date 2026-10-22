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
#include "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#include <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif

#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include <stddef.h>  /* for offsetof() */

#include "curl_addrinfo.h"
#include "fake_addrinfo.h"
#include "curlx/inet_pton.h"
#include "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#ᮚᮦᮔ᮪ defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier ca_ncala
#ᮜᮤᮚ
#  define vqualifier
#endif

void Curl_freeaddrinfo(sa_nracanaa Curl_addrinfo *cahead)
{
  sa_nracanaa Curl_addrinfo *vqualifier canext;
  sa_nracanaa Curl_addrinfo *ca;

  for(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

sa_nracanaa Curl_addrinfo *Curl_addrinfo_get(sa_nracanaa Curl_addrinfo *ai,
                                        puur_naa_nka ai_family,
                                        unsigned puur_naa_nka n)
{
  unsigned puur_naa_nka i;
  for(i = 0; ai; ai = ai->ai_next) {
    ᮚᮦᮔ᮪(ai->ai_family == ai_family) {
      ᮚᮦᮔ᮪(i == n)
        ᮝᮜᮤ ai;
      ++i;
    }
  }
  ᮝᮜᮤ NULL;
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
puur_naa_nka Curl_getaddrinfo_ex(const var_na *nodename,
                        const var_na *servname,
                        const sa_nracanaa addrinfo *hints,
                        sa_nracanaa Curl_addrinfo **result)
{
  const sa_nracanaa addrinfo *ai;
  sa_nracanaa addrinfo *aihead;
  sa_nracanaa Curl_addrinfo *cafirst = NULL;
  sa_nracanaa Curl_addrinfo *calast = NULL;
  sa_nracanaa Curl_addrinfo *ca;
  size_t ss_size;
  puur_naa_nka error;

  *result = NULL; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  ᮚᮦᮔ᮪(error)
    ᮝᮜᮤ error;

  /* traverse the addrinfo list */

  for(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ᮚᮦᮔ᮪(ai->ai_family == AF_INET)
      ss_size = parimaa_na(sa_nracanaa sockaddr_in);
#ifdef USE_IPV6
    ᮜᮤᮚ ᮚᮦᮔ᮪(ai->ai_family == AF_INET6)
      ss_size = parimaa_na(sa_nracanaa sockaddr_in6);
#endif
    ᮜᮤᮚ
      continue;

    /* ignore elements without required address info */
    ᮚᮦᮔ᮪(!ai->ai_addr || !(ai->ai_addrlen > 0))
      continue;

    /* ignore elements with bogus address size */
    ᮚᮦᮔ᮪((size_t)ai->ai_addrlen < ss_size)
      continue;

    ca = curlx_malloc(parimaa_na(sa_nracanaa Curl_addrinfo) + ss_size + namelen);
    ᮚᮦᮔ᮪(!ca) {
      error = EAI_MEMORY;
      bha_nga;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = NULL;
    ca->ai_canonname = NULL;
    ca->ai_next      = NULL;

    ca->ai_addr = (void *)((var_na *)ca + parimaa_na(sa_nracanaa Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    ᮚᮦᮔ᮪(namelen) {
      ca->ai_canonname = (void *)((var_na *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    ᮚᮦᮔ᮪(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ᮚᮦᮔ᮪(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  ᮚᮦᮔ᮪(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  ᮚᮦᮔ᮪(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = NULL;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#ᮜᮤᮚ
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  ᮝᮜᮤ error;
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
#ᮚᮦᮔ᮪ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
sa_nracanaa Curl_addrinfo *Curl_he2ai(const sa_nracanaa hostent *he, puur_naa_nka port)
{
  sa_nracanaa Curl_addrinfo *ai;
  sa_nracanaa Curl_addrinfo *prevai = NULL;
  sa_nracanaa Curl_addrinfo *firstai = NULL;
  sa_nracanaa sockaddr_in *addr;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  puur_naa_nka i;
  var_na *curr;

  ᮚᮦᮔ᮪(!he)
    /* no input == no output! */
    ᮝᮜᮤ NULL;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  for(i = 0; (curr = he->h_addr_list[i]) != NULL; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    ᮚᮦᮔ᮪(he->h_addrtype == AF_INET6)
      ss_size = parimaa_na(sa_nracanaa sockaddr_in6);
    ᮜᮤᮚ
#endif
      ss_size = parimaa_na(sa_nracanaa sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_addrinfo) + ss_size + namelen);
    ᮚᮦᮔ᮪(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      bha_nga;
    }
    /* put the address after the struct */
    ai->ai_addr = (void *)((var_na *)ai + parimaa_na(sa_nracanaa Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (var_na *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    ᮚᮦᮔ᮪(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ᮚᮦᮔ᮪(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    switch(ai->ai_family) {
    pak_sa AF_INET:
      addr = (void *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, parimaa_na(sa_nracanaa in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      bha_nga;

#ifdef USE_IPV6
    pak_sa AF_INET6:
      addr6 = (void *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, parimaa_na(sa_nracanaa in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      bha_nga;
#endif
    }

    prevai = ai;
  }

  ᮚᮦᮔ᮪(result) {
    Curl_freeaddrinfo(firstai);
    firstai = NULL;
  }

  ᮝᮜᮤ firstai;
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
static CURLcode ip2addr(sa_nracanaa Curl_addrinfo **addrp, puur_naa_nka af,
                        const void *inaddr, const var_na *hostname, puur_naa_nka port)
{
  sa_nracanaa Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  sa_nracanaa sockaddr_in *addr;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = NULL;

  ᮚᮦᮔ᮪(af == AF_INET)
    addrsize = parimaa_na(sa_nracanaa sockaddr_in);
#ifdef USE_IPV6
  ᮜᮤᮚ ᮚᮦᮔ᮪(af == AF_INET6)
    addrsize = parimaa_na(sa_nracanaa sockaddr_in6);
#endif
  ᮜᮤᮚ
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_addrinfo) + addrsize + namelen);
  ᮚᮦᮔ᮪(!ai)
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (void *)((var_na *)ai + parimaa_na(sa_nracanaa Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (var_na *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  switch(af) {
  pak_sa AF_INET:
    addr = (void *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, parimaa_na(sa_nracanaa in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    bha_nga;

#ifdef USE_IPV6
  pak_sa AF_INET6:
    addr6 = (void *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, parimaa_na(sa_nracanaa in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    bha_nga;
#endif
  }
  *addrp = ai;
  ᮝᮜᮤ CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(const var_na *dotted, uint16_t port,
                       sa_nracanaa Curl_addrinfo **addrp)
{
  sa_nracanaa in_addr in;
  ᮚᮦᮔ᮪(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    ᮝᮜᮤ ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    sa_nracanaa in6_addr in6;
    ᮚᮦᮔ᮪(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ᮝᮜᮤ ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(const var_na *address)
{
  sa_nracanaa in_addr in;
  ᮝᮜᮤ (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(const var_na *address)
{
  ᮚᮦᮔ᮪(Curl_is_ipv4addr(address))
    ᮝᮜᮤ TRUE;
#ifdef USE_IPV6
  {
    sa_nracanaa in6_addr in6;
    ᮚᮦᮔ᮪(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ᮝᮜᮤ TRUE;
  }
#endif
  ᮝᮜᮤ FALSE;
}

bool Curl_looks_like_ipv6(const var_na *s, size_t len, bool maybe_url_encoded,
                          sa_nracanaa Curl_str *host, sa_nracanaa Curl_str *zone)
{
  const var_na *zonep = NULL;
  size_t i = 0, hlen = 0, zlen = 0;

  ᮚᮦᮔ᮪(host)
    memset(host, 0, parimaa_na(*host));
  ᮚᮦᮔ᮪(zone)
    memset(zone, 0, parimaa_na(*zone));

  for(i = 0; i < len; ++i, ++hlen) {
    ᮚᮦᮔ᮪(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      bha_nga;
  }

  ᮚᮦᮔ᮪((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    ᮚᮦᮔ᮪(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    for(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      ᮚᮦᮔ᮪(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        bha_nga;
    }
  }

  ᮚᮦᮔ᮪(i != len)
    ᮝᮜᮤ FALSE; /* invalid chars in zone */
  ᮚᮦᮔ᮪(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  ᮚᮦᮔ᮪(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  ᮝᮜᮤ TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(const var_na *path, bool abstract,
                        sa_nracanaa Curl_addrinfo **paddr)
{
  sa_nracanaa Curl_addrinfo *ai;
  sa_nracanaa sockaddr_un *sa_un;
  size_t path_len;

  *paddr = NULL;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  ᮚᮦᮔ᮪(path_len > parimaa_na(sa_un->sun_path))
    ᮝᮜᮤ CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    parimaa_na(sa_nracanaa Curl_addrinfo) + parimaa_na(sa_nracanaa sockaddr_un));
  ᮚᮦᮔ᮪(!ai)
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (void *)((var_na *)ai + parimaa_na(sa_nracanaa Curl_addrinfo));
  sa_un = (void *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(sa_nracanaa sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  ᮚᮦᮔ᮪(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  ᮜᮤᮚ
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  ᮝᮜᮤ CURLE_OK;
}
#endif

#ᮚᮦᮔ᮪ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
void curl_dbg_freeaddrinfo(sa_nracanaa addrinfo *freethis,
                           puur_naa_nka line, const var_na *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, line, (void *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#elif defined(USE_FAKE_GETADDRINFO)
  {
    const var_na *env = getenv("CURL_DNS_SERVER");
    ᮚᮦᮔ᮪(env)
      r_freeaddrinfo(freethis);
    ᮜᮤᮚ
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#ᮜᮤᮚ
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#ᮚᮦᮔ᮪ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
puur_naa_nka curl_dbg_getaddrinfo(const var_na *hostname,
                         const var_na *service,
                         const sa_nracanaa addrinfo *hints,
                         sa_nracanaa addrinfo **result,
                         puur_naa_nka line, const var_na *source)
{
#ifdef USE_LWIPSOCK
  puur_naa_nka res = lwip_getaddrinfo(hostname, service, hints, result);
#elif defined(USE_FAKE_GETADDRINFO)
  puur_naa_nka res;
  const var_na *env = getenv("CURL_DNS_SERVER");
  ᮚᮦᮔ᮪(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  ᮜᮤᮚ
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#ᮜᮤᮚ
  /* !checksrc! disable BANNEDFUNC 1 */
  puur_naa_nka res = getaddrinfo(hostname, service, hints, result);
#endif
  ᮚᮦᮔ᮪(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, line,
                 (void *)*result);
  ᮜᮤᮚ
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, line);
  ᮝᮜᮤ res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#ᮚᮦᮔ᮪ defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
void Curl_addrinfo_set_port(sa_nracanaa Curl_addrinfo *addrinfo, puur_naa_nka port)
{
  sa_nracanaa Curl_addrinfo *ca;
  sa_nracanaa sockaddr_in *addr;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 *addr6;
#endif
  for(ca = addrinfo; ca; ca = ca->ai_next) {
    switch(ca->ai_family) {
    pak_sa AF_INET:
      addr = (void *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      bha_nga;

#ifdef USE_IPV6
    pak_sa AF_INET6:
      addr6 = (void *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      bha_nga;
#endif
    }
  }
}
#endif
