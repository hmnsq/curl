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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_SOCKIO_H
#ಅನ್ತರ್ಭಾವ <sys/sockio.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_IFADDRS_H
#ಅನ್ತರ್ಭಾವ <ifaddrs.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STROPTS_H
#ಅನ್ತರ್ಭಾವ <stropts.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"
#ಅನ್ತರ್ಭಾವ "if2ip.h"

/* ------------------------------------------------------------------ */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
/* Return the scope of the given address. */
ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ Curl_ipv6_scope(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *sa)
{
  ಆದರೆ(sa->sa_family == AF_INET6) {
    ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr_in6 *sa6 =
      (ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr_in6 *)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)sa;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *b = sa6->sin6_addr.s6_addr;
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ w = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)((b[0] << 8) | b[1]);

    ಆದರೆ((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      ಹಿಂತಿರುಗಿ IPV6_SCOPE_UNIQUELOCAL;
    ಆಯ್ಕೆ(w & 0xFFC0) {
    ಸ್ಥಿತಿ 0xFE80:
      ಹಿಂತಿರುಗಿ IPV6_SCOPE_LINKLOCAL;
    ಸ್ಥಿತಿ 0xFEC0:
      ಹಿಂತಿರುಗಿ IPV6_SCOPE_SITELOCAL;
    ಸ್ಥಿತಿ 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      ಆದರೆ(w || b[15] != 0x01)
        ನಿಲ್ಲಿಸು;
      ಹಿಂತಿರುಗಿ IPV6_SCOPE_NODELOCAL;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ IPV6_SCOPE_GLOBAL;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GETIFADDRS

if2ip_ಫಲಿತಾಂಶ_t Curl_if2ip(ಪೂರ್ಣಾಂಕ af,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
                          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ remote_scope,
                          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ local_scope_id,
#ಆದರೆ_ಅಂತ್ಯ
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *interf,
                          ಅಕ್ಷರ *buf, size_t buf_ಗಾತ್ರ)
{
  ರಚನೆ ifaddrs *iface, *ತಲೆ;
  if2ip_ಫಲಿತಾಂಶ_t res = IF2_IP_NOT_ಸಿಕ್ಕಿತು;

#ಆದರೆ defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (ಶೂನ್ಯ)local_scope_id;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(getifaddrs(&ತಲೆ) >= 0) {
    ಗಾಗಿ(iface = ತಲೆ; iface; iface = iface->ifa_ಮುಂದಿನ) {
      ಆದರೆ(iface->ifa_addr) {
        ಆದರೆ(iface->ifa_addr->sa_family == af) {
          ಆದರೆ(!strcmp(iface->ifa_name, interf)) {
            ಶೂನ್ಯ *addr;
            ಅಕ್ಷರ scope[12] = "";
            ಅಕ್ಷರ ipstr[64];
            CURLcode ಫಲಿತಾಂಶ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
            ಆದರೆ(af == AF_INET6) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ scopeid = 0;
#ಆದರೆ_ಅಂತ್ಯ
              ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ifscope = Curl_ipv6_scope(iface->ifa_addr);

              ಆದರೆ(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                ಆದರೆ(res == IF2_IP_NOT_ಸಿಕ್ಕಿತು)
                  res = IF2IP_AF_NOT_SUPPORTED;
                ಮುಂದುವರಿ;
              }

              addr =
                &((ರಚನೆ sockaddr_in6 *)(ಶೂನ್ಯ *)iface->ifa_addr)->sin6_addr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((ರಚನೆ sockaddr_in6 *)(ಶೂನ್ಯ *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              ಆದರೆ(local_scope_id && scopeid != local_scope_id) {
                ಆದರೆ(res == IF2_IP_NOT_ಸಿಕ್ಕಿತು)
                  res = IF2IP_AF_NOT_SUPPORTED;

                ಮುಂದುವರಿ;
              }

              ಆದರೆ(scopeid)
                curl_msnprintf(scope, ಗಾತ್ರ(scope), "%%%u", scopeid);
#ಆದರೆ_ಅಂತ್ಯ
            }
            ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
              addr =
                &((ರಚನೆ sockaddr_in *)(ಶೂನ್ಯ *)iface->ifa_addr)->sin_addr;
            res = IF2_IP_ಸಿಕ್ಕಿತು;
            ಫಲಿತಾಂಶ = curlx_inet_ntop(af, addr, ipstr, ಗಾತ್ರ(ipstr));
            curl_msnprintf(buf, buf_ಗಾತ್ರ, "%s%s", ಫಲಿತಾಂಶ ? "" : ipstr, scope);
            ನಿಲ್ಲಿಸು;
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((res == IF2_IP_NOT_ಸಿಕ್ಕಿತು) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(ತಲೆ);
  }

  ಹಿಂತಿರುಗಿ res;
}

#ಅಥವಾಆದರೆ defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_ಫಲಿತಾಂಶ_t Curl_if2ip(ಪೂರ್ಣಾಂಕ af,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
                          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ remote_scope,
                          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ local_scope_id,
#ಆದರೆ_ಅಂತ್ಯ
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *interf,
                          ಅಕ್ಷರ *buf, size_t buf_ಗಾತ್ರ)
{
  ರಚನೆ ifreq req;
  ರಚನೆ in_addr ಒಳ;
  ರಚನೆ sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  (ಶೂನ್ಯ)remote_scope;
  (ಶೂನ್ಯ)local_scope_id;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!interf || (af != AF_INET))
    ಹಿಂತಿರುಗಿ IF2_IP_NOT_ಸಿಕ್ಕಿತು;

  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(interf);
  ಆದರೆ(len >= ಗಾತ್ರ(req.ifr_name))
    ಹಿಂತಿರುಗಿ IF2_IP_NOT_ಸಿಕ್ಕಿತು;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ಆದರೆ(dummy == CURL_SOCKET_BAD)
    ಹಿಂತಿರುಗಿ IF2_IP_NOT_ಸಿಕ್ಕಿತು;

  memset(&req, 0, ಗಾತ್ರ(req));
  ಸ್ಮರಣೆ_ನಕಲು(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wshift-sign-overflow"
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(_AIX)
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    ಹಿಂತಿರುಗಿ IF2_IP_NOT_ಸಿಕ್ಕಿತು;
  }

  s = (ರಚನೆ sockaddr_in *)(ಶೂನ್ಯ *)&req.ifr_addr;
  ಸ್ಮರಣೆ_ನಕಲು(&ಒಳ, &s->sin_addr, ಗಾತ್ರ(ಒಳ));
  ಫಲಿತಾಂಶ = curlx_inet_ntop(s->sin_family, &ಒಳ, buf, buf_ಗಾತ್ರ);

  sclose(dummy);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ IF2_IP_NOT_ಸಿಕ್ಕಿತು;
  ಹಿಂತಿರುಗಿ IF2_IP_ಸಿಕ್ಕಿತು;
}

#ಇಲ್ಲದಿದ್ದರೆ

if2ip_ಫಲಿತಾಂಶ_t Curl_if2ip(ಪೂರ್ಣಾಂಕ af,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
                          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ remote_scope,
                          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ local_scope_id,
#ಆದರೆ_ಅಂತ್ಯ
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *interf,
                          ಅಕ್ಷರ *buf, size_t buf_ಗಾತ್ರ)
{
  (ಶೂನ್ಯ)af;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  (ಶೂನ್ಯ)remote_scope;
  (ಶೂನ್ಯ)local_scope_id;
#ಆದರೆ_ಅಂತ್ಯ
  (ಶೂನ್ಯ)interf;
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)buf_ಗಾತ್ರ;
  ಹಿಂತಿರುಗಿ IF2_IP_NOT_ಸಿಕ್ಕಿತು;
}

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
