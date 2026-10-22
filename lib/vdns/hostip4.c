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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

/***********************************************************************
 * Only for plain IPv4 builds
 **********************************************************************/
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_IPV4 /* plain IPv4 code coming up */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_SYNCH

/*
 * Curl_sync_getaddrinfo() - the IPv4 synchronous version.
 *
 * The original code to this function was from the Dancer source code, written
 * by Bjorn Reese, it has since been patched and modified considerably.
 *
 * gethostbyname_r() is the thread-safe version of the gethostbyname()
 * function. When we build for plain IPv4, we attempt to use this
 * function. There are _three_ different gethostbyname_r() versions, and we
 * detect which one this platform supports in the configure script and set up
 * the HAVE_GETHOSTBYNAME_R_3, HAVE_GETHOSTBYNAME_R_5 or
 * HAVE_GETHOSTBYNAME_R_6 defines accordingly. Note that HAVE_GETADDRBYNAME
 * has the corresponding rules. This is primarily on *nix. Note that some Unix
 * flavors have thread-safe versions of the plain gethostbyname() etc.
 *
 */
ਬਣਤਰ Curl_addrinfo *Curl_sync_getaddrinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            uint8_t dns_queries,
                                            ਅਚਲ ਅੱਖਰ *hostname,
                                            uint16_t ਪੋਰਟ,
                                            uint8_t transport)
{
  ਬਣਤਰ Curl_addrinfo *ai = ਨਿਰਾ;

  (ਖਾਲੀ)dns_queries;
  (ਖਾਲੀ)transport;

  ai = Curl_ipv4_resolve_r(hostname, ਪੋਰਟ);
  ਜੇ(!ai)
    infof(ਅੰਕੜੇ, "Curl_ipv4_resolve_r failed for %s", hostname);

  ਵਾਪਸ ai;
}
#ਜੇ_ਅੰਤ /* CURLRES_SYNCH */
#ਜੇ_ਅੰਤ /* CURLRES_IPV4 */

#ਜੇ defined(CURLRES_IPV4) && !defined(USE_RESOLV_ARES) && \
  !defined(CURLRES_AMIGA)

/*
 * Curl_ipv4_resolve_r() - IPv4 thread-safe resolver function.
 *
 * This is used for both synchronous and asynchronous resolver builds,
 * implying that only thread-safe code and function calls may be used.
 *
 */
ਬਣਤਰ Curl_addrinfo *Curl_ipv4_resolve_r(ਅਚਲ ਅੱਖਰ *hostname,
                                          uint16_t ਪੋਰਟ)
{
#ਜੇ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE)) && \
  defined(HAVE_GETHOSTBYNAME_R_3)
  ਪੂਰਨ res;
#ਜੇ_ਅੰਤ
  ਬਣਤਰ Curl_addrinfo *ai = ਨਿਰਾ;
#ਜੇ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
  ਬਣਤਰ hostent *h = ਨਿਰਾ;
  ਬਣਤਰ hostent *buf = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE)
  ਬਣਤਰ addrinfo hints;
  ਅੱਖਰ sbuf[12];
  ਅੱਖਰ *sbufptr = ਨਿਰਾ;

  memset(&hints, 0, ਆਕਾਰ(hints));
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  ਜੇ(ਪੋਰਟ) {
    curl_msnprintf(sbuf, ਆਕਾਰ(sbuf), "%d", ਪੋਰਟ);
    sbufptr = sbuf;
  }

  (ਖਾਲੀ)Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &ai);

#ਜਾਂਜੇ defined(HAVE_GETHOSTBYNAME_R)
  /*
   * gethostbyname_r() is the preferred resolve function for many platforms.
   * Since there are three different versions of it, the following code is
   * somewhat #ifdef-ridden.
   */
  ਪੂਰਨ h_errnop;

  buf = curlx_calloc(1, CURL_HOSTENT_ਮਾਪ);
  ਜੇ(!buf)
    ਵਾਪਸ ਨਿਰਾ; /* major failure */
  /*
   * The clearing of the buffer is a workaround for a gethostbyname_r bug in
   * QNX Neutrino and it is also _required_ for some of these functions on some
   * platforms.
   */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETHOSTBYNAME_R_5
  /* Solaris, IRIX and more */
  h = gethostbyname_r(hostname,
                      (ਬਣਤਰ hostent *)buf,
                      (ਅੱਖਰ *)buf + ਆਕਾਰ(ਬਣਤਰ hostent),
                      CURL_HOSTENT_ਮਾਪ - ਆਕਾਰ(ਬਣਤਰ hostent),
                      &h_errnop);

  /* If the buffer is too small, it returns NULL and sets errno to
   * ERANGE. The errno is thread-safe if this is compiled with
   * -D_REENTRANT as then the 'errno' variable is a macro defined to get
   * used properly for threads.
   */

  ਜੇ(h) {
    ;
  }
  ਨਹੀਂਤਾਂ
#ਜਾਂਜੇ defined(HAVE_GETHOSTBYNAME_R_6)
  /* Linux */

  (ਖਾਲੀ)gethostbyname_r(hostname,
                        (ਬਣਤਰ hostent *)buf,
                        (ਅੱਖਰ *)buf + ਆਕਾਰ(ਬਣਤਰ hostent),
                        CURL_HOSTENT_ਮਾਪ - ਆਕਾਰ(ਬਣਤਰ hostent),
                        &h, /* DIFFERENCE */
                        &h_errnop);
  /* Redhat 8, using glibc 2.2.93 changed the behavior. Now all of a
   * sudden this function returns EAGAIN if the given buffer size is too
   * small. Previous versions are known to return ERANGE for the same
   * problem.
   *
   * This would not be such a big problem if older versions would not
   * sometimes return EAGAIN on a common failure case. Alas, we cannot
   * assume that EAGAIN *or* ERANGE means ERANGE for any given version of
   * glibc.
   *
   * For now, we do that and thus we may call the function repeatedly and
   * fail for older glibc versions that return EAGAIN, until we run out of
   * buffer size (step_size grows beyond CURL_HOSTENT_SIZE).
   *
   * If anyone has a better fix, please tell us!
   *
   * -------------------------------------------------------------------
   *
   * On October 23rd 2003, Dan C dug up more details on the mysteries of
   * gethostbyname_r() in glibc:
   *
   * In glibc 2.2.5 the interface is different (this has also been
   * discovered in glibc 2.1.1-6 as shipped by Redhat 6). What I cannot
   * explain, is that tests performed on glibc 2.2.4-34 and 2.2.4-32
   * (shipped/upgraded by Redhat 7.2) do not show this behavior!
   *
   * In this "buggy" version, the return code is -1 on error and 'errno'
   * is set to the ERANGE or EAGAIN code. Note that 'errno' is not a
   * thread-safe variable.
   */

  ਜੇ(!h) /* failure */
#ਜਾਂਜੇ defined(HAVE_GETHOSTBYNAME_R_3)
  /* AIX, Digital UNIX/Tru64, HP-UX 10, more? */

  /* For AIX 4.3 or later, we do not use gethostbyname_r() at all, because of
   * the plain fact that it does not return unique full buffers on each
   * call, but instead several of the pointers in the hostent structs will
   * point to the same actual data! This have the unfortunate down-side that
   * our caching system breaks down horribly. Luckily for us though, AIX 4.3
   * and more recent versions have a "completely thread-safe"[*] libc where
   * all the data is stored in thread-specific memory areas making calls to
   * the plain old gethostbyname() work fine even for multi-threaded
   * programs.
   *
   * This AIX 4.3 or later detection is all made in the configure script.
   *
   * Troels Walsted Hansen helped us work this out on March 3rd, 2003.
   *
   * [*] = much later we have found out that it is not at all "completely
   * thread-safe", but at least the gethostbyname() function is.
   */

  ਜੇ(CURL_HOSTENT_ਮਾਪ >=
     (ਆਕਾਰ(ਬਣਤਰ hostent) + ਆਕਾਰ(ਬਣਤਰ hostent_ਅੰਕੜੇ))) {

    /* August 22nd, 2000: Albert Chin-A-Young brought an updated version
     * that should work! September 20: Richard Prescott worked on the buffer
     * size dilemma.
     */

    res = gethostbyname_r(hostname,
                          (ਬਣਤਰ hostent *)buf,
                          (ਬਣਤਰ hostent_ਅੰਕੜੇ *)((ਅੱਖਰ *)buf +
                                                  ਆਕਾਰ(ਬਣਤਰ hostent)));
    h_errnop = SOCKERRNO; /* we do not deal with this, but set it anyway */
  }
  ਨਹੀਂਤਾਂ
    res = -1; /* failure, too smallish buffer size */

  ਜੇ(!res) { /* success */

    h = buf; /* result expected in h */

    /* This is the worst kind of the different gethostbyname_r() interfaces.
     * Since we do not know how big buffer this particular lookup required,
     * we cannot realloc down the huge alloc without doing closer analysis of
     * the returned data. Thus, we always use CURL_HOSTENT_SIZE for every
     * name lookup. Fixing this would require an extra allocation and then
     * calling Curl_addrinfo_copy() that subsequent reallocation down the new
     * memory area to the actually used amount.
     */
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ /* HAVE_...BYNAME_R_5 || HAVE_...BYNAME_R_6 || HAVE_...BYNAME_R_3 */
  {
    h = ਨਿਰਾ; /* set return code to NULL */
    curlx_free(buf);
  }
#ਨਹੀਂਤਾਂ /* (HAVE_GETADDRINFO && HAVE_GETADDRINFO_THREADSAFE) ||
          HAVE_GETHOSTBYNAME_R */
  /*
   * Here is code for platforms that do not have a thread-safe
   * getaddrinfo() nor gethostbyname_r() function or for which
   * gethostbyname() is the preferred one.
   */
  h = gethostbyname(CURL_UNCONST(hostname));
#ਜੇ_ਅੰਤ /* (HAVE_GETADDRINFO && HAVE_GETADDRINFO_THREADSAFE) ||
           HAVE_GETHOSTBYNAME_R */

#ਜੇ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
  ਜੇ(h) {
    ai = Curl_he2ai(h, ਪੋਰਟ);

    ਜੇ(buf) /* used a *_r() function */
      curlx_free(buf);
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ ai;
}
#ਜੇ_ਅੰਤ /* CURLRES_IPV4 && !USE_RESOLV_ARES && !CURLRES_AMIGA */
