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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#અન્તર્ભાવ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#અન્તર્ભાવ <sys/un.h>
#endif

#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ <stddef.h>  /* for offsetof() */

#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "fake_addrinfo.h"
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#જો defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier અસ્થિર
#નહીંતર
#  define vqualifier
#endif

શૂન્ય Curl_freeaddrinfo(સંરચના Curl_addrinfo *cahead)
{
  સંરચના Curl_addrinfo *vqualifier canext;
  સંરચના Curl_addrinfo *ca;

  માટે(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

સંરચના Curl_addrinfo *Curl_addrinfo_get(સંરચના Curl_addrinfo *ai,
                                        પૂર્ણાંક ai_family,
                                        unsigned પૂર્ણાંક n)
{
  unsigned પૂર્ણાંક i;
  માટે(i = 0; ai; ai = ai->ai_next) {
    જો(ai->ai_family == ai_family) {
      જો(i == n)
        પાછા ai;
      ++i;
    }
  }
  પાછા ખાલી;
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
પૂર્ણાંક Curl_getaddrinfo_ex(અચલ અક્ષર *nodename,
                        અચલ અક્ષર *servname,
                        અચલ સંરચના addrinfo *hints,
                        સંરચના Curl_addrinfo **result)
{
  અચલ સંરચના addrinfo *ai;
  સંરચના addrinfo *aihead;
  સંરચના Curl_addrinfo *cafirst = ખાલી;
  સંરચના Curl_addrinfo *calast = ખાલી;
  સંરચના Curl_addrinfo *ca;
  size_t ss_size;
  પૂર્ણાંક error;

  *result = ખાલી; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  જો(error)
    પાછા error;

  /* traverse the addrinfo list */

  માટે(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    જો(ai->ai_family == AF_INET)
      ss_size = કદ(સંરચના sockaddr_in);
#ifdef USE_IPV6
    નહીંતર જો(ai->ai_family == AF_INET6)
      ss_size = કદ(સંરચના sockaddr_in6);
#endif
    નહીંતર
      ચાલુ;

    /* ignore elements without required address info */
    જો(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ચાલુ;

    /* ignore elements with bogus address size */
    જો((size_t)ai->ai_addrlen < ss_size)
      ચાલુ;

    ca = curlx_malloc(કદ(સંરચના Curl_addrinfo) + ss_size + namelen);
    જો(!ca) {
      error = EAI_MEMORY;
      તોડો;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ખાલી;
    ca->ai_canonname = ખાલી;
    ca->ai_next      = ખાલી;

    ca->ai_addr = (શૂન્ય *)((અક્ષર *)ca + કદ(સંરચના Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    જો(namelen) {
      ca->ai_canonname = (શૂન્ય *)((અક્ષર *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    જો(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    જો(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  જો(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  જો(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ખાલી;
  }
  નહીંતર જો(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#નહીંતર
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  પાછા error;
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
#જો !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
સંરચના Curl_addrinfo *Curl_he2ai(અચલ સંરચના hostent *he, પૂર્ણાંક port)
{
  સંરચના Curl_addrinfo *ai;
  સંરચના Curl_addrinfo *prevai = ખાલી;
  સંરચના Curl_addrinfo *firstai = ખાલી;
  સંરચના sockaddr_in *addr;
#ifdef USE_IPV6
  સંરચના sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  પૂર્ણાંક i;
  અક્ષર *curr;

  જો(!he)
    /* no input == no output! */
    પાછા ખાલી;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  માટે(i = 0; (curr = he->h_addr_list[i]) != ખાલી; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    જો(he->h_addrtype == AF_INET6)
      ss_size = કદ(સંરચના sockaddr_in6);
    નહીંતર
#endif
      ss_size = કદ(સંરચના sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, કદ(સંરચના Curl_addrinfo) + ss_size + namelen);
    જો(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      તોડો;
    }
    /* put the address after the struct */
    ai->ai_addr = (શૂન્ય *)((અક્ષર *)ai + કદ(સંરચના Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (અક્ષર *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    જો(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    જો(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    પસંદગી(ai->ai_family) {
    સ્થિતિ AF_INET:
      addr = (શૂન્ય *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, કદ(સંરચના in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      તોડો;

#ifdef USE_IPV6
    સ્થિતિ AF_INET6:
      addr6 = (શૂન્ય *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, કદ(સંરચના in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      તોડો;
#endif
    }

    prevai = ai;
  }

  જો(result) {
    Curl_freeaddrinfo(firstai);
    firstai = ખાલી;
  }

  પાછા firstai;
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
સ્થિર CURLcode ip2addr(સંરચના Curl_addrinfo **addrp, પૂર્ણાંક af,
                        અચલ શૂન્ય *inaddr, અચલ અક્ષર *hostname, પૂર્ણાંક port)
{
  સંરચના Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  સંરચના sockaddr_in *addr;
#ifdef USE_IPV6
  સંરચના sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = ખાલી;

  જો(af == AF_INET)
    addrsize = કદ(સંરચના sockaddr_in);
#ifdef USE_IPV6
  નહીંતર જો(af == AF_INET6)
    addrsize = કદ(સંરચના sockaddr_in6);
#endif
  નહીંતર
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, કદ(સંરચના Curl_addrinfo) + addrsize + namelen);
  જો(!ai)
    પાછા CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (શૂન્ય *)((અક્ષર *)ai + કદ(સંરચના Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (અક્ષર *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  પસંદગી(af) {
  સ્થિતિ AF_INET:
    addr = (શૂન્ય *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, કદ(સંરચના in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    તોડો;

#ifdef USE_IPV6
  સ્થિતિ AF_INET6:
    addr6 = (શૂન્ય *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, કદ(સંરચના in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    તોડો;
#endif
  }
  *addrp = ai;
  પાછા CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(અચલ અક્ષર *dotted, uint16_t port,
                       સંરચના Curl_addrinfo **addrp)
{
  સંરચના in_addr in;
  જો(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    પાછા ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    સંરચના in6_addr in6;
    જો(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      પાછા ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  પાછા CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(અચલ અક્ષર *address)
{
  સંરચના in_addr in;
  પાછા (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(અચલ અક્ષર *address)
{
  જો(Curl_is_ipv4addr(address))
    પાછા TRUE;
#ifdef USE_IPV6
  {
    સંરચના in6_addr in6;
    જો(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      પાછા TRUE;
  }
#endif
  પાછા FALSE;
}

bool Curl_looks_like_ipv6(અચલ અક્ષર *s, size_t len, bool maybe_url_encoded,
                          સંરચના Curl_str *host, સંરચના Curl_str *zone)
{
  અચલ અક્ષર *zonep = ખાલી;
  size_t i = 0, hlen = 0, zlen = 0;

  જો(host)
    memset(host, 0, કદ(*host));
  જો(zone)
    memset(zone, 0, કદ(*zone));

  માટે(i = 0; i < len; ++i, ++hlen) {
    જો(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      તોડો;
  }

  જો((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    જો(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    માટે(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      જો(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        તોડો;
    }
  }

  જો(i != len)
    પાછા FALSE; /* invalid chars in zone */
  જો(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  જો(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  પાછા TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(અચલ અક્ષર *path, bool abstract,
                        સંરચના Curl_addrinfo **paddr)
{
  સંરચના Curl_addrinfo *ai;
  સંરચના sockaddr_un *sa_un;
  size_t path_len;

  *paddr = ખાલી;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  જો(path_len > કદ(sa_un->sun_path))
    પાછા CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    કદ(સંરચના Curl_addrinfo) + કદ(સંરચના sockaddr_un));
  જો(!ai)
    પાછા CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (શૂન્ય *)((અક્ષર *)ai + કદ(સંરચના Curl_addrinfo));
  sa_un = (શૂન્ય *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(સંરચના sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  જો(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  નહીંતર
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  પાછા CURLE_OK;
}
#endif

#જો defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
શૂન્ય curl_dbg_freeaddrinfo(સંરચના addrinfo *freethis,
                           પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, લીટી, (શૂન્ય *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#અથવાજો defined(USE_FAKE_GETADDRINFO)
  {
    અચલ અક્ષર *env = getenv("CURL_DNS_SERVER");
    જો(env)
      r_freeaddrinfo(freethis);
    નહીંતર
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#નહીંતર
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#જો defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
પૂર્ણાંક curl_dbg_getaddrinfo(અચલ અક્ષર *hostname,
                         અચલ અક્ષર *service,
                         અચલ સંરચના addrinfo *hints,
                         સંરચના addrinfo **result,
                         પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
#ifdef USE_LWIPSOCK
  પૂર્ણાંક res = lwip_getaddrinfo(hostname, service, hints, result);
#અથવાજો defined(USE_FAKE_GETADDRINFO)
  પૂર્ણાંક res;
  અચલ અક્ષર *env = getenv("CURL_DNS_SERVER");
  જો(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  નહીંતર
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#નહીંતર
  /* !checksrc! disable BANNEDFUNC 1 */
  પૂર્ણાંક res = getaddrinfo(hostname, service, hints, result);
#endif
  જો(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, લીટી,
                 (શૂન્ય *)*result);
  નહીંતર
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, લીટી);
  પાછા res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#જો defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
શૂન્ય Curl_addrinfo_set_port(સંરચના Curl_addrinfo *addrinfo, પૂર્ણાંક port)
{
  સંરચના Curl_addrinfo *ca;
  સંરચના sockaddr_in *addr;
#ifdef USE_IPV6
  સંરચના sockaddr_in6 *addr6;
#endif
  માટે(ca = addrinfo; ca; ca = ca->ai_next) {
    પસંદગી(ca->ai_family) {
    સ્થિતિ AF_INET:
      addr = (શૂન્ય *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      તોડો;

#ifdef USE_IPV6
    સ્થિતિ AF_INET6:
      addr6 = (શૂન્ય *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      તોડો;
#endif
    }
  }
}
#endif
