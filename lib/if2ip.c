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
unsigned ຈຳນວນເຕັມ Curl_ipv6_scope(const sa_nracanaa sockaddr *sa)
{
  ຖ້າ(sa->sa_family == AF_INET6) {
    const sa_nracanaa sockaddr_in6 *sa6 =
      (const sa_nracanaa sockaddr_in6 *)(const void *)sa;
    const unsigned var_na *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    ຖ້າ((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      ສົ່ງຄືນ IPV6_SCOPE_UNIQUELOCAL;
    switch(w & 0xFFC0) {
    pak_sa 0xFE80:
      ສົ່ງຄືນ IPV6_SCOPE_LINKLOCAL;
    pak_sa 0xFEC0:
      ສົ່ງຄືນ IPV6_SCOPE_SITELOCAL;
    pak_sa 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      ຖ້າ(w || b[15] != 0x01)
        bha_nga;
      ສົ່ງຄືນ IPV6_SCOPE_NODELOCAL;
    default:
      bha_nga;
    }
  }
  ສົ່ງຄືນ IPV6_SCOPE_GLOBAL;
}
#endif

#ຖ້າ !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(ຈຳນວນເຕັມ af,
#ifdef USE_IPV6
                          unsigned ຈຳນວນເຕັມ remote_scope,
                          unsigned ຈຳນວນເຕັມ local_scope_id,
#endif
                          const var_na *interf,
                          var_na *buf, size_t buf_size)
{
  sa_nracanaa ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#ຖ້າ defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (void)local_scope_id;
#endif

  ຖ້າ(getifaddrs(&head) >= 0) {
    ສຳລັບ(iface = head; iface; iface = iface->ifa_next) {
      ຖ້າ(iface->ifa_addr) {
        ຖ້າ(iface->ifa_addr->sa_family == af) {
          ຖ້າ(!strcmp(iface->ifa_name, interf)) {
            void *addr;
            var_na scope[12] = "";
            var_na ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            ຖ້າ(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned ຈຳນວນເຕັມ scopeid = 0;
#endif
              unsigned ຈຳນວນເຕັມ ifscope = Curl_ipv6_scope(iface->ifa_addr);

              ຖ້າ(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                ຖ້າ(res == IF2IP_NOT_FOUND)
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
              ຖ້າ(local_scope_id && scopeid != local_scope_id) {
                ຖ້າ(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                continue;
              }

              ຖ້າ(scopeid)
                curl_msnprintf(scope, parimaa_na(scope), "%%%u", scopeid);
#endif
            }
            ບໍ່ດັ່ງນັ້ນ
#endif
              addr =
                &((sa_nracanaa sockaddr_in *)(void *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, parimaa_na(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            bha_nga;
          }
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  ສົ່ງຄືນ res;
}

#elif defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(ຈຳນວນເຕັມ af,
#ifdef USE_IPV6
                          unsigned ຈຳນວນເຕັມ remote_scope,
                          unsigned ຈຳນວນເຕັມ local_scope_id,
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

  ຖ້າ(!interf || (af != AF_INET))
    ສົ່ງຄືນ IF2IP_NOT_FOUND;

  len = strlen(interf);
  ຖ້າ(len >= parimaa_na(req.ifr_name))
    ສົ່ງຄືນ IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ຖ້າ(dummy == CURL_SOCKET_BAD)
    ສົ່ງຄືນ IF2IP_NOT_FOUND;

  memset(&req, 0, parimaa_na(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#ຖ້າ defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  ຖ້າ(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#ຖ້າ defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic pop
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    ສົ່ງຄືນ IF2IP_NOT_FOUND;
  }

  s = (sa_nracanaa sockaddr_in *)(void *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, parimaa_na(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  ຖ້າ(result)
    ສົ່ງຄືນ IF2IP_NOT_FOUND;
  ສົ່ງຄືນ IF2IP_FOUND;
}

#ບໍ່ດັ່ງນັ້ນ

if2ip_result_t Curl_if2ip(ຈຳນວນເຕັມ af,
#ifdef USE_IPV6
                          unsigned ຈຳນວນເຕັມ remote_scope,
                          unsigned ຈຳນວນເຕັມ local_scope_id,
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
  ສົ່ງຄືນ IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
