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

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_SOCKIO_H
#ضمن <sys/sockio.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_IFADDRS_H
#ضمن <ifaddrs.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_STROPTS_H
#ضمن <stropts.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <inet.h>
#پایان_اگر

#ضمن "curlx/inet_ntop.h"
#ضمن "if2ip.h"

/* ------------------------------------------------------------------ */

#اگر_تعریف_شده USE_IPV6
/* Return the scope of the given address. */
بیعلامت صحيح Curl_ipv6_scope(ثابت بنية sockaddr *sa)
{
  إذا(sa->sa_family == AF_INET6) {
    ثابت بنية sockaddr_in6 *sa6 =
      (ثابت بنية sockaddr_in6 *)(ثابت فراغ *)sa;
    ثابت بیعلامت حرف *b = sa6->sin6_addr.s6_addr;
    بیعلامت کوتاه w = (بیعلامت کوتاه)((b[0] << 8) | b[1]);

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
#پایان_اگر

#إذا !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#اگر_تعریف_شده HAVE_GETIFADDRS

if2ip_نتیجه_t Curl_if2ip(صحيح af,
#اگر_تعریف_شده USE_IPV6
                          بیعلامت صحيح remote_scope,
                          بیعلامت صحيح local_scope_id,
#پایان_اگر
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_اندازه)
{
  بنية ifaddrs *iface, *سر;
  if2ip_نتیجه_t res = IF2_IP_NOT_یافت;

#إذا defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (فراغ)local_scope_id;
#پایان_اگر

  إذا(getifaddrs(&سر) >= 0) {
    لكل(iface = سر; iface; iface = iface->ifa_بعدی) {
      إذا(iface->ifa_addr) {
        إذا(iface->ifa_addr->sa_family == af) {
          إذا(!strcmp(iface->ifa_name, interf)) {
            فراغ *addr;
            حرف scope[12] = "";
            حرف ipstr[64];
            CURLcode نتیجه;
#اگر_تعریف_شده USE_IPV6
            إذا(af == AF_INET6) {
#اگر_تعریف_شده HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              بیعلامت صحيح scopeid = 0;
#پایان_اگر
              بیعلامت صحيح ifscope = Curl_ipv6_scope(iface->ifa_addr);

              إذا(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                إذا(res == IF2_IP_NOT_یافت)
                  res = IF2IP_AF_NOT_SUPPORTED;
                استمر;
              }

              addr =
                &((بنية sockaddr_in6 *)(فراغ *)iface->ifa_addr)->sin6_addr;
#اگر_تعریف_شده HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((بنية sockaddr_in6 *)(فراغ *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              إذا(local_scope_id && scopeid != local_scope_id) {
                إذا(res == IF2_IP_NOT_یافت)
                  res = IF2IP_AF_NOT_SUPPORTED;

                استمر;
              }

              إذا(scopeid)
                curl_msnprintf(scope, حجم(scope), "%%%u", scopeid);
#پایان_اگر
            }
            وإلا
#پایان_اگر
              addr =
                &((بنية sockaddr_in *)(فراغ *)iface->ifa_addr)->sin_addr;
            res = IF2_IP_یافت;
            نتیجه = curlx_inet_ntop(af, addr, ipstr, حجم(ipstr));
            curl_msnprintf(buf, buf_اندازه, "%s%s", نتیجه ? "" : ipstr, scope);
            اكسر;
          }
        }
        وإلا إذا((res == IF2_IP_NOT_یافت) &&
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

if2ip_نتیجه_t Curl_if2ip(صحيح af,
#اگر_تعریف_شده USE_IPV6
                          بیعلامت صحيح remote_scope,
                          بیعلامت صحيح local_scope_id,
#پایان_اگر
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_اندازه)
{
  بنية ifreq req;
  بنية in_addr درون;
  بنية sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode نتیجه;

#اگر_تعریف_شده USE_IPV6
  (فراغ)remote_scope;
  (فراغ)local_scope_id;
#پایان_اگر

  إذا(!interf || (af != AF_INET))
    أرجع IF2_IP_NOT_یافت;

  len = طول_رشته(interf);
  إذا(len >= حجم(req.ifr_name))
    أرجع IF2_IP_NOT_یافت;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  إذا(dummy == CURL_SOCKET_BAD)
    أرجع IF2_IP_NOT_یافت;

  memset(&req, 0, حجم(req));
  کپی_حافظه(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#إذا defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wshift-sign-overflow"
#پایان_اگر
  إذا(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#إذا defined(CURL_HAVE_DIAG) && defined(_AIX)
#رهنمود GCC diagnostic برداشت
#پایان_اگر
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    أرجع IF2_IP_NOT_یافت;
  }

  s = (بنية sockaddr_in *)(فراغ *)&req.ifr_addr;
  کپی_حافظه(&درون, &s->sin_addr, حجم(درون));
  نتیجه = curlx_inet_ntop(s->sin_family, &درون, buf, buf_اندازه);

  sclose(dummy);
  إذا(نتیجه)
    أرجع IF2_IP_NOT_یافت;
  أرجع IF2_IP_یافت;
}

#وإلا

if2ip_نتیجه_t Curl_if2ip(صحيح af,
#اگر_تعریف_شده USE_IPV6
                          بیعلامت صحيح remote_scope,
                          بیعلامت صحيح local_scope_id,
#پایان_اگر
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_اندازه)
{
  (فراغ)af;
#اگر_تعریف_شده USE_IPV6
  (فراغ)remote_scope;
  (فراغ)local_scope_id;
#پایان_اگر
  (فراغ)interf;
  (فراغ)buf;
  (فراغ)buf_اندازه;
  أرجع IF2_IP_NOT_یافت;
}

#پایان_اگر

#پایان_اگر /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
