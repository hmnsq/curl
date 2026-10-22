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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETINET_IN6_H
#अन्तर्भाव <netinet/in6.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_UN_H
#अन्तर्भाव <sys/un.h>
#यदि_अन्तः

#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

#अन्तर्भाव <stddef.h>  /* for offsetof() */

#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "fake_addrinfo.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#यदि defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier चञ्चल
#अन्यथा
#  define vqualifier
#यदि_अन्तः

शून्य Curl_freeaddrinfo(संरचना Curl_addrinfo *cahead)
{
  संरचना Curl_addrinfo *vqualifier canext;
  संरचना Curl_addrinfo *ca;

  प्रति(ca = cahead; ca; ca = canext) {
    canext = ca->ai_अग्र;
    curlx_free(ca);
  }
}

संरचना Curl_addrinfo *Curl_addrinfo_get(संरचना Curl_addrinfo *ai,
                                        पूर्णाङ्क ai_family,
                                        अचिह्नित पूर्णाङ्क n)
{
  अचिह्नित पूर्णाङ्क i;
  प्रति(i = 0; ai; ai = ai->ai_अग्र) {
    यदि(ai->ai_family == ai_family) {
      यदि(i == n)
        प्रत्यागम ai;
      ++i;
    }
  }
  प्रत्यागम रिक्त;
}

#यदि_परिभाषितम् HAVE_GETADDRINFO
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
पूर्णाङ्क Curl_getaddrinfo_ex(ध्रुव वर्ण *nodename,
                        ध्रुव वर्ण *servname,
                        ध्रुव संरचना addrinfo *hints,
                        संरचना Curl_addrinfo **परिणाम)
{
  ध्रुव संरचना addrinfo *ai;
  संरचना addrinfo *aihead;
  संरचना Curl_addrinfo *cafirst = रिक्त;
  संरचना Curl_addrinfo *calast = रिक्त;
  संरचना Curl_addrinfo *ca;
  size_t ss_आकार;
  पूर्णाङ्क error;

  *परिणाम = रिक्त; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  यदि(error)
    प्रत्यागम error;

  /* traverse the addrinfo list */

  प्रति(ai = aihead; ai; ai = ai->ai_अग्र) {
    size_t namelen = ai->ai_canonname ? वाक्यदैर्घ्यम्(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    यदि(ai->ai_family == AF_INET)
      ss_आकार = परिमाण(संरचना sockaddr_in);
#यदि_परिभाषितम् USE_IPV6
    अन्यथा यदि(ai->ai_family == AF_INET6)
      ss_आकार = परिमाण(संरचना sockaddr_in6);
#यदि_अन्तः
    अन्यथा
      अनुवर्त;

    /* ignore elements without required address info */
    यदि(!ai->ai_addr || !(ai->ai_addrlen > 0))
      अनुवर्त;

    /* ignore elements with bogus address size */
    यदि((size_t)ai->ai_addrlen < ss_आकार)
      अनुवर्त;

    ca = curlx_malloc(परिमाण(संरचना Curl_addrinfo) + ss_आकार + namelen);
    यदि(!ca) {
      error = EAI_MEMORY;
      भङ्ग;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_आकार;
    ca->ai_addr      = रिक्त;
    ca->ai_canonname = रिक्त;
    ca->ai_अग्र      = रिक्त;

    ca->ai_addr = (शून्य *)((वर्ण *)ca + परिमाण(संरचना Curl_addrinfo));
    स्मृतिं_प्रतिलिख(ca->ai_addr, ai->ai_addr, ss_आकार);

    यदि(namelen) {
      ca->ai_canonname = (शून्य *)((वर्ण *)ca->ai_addr + ss_आकार);
      स्मृतिं_प्रतिलिख(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    यदि(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    यदि(calast)
      calast->ai_अग्र = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  यदि(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  यदि(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = रिक्त;
  }
  अन्यथा यदि(!cafirst) {
#यदि_परिभाषितम् EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#अन्यथा
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#यदि_अन्तः
#यदि_परिभाषितम् USE_WINSOCK
    समुच्चयः_SOCKERRNO(error);
#यदि_अन्तः
  }

  *परिणाम = cafirst;

  /* This is not a CURLcode */
  प्रत्यागम error;
}
#यदि_अन्तः /* HAVE_GETADDRINFO */

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
#यदि !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
संरचना Curl_addrinfo *Curl_he2ai(ध्रुव संरचना hostent *he, पूर्णाङ्क पत्तनम्)
{
  संरचना Curl_addrinfo *ai;
  संरचना Curl_addrinfo *prevai = रिक्त;
  संरचना Curl_addrinfo *firstai = रिक्त;
  संरचना sockaddr_in *addr;
#यदि_परिभाषितम् USE_IPV6
  संरचना sockaddr_in6 *addr6;
#यदि_अन्तः
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क i;
  वर्ण *curr;

  यदि(!he)
    /* no input == no output! */
    प्रत्यागम रिक्त;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  प्रति(i = 0; (curr = he->h_addr_list[i]) != रिक्त; i++) {
    size_t ss_आकार;
    size_t namelen = वाक्यदैर्घ्यम्(he->h_name) + 1; /* include null-terminator */
#यदि_परिभाषितम् USE_IPV6
    यदि(he->h_addrtype == AF_INET6)
      ss_आकार = परिमाण(संरचना sockaddr_in6);
    अन्यथा
#यदि_अन्तः
      ss_आकार = परिमाण(संरचना sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, परिमाण(संरचना Curl_addrinfo) + ss_आकार + namelen);
    यदि(!ai) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      भङ्ग;
    }
    /* put the address after the struct */
    ai->ai_addr = (शून्य *)((वर्ण *)ai + परिमाण(संरचना Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (वर्ण *)ai->ai_addr + ss_आकार;
    स्मृतिं_प्रतिलिख(ai->ai_canonname, he->h_name, namelen);

    यदि(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    यदि(prevai)
      /* make the previous entry point to this */
      prevai->ai_अग्र = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_आकार;

    /* leave the rest of the struct filled with zero */

    विकल्प(ai->ai_family) {
    पक्ष AF_INET:
      addr = (शून्य *)ai->ai_addr; /* storage area for this info */

      स्मृतिं_प्रतिलिख(&addr->sin_addr, curr, परिमाण(संरचना in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((अचिह्नित ह्रस्व)पत्तनम्);
      भङ्ग;

#यदि_परिभाषितम् USE_IPV6
    पक्ष AF_INET6:
      addr6 = (शून्य *)ai->ai_addr; /* storage area for this info */

      स्मृतिं_प्रतिलिख(&addr6->sin6_addr, curr, परिमाण(संरचना in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((अचिह्नित ह्रस्व)पत्तनम्);
      भङ्ग;
#यदि_अन्तः
    }

    prevai = ai;
  }

  यदि(परिणाम) {
    Curl_freeaddrinfo(firstai);
    firstai = रिक्त;
  }

  प्रत्यागम firstai;
}
#यदि_अन्तः

/*
 * ip2addr()
 *
 * This function takes an Internet address, in binary form, as input parameter
 * along with its address family and the string version of the address, and it
 * returns a Curl_addrinfo chain filled in correctly with information for the
 * given address/host
 */
स्थिर CURLcode ip2addr(संरचना Curl_addrinfo **addrp, पूर्णाङ्क af,
                        ध्रुव शून्य *inaddr, ध्रुव वर्ण *hostname, पूर्णाङ्क पत्तनम्)
{
  संरचना Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  संरचना sockaddr_in *addr;
#यदि_परिभाषितम् USE_IPV6
  संरचना sockaddr_in6 *addr6;
#यदि_अन्तः

  DEBUGASSERT(inaddr && hostname);

  namelen = वाक्यदैर्घ्यम्(hostname) + 1;
  *addrp = रिक्त;

  यदि(af == AF_INET)
    addrsize = परिमाण(संरचना sockaddr_in);
#यदि_परिभाषितम् USE_IPV6
  अन्यथा यदि(af == AF_INET6)
    addrsize = परिमाण(संरचना sockaddr_in6);
#यदि_अन्तः
  अन्यथा
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, परिमाण(संरचना Curl_addrinfo) + addrsize + namelen);
  यदि(!ai)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  /* put the address after the struct */
  ai->ai_addr = (शून्य *)((वर्ण *)ai + परिमाण(संरचना Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (वर्ण *)ai->ai_addr + addrsize;
  स्मृतिं_प्रतिलिख(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  विकल्प(af) {
  पक्ष AF_INET:
    addr = (शून्य *)ai->ai_addr; /* storage area for this info */

    स्मृतिं_प्रतिलिख(&addr->sin_addr, inaddr, परिमाण(संरचना in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((अचिह्नित ह्रस्व)पत्तनम्);
    भङ्ग;

#यदि_परिभाषितम् USE_IPV6
  पक्ष AF_INET6:
    addr6 = (शून्य *)ai->ai_addr; /* storage area for this info */

    स्मृतिं_प्रतिलिख(&addr6->sin6_addr, inaddr, परिमाण(संरचना in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((अचिह्नित ह्रस्व)पत्तनम्);
    भङ्ग;
#यदि_अन्तः
  }
  *addrp = ai;
  प्रत्यागम CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ध्रुव वर्ण *dotted, uint16_t पत्तनम्,
                       संरचना Curl_addrinfo **addrp)
{
  संरचना in_addr अन्तर्;
  यदि(curlx_inet_pton(AF_INET, dotted, &अन्तर्) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    प्रत्यागम ip2addr(addrp, AF_INET, &अन्तर्, dotted, पत्तनम्);
#यदि_परिभाषितम् USE_IPV6
  {
    संरचना in6_addr in6;
    यदि(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      प्रत्यागम ip2addr(addrp, AF_INET6, &in6, dotted, पत्तनम्);
  }
#यदि_अन्तः
  प्रत्यागम CURLE_BAD_FUNCTION_तर्कः; /* bad input format */
}

bool Curl_is_ipv4addr(ध्रुव वर्ण *स्थानाङ्कः)
{
  संरचना in_addr अन्तर्;
  प्रत्यागम (curlx_inet_pton(AF_INET, स्थानाङ्कः, &अन्तर्) > 0);
}

bool Curl_is_ipaddr(ध्रुव वर्ण *स्थानाङ्कः)
{
  यदि(Curl_is_ipv4addr(स्थानाङ्कः))
    प्रत्यागम TRUE;
#यदि_परिभाषितम् USE_IPV6
  {
    संरचना in6_addr in6;
    यदि(curlx_inet_pton(AF_INET6, स्थानाङ्कः, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      प्रत्यागम TRUE;
  }
#यदि_अन्तः
  प्रत्यागम FALSE;
}

bool Curl_looks_like_ipv6(ध्रुव वर्ण *s, size_t len, bool maybe_url_encoded,
                          संरचना Curl_str *host, संरचना Curl_str *zone)
{
  ध्रुव वर्ण *zonep = रिक्त;
  size_t i = 0, hlen = 0, zlen = 0;

  यदि(host)
    स्मृतिं_पूरय(host, 0, परिमाण(*host));
  यदि(zone)
    स्मृतिं_पूरय(zone, 0, परिमाण(*zone));

  प्रति(i = 0; i < len; ++i, ++hlen) {
    यदि(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      भङ्ग;
  }

  यदि((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    यदि(maybe_url_encoded && !वाक्ये_तुलय("25", s + i, 2))
      i += 2;
    zonep = s + i;
    प्रति(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      यदि(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        भङ्ग;
    }
  }

  यदि(i != len)
    प्रत्यागम FALSE; /* invalid chars in zone */
  यदि(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  यदि(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  प्रत्यागम TRUE;
}

#यदि_परिभाषितम् USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ध्रुव वर्ण *मार्गः, bool abstract,
                        संरचना Curl_addrinfo **paddr)
{
  संरचना Curl_addrinfo *ai;
  संरचना sockaddr_un *sa_un;
  size_t मार्गः_len;

  *paddr = रिक्त;

  /* sun_path must be able to store the null-terminated path */
  मार्गः_len = वाक्यदैर्घ्यम्(मार्गः) + 1;
  यदि(मार्गः_len > परिमाण(sa_un->sun_मार्गः))
    प्रत्यागम CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    परिमाण(संरचना Curl_addrinfo) + परिमाण(संरचना sockaddr_un));
  यदि(!ai)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  ai->ai_addr = (शून्य *)((वर्ण *)ai + परिमाण(संरचना Curl_addrinfo));
  sa_un = (शून्य *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(संरचना sockaddr_un, sun_मार्गः) + मार्गः_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  यदि(abstract)
    स्मृतिं_प्रतिलिख(sa_un->sun_मार्गः + 1, मार्गः, मार्गः_len - 1);
  अन्यथा
    स्मृतिं_प्रतिलिख(sa_un->sun_मार्गः, मार्गः, मार्गः_len); /* copy NUL byte */

  *paddr = ai;
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः

#यदि defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
शून्य curl_dbg_freeaddrinfo(संरचना addrinfo *freethis,
                           पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, पङ्क्तिः, (शून्य *)freethis);
#यदि_परिभाषितम् USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#नोचेद्यदि defined(USE_FAKE_GETADDRINFO)
  {
    ध्रुव वर्ण *env = getenv("CURL_DNS_SERVER");
    यदि(env)
      r_freeaddrinfo(freethis);
    अन्यथा
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#अन्यथा
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#यदि_अन्तः
}
#यदि_अन्तः /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#यदि defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
पूर्णाङ्क curl_dbg_getaddrinfo(ध्रुव वर्ण *hostname,
                         ध्रुव वर्ण *service,
                         ध्रुव संरचना addrinfo *hints,
                         संरचना addrinfo **परिणाम,
                         पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source)
{
#यदि_परिभाषितम् USE_LWIPSOCK
  पूर्णाङ्क res = lwip_getaddrinfo(hostname, service, hints, परिणाम);
#नोचेद्यदि defined(USE_FAKE_GETADDRINFO)
  पूर्णाङ्क res;
  ध्रुव वर्ण *env = getenv("CURL_DNS_SERVER");
  यदि(env)
    res = r_getaddrinfo(hostname, service, hints, परिणाम);
  अन्यथा
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, परिणाम);
#अन्यथा
  /* !checksrc! disable BANNEDFUNC 1 */
  पूर्णाङ्क res = getaddrinfo(hostname, service, hints, परिणाम);
#यदि_अन्तः
  यदि(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, पङ्क्तिः,
                 (शून्य *)*परिणाम);
  अन्यथा
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, पङ्क्तिः);
  प्रत्यागम res;
}
#यदि_अन्तः /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#यदि defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
शून्य Curl_addrinfo_समुच्चयः_पत्तनम्(संरचना Curl_addrinfo *addrinfo, पूर्णाङ्क पत्तनम्)
{
  संरचना Curl_addrinfo *ca;
  संरचना sockaddr_in *addr;
#यदि_परिभाषितम् USE_IPV6
  संरचना sockaddr_in6 *addr6;
#यदि_अन्तः
  प्रति(ca = addrinfo; ca; ca = ca->ai_अग्र) {
    विकल्प(ca->ai_family) {
    पक्ष AF_INET:
      addr = (शून्य *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((अचिह्नित ह्रस्व)पत्तनम्);
      भङ्ग;

#यदि_परिभाषितम् USE_IPV6
    पक्ष AF_INET6:
      addr6 = (शून्य *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((अचिह्नित ह्रस्व)पत्तनम्);
      भङ्ग;
#यदि_अन्तः
    }
  }
}
#यदि_अन्तः
