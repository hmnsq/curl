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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#அந்தர்பாவ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#அந்தர்பாவ <sys/sockio.h>
#endif
#ifdef HAVE_IFADDRS_H
#அந்தர்பாவ <ifaddrs.h>
#endif
#ifdef HAVE_STROPTS_H
#அந்தர்பாவ <stropts.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ "curlx/inet_ntop.h"
#அந்தர்பாவ "if2ip.h"

/* ------------------------------------------------------------------ */

#ifdef USE_IPV6
/* Return the scope of the given address. */
unsigned முழு Curl_ipv6_scope(மாறிலி கட்டமைப்பு sockaddr *sa)
{
  எனில்(sa->sa_family == AF_INET6) {
    மாறிலி கட்டமைப்பு sockaddr_in6 *sa6 =
      (மாறிலி கட்டமைப்பு sockaddr_in6 *)(மாறிலி வெறுமை *)sa;
    மாறிலி unsigned எழுத்து *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    எனில்((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      திரும்பு IPV6_SCOPE_UNIQUELOCAL;
    தேர்வு(w & 0xFFC0) {
    நிலை 0xFE80:
      திரும்பு IPV6_SCOPE_LINKLOCAL;
    நிலை 0xFEC0:
      திரும்பு IPV6_SCOPE_SITELOCAL;
    நிலை 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      எனில்(w || b[15] != 0x01)
        நிறுத்து;
      திரும்பு IPV6_SCOPE_NODELOCAL;
    இயல்பு:
      நிறுத்து;
    }
  }
  திரும்பு IPV6_SCOPE_GLOBAL;
}
#endif

#எனில் !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(முழு af,
#ifdef USE_IPV6
                          unsigned முழு remote_scope,
                          unsigned முழு local_scope_id,
#endif
                          மாறிலி எழுத்து *interf,
                          எழுத்து *buf, size_t buf_size)
{
  கட்டமைப்பு ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#எனில் defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (வெறுமை)local_scope_id;
#endif

  எனில்(getifaddrs(&head) >= 0) {
    க்கு(iface = head; iface; iface = iface->ifa_next) {
      எனில்(iface->ifa_addr) {
        எனில்(iface->ifa_addr->sa_family == af) {
          எனில்(!strcmp(iface->ifa_name, interf)) {
            வெறுமை *addr;
            எழுத்து scope[12] = "";
            எழுத்து ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            எனில்(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned முழு scopeid = 0;
#endif
              unsigned முழு ifscope = Curl_ipv6_scope(iface->ifa_addr);

              எனில்(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                எனில்(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                தொடர்;
              }

              addr =
                &((கட்டமைப்பு sockaddr_in6 *)(வெறுமை *)iface->ifa_addr)->sin6_addr;
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((கட்டமைப்பு sockaddr_in6 *)(வெறுமை *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              எனில்(local_scope_id && scopeid != local_scope_id) {
                எனில்(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                தொடர்;
              }

              எனில்(scopeid)
                curl_msnprintf(scope, அளவு(scope), "%%%u", scopeid);
#endif
            }
            இல்லையெனில்
#endif
              addr =
                &((கட்டமைப்பு sockaddr_in *)(வெறுமை *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, அளவு(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            நிறுத்து;
          }
        }
        இல்லையெனில் எனில்((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  திரும்பு res;
}

#அல்லதெனில் defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(முழு af,
#ifdef USE_IPV6
                          unsigned முழு remote_scope,
                          unsigned முழு local_scope_id,
#endif
                          மாறிலி எழுத்து *interf,
                          எழுத்து *buf, size_t buf_size)
{
  கட்டமைப்பு ifreq req;
  கட்டமைப்பு in_addr in;
  கட்டமைப்பு sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode result;

#ifdef USE_IPV6
  (வெறுமை)remote_scope;
  (வெறுமை)local_scope_id;
#endif

  எனில்(!interf || (af != AF_INET))
    திரும்பு IF2IP_NOT_FOUND;

  len = strlen(interf);
  எனில்(len >= அளவு(req.ifr_name))
    திரும்பு IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  எனில்(dummy == CURL_SOCKET_BAD)
    திரும்பு IF2IP_NOT_FOUND;

  memset(&req, 0, அளவு(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#எனில் defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  எனில்(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#எனில் defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic எடு
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    திரும்பு IF2IP_NOT_FOUND;
  }

  s = (கட்டமைப்பு sockaddr_in *)(வெறுமை *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, அளவு(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  எனில்(result)
    திரும்பு IF2IP_NOT_FOUND;
  திரும்பு IF2IP_FOUND;
}

#இல்லையெனில்

if2ip_result_t Curl_if2ip(முழு af,
#ifdef USE_IPV6
                          unsigned முழு remote_scope,
                          unsigned முழு local_scope_id,
#endif
                          மாறிலி எழுத்து *interf,
                          எழுத்து *buf, size_t buf_size)
{
  (வெறுமை)af;
#ifdef USE_IPV6
  (வெறுமை)remote_scope;
  (வெறுமை)local_scope_id;
#endif
  (வெறுமை)interf;
  (வெறுமை)buf;
  (வெறுமை)buf_size;
  திரும்பு IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
