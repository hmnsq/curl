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

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NET_IF_H
#ضمن <net/if.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_SOCKIO_H
#ضمن <sys/sockio.h>
#جے_انت
#جے_تعریف_شدہ HAVE_IFADDRS_H
#ضمن <ifaddrs.h>
#جے_انت
#جے_تعریف_شدہ HAVE_STROPTS_H
#ضمن <stropts.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <inet.h>
#جے_انت

#ضمن "curlx/inet_ntop.h"
#ضمن "if2ip.h"

/* ------------------------------------------------------------------ */

#جے_تعریف_شدہ USE_IPV6
/* Return the scope of the given address. */
بنانشانی صحيح Curl_ipv6_scope(ثابت بنية sockaddr *sa)
{
  إذا(sa->sa_family == AF_INET6) {
    ثابت بنية sockaddr_in6 *sa6 =
      (ثابت بنية sockaddr_in6 *)(ثابت فراغ *)sa;
    ثابت بنانشانی حرف *b = sa6->sin6_addr.s6_addr;
    بنانشانی چھوٹا w = (بنانشانی چھوٹا)((b[0] << 8) | b[1]);

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
#جے_انت

#إذا !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#جے_تعریف_شدہ HAVE_GETIFADDRS

if2ip_نتیجہ_t Curl_if2ip(صحيح af,
#جے_تعریف_شدہ USE_IPV6
                          بنانشانی صحيح remote_scope,
                          بنانشانی صحيح local_scope_id,
#جے_انت
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_ناپ)
{
  بنية ifaddrs *iface, *سر;
  if2ip_نتیجہ_t res = IF2_IP_NOT_لبھیا;

#إذا defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (فراغ)local_scope_id;
#جے_انت

  إذا(getifaddrs(&سر) >= 0) {
    لكل(iface = سر; iface; iface = iface->ifa_اگلا) {
      إذا(iface->ifa_addr) {
        إذا(iface->ifa_addr->sa_family == af) {
          إذا(!strcmp(iface->ifa_name, interf)) {
            فراغ *addr;
            حرف scope[12] = "";
            حرف ipstr[64];
            CURLcode نتیجہ;
#جے_تعریف_شدہ USE_IPV6
            إذا(af == AF_INET6) {
#جے_تعریف_شدہ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              بنانشانی صحيح scopeid = 0;
#جے_انت
              بنانشانی صحيح ifscope = Curl_ipv6_scope(iface->ifa_addr);

              إذا(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                إذا(res == IF2_IP_NOT_لبھیا)
                  res = IF2IP_AF_NOT_SUPPORTED;
                استمر;
              }

              addr =
                &((بنية sockaddr_in6 *)(فراغ *)iface->ifa_addr)->sin6_addr;
#جے_تعریف_شدہ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((بنية sockaddr_in6 *)(فراغ *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              إذا(local_scope_id && scopeid != local_scope_id) {
                إذا(res == IF2_IP_NOT_لبھیا)
                  res = IF2IP_AF_NOT_SUPPORTED;

                استمر;
              }

              إذا(scopeid)
                curl_msnprintf(scope, حجم(scope), "%%%u", scopeid);
#جے_انت
            }
            وإلا
#جے_انت
              addr =
                &((بنية sockaddr_in *)(فراغ *)iface->ifa_addr)->sin_addr;
            res = IF2_IP_لبھیا;
            نتیجہ = curlx_inet_ntop(af, addr, ipstr, حجم(ipstr));
            curl_msnprintf(buf, buf_ناپ, "%s%s", نتیجہ ? "" : ipstr, scope);
            اكسر;
          }
        }
        وإلا إذا((res == IF2_IP_NOT_لبھیا) &&
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

if2ip_نتیجہ_t Curl_if2ip(صحيح af,
#جے_تعریف_شدہ USE_IPV6
                          بنانشانی صحيح remote_scope,
                          بنانشانی صحيح local_scope_id,
#جے_انت
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_ناپ)
{
  بنية ifreq req;
  بنية in_addr وچ;
  بنية sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode نتیجہ;

#جے_تعریف_شدہ USE_IPV6
  (فراغ)remote_scope;
  (فراغ)local_scope_id;
#جے_انت

  إذا(!interf || (af != AF_INET))
    أرجع IF2_IP_NOT_لبھیا;

  len = لڑی_لمبائی(interf);
  إذا(len >= حجم(req.ifr_name))
    أرجع IF2_IP_NOT_لبھیا;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  إذا(dummy == CURL_SOCKET_BAD)
    أرجع IF2_IP_NOT_لبھیا;

  memset(&req, 0, حجم(req));
  حافظہ_نقل(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#إذا defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wshift-sign-overflow"
#جے_انت
  إذا(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#إذا defined(CURL_HAVE_DIAG) && defined(_AIX)
#ہدایت GCC diagnostic نکالو
#جے_انت
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    أرجع IF2_IP_NOT_لبھیا;
  }

  s = (بنية sockaddr_in *)(فراغ *)&req.ifr_addr;
  حافظہ_نقل(&وچ, &s->sin_addr, حجم(وچ));
  نتیجہ = curlx_inet_ntop(s->sin_family, &وچ, buf, buf_ناپ);

  sclose(dummy);
  إذا(نتیجہ)
    أرجع IF2_IP_NOT_لبھیا;
  أرجع IF2_IP_لبھیا;
}

#وإلا

if2ip_نتیجہ_t Curl_if2ip(صحيح af,
#جے_تعریف_شدہ USE_IPV6
                          بنانشانی صحيح remote_scope,
                          بنانشانی صحيح local_scope_id,
#جے_انت
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_ناپ)
{
  (فراغ)af;
#جے_تعریف_شدہ USE_IPV6
  (فراغ)remote_scope;
  (فراغ)local_scope_id;
#جے_انت
  (فراغ)interf;
  (فراغ)buf;
  (فراغ)buf_ناپ;
  أرجع IF2_IP_NOT_لبھیا;
}

#جے_انت

#جے_انت /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
