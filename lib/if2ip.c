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

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#شامل <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#شامل <sys/sockio.h>
#endif
#ifdef HAVE_IFADDRS_H
#شامل <ifaddrs.h>
#endif
#ifdef HAVE_STROPTS_H
#شامل <stropts.h>
#endif
#ifdef __VMS
#شامل <inet.h>
#endif

#شامل "curlx/inet_ntop.h"
#شامل "if2ip.h"

/* ------------------------------------------------------------------ */

#ifdef USE_IPV6
/* Return the scope of the given address. */
unsigned صحیح Curl_ipv6_scope(ثابت ساختار sockaddr *sa)
{
  اگر(sa->sa_family == AF_INET6) {
    ثابت ساختار sockaddr_in6 *sa6 =
      (ثابت ساختار sockaddr_in6 *)(ثابت تهی *)sa;
    ثابت unsigned نویسه *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    اگر((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      بازگشت IPV6_SCOPE_UNIQUELOCAL;
    انتخاب(w & 0xFFC0) {
    حالت 0xFE80:
      بازگشت IPV6_SCOPE_LINKLOCAL;
    حالت 0xFEC0:
      بازگشت IPV6_SCOPE_SITELOCAL;
    حالت 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      اگر(w || b[15] != 0x01)
        بشکن;
      بازگشت IPV6_SCOPE_NODELOCAL;
    پیشفرض:
      بشکن;
    }
  }
  بازگشت IPV6_SCOPE_GLOBAL;
}
#endif

#اگر !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(صحیح af,
#ifdef USE_IPV6
                          unsigned صحیح remote_scope,
                          unsigned صحیح local_scope_id,
#endif
                          ثابت نویسه *interf,
                          نویسه *buf, size_t buf_size)
{
  ساختار ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#اگر defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (تهی)local_scope_id;
#endif

  اگر(getifaddrs(&head) >= 0) {
    برای(iface = head; iface; iface = iface->ifa_next) {
      اگر(iface->ifa_addr) {
        اگر(iface->ifa_addr->sa_family == af) {
          اگر(!strcmp(iface->ifa_name, interf)) {
            تهی *addr;
            نویسه scope[12] = "";
            نویسه ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            اگر(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned صحیح scopeid = 0;
#endif
              unsigned صحیح ifscope = Curl_ipv6_scope(iface->ifa_addr);

              اگر(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                اگر(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                ادامه;
              }

              addr =
                &((ساختار sockaddr_in6 *)(تهی *)iface->ifa_addr)->sin6_addr;
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((ساختار sockaddr_in6 *)(تهی *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              اگر(local_scope_id && scopeid != local_scope_id) {
                اگر(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                ادامه;
              }

              اگر(scopeid)
                curl_msnprintf(scope, اندازه(scope), "%%%u", scopeid);
#endif
            }
            وگرنه
#endif
              addr =
                &((ساختار sockaddr_in *)(تهی *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, اندازه(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            بشکن;
          }
        }
        وگرنه اگر((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  بازگشت res;
}

#وگرنهاگر defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(صحیح af,
#ifdef USE_IPV6
                          unsigned صحیح remote_scope,
                          unsigned صحیح local_scope_id,
#endif
                          ثابت نویسه *interf,
                          نویسه *buf, size_t buf_size)
{
  ساختار ifreq req;
  ساختار in_addr in;
  ساختار sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode result;

#ifdef USE_IPV6
  (تهی)remote_scope;
  (تهی)local_scope_id;
#endif

  اگر(!interf || (af != AF_INET))
    بازگشت IF2IP_NOT_FOUND;

  len = strlen(interf);
  اگر(len >= اندازه(req.ifr_name))
    بازگشت IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  اگر(dummy == CURL_SOCKET_BAD)
    بازگشت IF2IP_NOT_FOUND;

  memset(&req, 0, اندازه(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#اگر defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  اگر(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#اگر defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic برداشت
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    بازگشت IF2IP_NOT_FOUND;
  }

  s = (ساختار sockaddr_in *)(تهی *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, اندازه(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  اگر(result)
    بازگشت IF2IP_NOT_FOUND;
  بازگشت IF2IP_FOUND;
}

#وگرنه

if2ip_result_t Curl_if2ip(صحیح af,
#ifdef USE_IPV6
                          unsigned صحیح remote_scope,
                          unsigned صحیح local_scope_id,
#endif
                          ثابت نویسه *interf,
                          نویسه *buf, size_t buf_size)
{
  (تهی)af;
#ifdef USE_IPV6
  (تهی)remote_scope;
  (تهی)local_scope_id;
#endif
  (تهی)interf;
  (تهی)buf;
  (تهی)buf_size;
  بازگشت IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
