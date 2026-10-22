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
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#ଅନ୍ତର୍ଭାଵ <sys/sockio.h>
#endif
#ifdef HAVE_IFADDRS_H
#ଅନ୍ତର୍ଭାଵ <ifaddrs.h>
#endif
#ifdef HAVE_STROPTS_H
#ଅନ୍ତର୍ଭାଵ <stropts.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "curlx/inet_ntop.h"
#ଅନ୍ତର୍ଭାଵ "if2ip.h"

/* ------------------------------------------------------------------ */

#ifdef USE_IPV6
/* Return the scope of the given address. */
unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ipv6_scope(ଅଚଳ ସଂରଚନା sockaddr *sa)
{
  ଯଦି(sa->sa_family == AF_INET6) {
    ଅଚଳ ସଂରଚନା sockaddr_in6 *sa6 =
      (ଅଚଳ ସଂରଚନା sockaddr_in6 *)(ଅଚଳ ଶୂନ୍ୟ *)sa;
    ଅଚଳ unsigned ଅକ୍ଷର *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    ଯଦି((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      ଫେରନ୍ତୁ IPV6_SCOPE_UNIQUELOCAL;
    ବାଛ(w & 0xFFC0) {
    ସ୍ଥିତି 0xFE80:
      ଫେରନ୍ତୁ IPV6_SCOPE_LINKLOCAL;
    ସ୍ଥିତି 0xFEC0:
      ଫେରନ୍ତୁ IPV6_SCOPE_SITELOCAL;
    ସ୍ଥିତି 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      ଯଦି(w || b[15] != 0x01)
        ଭାଙ୍ଗ;
      ଫେରନ୍ତୁ IPV6_SCOPE_NODELOCAL;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ IPV6_SCOPE_GLOBAL;
}
#endif

#ଯଦି !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af,
#ifdef USE_IPV6
                          unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା remote_scope,
                          unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା local_scope_id,
#endif
                          ଅଚଳ ଅକ୍ଷର *interf,
                          ଅକ୍ଷର *buf, size_t buf_size)
{
  ସଂରଚନା ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#ଯଦି defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (ଶୂନ୍ୟ)local_scope_id;
#endif

  ଯଦି(getifaddrs(&head) >= 0) {
    ପାଇଁ(iface = head; iface; iface = iface->ifa_next) {
      ଯଦି(iface->ifa_addr) {
        ଯଦି(iface->ifa_addr->sa_family == af) {
          ଯଦି(!strcmp(iface->ifa_name, interf)) {
            ଶୂନ୍ୟ *addr;
            ଅକ୍ଷର scope[12] = "";
            ଅକ୍ଷର ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            ଯଦି(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା scopeid = 0;
#endif
              unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifscope = Curl_ipv6_scope(iface->ifa_addr);

              ଯଦି(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                ଯଦି(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                ଜାରି;
              }

              addr =
                &((ସଂରଚନା sockaddr_in6 *)(ଶୂନ୍ୟ *)iface->ifa_addr)->sin6_addr;
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((ସଂରଚନା sockaddr_in6 *)(ଶୂନ୍ୟ *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              ଯଦି(local_scope_id && scopeid != local_scope_id) {
                ଯଦି(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                ଜାରି;
              }

              ଯଦି(scopeid)
                curl_msnprintf(scope, ଆକାର(scope), "%%%u", scopeid);
#endif
            }
            ଅନ୍ୟଥା
#endif
              addr =
                &((ସଂରଚନା sockaddr_in *)(ଶୂନ୍ୟ *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, ଆକାର(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            ଭାଙ୍ଗ;
          }
        }
        ଅନ୍ୟଥା ଯଦି((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  ଫେରନ୍ତୁ res;
}

#ନଚେତଯଦି defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af,
#ifdef USE_IPV6
                          unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା remote_scope,
                          unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା local_scope_id,
#endif
                          ଅଚଳ ଅକ୍ଷର *interf,
                          ଅକ୍ଷର *buf, size_t buf_size)
{
  ସଂରଚନା ifreq req;
  ସଂରଚନା in_addr in;
  ସଂରଚନା sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode result;

#ifdef USE_IPV6
  (ଶୂନ୍ୟ)remote_scope;
  (ଶୂନ୍ୟ)local_scope_id;
#endif

  ଯଦି(!interf || (af != AF_INET))
    ଫେରନ୍ତୁ IF2IP_NOT_FOUND;

  len = strlen(interf);
  ଯଦି(len >= ଆକାର(req.ifr_name))
    ଫେରନ୍ତୁ IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ଯଦି(dummy == CURL_SOCKET_BAD)
    ଫେରନ୍ତୁ IF2IP_NOT_FOUND;

  memset(&req, 0, ଆକାର(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#ଯଦି defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  ଯଦି(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#ଯଦି defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic କାଢ଼
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    ଫେରନ୍ତୁ IF2IP_NOT_FOUND;
  }

  s = (ସଂରଚନା sockaddr_in *)(ଶୂନ୍ୟ *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, ଆକାର(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  ଯଦି(result)
    ଫେରନ୍ତୁ IF2IP_NOT_FOUND;
  ଫେରନ୍ତୁ IF2IP_FOUND;
}

#ଅନ୍ୟଥା

if2ip_result_t Curl_if2ip(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af,
#ifdef USE_IPV6
                          unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା remote_scope,
                          unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା local_scope_id,
#endif
                          ଅଚଳ ଅକ୍ଷର *interf,
                          ଅକ୍ଷର *buf, size_t buf_size)
{
  (ଶୂନ୍ୟ)af;
#ifdef USE_IPV6
  (ଶୂନ୍ୟ)remote_scope;
  (ଶୂନ୍ୟ)local_scope_id;
#endif
  (ଶୂନ୍ୟ)interf;
  (ଶୂନ୍ୟ)buf;
  (ଶୂନ୍ୟ)buf_size;
  ଫେରନ୍ତୁ IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
