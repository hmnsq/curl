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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#അന്തര്ഭാവ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#അന്തര്ഭാവ <sys/sockio.h>
#endif
#ifdef HAVE_IFADDRS_H
#അന്തര്ഭാവ <ifaddrs.h>
#endif
#ifdef HAVE_STROPTS_H
#അന്തര്ഭാവ <stropts.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "curlx/inet_ntop.h"
#അന്തര്ഭാവ "if2ip.h"

/* ------------------------------------------------------------------ */

#ifdef USE_IPV6
/* Return the scope of the given address. */
unsigned പൂർണ്ണസംഖ്യ Curl_ipv6_scope(സ്ഥിരം ഘടന sockaddr *sa)
{
  എങ്കിൽ(sa->sa_family == AF_INET6) {
    സ്ഥിരം ഘടന sockaddr_in6 *sa6 =
      (സ്ഥിരം ഘടന sockaddr_in6 *)(സ്ഥിരം ശൂന്യം *)sa;
    സ്ഥിരം unsigned അക്ഷരം *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    എങ്കിൽ((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      തിരികെ IPV6_SCOPE_UNIQUELOCAL;
    തിരഞ്ഞെടുപ്പ്(w & 0xFFC0) {
    അവസ്ഥ 0xFE80:
      തിരികെ IPV6_SCOPE_LINKLOCAL;
    അവസ്ഥ 0xFEC0:
      തിരികെ IPV6_SCOPE_SITELOCAL;
    അവസ്ഥ 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      എങ്കിൽ(w || b[15] != 0x01)
        നിർത്തുക;
      തിരികെ IPV6_SCOPE_NODELOCAL;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
  }
  തിരികെ IPV6_SCOPE_GLOBAL;
}
#endif

#എങ്കിൽ !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(പൂർണ്ണസംഖ്യ af,
#ifdef USE_IPV6
                          unsigned പൂർണ്ണസംഖ്യ remote_scope,
                          unsigned പൂർണ്ണസംഖ്യ local_scope_id,
#endif
                          സ്ഥിരം അക്ഷരം *interf,
                          അക്ഷരം *buf, size_t buf_size)
{
  ഘടന ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#എങ്കിൽ defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (ശൂന്യം)local_scope_id;
#endif

  എങ്കിൽ(getifaddrs(&head) >= 0) {
    വേണ്ടി(iface = head; iface; iface = iface->ifa_next) {
      എങ്കിൽ(iface->ifa_addr) {
        എങ്കിൽ(iface->ifa_addr->sa_family == af) {
          എങ്കിൽ(!strcmp(iface->ifa_name, interf)) {
            ശൂന്യം *addr;
            അക്ഷരം scope[12] = "";
            അക്ഷരം ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            എങ്കിൽ(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned പൂർണ്ണസംഖ്യ scopeid = 0;
#endif
              unsigned പൂർണ്ണസംഖ്യ ifscope = Curl_ipv6_scope(iface->ifa_addr);

              എങ്കിൽ(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                എങ്കിൽ(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                തുടരുക;
              }

              addr =
                &((ഘടന sockaddr_in6 *)(ശൂന്യം *)iface->ifa_addr)->sin6_addr;
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((ഘടന sockaddr_in6 *)(ശൂന്യം *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              എങ്കിൽ(local_scope_id && scopeid != local_scope_id) {
                എങ്കിൽ(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                തുടരുക;
              }

              എങ്കിൽ(scopeid)
                curl_msnprintf(scope, വലുപ്പം(scope), "%%%u", scopeid);
#endif
            }
            അല്ലെങ്കിൽ
#endif
              addr =
                &((ഘടന sockaddr_in *)(ശൂന്യം *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, വലുപ്പം(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            നിർത്തുക;
          }
        }
        അല്ലെങ്കിൽ എങ്കിൽ((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  തിരികെ res;
}

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(പൂർണ്ണസംഖ്യ af,
#ifdef USE_IPV6
                          unsigned പൂർണ്ണസംഖ്യ remote_scope,
                          unsigned പൂർണ്ണസംഖ്യ local_scope_id,
#endif
                          സ്ഥിരം അക്ഷരം *interf,
                          അക്ഷരം *buf, size_t buf_size)
{
  ഘടന ifreq req;
  ഘടന in_addr in;
  ഘടന sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode result;

#ifdef USE_IPV6
  (ശൂന്യം)remote_scope;
  (ശൂന്യം)local_scope_id;
#endif

  എങ്കിൽ(!interf || (af != AF_INET))
    തിരികെ IF2IP_NOT_FOUND;

  len = strlen(interf);
  എങ്കിൽ(len >= വലുപ്പം(req.ifr_name))
    തിരികെ IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  എങ്കിൽ(dummy == CURL_SOCKET_BAD)
    തിരികെ IF2IP_NOT_FOUND;

  memset(&req, 0, വലുപ്പം(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  എങ്കിൽ(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic എടുക്കുക
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    തിരികെ IF2IP_NOT_FOUND;
  }

  s = (ഘടന sockaddr_in *)(ശൂന്യം *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, വലുപ്പം(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  എങ്കിൽ(result)
    തിരികെ IF2IP_NOT_FOUND;
  തിരികെ IF2IP_FOUND;
}

#അല്ലെങ്കിൽ

if2ip_result_t Curl_if2ip(പൂർണ്ണസംഖ്യ af,
#ifdef USE_IPV6
                          unsigned പൂർണ്ണസംഖ്യ remote_scope,
                          unsigned പൂർണ്ണസംഖ്യ local_scope_id,
#endif
                          സ്ഥിരം അക്ഷരം *interf,
                          അക്ഷരം *buf, size_t buf_size)
{
  (ശൂന്യം)af;
#ifdef USE_IPV6
  (ശൂന്യം)remote_scope;
  (ശൂന്യം)local_scope_id;
#endif
  (ശൂന്യം)interf;
  (ശൂന്യം)buf;
  (ശൂന്യം)buf_size;
  തിരികെ IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
