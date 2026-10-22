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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETINET_IN6_H
#समावेश <netinet/in6.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_UN_H
#समावेश <sys/un.h>
#पूर्ण_अगर

#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश <stddef.h>  /* for offsetof() */

#समावेश "curl_addrinfo.h"
#समावेश "fake_addrinfo.h"
#समावेश "curlx/inet_pton.h"
#समावेश "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#अगर defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier अस्थिर
#अन्यथा
#  define vqualifier
#पूर्ण_अगर

व्योम Curl_freeaddrinfo(काष्ठा Curl_addrinfo *cahead)
{
  काष्ठा Curl_addrinfo *vqualifier canext;
  काष्ठा Curl_addrinfo *ca;

  क्रम(ca = cahead; ca; ca = canext) {
    canext = ca->ai_अग्र;
    curlx_free(ca);
  }
}

काष्ठा Curl_addrinfo *Curl_addrinfo_get(काष्ठा Curl_addrinfo *ai,
                                        पूर्णांक ai_family,
                                        अचिन्हित पूर्णांक n)
{
  अचिन्हित पूर्णांक i;
  क्रम(i = 0; ai; ai = ai->ai_अग्र) {
    अगर(ai->ai_family == ai_family) {
      अगर(i == n)
        वापस ai;
      ++i;
    }
  }
  वापस शून्य;
}

#अगर_घोषित HAVE_GETADDRINFO
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
पूर्णांक Curl_getaddrinfo_ex(स्थिर अक्षर *nodename,
                        स्थिर अक्षर *servname,
                        स्थिर काष्ठा addrinfo *hints,
                        काष्ठा Curl_addrinfo **परिणाम)
{
  स्थिर काष्ठा addrinfo *ai;
  काष्ठा addrinfo *aihead;
  काष्ठा Curl_addrinfo *cafirst = शून्य;
  काष्ठा Curl_addrinfo *calast = शून्य;
  काष्ठा Curl_addrinfo *ca;
  माप_प्रकार ss_आकार;
  पूर्णांक error;

  *परिणाम = शून्य; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  अगर(error)
    वापस error;

  /* traverse the addrinfo list */

  क्रम(ai = aihead; ai; ai = ai->ai_अग्र) {
    माप_प्रकार namelen = ai->ai_canonname ? म_माप(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    अगर(ai->ai_family == AF_INET)
      ss_आकार = माप(काष्ठा sockaddr_in);
#अगर_घोषित USE_IPV6
    अन्यथा अगर(ai->ai_family == AF_INET6)
      ss_आकार = माप(काष्ठा sockaddr_in6);
#पूर्ण_अगर
    अन्यथा
      जारी;

    /* ignore elements without required address info */
    अगर(!ai->ai_addr || !(ai->ai_addrlen > 0))
      जारी;

    /* ignore elements with bogus address size */
    अगर((माप_प्रकार)ai->ai_addrlen < ss_आकार)
      जारी;

    ca = curlx_malloc(माप(काष्ठा Curl_addrinfo) + ss_आकार + namelen);
    अगर(!ca) {
      error = EAI_MEMORY;
      अवरोध;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_आकार;
    ca->ai_addr      = शून्य;
    ca->ai_canonname = शून्य;
    ca->ai_अग्र      = शून्य;

    ca->ai_addr = (व्योम *)((अक्षर *)ca + माप(काष्ठा Curl_addrinfo));
    स_नकल(ca->ai_addr, ai->ai_addr, ss_आकार);

    अगर(namelen) {
      ca->ai_canonname = (व्योम *)((अक्षर *)ca->ai_addr + ss_आकार);
      स_नकल(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    अगर(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    अगर(calast)
      calast->ai_अग्र = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  अगर(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  अगर(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = शून्य;
  }
  अन्यथा अगर(!cafirst) {
#अगर_घोषित EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#अन्यथा
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#पूर्ण_अगर
#अगर_घोषित USE_WINSOCK
    SET_SOCKERRNO(error);
#पूर्ण_अगर
  }

  *परिणाम = cafirst;

  /* This is not a CURLcode */
  वापस error;
}
#पूर्ण_अगर /* HAVE_GETADDRINFO */

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
#अगर !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
काष्ठा Curl_addrinfo *Curl_he2ai(स्थिर काष्ठा hostent *he, पूर्णांक port)
{
  काष्ठा Curl_addrinfo *ai;
  काष्ठा Curl_addrinfo *prevai = शून्य;
  काष्ठा Curl_addrinfo *firstai = शून्य;
  काष्ठा sockaddr_in *addr;
#अगर_घोषित USE_IPV6
  काष्ठा sockaddr_in6 *addr6;
#पूर्ण_अगर
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक i;
  अक्षर *curr;

  अगर(!he)
    /* no input == no output! */
    वापस शून्य;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  क्रम(i = 0; (curr = he->h_addr_list[i]) != शून्य; i++) {
    माप_प्रकार ss_आकार;
    माप_प्रकार namelen = म_माप(he->h_name) + 1; /* include null-terminator */
#अगर_घोषित USE_IPV6
    अगर(he->h_addrtype == AF_INET6)
      ss_आकार = माप(काष्ठा sockaddr_in6);
    अन्यथा
#पूर्ण_अगर
      ss_आकार = माप(काष्ठा sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, माप(काष्ठा Curl_addrinfo) + ss_आकार + namelen);
    अगर(!ai) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      अवरोध;
    }
    /* put the address after the struct */
    ai->ai_addr = (व्योम *)((अक्षर *)ai + माप(काष्ठा Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (अक्षर *)ai->ai_addr + ss_आकार;
    स_नकल(ai->ai_canonname, he->h_name, namelen);

    अगर(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    अगर(prevai)
      /* make the previous entry point to this */
      prevai->ai_अग्र = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_आकार;

    /* leave the rest of the struct filled with zero */

    चयन(ai->ai_family) {
    हाल AF_INET:
      addr = (व्योम *)ai->ai_addr; /* storage area for this info */

      स_नकल(&addr->sin_addr, curr, माप(काष्ठा in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((अचिन्हित लघु)port);
      अवरोध;

#अगर_घोषित USE_IPV6
    हाल AF_INET6:
      addr6 = (व्योम *)ai->ai_addr; /* storage area for this info */

      स_नकल(&addr6->sin6_addr, curr, माप(काष्ठा in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((अचिन्हित लघु)port);
      अवरोध;
#पूर्ण_अगर
    }

    prevai = ai;
  }

  अगर(परिणाम) {
    Curl_freeaddrinfo(firstai);
    firstai = शून्य;
  }

  वापस firstai;
}
#पूर्ण_अगर

/*
 * ip2addr()
 *
 * This function takes an Internet address, in binary form, as input parameter
 * along with its address family and the string version of the address, and it
 * returns a Curl_addrinfo chain filled in correctly with information for the
 * given address/host
 */
अटल CURLcode ip2addr(काष्ठा Curl_addrinfo **addrp, पूर्णांक af,
                        स्थिर व्योम *inaddr, स्थिर अक्षर *hostname, पूर्णांक port)
{
  काष्ठा Curl_addrinfo *ai;
  माप_प्रकार addrsize;
  माप_प्रकार namelen;
  काष्ठा sockaddr_in *addr;
#अगर_घोषित USE_IPV6
  काष्ठा sockaddr_in6 *addr6;
#पूर्ण_अगर

  DEBUGASSERT(inaddr && hostname);

  namelen = म_माप(hostname) + 1;
  *addrp = शून्य;

  अगर(af == AF_INET)
    addrsize = माप(काष्ठा sockaddr_in);
#अगर_घोषित USE_IPV6
  अन्यथा अगर(af == AF_INET6)
    addrsize = माप(काष्ठा sockaddr_in6);
#पूर्ण_अगर
  अन्यथा
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, माप(काष्ठा Curl_addrinfo) + addrsize + namelen);
  अगर(!ai)
    वापस CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (व्योम *)((अक्षर *)ai + माप(काष्ठा Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (अक्षर *)ai->ai_addr + addrsize;
  स_नकल(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  चयन(af) {
  हाल AF_INET:
    addr = (व्योम *)ai->ai_addr; /* storage area for this info */

    स_नकल(&addr->sin_addr, inaddr, माप(काष्ठा in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((अचिन्हित लघु)port);
    अवरोध;

#अगर_घोषित USE_IPV6
  हाल AF_INET6:
    addr6 = (व्योम *)ai->ai_addr; /* storage area for this info */

    स_नकल(&addr6->sin6_addr, inaddr, माप(काष्ठा in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((अचिन्हित लघु)port);
    अवरोध;
#पूर्ण_अगर
  }
  *addrp = ai;
  वापस CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(स्थिर अक्षर *dotted, uint16_t port,
                       काष्ठा Curl_addrinfo **addrp)
{
  काष्ठा in_addr in;
  अगर(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    वापस ip2addr(addrp, AF_INET, &in, dotted, port);
#अगर_घोषित USE_IPV6
  {
    काष्ठा in6_addr in6;
    अगर(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      वापस ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#पूर्ण_अगर
  वापस CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(स्थिर अक्षर *address)
{
  काष्ठा in_addr in;
  वापस (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(स्थिर अक्षर *address)
{
  अगर(Curl_is_ipv4addr(address))
    वापस TRUE;
#अगर_घोषित USE_IPV6
  {
    काष्ठा in6_addr in6;
    अगर(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      वापस TRUE;
  }
#पूर्ण_अगर
  वापस FALSE;
}

bool Curl_looks_like_ipv6(स्थिर अक्षर *s, माप_प्रकार len, bool maybe_url_encoded,
                          काष्ठा Curl_str *host, काष्ठा Curl_str *zone)
{
  स्थिर अक्षर *zonep = शून्य;
  माप_प्रकार i = 0, hlen = 0, zlen = 0;

  अगर(host)
    स_रखो(host, 0, माप(*host));
  अगर(zone)
    स_रखो(zone, 0, माप(*zone));

  क्रम(i = 0; i < len; ++i, ++hlen) {
    अगर(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      अवरोध;
  }

  अगर((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    अगर(maybe_url_encoded && !म_भेदन("25", s + i, 2))
      i += 2;
    zonep = s + i;
    क्रम(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      अगर(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        अवरोध;
    }
  }

  अगर(i != len)
    वापस FALSE; /* invalid chars in zone */
  अगर(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  अगर(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  वापस TRUE;
}

#अगर_घोषित USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(स्थिर अक्षर *path, bool abstract,
                        काष्ठा Curl_addrinfo **paddr)
{
  काष्ठा Curl_addrinfo *ai;
  काष्ठा sockaddr_un *sa_un;
  माप_प्रकार path_len;

  *paddr = शून्य;

  /* sun_path must be able to store the null-terminated path */
  path_len = म_माप(path) + 1;
  अगर(path_len > माप(sa_un->sun_path))
    वापस CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    माप(काष्ठा Curl_addrinfo) + माप(काष्ठा sockaddr_un));
  अगर(!ai)
    वापस CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (व्योम *)((अक्षर *)ai + माप(काष्ठा Curl_addrinfo));
  sa_un = (व्योम *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((दुरत्व(काष्ठा sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  अगर(abstract)
    स_नकल(sa_un->sun_path + 1, path, path_len - 1);
  अन्यथा
    स_नकल(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  वापस CURLE_OK;
}
#पूर्ण_अगर

#अगर defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
व्योम curl_dbg_freeaddrinfo(काष्ठा addrinfo *freethis,
                           पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, पंक्ति, (व्योम *)freethis);
#अगर_घोषित USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#या_अगर defined(USE_FAKE_GETADDRINFO)
  {
    स्थिर अक्षर *env = दो_पर्या("CURL_DNS_SERVER");
    अगर(env)
      r_freeaddrinfo(freethis);
    अन्यथा
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#अन्यथा
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#पूर्ण_अगर
}
#पूर्ण_अगर /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#अगर defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
पूर्णांक curl_dbg_getaddrinfo(स्थिर अक्षर *hostname,
                         स्थिर अक्षर *service,
                         स्थिर काष्ठा addrinfo *hints,
                         काष्ठा addrinfo **परिणाम,
                         पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
#अगर_घोषित USE_LWIPSOCK
  पूर्णांक res = lwip_getaddrinfo(hostname, service, hints, परिणाम);
#या_अगर defined(USE_FAKE_GETADDRINFO)
  पूर्णांक res;
  स्थिर अक्षर *env = दो_पर्या("CURL_DNS_SERVER");
  अगर(env)
    res = r_getaddrinfo(hostname, service, hints, परिणाम);
  अन्यथा
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, परिणाम);
#अन्यथा
  /* !checksrc! disable BANNEDFUNC 1 */
  पूर्णांक res = getaddrinfo(hostname, service, hints, परिणाम);
#पूर्ण_अगर
  अगर(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, पंक्ति,
                 (व्योम *)*परिणाम);
  अन्यथा
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, पंक्ति);
  वापस res;
}
#पूर्ण_अगर /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#अगर defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
व्योम Curl_addrinfo_set_port(काष्ठा Curl_addrinfo *addrinfo, पूर्णांक port)
{
  काष्ठा Curl_addrinfo *ca;
  काष्ठा sockaddr_in *addr;
#अगर_घोषित USE_IPV6
  काष्ठा sockaddr_in6 *addr6;
#पूर्ण_अगर
  क्रम(ca = addrinfo; ca; ca = ca->ai_अग्र) {
    चयन(ca->ai_family) {
    हाल AF_INET:
      addr = (व्योम *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((अचिन्हित लघु)port);
      अवरोध;

#अगर_घोषित USE_IPV6
    हाल AF_INET6:
      addr6 = (व्योम *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((अचिन्हित लघु)port);
      अवरोध;
#पूर्ण_अगर
    }
  }
}
#पूर्ण_अगर
