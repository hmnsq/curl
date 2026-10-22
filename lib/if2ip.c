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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাৱ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাৱ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#অন্তর্ভাৱ <sys/sockio.h>
#endif
#ifdef HAVE_IFADDRS_H
#অন্তর্ভাৱ <ifaddrs.h>
#endif
#ifdef HAVE_STROPTS_H
#অন্তর্ভাৱ <stropts.h>
#endif
#ifdef __VMS
#অন্তর্ভাৱ <inet.h>
#endif

#অন্তর্ভাৱ "curlx/inet_ntop.h"
#অন্তর্ভাৱ "if2ip.h"

/* ------------------------------------------------------------------ */

#ifdef USE_IPV6
/* Return the scope of the given address. */
unsigned পূর্ণাঙ্ক Curl_ipv6_scope(ধ্রুৱ সংরচনা sockaddr *sa)
{
  যদি(sa->sa_family == AF_INET6) {
    ধ্রুৱ সংরচনা sockaddr_in6 *sa6 =
      (ধ্রুৱ সংরচনা sockaddr_in6 *)(ধ্রুৱ শূন্য *)sa;
    ধ্রুৱ unsigned ৱর্ণ *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    যদি((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      ঘূৰাই IPV6_SCOPE_UNIQUELOCAL;
    ৱিকল্প(w & 0xFFC0) {
    পক্ষ 0xFE80:
      ঘূৰাই IPV6_SCOPE_LINKLOCAL;
    পক্ষ 0xFEC0:
      ঘূৰাই IPV6_SCOPE_SITELOCAL;
    পক্ষ 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      যদি(w || b[15] != 0x01)
        ভঙ্গ;
      ঘূৰাই IPV6_SCOPE_NODELOCAL;
    স্ৱভাৱ:
      ভঙ্গ;
    }
  }
  ঘূৰাই IPV6_SCOPE_GLOBAL;
}
#endif

#যদি !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(পূর্ণাঙ্ক af,
#ifdef USE_IPV6
                          unsigned পূর্ণাঙ্ক remote_scope,
                          unsigned পূর্ণাঙ্ক local_scope_id,
#endif
                          ধ্রুৱ ৱর্ণ *interf,
                          ৱর্ণ *buf, size_t buf_size)
{
  সংরচনা ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#যদি defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (শূন্য)local_scope_id;
#endif

  যদি(getifaddrs(&head) >= 0) {
    বাবে(iface = head; iface; iface = iface->ifa_next) {
      যদি(iface->ifa_addr) {
        যদি(iface->ifa_addr->sa_family == af) {
          যদি(!strcmp(iface->ifa_name, interf)) {
            শূন্য *addr;
            ৱর্ণ scope[12] = "";
            ৱর্ণ ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            যদি(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned পূর্ণাঙ্ক scopeid = 0;
#endif
              unsigned পূর্ণাঙ্ক ifscope = Curl_ipv6_scope(iface->ifa_addr);

              যদি(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                যদি(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                অনুৱর্ত;
              }

              addr =
                &((সংরচনা sockaddr_in6 *)(শূন্য *)iface->ifa_addr)->sin6_addr;
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((সংরচনা sockaddr_in6 *)(শূন্য *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              যদি(local_scope_id && scopeid != local_scope_id) {
                যদি(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                অনুৱর্ত;
              }

              যদি(scopeid)
                curl_msnprintf(scope, পরিমাণ(scope), "%%%u", scopeid);
#endif
            }
            অন্যথা
#endif
              addr =
                &((সংরচনা sockaddr_in *)(শূন্য *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, পরিমাণ(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            ভঙ্গ;
          }
        }
        অন্যথা যদি((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  ঘূৰাই res;
}

#নোচেদ্যদি defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(পূর্ণাঙ্ক af,
#ifdef USE_IPV6
                          unsigned পূর্ণাঙ্ক remote_scope,
                          unsigned পূর্ণাঙ্ক local_scope_id,
#endif
                          ধ্রুৱ ৱর্ণ *interf,
                          ৱর্ণ *buf, size_t buf_size)
{
  সংরচনা ifreq req;
  সংরচনা in_addr in;
  সংরচনা sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode result;

#ifdef USE_IPV6
  (শূন্য)remote_scope;
  (শূন্য)local_scope_id;
#endif

  যদি(!interf || (af != AF_INET))
    ঘূৰাই IF2IP_NOT_FOUND;

  len = strlen(interf);
  যদি(len >= পরিমাণ(req.ifr_name))
    ঘূৰাই IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  যদি(dummy == CURL_SOCKET_BAD)
    ঘূৰাই IF2IP_NOT_FOUND;

  memset(&req, 0, পরিমাণ(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#যদি defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  যদি(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#যদি defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic বেৰ_কৰো
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    ঘূৰাই IF2IP_NOT_FOUND;
  }

  s = (সংরচনা sockaddr_in *)(শূন্য *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, পরিমাণ(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  যদি(result)
    ঘূৰাই IF2IP_NOT_FOUND;
  ঘূৰাই IF2IP_FOUND;
}

#অন্যথা

if2ip_result_t Curl_if2ip(পূর্ণাঙ্ক af,
#ifdef USE_IPV6
                          unsigned পূর্ণাঙ্ক remote_scope,
                          unsigned পূর্ণাঙ্ক local_scope_id,
#endif
                          ধ্রুৱ ৱর্ণ *interf,
                          ৱর্ণ *buf, size_t buf_size)
{
  (শূন্য)af;
#ifdef USE_IPV6
  (শূন্য)remote_scope;
  (শূন্য)local_scope_id;
#endif
  (শূন্য)interf;
  (শূন্য)buf;
  (শূন্য)buf_size;
  ঘূৰাই IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
