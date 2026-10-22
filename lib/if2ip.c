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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_SOCKIO_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/sockio.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_IFADDRS_H
#ਅਨ੍ਤਰ੍ਭਾਵ <ifaddrs.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STROPTS_H
#ਅਨ੍ਤਰ੍ਭਾਵ <stropts.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_ntop.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "if2ip.h"

/* ------------------------------------------------------------------ */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
/* Return the scope of the given address. */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ Curl_ipv6_scope(ਅਚਲ ਬਣਤਰ sockaddr *sa)
{
  ਜੇ(sa->sa_family == AF_INET6) {
    ਅਚਲ ਬਣਤਰ sockaddr_in6 *sa6 =
      (ਅਚਲ ਬਣਤਰ sockaddr_in6 *)(ਅਚਲ ਖਾਲੀ *)sa;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *b = sa6->sin6_addr.s6_addr;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ w = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)((b[0] << 8) | b[1]);

    ਜੇ((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      ਵਾਪਸ IPV6_SCOPE_UNIQUELOCAL;
    ਚੋਣ(w & 0xFFC0) {
    ਹਾਲਤ 0xFE80:
      ਵਾਪਸ IPV6_SCOPE_LINKLOCAL;
    ਹਾਲਤ 0xFEC0:
      ਵਾਪਸ IPV6_SCOPE_SITELOCAL;
    ਹਾਲਤ 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      ਜੇ(w || b[15] != 0x01)
        ਰੋਕੋ;
      ਵਾਪਸ IPV6_SCOPE_NODELOCAL;
    ਮੂਲ:
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ IPV6_SCOPE_GLOBAL;
}
#ਜੇ_ਅੰਤ

#ਜੇ !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETIFADDRS

if2ip_ਨਤੀਜਾ_t Curl_if2ip(ਪੂਰਨ af,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ remote_scope,
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ local_scope_id,
#ਜੇ_ਅੰਤ
                          ਅਚਲ ਅੱਖਰ *interf,
                          ਅੱਖਰ *buf, size_t buf_ਮਾਪ)
{
  ਬਣਤਰ ifaddrs *iface, *ਸਿਰ;
  if2ip_ਨਤੀਜਾ_t res = IF2_IP_NOT_ਮਿਲਿਆ;

#ਜੇ defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (ਖਾਲੀ)local_scope_id;
#ਜੇ_ਅੰਤ

  ਜੇ(getifaddrs(&ਸਿਰ) >= 0) {
    ਲਈ(iface = ਸਿਰ; iface; iface = iface->ifa_ਅਗਲਾ) {
      ਜੇ(iface->ifa_addr) {
        ਜੇ(iface->ifa_addr->sa_family == af) {
          ਜੇ(!strcmp(iface->ifa_name, interf)) {
            ਖਾਲੀ *addr;
            ਅੱਖਰ scope[12] = "";
            ਅੱਖਰ ipstr[64];
            CURLcode ਨਤੀਜਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
            ਜੇ(af == AF_INET6) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ scopeid = 0;
#ਜੇ_ਅੰਤ
              ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ifscope = Curl_ipv6_scope(iface->ifa_addr);

              ਜੇ(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                ਜੇ(res == IF2_IP_NOT_ਮਿਲਿਆ)
                  res = IF2IP_AF_NOT_SUPPORTED;
                ਜਾਰੀ;
              }

              addr =
                &((ਬਣਤਰ sockaddr_in6 *)(ਖਾਲੀ *)iface->ifa_addr)->sin6_addr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((ਬਣਤਰ sockaddr_in6 *)(ਖਾਲੀ *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              ਜੇ(local_scope_id && scopeid != local_scope_id) {
                ਜੇ(res == IF2_IP_NOT_ਮਿਲਿਆ)
                  res = IF2IP_AF_NOT_SUPPORTED;

                ਜਾਰੀ;
              }

              ਜੇ(scopeid)
                curl_msnprintf(scope, ਆਕਾਰ(scope), "%%%u", scopeid);
#ਜੇ_ਅੰਤ
            }
            ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
              addr =
                &((ਬਣਤਰ sockaddr_in *)(ਖਾਲੀ *)iface->ifa_addr)->sin_addr;
            res = IF2_IP_ਮਿਲਿਆ;
            ਨਤੀਜਾ = curlx_inet_ntop(af, addr, ipstr, ਆਕਾਰ(ipstr));
            curl_msnprintf(buf, buf_ਮਾਪ, "%s%s", ਨਤੀਜਾ ? "" : ipstr, scope);
            ਰੋਕੋ;
          }
        }
        ਨਹੀਂਤਾਂ ਜੇ((res == IF2_IP_NOT_ਮਿਲਿਆ) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(ਸਿਰ);
  }

  ਵਾਪਸ res;
}

#ਜਾਂਜੇ defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_ਨਤੀਜਾ_t Curl_if2ip(ਪੂਰਨ af,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ remote_scope,
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ local_scope_id,
#ਜੇ_ਅੰਤ
                          ਅਚਲ ਅੱਖਰ *interf,
                          ਅੱਖਰ *buf, size_t buf_ਮਾਪ)
{
  ਬਣਤਰ ifreq req;
  ਬਣਤਰ in_addr ਵਿੱਚ;
  ਬਣਤਰ sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode ਨਤੀਜਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  (ਖਾਲੀ)remote_scope;
  (ਖਾਲੀ)local_scope_id;
#ਜੇ_ਅੰਤ

  ਜੇ(!interf || (af != AF_INET))
    ਵਾਪਸ IF2_IP_NOT_ਮਿਲਿਆ;

  len = ਲੜੀ_ਲੰਬਾਈ(interf);
  ਜੇ(len >= ਆਕਾਰ(req.ifr_name))
    ਵਾਪਸ IF2_IP_NOT_ਮਿਲਿਆ;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ਜੇ(dummy == CURL_SOCKET_BAD)
    ਵਾਪਸ IF2_IP_NOT_ਮਿਲਿਆ;

  memset(&req, 0, ਆਕਾਰ(req));
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#ਜੇ defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wshift-sign-overflow"
#ਜੇ_ਅੰਤ
  ਜੇ(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#ਜੇ defined(CURL_HAVE_DIAG) && defined(_AIX)
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    ਵਾਪਸ IF2_IP_NOT_ਮਿਲਿਆ;
  }

  s = (ਬਣਤਰ sockaddr_in *)(ਖਾਲੀ *)&req.ifr_addr;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ਵਿੱਚ, &s->sin_addr, ਆਕਾਰ(ਵਿੱਚ));
  ਨਤੀਜਾ = curlx_inet_ntop(s->sin_family, &ਵਿੱਚ, buf, buf_ਮਾਪ);

  sclose(dummy);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ IF2_IP_NOT_ਮਿਲਿਆ;
  ਵਾਪਸ IF2_IP_ਮਿਲਿਆ;
}

#ਨਹੀਂਤਾਂ

if2ip_ਨਤੀਜਾ_t Curl_if2ip(ਪੂਰਨ af,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ remote_scope,
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ local_scope_id,
#ਜੇ_ਅੰਤ
                          ਅਚਲ ਅੱਖਰ *interf,
                          ਅੱਖਰ *buf, size_t buf_ਮਾਪ)
{
  (ਖਾਲੀ)af;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  (ਖਾਲੀ)remote_scope;
  (ਖਾਲੀ)local_scope_id;
#ਜੇ_ਅੰਤ
  (ਖਾਲੀ)interf;
  (ਖਾਲੀ)buf;
  (ਖਾਲੀ)buf_ਮਾਪ;
  ਵਾਪਸ IF2_IP_NOT_ਮਿਲਿਆ;
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
