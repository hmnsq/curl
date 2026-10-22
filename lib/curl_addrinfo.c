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

#якщо defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier volatile
#інакше
#  define vqualifier
#endif

void Curl_freeaddrinfo(struct Curl_addrinfo *cahead)
{
  struct Curl_addrinfo *vqualifier canext;
  struct Curl_addrinfo *ca;

  для(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

struct Curl_addrinfo *Curl_addrinfo_get(struct Curl_addrinfo *ai,
                                        ціле ai_family,
                                        unsigned ціле n)
{
  unsigned ціле i;
  для(i = 0; ai; ai = ai->ai_next) {
    якщо(ai->ai_family == ai_family) {
      якщо(i == n)
        повернути ai;
      ++i;
    }
  }
  повернути NULL;
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
ціле Curl_getaddrinfo_ex(const char *nodename,
                        const char *servname,
                        const struct addrinfo *hints,
                        struct Curl_addrinfo **result)
{
  const struct addrinfo *ai;
  struct addrinfo *aihead;
  struct Curl_addrinfo *cafirst = NULL;
  struct Curl_addrinfo *calast = NULL;
  struct Curl_addrinfo *ca;
  size_t ss_size;
  ціле error;

  *result = NULL; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  якщо(error)
    повернути error;

  /* traverse the addrinfo list */

  для(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    якщо(ai->ai_family == AF_INET)
      ss_size = sizeof(struct sockaddr_in);
#ifdef USE_IPV6
    інакше якщо(ai->ai_family == AF_INET6)
      ss_size = sizeof(struct sockaddr_in6);
#endif
    інакше
      continue;

    /* ignore elements without required address info */
    якщо(!ai->ai_addr || !(ai->ai_addrlen > 0))
      continue;

    /* ignore elements with bogus address size */
    якщо((size_t)ai->ai_addrlen < ss_size)
      continue;

    ca = curlx_malloc(sizeof(struct Curl_addrinfo) + ss_size + namelen);
    якщо(!ca) {
      error = EAI_MEMORY;
      break;
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

    ca->ai_addr = (void *)((char *)ca + sizeof(struct Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    якщо(namelen) {
      ca->ai_canonname = (void *)((char *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    якщо(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    якщо(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  якщо(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  якщо(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = NULL;
  }
  інакше якщо(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#інакше
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  повернути error;
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
#якщо !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
struct Curl_addrinfo *Curl_he2ai(const struct hostent *he, ціле port)
{
  struct Curl_addrinfo *ai;
  struct Curl_addrinfo *prevai = NULL;
  struct Curl_addrinfo *firstai = NULL;
  struct sockaddr_in *addr;
#ifdef USE_IPV6
  struct sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  ціле i;
  char *curr;

  якщо(!he)
    /* no input == no output! */
    повернути NULL;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  для(i = 0; (curr = he->h_addr_list[i]) != NULL; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    якщо(he->h_addrtype == AF_INET6)
      ss_size = sizeof(struct sockaddr_in6);
    інакше
#endif
      ss_size = sizeof(struct sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, sizeof(struct Curl_addrinfo) + ss_size + namelen);
    якщо(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      break;
    }
    /* put the address after the struct */
    ai->ai_addr = (void *)((char *)ai + sizeof(struct Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (char *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    якщо(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    якщо(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    switch(ai->ai_family) {
    case AF_INET:
      addr = (void *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, sizeof(struct in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      break;

#ifdef USE_IPV6
    case AF_INET6:
      addr6 = (void *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, sizeof(struct in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      break;
#endif
    }

    prevai = ai;
  }

  якщо(result) {
    Curl_freeaddrinfo(firstai);
    firstai = NULL;
  }

  повернути firstai;
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
static CURLcode ip2addr(struct Curl_addrinfo **addrp, ціле af,
                        const void *inaddr, const char *hostname, ціле port)
{
  struct Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  struct sockaddr_in *addr;
#ifdef USE_IPV6
  struct sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = NULL;

  якщо(af == AF_INET)
    addrsize = sizeof(struct sockaddr_in);
#ifdef USE_IPV6
  інакше якщо(af == AF_INET6)
    addrsize = sizeof(struct sockaddr_in6);
#endif
  інакше
    повернути CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, sizeof(struct Curl_addrinfo) + addrsize + namelen);
  якщо(!ai)
    повернути CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (void *)((char *)ai + sizeof(struct Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (char *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  switch(af) {
  case AF_INET:
    addr = (void *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, sizeof(struct in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    break;

#ifdef USE_IPV6
  case AF_INET6:
    addr6 = (void *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, sizeof(struct in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    break;
#endif
  }
  *addrp = ai;
  повернути CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(const char *dotted, uint16_t port,
                       struct Curl_addrinfo **addrp)
{
  struct in_addr in;
  якщо(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    повернути ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    struct in6_addr in6;
    якщо(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      повернути ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  повернути CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(const char *address)
{
  struct in_addr in;
  повернути (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(const char *address)
{
  якщо(Curl_is_ipv4addr(address))
    повернути TRUE;
#ifdef USE_IPV6
  {
    struct in6_addr in6;
    якщо(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      повернути TRUE;
  }
#endif
  повернути FALSE;
}

bool Curl_looks_like_ipv6(const char *s, size_t len, bool maybe_url_encoded,
                          struct Curl_str *host, struct Curl_str *zone)
{
  const char *zonep = NULL;
  size_t i = 0, hlen = 0, zlen = 0;

  якщо(host)
    memset(host, 0, sizeof(*host));
  якщо(zone)
    memset(zone, 0, sizeof(*zone));

  для(i = 0; i < len; ++i, ++hlen) {
    якщо(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      break;
  }

  якщо((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    якщо(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    для(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      якщо(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        break;
    }
  }

  якщо(i != len)
    повернути FALSE; /* invalid chars in zone */
  якщо(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  якщо(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  повернути TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(const char *path, bool abstract,
                        struct Curl_addrinfo **paddr)
{
  struct Curl_addrinfo *ai;
  struct sockaddr_un *sa_un;
  size_t path_len;

  *paddr = NULL;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  якщо(path_len > sizeof(sa_un->sun_path))
    повернути CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    sizeof(struct Curl_addrinfo) + sizeof(struct sockaddr_un));
  якщо(!ai)
    повернути CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (void *)((char *)ai + sizeof(struct Curl_addrinfo));
  sa_un = (void *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(struct sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  якщо(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  інакше
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  повернути CURLE_OK;
}
#endif

#якщо defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
void curl_dbg_freeaddrinfo(struct addrinfo *freethis,
                           ціле line, const char *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, line, (void *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#elif defined(USE_FAKE_GETADDRINFO)
  {
    const char *env = getenv("CURL_DNS_SERVER");
    якщо(env)
      r_freeaddrinfo(freethis);
    інакше
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#інакше
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#якщо defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ціле curl_dbg_getaddrinfo(const char *hostname,
                         const char *service,
                         const struct addrinfo *hints,
                         struct addrinfo **result,
                         ціле line, const char *source)
{
#ifdef USE_LWIPSOCK
  ціле res = lwip_getaddrinfo(hostname, service, hints, result);
#elif defined(USE_FAKE_GETADDRINFO)
  ціле res;
  const char *env = getenv("CURL_DNS_SERVER");
  якщо(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  інакше
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#інакше
  /* !checksrc! disable BANNEDFUNC 1 */
  ціле res = getaddrinfo(hostname, service, hints, result);
#endif
  якщо(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, line,
                 (void *)*result);
  інакше
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, line);
  повернути res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#якщо defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
void Curl_addrinfo_set_port(struct Curl_addrinfo *addrinfo, ціле port)
{
  struct Curl_addrinfo *ca;
  struct sockaddr_in *addr;
#ifdef USE_IPV6
  struct sockaddr_in6 *addr6;
#endif
  для(ca = addrinfo; ca; ca = ca->ai_next) {
    switch(ca->ai_family) {
    case AF_INET:
      addr = (void *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      break;

#ifdef USE_IPV6
    case AF_INET6:
      addr6 = (void *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      break;
#endif
    }
  }
}
#endif
