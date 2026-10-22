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
#அந்தர்பாவ "curl_setup.h"

/***********************************************************************
 * Only for plain IPv4 builds
 **********************************************************************/
#ifdef CURLRES_IPV4 /* plain IPv4 code coming up */

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "vdns/hostip.h"
#அந்தர்பாவ "url.h"


#ifdef CURLRES_SYNCH

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
கட்டமைப்பு Curl_addrinfo *Curl_sync_getaddrinfo(கட்டமைப்பு Curl_easy *data,
                                            uint8_t dns_queries,
                                            மாறிலி எழுத்து *hostname,
                                            uint16_t port,
                                            uint8_t transport)
{
  கட்டமைப்பு Curl_addrinfo *ai = வெற்று;

  (வெறுமை)dns_queries;
  (வெறுமை)transport;

  ai = Curl_ipv4_resolve_r(hostname, port);
  எனில்(!ai)
    infof(data, "Curl_ipv4_resolve_r failed for %s", hostname);

  திரும்பு ai;
}
#endif /* CURLRES_SYNCH */
#endif /* CURLRES_IPV4 */

#எனில் defined(CURLRES_IPV4) && !defined(USE_RESOLV_ARES) && \
  !defined(CURLRES_AMIGA)

/*
 * Curl_ipv4_resolve_r() - IPv4 thread-safe resolver function.
 *
 * This is used for both synchronous and asynchronous resolver builds,
 * implying that only thread-safe code and function calls may be used.
 *
 */
கட்டமைப்பு Curl_addrinfo *Curl_ipv4_resolve_r(மாறிலி எழுத்து *hostname,
                                          uint16_t port)
{
#எனில் !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE)) && \
  defined(HAVE_GETHOSTBYNAME_R_3)
  முழு res;
#endif
  கட்டமைப்பு Curl_addrinfo *ai = வெற்று;
#எனில் !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
  கட்டமைப்பு hostent *h = வெற்று;
  கட்டமைப்பு hostent *buf = வெற்று;
#endif

#எனில் defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE)
  கட்டமைப்பு addrinfo hints;
  எழுத்து sbuf[12];
  எழுத்து *sbufptr = வெற்று;

  memset(&hints, 0, அளவு(hints));
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  எனில்(port) {
    curl_msnprintf(sbuf, அளவு(sbuf), "%d", port);
    sbufptr = sbuf;
  }

  (வெறுமை)Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &ai);

#அல்லதெனில் defined(HAVE_GETHOSTBYNAME_R)
  /*
   * gethostbyname_r() is the preferred resolve function for many platforms.
   * Since there are three different versions of it, the following code is
   * somewhat #ifdef-ridden.
   */
  முழு h_errnop;

  buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
  எனில்(!buf)
    திரும்பு வெற்று; /* major failure */
  /*
   * The clearing of the buffer is a workaround for a gethostbyname_r bug in
   * QNX Neutrino and it is also _required_ for some of these functions on some
   * platforms.
   */

#ifdef HAVE_GETHOSTBYNAME_R_5
  /* Solaris, IRIX and more */
  h = gethostbyname_r(hostname,
                      (கட்டமைப்பு hostent *)buf,
                      (எழுத்து *)buf + அளவு(கட்டமைப்பு hostent),
                      CURL_HOSTENT_SIZE - அளவு(கட்டமைப்பு hostent),
                      &h_errnop);

  /* If the buffer is too small, it returns NULL and sets errno to
   * ERANGE. The errno is thread-safe if this is compiled with
   * -D_REENTRANT as then the 'errno' variable is a macro defined to get
   * used properly for threads.
   */

  எனில்(h) {
    ;
  }
  இல்லையெனில்
#அல்லதெனில் defined(HAVE_GETHOSTBYNAME_R_6)
  /* Linux */

  (வெறுமை)gethostbyname_r(hostname,
                        (கட்டமைப்பு hostent *)buf,
                        (எழுத்து *)buf + அளவு(கட்டமைப்பு hostent),
                        CURL_HOSTENT_SIZE - அளவு(கட்டமைப்பு hostent),
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

  எனில்(!h) /* failure */
#அல்லதெனில் defined(HAVE_GETHOSTBYNAME_R_3)
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

  எனில்(CURL_HOSTENT_SIZE >=
     (அளவு(கட்டமைப்பு hostent) + அளவு(கட்டமைப்பு hostent_data))) {

    /* August 22nd, 2000: Albert Chin-A-Young brought an updated version
     * that should work! September 20: Richard Prescott worked on the buffer
     * size dilemma.
     */

    res = gethostbyname_r(hostname,
                          (கட்டமைப்பு hostent *)buf,
                          (கட்டமைப்பு hostent_data *)((எழுத்து *)buf +
                                                  அளவு(கட்டமைப்பு hostent)));
    h_errnop = SOCKERRNO; /* we do not deal with this, but set it anyway */
  }
  இல்லையெனில்
    res = -1; /* failure, too smallish buffer size */

  எனில்(!res) { /* success */

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
  இல்லையெனில்
#endif /* HAVE_...BYNAME_R_5 || HAVE_...BYNAME_R_6 || HAVE_...BYNAME_R_3 */
  {
    h = வெற்று; /* set return code to NULL */
    curlx_free(buf);
  }
#இல்லையெனில் /* (HAVE_GETADDRINFO && HAVE_GETADDRINFO_THREADSAFE) ||
          HAVE_GETHOSTBYNAME_R */
  /*
   * Here is code for platforms that do not have a thread-safe
   * getaddrinfo() nor gethostbyname_r() function or for which
   * gethostbyname() is the preferred one.
   */
  h = gethostbyname(CURL_UNCONST(hostname));
#endif /* (HAVE_GETADDRINFO && HAVE_GETADDRINFO_THREADSAFE) ||
           HAVE_GETHOSTBYNAME_R */

#எனில் !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
  எனில்(h) {
    ai = Curl_he2ai(h, port);

    எனில்(buf) /* used a *_r() function */
      curlx_free(buf);
  }
#endif

  திரும்பு ai;
}
#endif /* CURLRES_IPV4 && !USE_RESOLV_ARES && !CURLRES_AMIGA */
