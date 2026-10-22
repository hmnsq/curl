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
#ضمن "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ضمن <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#ضمن <sys/sockio.h>
#endif
#ifdef HAVE_IFADDRS_H
#ضمن <ifaddrs.h>
#endif
#ifdef HAVE_STROPTS_H
#ضمن <stropts.h>
#endif
#ifdef __VMS
#ضمن <inet.h>
#endif

#ضمن "curlx/inet_ntop.h"
#ضمن "if2ip.h"

/* ------------------------------------------------------------------ */

#ifdef USE_IPV6
/* Return the scope of the given address. */
unsigned صحيح Curl_ipv6_scope(ثابت بنية sockaddr *sa)
{
  𐡄𐡍(sa->sa_family == AF_INET6) {
    ثابت بنية sockaddr_in6 *sa6 =
      (ثابت بنية sockaddr_in6 *)(ثابت فراغ *)sa;
    ثابت unsigned حرف *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    𐡄𐡍((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      أرجع IPV6_SCOPE_UNIQUELOCAL;
    بدل(w & 0xFFC0) {
    حالة 0xFE80:
      أرجع IPV6_SCOPE_LINKLOCAL;
    حالة 0xFEC0:
      أرجع IPV6_SCOPE_SITELOCAL;
    حالة 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      𐡄𐡍(w || b[15] != 0x01)
        اكسر;
      أرجع IPV6_SCOPE_NODELOCAL;
    افتراضي:
      اكسر;
    }
  }
  أرجع IPV6_SCOPE_GLOBAL;
}
#endif

#𐡄𐡍 !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(صحيح af,
#ifdef USE_IPV6
                          unsigned صحيح remote_scope,
                          unsigned صحيح local_scope_id,
#endif
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_size)
{
  بنية ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#𐡄𐡍 defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (فراغ)local_scope_id;
#endif

  𐡄𐡍(getifaddrs(&head) >= 0) {
    لكل(iface = head; iface; iface = iface->ifa_next) {
      𐡄𐡍(iface->ifa_addr) {
        𐡄𐡍(iface->ifa_addr->sa_family == af) {
          𐡄𐡍(!strcmp(iface->ifa_name, interf)) {
            فراغ *addr;
            حرف scope[12] = "";
            حرف ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            𐡄𐡍(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned صحيح scopeid = 0;
#endif
              unsigned صحيح ifscope = Curl_ipv6_scope(iface->ifa_addr);

              𐡄𐡍(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                𐡄𐡍(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                استمر;
              }

              addr =
                &((بنية sockaddr_in6 *)(فراغ *)iface->ifa_addr)->sin6_addr;
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((بنية sockaddr_in6 *)(فراغ *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              𐡄𐡍(local_scope_id && scopeid != local_scope_id) {
                𐡄𐡍(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                استمر;
              }

              𐡄𐡍(scopeid)
                curl_msnprintf(scope, حجم(scope), "%%%u", scopeid);
#endif
            }
            وإلا
#endif
              addr =
                &((بنية sockaddr_in *)(فراغ *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, حجم(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            اكسر;
          }
        }
        وإلا 𐡄𐡍((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  أرجع res;
}

#وإلاإذا defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(صحيح af,
#ifdef USE_IPV6
                          unsigned صحيح remote_scope,
                          unsigned صحيح local_scope_id,
#endif
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_size)
{
  بنية ifreq req;
  بنية in_addr in;
  بنية sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode result;

#ifdef USE_IPV6
  (فراغ)remote_scope;
  (فراغ)local_scope_id;
#endif

  𐡄𐡍(!interf || (af != AF_INET))
    أرجع IF2IP_NOT_FOUND;

  len = strlen(interf);
  𐡄𐡍(len >= حجم(req.ifr_name))
    أرجع IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  𐡄𐡍(dummy == CURL_SOCKET_BAD)
    أرجع IF2IP_NOT_FOUND;

  memset(&req, 0, حجم(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#𐡄𐡍 defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  𐡄𐡍(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#𐡄𐡍 defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic pop
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    أرجع IF2IP_NOT_FOUND;
  }

  s = (بنية sockaddr_in *)(فراغ *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, حجم(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  𐡄𐡍(result)
    أرجع IF2IP_NOT_FOUND;
  أرجع IF2IP_FOUND;
}

#وإلا

if2ip_result_t Curl_if2ip(صحيح af,
#ifdef USE_IPV6
                          unsigned صحيح remote_scope,
                          unsigned صحيح local_scope_id,
#endif
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_size)
{
  (فراغ)af;
#ifdef USE_IPV6
  (فراغ)remote_scope;
  (فراغ)local_scope_id;
#endif
  (فراغ)interf;
  (فراغ)buf;
  (فراغ)buf_size;
  أرجع IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
