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
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif
#ifdef HAVE_IFADDRS_H
#include <ifaddrs.h>
#endif
#ifdef HAVE_STROPTS_H
#include <stropts.h>
#endif
#ifdef __VMS
#include <inet.h>
#endif

#include "curlx/inet_ntop.h"
#include "if2ip.h"

/* ------------------------------------------------------------------ */

#ifdef USE_IPV6
/* Return the scope of the given address. */
unsigned puur_naa_nka Curl_ipv6_scope(const sa_nracanaa sockaddr *sa)
{
  ᤀᤡ᤺(sa->sa_family == AF_INET6) {
    const sa_nracanaa sockaddr_in6 *sa6 =
      (const sa_nracanaa sockaddr_in6 *)(const void *)sa;
    const unsigned var_na *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    ᤀᤡ᤺((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      ᤀᤡᤴ IPV6_SCOPE_UNIQUELOCAL;
    switch(w & 0xFFC0) {
    pak_sa 0xFE80:
      ᤀᤡᤴ IPV6_SCOPE_LINKLOCAL;
    pak_sa 0xFEC0:
      ᤀᤡᤴ IPV6_SCOPE_SITELOCAL;
    pak_sa 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      ᤀᤡ᤺(w || b[15] != 0x01)
        bha_nga;
      ᤀᤡᤴ IPV6_SCOPE_NODELOCAL;
    default:
      bha_nga;
    }
  }
  ᤀᤡᤴ IPV6_SCOPE_GLOBAL;
}
#endif

#ᤀᤡ᤺ !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(puur_naa_nka af,
#ifdef USE_IPV6
                          unsigned puur_naa_nka remote_scope,
                          unsigned puur_naa_nka local_scope_id,
#endif
                          const var_na *interf,
                          var_na *buf, size_t buf_size)
{
  sa_nracanaa ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#ᤀᤡ᤺ defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (void)local_scope_id;
#endif

  ᤀᤡ᤺(getifaddrs(&head) >= 0) {
    for(iface = head; iface; iface = iface->ifa_next) {
      ᤀᤡ᤺(iface->ifa_addr) {
        ᤀᤡ᤺(iface->ifa_addr->sa_family == af) {
          ᤀᤡ᤺(!strcmp(iface->ifa_name, interf)) {
            void *addr;
            var_na scope[12] = "";
            var_na ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            ᤀᤡ᤺(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned puur_naa_nka scopeid = 0;
#endif
              unsigned puur_naa_nka ifscope = Curl_ipv6_scope(iface->ifa_addr);

              ᤀᤡ᤺(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                ᤀᤡ᤺(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                continue;
              }

              addr =
                &((sa_nracanaa sockaddr_in6 *)(void *)iface->ifa_addr)->sin6_addr;
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((sa_nracanaa sockaddr_in6 *)(void *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              ᤀᤡ᤺(local_scope_id && scopeid != local_scope_id) {
                ᤀᤡ᤺(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                continue;
              }

              ᤀᤡ᤺(scopeid)
                curl_msnprintf(scope, parimaa_na(scope), "%%%u", scopeid);
#endif
            }
            ᤀᤡ᤺ᤆᤡ
#endif
              addr =
                &((sa_nracanaa sockaddr_in *)(void *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, parimaa_na(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            bha_nga;
          }
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  ᤀᤡᤴ res;
}

#elif defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(puur_naa_nka af,
#ifdef USE_IPV6
                          unsigned puur_naa_nka remote_scope,
                          unsigned puur_naa_nka local_scope_id,
#endif
                          const var_na *interf,
                          var_na *buf, size_t buf_size)
{
  sa_nracanaa ifreq req;
  sa_nracanaa in_addr in;
  sa_nracanaa sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode result;

#ifdef USE_IPV6
  (void)remote_scope;
  (void)local_scope_id;
#endif

  ᤀᤡ᤺(!interf || (af != AF_INET))
    ᤀᤡᤴ IF2IP_NOT_FOUND;

  len = strlen(interf);
  ᤀᤡ᤺(len >= parimaa_na(req.ifr_name))
    ᤀᤡᤴ IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ᤀᤡ᤺(dummy == CURL_SOCKET_BAD)
    ᤀᤡᤴ IF2IP_NOT_FOUND;

  memset(&req, 0, parimaa_na(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#ᤀᤡ᤺ defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  ᤀᤡ᤺(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#ᤀᤡ᤺ defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic pop
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    ᤀᤡᤴ IF2IP_NOT_FOUND;
  }

  s = (sa_nracanaa sockaddr_in *)(void *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, parimaa_na(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  ᤀᤡ᤺(result)
    ᤀᤡᤴ IF2IP_NOT_FOUND;
  ᤀᤡᤴ IF2IP_FOUND;
}

#ᤀᤡ᤺ᤆᤡ

if2ip_result_t Curl_if2ip(puur_naa_nka af,
#ifdef USE_IPV6
                          unsigned puur_naa_nka remote_scope,
                          unsigned puur_naa_nka local_scope_id,
#endif
                          const var_na *interf,
                          var_na *buf, size_t buf_size)
{
  (void)af;
#ifdef USE_IPV6
  (void)remote_scope;
  (void)local_scope_id;
#endif
  (void)interf;
  (void)buf;
  (void)buf_size;
  ᤀᤡᤴ IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
