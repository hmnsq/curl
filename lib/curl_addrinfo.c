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

#အကယ်၍ defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier ca_ncala
#မဟုတ်ပါက
#  define vqualifier
#endif

void Curl_freeaddrinfo(sa_nracanaa Curl_addrinfo *cahead)
{
  sa_nracanaa Curl_addrinfo *vqualifier canext;
  sa_nracanaa Curl_addrinfo *ca;

  အတွက်(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

sa_nracanaa Curl_addrinfo *Curl_addrinfo_get(sa_nracanaa Curl_addrinfo *ai,
                                        ကိန်းပြည့် ai_family,
                                        unsigned ကိန်းပြည့် n)
{
  unsigned ကိန်းပြည့် i;
  အတွက်(i = 0; ai; ai = ai->ai_next) {
    အကယ်၍(ai->ai_family == ai_family) {
      အကယ်၍(i == n)
        ပြန်ပေး ai;
      ++i;
    }
  }
  ပြန်ပေး NULL;
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
ကိန်းပြည့် Curl_getaddrinfo_ex(const var_na *nodename,
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
  ကိန်းပြည့် error;

  *result = NULL; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  အကယ်၍(error)
    ပြန်ပေး error;

  /* traverse the addrinfo list */

  အတွက်(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    အကယ်၍(ai->ai_family == AF_INET)
      ss_size = parimaa_na(sa_nracanaa sockaddr_in);
#ifdef USE_IPV6
    မဟုတ်ပါက အကယ်၍(ai->ai_family == AF_INET6)
      ss_size = parimaa_na(sa_nracanaa sockaddr_in6);
#endif
    မဟုတ်ပါက
      continue;

    /* ignore elements without required address info */
    အကယ်၍(!ai->ai_addr || !(ai->ai_addrlen > 0))
      continue;

    /* ignore elements with bogus address size */
    အကယ်၍((size_t)ai->ai_addrlen < ss_size)
      continue;

    ca = curlx_malloc(parimaa_na(sa_nracanaa Curl_addrinfo) + ss_size + namelen);
    အကယ်၍(!ca) {
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

    အကယ်၍(namelen) {
      ca->ai_canonname = (void *)((var_na *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    အကယ်၍(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    အကယ်၍(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  အကယ်၍(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  အကယ်၍(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = NULL;
  }
  မဟုတ်ပါက အကယ်၍(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#မဟုတ်ပါက
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  ပြန်ပေး error;
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
#အကယ်၍ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
sa_nracanaa Curl_addrinfo *Curl_he2ai(const sa_nracanaa hostent *he, ကိန်းပြည့် port)
{
  sa_nracanaa Curl_addrinfo *ai;
  sa_nracanaa Curl_addrinfo *prevai = NULL;
  sa_nracanaa Curl_addrinfo *firstai = NULL;
  sa_nracanaa sockaddr_in *addr;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  ကိန်းပြည့် i;
  var_na *curr;

  အကယ်၍(!he)
    /* no input == no output! */
    ပြန်ပေး NULL;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  အတွက်(i = 0; (curr = he->h_addr_list[i]) != NULL; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    အကယ်၍(he->h_addrtype == AF_INET6)
      ss_size = parimaa_na(sa_nracanaa sockaddr_in6);
    မဟုတ်ပါက
#endif
      ss_size = parimaa_na(sa_nracanaa sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_addrinfo) + ss_size + namelen);
    အကယ်၍(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      bha_nga;
    }
    /* put the address after the struct */
    ai->ai_addr = (void *)((var_na *)ai + parimaa_na(sa_nracanaa Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (var_na *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    အကယ်၍(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    အကယ်၍(prevai)
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

  အကယ်၍(result) {
    Curl_freeaddrinfo(firstai);
    firstai = NULL;
  }

  ပြန်ပေး firstai;
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
static CURLcode ip2addr(sa_nracanaa Curl_addrinfo **addrp, ကိန်းပြည့် af,
                        const void *inaddr, const var_na *hostname, ကိန်းပြည့် port)
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

  အကယ်၍(af == AF_INET)
    addrsize = parimaa_na(sa_nracanaa sockaddr_in);
#ifdef USE_IPV6
  မဟုတ်ပါက အကယ်၍(af == AF_INET6)
    addrsize = parimaa_na(sa_nracanaa sockaddr_in6);
#endif
  မဟုတ်ပါက
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_addrinfo) + addrsize + namelen);
  အကယ်၍(!ai)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;
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
  ပြန်ပေး CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(const var_na *dotted, uint16_t port,
                       sa_nracanaa Curl_addrinfo **addrp)
{
  sa_nracanaa in_addr in;
  အကယ်၍(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    ပြန်ပေး ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    sa_nracanaa in6_addr in6;
    အကယ်၍(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ပြန်ပေး ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(const var_na *address)
{
  sa_nracanaa in_addr in;
  ပြန်ပေး (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(const var_na *address)
{
  အကယ်၍(Curl_is_ipv4addr(address))
    ပြန်ပေး TRUE;
#ifdef USE_IPV6
  {
    sa_nracanaa in6_addr in6;
    အကယ်၍(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ပြန်ပေး TRUE;
  }
#endif
  ပြန်ပေး FALSE;
}

bool Curl_looks_like_ipv6(const var_na *s, size_t len, bool maybe_url_encoded,
                          sa_nracanaa Curl_str *host, sa_nracanaa Curl_str *zone)
{
  const var_na *zonep = NULL;
  size_t i = 0, hlen = 0, zlen = 0;

  အကယ်၍(host)
    memset(host, 0, parimaa_na(*host));
  အကယ်၍(zone)
    memset(zone, 0, parimaa_na(*zone));

  အတွက်(i = 0; i < len; ++i, ++hlen) {
    အကယ်၍(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      bha_nga;
  }

  အကယ်၍((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    အကယ်၍(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    အတွက်(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      အကယ်၍(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        bha_nga;
    }
  }

  အကယ်၍(i != len)
    ပြန်ပေး FALSE; /* invalid chars in zone */
  အကယ်၍(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  အကယ်၍(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  ပြန်ပေး TRUE;
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
  အကယ်၍(path_len > parimaa_na(sa_un->sun_path))
    ပြန်ပေး CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    parimaa_na(sa_nracanaa Curl_addrinfo) + parimaa_na(sa_nracanaa sockaddr_un));
  အကယ်၍(!ai)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (void *)((var_na *)ai + parimaa_na(sa_nracanaa Curl_addrinfo));
  sa_un = (void *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(sa_nracanaa sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  အကယ်၍(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  မဟုတ်ပါက
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  ပြန်ပေး CURLE_OK;
}
#endif

#အကယ်၍ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
void curl_dbg_freeaddrinfo(sa_nracanaa addrinfo *freethis,
                           ကိန်းပြည့် line, const var_na *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, line, (void *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#elif defined(USE_FAKE_GETADDRINFO)
  {
    const var_na *env = getenv("CURL_DNS_SERVER");
    အကယ်၍(env)
      r_freeaddrinfo(freethis);
    မဟုတ်ပါက
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#မဟုတ်ပါက
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#အကယ်၍ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ကိန်းပြည့် curl_dbg_getaddrinfo(const var_na *hostname,
                         const var_na *service,
                         const sa_nracanaa addrinfo *hints,
                         sa_nracanaa addrinfo **result,
                         ကိန်းပြည့် line, const var_na *source)
{
#ifdef USE_LWIPSOCK
  ကိန်းပြည့် res = lwip_getaddrinfo(hostname, service, hints, result);
#elif defined(USE_FAKE_GETADDRINFO)
  ကိန်းပြည့် res;
  const var_na *env = getenv("CURL_DNS_SERVER");
  အကယ်၍(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  မဟုတ်ပါက
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#မဟုတ်ပါက
  /* !checksrc! disable BANNEDFUNC 1 */
  ကိန်းပြည့် res = getaddrinfo(hostname, service, hints, result);
#endif
  အကယ်၍(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, line,
                 (void *)*result);
  မဟုတ်ပါက
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, line);
  ပြန်ပေး res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#အကယ်၍ defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
void Curl_addrinfo_set_port(sa_nracanaa Curl_addrinfo *addrinfo, ကိန်းပြည့် port)
{
  sa_nracanaa Curl_addrinfo *ca;
  sa_nracanaa sockaddr_in *addr;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 *addr6;
#endif
  အတွက်(ca = addrinfo; ca; ca = ca->ai_next) {
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
