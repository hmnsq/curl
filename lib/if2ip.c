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

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی HAVE_NET_IF_H
#ضمن <net/if.h>
#د_که_پای
#که_تعریف_شوی HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_SYS_SOCKIO_H
#ضمن <sys/sockio.h>
#د_که_پای
#که_تعریف_شوی HAVE_IFADDRS_H
#ضمن <ifaddrs.h>
#د_که_پای
#که_تعریف_شوی HAVE_STROPTS_H
#ضمن <stropts.h>
#د_که_پای
#که_تعریف_شوی __VMS
#ضمن <inet.h>
#د_که_پای

#ضمن "curlx/inet_ntop.h"
#ضمن "if2ip.h"

/* ------------------------------------------------------------------ */

#که_تعریف_شوی USE_IPV6
/* Return the scope of the given address. */
بینښه صحيح Curl_ipv6_scope(ثابت بنية sockaddr *sa)
{
  إذا(sa->sa_family == AF_INET6) {
    ثابت بنية sockaddr_in6 *sa6 =
      (ثابت بنية sockaddr_in6 *)(ثابت فراغ *)sa;
    ثابت بینښه حرف *b = sa6->sin6_addr.s6_addr;
    بینښه لنډ w = (بینښه لنډ)((b[0] << 8) | b[1]);

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
#د_که_پای

#إذا !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#که_تعریف_شوی HAVE_GETIFADDRS

if2ip_پایله_t Curl_if2ip(صحيح af,
#که_تعریف_شوی USE_IPV6
                          بینښه صحيح remote_scope,
                          بینښه صحيح local_scope_id,
#د_که_پای
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_اندازه)
{
  بنية ifaddrs *iface, *سر;
  if2ip_پایله_t res = IF2_IP_NOT_وموندل;

#إذا defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (فراغ)local_scope_id;
#د_که_پای

  إذا(getifaddrs(&سر) >= 0) {
    لكل(iface = سر; iface; iface = iface->ifa_بل) {
      إذا(iface->ifa_addr) {
        إذا(iface->ifa_addr->sa_family == af) {
          إذا(!strcmp(iface->ifa_name, interf)) {
            فراغ *addr;
            حرف scope[12] = "";
            حرف ipstr[64];
            CURLcode پایله;
#که_تعریف_شوی USE_IPV6
            إذا(af == AF_INET6) {
#که_تعریف_شوی HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              بینښه صحيح scopeid = 0;
#د_که_پای
              بینښه صحيح ifscope = Curl_ipv6_scope(iface->ifa_addr);

              إذا(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                إذا(res == IF2_IP_NOT_وموندل)
                  res = IF2IP_AF_NOT_SUPPORTED;
                استمر;
              }

              addr =
                &((بنية sockaddr_in6 *)(فراغ *)iface->ifa_addr)->sin6_addr;
#که_تعریف_شوی HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((بنية sockaddr_in6 *)(فراغ *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              إذا(local_scope_id && scopeid != local_scope_id) {
                إذا(res == IF2_IP_NOT_وموندل)
                  res = IF2IP_AF_NOT_SUPPORTED;

                استمر;
              }

              إذا(scopeid)
                curl_msnprintf(scope, حجم(scope), "%%%u", scopeid);
#د_که_پای
            }
            وإلا
#د_که_پای
              addr =
                &((بنية sockaddr_in *)(فراغ *)iface->ifa_addr)->sin_addr;
            res = IF2_IP_وموندل;
            پایله = curlx_inet_ntop(af, addr, ipstr, حجم(ipstr));
            curl_msnprintf(buf, buf_اندازه, "%s%s", پایله ? "" : ipstr, scope);
            اكسر;
          }
        }
        وإلا إذا((res == IF2_IP_NOT_وموندل) &&
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

if2ip_پایله_t Curl_if2ip(صحيح af,
#که_تعریف_شوی USE_IPV6
                          بینښه صحيح remote_scope,
                          بینښه صحيح local_scope_id,
#د_که_پای
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_اندازه)
{
  بنية ifreq req;
  بنية in_addr دننه;
  بنية sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode پایله;

#که_تعریف_شوی USE_IPV6
  (فراغ)remote_scope;
  (فراغ)local_scope_id;
#د_که_پای

  إذا(!interf || (af != AF_INET))
    أرجع IF2_IP_NOT_وموندل;

  len = مزي_اوږدوالی(interf);
  إذا(len >= حجم(req.ifr_name))
    أرجع IF2_IP_NOT_وموندل;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  إذا(dummy == CURL_SOCKET_BAD)
    أرجع IF2_IP_NOT_وموندل;

  memset(&req, 0, حجم(req));
  حافظه_نقل(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#إذا defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wshift-sign-overflow"
#د_که_پای
  إذا(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#إذا defined(CURL_HAVE_DIAG) && defined(_AIX)
#لارښوونه GCC diagnostic راوباسه
#د_که_پای
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    أرجع IF2_IP_NOT_وموندل;
  }

  s = (بنية sockaddr_in *)(فراغ *)&req.ifr_addr;
  حافظه_نقل(&دننه, &s->sin_addr, حجم(دننه));
  پایله = curlx_inet_ntop(s->sin_family, &دننه, buf, buf_اندازه);

  sclose(dummy);
  إذا(پایله)
    أرجع IF2_IP_NOT_وموندل;
  أرجع IF2_IP_وموندل;
}

#وإلا

if2ip_پایله_t Curl_if2ip(صحيح af,
#که_تعریف_شوی USE_IPV6
                          بینښه صحيح remote_scope,
                          بینښه صحيح local_scope_id,
#د_که_پای
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_اندازه)
{
  (فراغ)af;
#که_تعریف_شوی USE_IPV6
  (فراغ)remote_scope;
  (فراغ)local_scope_id;
#د_که_پای
  (فراغ)interf;
  (فراغ)buf;
  (فراغ)buf_اندازه;
  أرجع IF2_IP_NOT_وموندل;
}

#د_که_پای

#د_که_پای /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
