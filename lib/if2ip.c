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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NET_IF_H
#شامل <net/if.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_SOCKIO_H
#شامل <sys/sockio.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_IFADDRS_H
#شامل <ifaddrs.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_STROPTS_H
#شامل <stropts.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <inet.h>
#اگر_ختم

#شامل "curlx/inet_ntop.h"
#شامل "if2ip.h"

/* ------------------------------------------------------------------ */

#اگر_تعریف_شدہ USE_IPV6
/* Return the scope of the given address. */
غیرعلامتی صحیح Curl_ipv6_scope(ثابت ساخت sockaddr *sa)
{
  اگر(sa->sa_family == AF_INET6) {
    ثابت ساخت sockaddr_in6 *sa6 =
      (ثابت ساخت sockaddr_in6 *)(ثابت خالی *)sa;
    ثابت غیرعلامتی حرف *b = sa6->sin6_addr.s6_addr;
    غیرعلامتی مختصر w = (غیرعلامتی مختصر)((b[0] << 8) | b[1]);

    اگر((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      واپس IPV6_SCOPE_UNIQUELOCAL;
    انتخاب(w & 0xFFC0) {
    حالت 0xFE80:
      واپس IPV6_SCOPE_LINKLOCAL;
    حالت 0xFEC0:
      واپس IPV6_SCOPE_SITELOCAL;
    حالت 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      اگر(w || b[15] != 0x01)
        توڑو;
      واپس IPV6_SCOPE_NODELOCAL;
    طےشدہ:
      توڑو;
    }
  }
  واپس IPV6_SCOPE_GLOBAL;
}
#اگر_ختم

#اگر !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#اگر_تعریف_شدہ HAVE_GETIFADDRS

if2ip_نتیجہ_t Curl_if2ip(صحیح af,
#اگر_تعریف_شدہ USE_IPV6
                          غیرعلامتی صحیح remote_scope,
                          غیرعلامتی صحیح local_scope_id,
#اگر_ختم
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_سائز)
{
  ساخت ifaddrs *iface, *سر;
  if2ip_نتیجہ_t res = IF2_IP_NOT_ملا;

#اگر defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (خالی)local_scope_id;
#اگر_ختم

  اگر(getifaddrs(&سر) >= 0) {
    برائے(iface = سر; iface; iface = iface->ifa_اگلا) {
      اگر(iface->ifa_addr) {
        اگر(iface->ifa_addr->sa_family == af) {
          اگر(!strcmp(iface->ifa_name, interf)) {
            خالی *addr;
            حرف scope[12] = "";
            حرف ipstr[64];
            CURLcode نتیجہ;
#اگر_تعریف_شدہ USE_IPV6
            اگر(af == AF_INET6) {
#اگر_تعریف_شدہ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              غیرعلامتی صحیح scopeid = 0;
#اگر_ختم
              غیرعلامتی صحیح ifscope = Curl_ipv6_scope(iface->ifa_addr);

              اگر(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                اگر(res == IF2_IP_NOT_ملا)
                  res = IF2IP_AF_NOT_SUPPORTED;
                جاری;
              }

              addr =
                &((ساخت sockaddr_in6 *)(خالی *)iface->ifa_addr)->sin6_addr;
#اگر_تعریف_شدہ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((ساخت sockaddr_in6 *)(خالی *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              اگر(local_scope_id && scopeid != local_scope_id) {
                اگر(res == IF2_IP_NOT_ملا)
                  res = IF2IP_AF_NOT_SUPPORTED;

                جاری;
              }

              اگر(scopeid)
                curl_msnprintf(scope, حجم(scope), "%%%u", scopeid);
#اگر_ختم
            }
            ورنہ
#اگر_ختم
              addr =
                &((ساخت sockaddr_in *)(خالی *)iface->ifa_addr)->sin_addr;
            res = IF2_IP_ملا;
            نتیجہ = curlx_inet_ntop(af, addr, ipstr, حجم(ipstr));
            curl_msnprintf(buf, buf_سائز, "%s%s", نتیجہ ? "" : ipstr, scope);
            توڑو;
          }
        }
        ورنہ اگر((res == IF2_IP_NOT_ملا) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(سر);
  }

  واپس res;
}

#وگرنہ_اگر defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_نتیجہ_t Curl_if2ip(صحیح af,
#اگر_تعریف_شدہ USE_IPV6
                          غیرعلامتی صحیح remote_scope,
                          غیرعلامتی صحیح local_scope_id,
#اگر_ختم
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_سائز)
{
  ساخت ifreq req;
  ساخت in_addr اندر;
  ساخت sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode نتیجہ;

#اگر_تعریف_شدہ USE_IPV6
  (خالی)remote_scope;
  (خالی)local_scope_id;
#اگر_ختم

  اگر(!interf || (af != AF_INET))
    واپس IF2_IP_NOT_ملا;

  len = لڑی_لمبائی(interf);
  اگر(len >= حجم(req.ifr_name))
    واپس IF2_IP_NOT_ملا;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  اگر(dummy == CURL_SOCKET_BAD)
    واپس IF2_IP_NOT_ملا;

  memset(&req, 0, حجم(req));
  حافظہ_نقل(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#اگر defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wshift-sign-overflow"
#اگر_ختم
  اگر(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#اگر defined(CURL_HAVE_DIAG) && defined(_AIX)
#ہدایت GCC diagnostic نکالو
#اگر_ختم
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    واپس IF2_IP_NOT_ملا;
  }

  s = (ساخت sockaddr_in *)(خالی *)&req.ifr_addr;
  حافظہ_نقل(&اندر, &s->sin_addr, حجم(اندر));
  نتیجہ = curlx_inet_ntop(s->sin_family, &اندر, buf, buf_سائز);

  sclose(dummy);
  اگر(نتیجہ)
    واپس IF2_IP_NOT_ملا;
  واپس IF2_IP_ملا;
}

#ورنہ

if2ip_نتیجہ_t Curl_if2ip(صحیح af,
#اگر_تعریف_شدہ USE_IPV6
                          غیرعلامتی صحیح remote_scope,
                          غیرعلامتی صحیح local_scope_id,
#اگر_ختم
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_سائز)
{
  (خالی)af;
#اگر_تعریف_شدہ USE_IPV6
  (خالی)remote_scope;
  (خالی)local_scope_id;
#اگر_ختم
  (خالی)interf;
  (خالی)buf;
  (خالی)buf_سائز;
  واپس IF2_IP_NOT_ملا;
}

#اگر_ختم

#اگر_ختم /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
