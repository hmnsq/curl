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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#ಅನ್ತರ್ಭಾವ <sys/sockio.h>
#endif
#ifdef HAVE_IFADDRS_H
#ಅನ್ತರ್ಭಾವ <ifaddrs.h>
#endif
#ifdef HAVE_STROPTS_H
#ಅನ್ತರ್ಭಾವ <stropts.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"
#ಅನ್ತರ್ಭಾವ "if2ip.h"

/* ------------------------------------------------------------------ */

#ifdef USE_IPV6
/* Return the scope of the given address. */
unsigned ಪೂರ್ಣಾಙ್ಕ Curl_ipv6_scope(ಧ್ರುವ ಸಂರಚನಾ sockaddr *sa)
{
  ಯದಿ(sa->sa_family == AF_INET6) {
    ಧ್ರುವ ಸಂರಚನಾ sockaddr_in6 *sa6 =
      (ಧ್ರುವ ಸಂರಚನಾ sockaddr_in6 *)(ಧ್ರುವ ಶೂನ್ಯ *)sa;
    ಧ್ರುವ unsigned ವರ್ಣ *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    ಯದಿ((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      ಪ್ರತ್ಯಾಗಮ IPV6_SCOPE_UNIQUELOCAL;
    ವಿಕಲ್ಪ(w & 0xFFC0) {
    ಪಕ್ಷ 0xFE80:
      ಪ್ರತ್ಯಾಗಮ IPV6_SCOPE_LINKLOCAL;
    ಪಕ್ಷ 0xFEC0:
      ಪ್ರತ್ಯಾಗಮ IPV6_SCOPE_SITELOCAL;
    ಪಕ್ಷ 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      ಯದಿ(w || b[15] != 0x01)
        ಭಙ್ಗ;
      ಪ್ರತ್ಯಾಗಮ IPV6_SCOPE_NODELOCAL;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ IPV6_SCOPE_GLOBAL;
}
#endif

#ಯದಿ !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(ಪೂರ್ಣಾಙ್ಕ af,
#ifdef USE_IPV6
                          unsigned ಪೂರ್ಣಾಙ್ಕ remote_scope,
                          unsigned ಪೂರ್ಣಾಙ್ಕ local_scope_id,
#endif
                          ಧ್ರುವ ವರ್ಣ *interf,
                          ವರ್ಣ *buf, size_t buf_size)
{
  ಸಂರಚನಾ ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#ಯದಿ defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (ಶೂನ್ಯ)local_scope_id;
#endif

  ಯದಿ(getifaddrs(&head) >= 0) {
    ಪ್ರತಿ(iface = head; iface; iface = iface->ifa_next) {
      ಯದಿ(iface->ifa_addr) {
        ಯದಿ(iface->ifa_addr->sa_family == af) {
          ಯದಿ(!strcmp(iface->ifa_name, interf)) {
            ಶೂನ್ಯ *addr;
            ವರ್ಣ scope[12] = "";
            ವರ್ಣ ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            ಯದಿ(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned ಪೂರ್ಣಾಙ್ಕ scopeid = 0;
#endif
              unsigned ಪೂರ್ಣಾಙ್ಕ ifscope = Curl_ipv6_scope(iface->ifa_addr);

              ಯದಿ(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                ಯದಿ(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                ಅನುವರ್ತ;
              }

              addr =
                &((ಸಂರಚನಾ sockaddr_in6 *)(ಶೂನ್ಯ *)iface->ifa_addr)->sin6_addr;
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((ಸಂರಚನಾ sockaddr_in6 *)(ಶೂನ್ಯ *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              ಯದಿ(local_scope_id && scopeid != local_scope_id) {
                ಯದಿ(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                ಅನುವರ್ತ;
              }

              ಯದಿ(scopeid)
                curl_msnprintf(scope, ಪರಿಮಾಣ(scope), "%%%u", scopeid);
#endif
            }
            ಅನ್ಯಥಾ
#endif
              addr =
                &((ಸಂರಚನಾ sockaddr_in *)(ಶೂನ್ಯ *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, ಪರಿಮಾಣ(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            ಭಙ್ಗ;
          }
        }
        ಅನ್ಯಥಾ ಯದಿ((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  ಪ್ರತ್ಯಾಗಮ res;
}

#ನೋಚೇದ್ಯದಿ defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(ಪೂರ್ಣಾಙ್ಕ af,
#ifdef USE_IPV6
                          unsigned ಪೂರ್ಣಾಙ್ಕ remote_scope,
                          unsigned ಪೂರ್ಣಾಙ್ಕ local_scope_id,
#endif
                          ಧ್ರುವ ವರ್ಣ *interf,
                          ವರ್ಣ *buf, size_t buf_size)
{
  ಸಂರಚನಾ ifreq req;
  ಸಂರಚನಾ in_addr in;
  ಸಂರಚನಾ sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode result;

#ifdef USE_IPV6
  (ಶೂನ್ಯ)remote_scope;
  (ಶೂನ್ಯ)local_scope_id;
#endif

  ಯದಿ(!interf || (af != AF_INET))
    ಪ್ರತ್ಯಾಗಮ IF2IP_NOT_FOUND;

  len = strlen(interf);
  ಯದಿ(len >= ಪರಿಮಾಣ(req.ifr_name))
    ಪ್ರತ್ಯಾಗಮ IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ಯದಿ(dummy == CURL_SOCKET_BAD)
    ಪ್ರತ್ಯಾಗಮ IF2IP_NOT_FOUND;

  memset(&req, 0, ಪರಿಮಾಣ(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#ಯದಿ defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  ಯದಿ(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#ಯದಿ defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic ತೆಗೆ
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    ಪ್ರತ್ಯಾಗಮ IF2IP_NOT_FOUND;
  }

  s = (ಸಂರಚನಾ sockaddr_in *)(ಶೂನ್ಯ *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, ಪರಿಮಾಣ(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ IF2IP_NOT_FOUND;
  ಪ್ರತ್ಯಾಗಮ IF2IP_FOUND;
}

#ಅನ್ಯಥಾ

if2ip_result_t Curl_if2ip(ಪೂರ್ಣಾಙ್ಕ af,
#ifdef USE_IPV6
                          unsigned ಪೂರ್ಣಾಙ್ಕ remote_scope,
                          unsigned ಪೂರ್ಣಾಙ್ಕ local_scope_id,
#endif
                          ಧ್ರುವ ವರ್ಣ *interf,
                          ವರ್ಣ *buf, size_t buf_size)
{
  (ಶೂನ್ಯ)af;
#ifdef USE_IPV6
  (ಶೂನ್ಯ)remote_scope;
  (ಶೂನ್ಯ)local_scope_id;
#endif
  (ಶೂನ್ಯ)interf;
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)buf_size;
  ಪ್ರತ್ಯಾಗಮ IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
