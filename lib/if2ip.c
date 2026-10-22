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

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NET_IF_H
#ضمن <net/if.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_SOCKIO_H
#ضمن <sys/sockio.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_IFADDRS_H
#ضمن <ifaddrs.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_STROPTS_H
#ضمن <stropts.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل __VMS
#ضمن <inet.h>
#جيڪڏهن_پورو

#ضمن "curlx/inet_ntop.h"
#ضمن "if2ip.h"

/* ------------------------------------------------------------------ */

#جيڪڏهن_وصف_ٿيل USE_IPV6
/* Return the scope of the given address. */
بنانشان صحيح Curl_ipv6_scope(ثابت بنية sockaddr *sa)
{
  إذا(sa->sa_family == AF_INET6) {
    ثابت بنية sockaddr_in6 *sa6 =
      (ثابت بنية sockaddr_in6 *)(ثابت فراغ *)sa;
    ثابت بنانشان حرف *b = sa6->sin6_addr.s6_addr;
    بنانشان ننڍو w = (بنانشان ننڍو)((b[0] << 8) | b[1]);

    إذا((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      أرجع IPV6_SCOPE_UNIQUELOCAL;
    بدل(w & 0xFFC0) {
    حالة 0xFE80:
      أرجع IPV6_SCOPE_LINKLOCAL;
    حالة 0xFEC0:
      أرجع IPV6_SCOPE_SITELOCAL;
    حالة 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      إذا(w || b[15] != 0x01)
        اكسر;
      أرجع IPV6_SCOPE_NODELOCAL;
    افتراضي:
      اكسر;
    }
  }
  أرجع IPV6_SCOPE_GLOBAL;
}
#جيڪڏهن_پورو

#إذا !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#جيڪڏهن_وصف_ٿيل HAVE_GETIFADDRS

if2ip_نتيجو_t Curl_if2ip(صحيح af,
#جيڪڏهن_وصف_ٿيل USE_IPV6
                          بنانشان صحيح remote_scope,
                          بنانشان صحيح local_scope_id,
#جيڪڏهن_پورو
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_سائيز)
{
  بنية ifaddrs *iface, *سر;
  if2ip_نتيجو_t res = IF2_IP_NOT_مليو;

#إذا defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (فراغ)local_scope_id;
#جيڪڏهن_پورو

  إذا(getifaddrs(&سر) >= 0) {
    لكل(iface = سر; iface; iface = iface->ifa_ايندڙ) {
      إذا(iface->ifa_addr) {
        إذا(iface->ifa_addr->sa_family == af) {
          إذا(!strcmp(iface->ifa_name, interf)) {
            فراغ *addr;
            حرف scope[12] = "";
            حرف ipstr[64];
            CURLcode نتيجو;
#جيڪڏهن_وصف_ٿيل USE_IPV6
            إذا(af == AF_INET6) {
#جيڪڏهن_وصف_ٿيل HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              بنانشان صحيح scopeid = 0;
#جيڪڏهن_پورو
              بنانشان صحيح ifscope = Curl_ipv6_scope(iface->ifa_addr);

              إذا(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                إذا(res == IF2_IP_NOT_مليو)
                  res = IF2IP_AF_NOT_SUPPORTED;
                استمر;
              }

              addr =
                &((بنية sockaddr_in6 *)(فراغ *)iface->ifa_addr)->sin6_addr;
#جيڪڏهن_وصف_ٿيل HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((بنية sockaddr_in6 *)(فراغ *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              إذا(local_scope_id && scopeid != local_scope_id) {
                إذا(res == IF2_IP_NOT_مليو)
                  res = IF2IP_AF_NOT_SUPPORTED;

                استمر;
              }

              إذا(scopeid)
                curl_msnprintf(scope, حجم(scope), "%%%u", scopeid);
#جيڪڏهن_پورو
            }
            وإلا
#جيڪڏهن_پورو
              addr =
                &((بنية sockaddr_in *)(فراغ *)iface->ifa_addr)->sin_addr;
            res = IF2_IP_مليو;
            نتيجو = curlx_inet_ntop(af, addr, ipstr, حجم(ipstr));
            curl_msnprintf(buf, buf_سائيز, "%s%s", نتيجو ? "" : ipstr, scope);
            اكسر;
          }
        }
        وإلا إذا((res == IF2_IP_NOT_مليو) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(سر);
  }

  أرجع res;
}

#وإلاإذا defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_نتيجو_t Curl_if2ip(صحيح af,
#جيڪڏهن_وصف_ٿيل USE_IPV6
                          بنانشان صحيح remote_scope,
                          بنانشان صحيح local_scope_id,
#جيڪڏهن_پورو
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_سائيز)
{
  بنية ifreq req;
  بنية in_addr اندريون;
  بنية sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode نتيجو;

#جيڪڏهن_وصف_ٿيل USE_IPV6
  (فراغ)remote_scope;
  (فراغ)local_scope_id;
#جيڪڏهن_پورو

  إذا(!interf || (af != AF_INET))
    أرجع IF2_IP_NOT_مليو;

  len = لڙي_ڊيگهه(interf);
  إذا(len >= حجم(req.ifr_name))
    أرجع IF2_IP_NOT_مليو;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  إذا(dummy == CURL_SOCKET_BAD)
    أرجع IF2_IP_NOT_مليو;

  memset(&req, 0, حجم(req));
  يادگيري_نقل(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#إذا defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wshift-sign-overflow"
#جيڪڏهن_پورو
  إذا(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#إذا defined(CURL_HAVE_DIAG) && defined(_AIX)
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    أرجع IF2_IP_NOT_مليو;
  }

  s = (بنية sockaddr_in *)(فراغ *)&req.ifr_addr;
  يادگيري_نقل(&اندريون, &s->sin_addr, حجم(اندريون));
  نتيجو = curlx_inet_ntop(s->sin_family, &اندريون, buf, buf_سائيز);

  sclose(dummy);
  إذا(نتيجو)
    أرجع IF2_IP_NOT_مليو;
  أرجع IF2_IP_مليو;
}

#وإلا

if2ip_نتيجو_t Curl_if2ip(صحيح af,
#جيڪڏهن_وصف_ٿيل USE_IPV6
                          بنانشان صحيح remote_scope,
                          بنانشان صحيح local_scope_id,
#جيڪڏهن_پورو
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_سائيز)
{
  (فراغ)af;
#جيڪڏهن_وصف_ٿيل USE_IPV6
  (فراغ)remote_scope;
  (فراغ)local_scope_id;
#جيڪڏهن_پورو
  (فراغ)interf;
  (فراغ)buf;
  (فراغ)buf_سائيز;
  أرجع IF2_IP_NOT_مليو;
}

#جيڪڏهن_پورو

#جيڪڏهن_پورو /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
