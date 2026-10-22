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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#अन्तर्भाव <sys/sockio.h>
#endif
#ifdef HAVE_IFADDRS_H
#अन्तर्भाव <ifaddrs.h>
#endif
#ifdef HAVE_STROPTS_H
#अन्तर्भाव <stropts.h>
#endif
#ifdef __VMS
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "curlx/inet_ntop.h"
#अन्तर्भाव "if2ip.h"

/* ------------------------------------------------------------------ */

#ifdef USE_IPV6
/* Return the scope of the given address. */
unsigned पूर्णाङ्क Curl_ipv6_scope(ध्रुव संरचना sockaddr *sa)
{
  यदि(sa->sa_family == AF_INET6) {
    ध्रुव संरचना sockaddr_in6 *sa6 =
      (ध्रुव संरचना sockaddr_in6 *)(ध्रुव शून्य *)sa;
    ध्रुव unsigned वर्ण *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    यदि((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      प्रत्यागम IPV6_SCOPE_UNIQUELOCAL;
    विकल्प(w & 0xFFC0) {
    पक्ष 0xFE80:
      प्रत्यागम IPV6_SCOPE_LINKLOCAL;
    पक्ष 0xFEC0:
      प्रत्यागम IPV6_SCOPE_SITELOCAL;
    पक्ष 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      यदि(w || b[15] != 0x01)
        भङ्ग;
      प्रत्यागम IPV6_SCOPE_NODELOCAL;
    स्वभाव:
      भङ्ग;
    }
  }
  प्रत्यागम IPV6_SCOPE_GLOBAL;
}
#endif

#यदि !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(पूर्णाङ्क af,
#ifdef USE_IPV6
                          unsigned पूर्णाङ्क remote_scope,
                          unsigned पूर्णाङ्क local_scope_id,
#endif
                          ध्रुव वर्ण *interf,
                          वर्ण *buf, size_t buf_size)
{
  संरचना ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#यदि defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (शून्य)local_scope_id;
#endif

  यदि(getifaddrs(&head) >= 0) {
    प्रति(iface = head; iface; iface = iface->ifa_next) {
      यदि(iface->ifa_addr) {
        यदि(iface->ifa_addr->sa_family == af) {
          यदि(!strcmp(iface->ifa_name, interf)) {
            शून्य *addr;
            वर्ण scope[12] = "";
            वर्ण ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            यदि(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned पूर्णाङ्क scopeid = 0;
#endif
              unsigned पूर्णाङ्क ifscope = Curl_ipv6_scope(iface->ifa_addr);

              यदि(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                यदि(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                अनुवर्त;
              }

              addr =
                &((संरचना sockaddr_in6 *)(शून्य *)iface->ifa_addr)->sin6_addr;
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((संरचना sockaddr_in6 *)(शून्य *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              यदि(local_scope_id && scopeid != local_scope_id) {
                यदि(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                अनुवर्त;
              }

              यदि(scopeid)
                curl_msnprintf(scope, परिमाण(scope), "%%%u", scopeid);
#endif
            }
            अन्यथा
#endif
              addr =
                &((संरचना sockaddr_in *)(शून्य *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, परिमाण(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            भङ्ग;
          }
        }
        अन्यथा यदि((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  प्रत्यागम res;
}

#नोचेद्यदि defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(पूर्णाङ्क af,
#ifdef USE_IPV6
                          unsigned पूर्णाङ्क remote_scope,
                          unsigned पूर्णाङ्क local_scope_id,
#endif
                          ध्रुव वर्ण *interf,
                          वर्ण *buf, size_t buf_size)
{
  संरचना ifreq req;
  संरचना in_addr in;
  संरचना sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode result;

#ifdef USE_IPV6
  (शून्य)remote_scope;
  (शून्य)local_scope_id;
#endif

  यदि(!interf || (af != AF_INET))
    प्रत्यागम IF2IP_NOT_FOUND;

  len = strlen(interf);
  यदि(len >= परिमाण(req.ifr_name))
    प्रत्यागम IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  यदि(dummy == CURL_SOCKET_BAD)
    प्रत्यागम IF2IP_NOT_FOUND;

  memset(&req, 0, परिमाण(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#यदि defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic धकेलो
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  यदि(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#यदि defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic निकालो
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    प्रत्यागम IF2IP_NOT_FOUND;
  }

  s = (संरचना sockaddr_in *)(शून्य *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, परिमाण(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  यदि(result)
    प्रत्यागम IF2IP_NOT_FOUND;
  प्रत्यागम IF2IP_FOUND;
}

#अन्यथा

if2ip_result_t Curl_if2ip(पूर्णाङ्क af,
#ifdef USE_IPV6
                          unsigned पूर्णाङ्क remote_scope,
                          unsigned पूर्णाङ्क local_scope_id,
#endif
                          ध्रुव वर्ण *interf,
                          वर्ण *buf, size_t buf_size)
{
  (शून्य)af;
#ifdef USE_IPV6
  (शून्य)remote_scope;
  (शून्य)local_scope_id;
#endif
  (शून्य)interf;
  (शून्य)buf;
  (शून्य)buf_size;
  प्रत्यागम IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
