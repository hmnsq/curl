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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_SOCKIO_H
#समावेश <sys/sockio.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_IFADDRS_H
#समावेश <ifaddrs.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_STROPTS_H
#समावेश <stropts.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "curlx/inet_ntop.h"
#समावेश "if2ip.h"

/* ------------------------------------------------------------------ */

#अगर_घोषित USE_IPV6
/* Return the scope of the given address. */
अचिन्हित पूर्णांक Curl_ipv6_scope(स्थिर काष्ठा sockaddr *sa)
{
  अगर(sa->sa_family == AF_INET6) {
    स्थिर काष्ठा sockaddr_in6 *sa6 =
      (स्थिर काष्ठा sockaddr_in6 *)(स्थिर व्योम *)sa;
    स्थिर अचिन्हित अक्षर *b = sa6->sin6_addr.s6_addr;
    अचिन्हित लघु w = (अचिन्हित लघु)((b[0] << 8) | b[1]);

    अगर((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      वापस IPV6_SCOPE_UNIQUELOCAL;
    चयन(w & 0xFFC0) {
    हाल 0xFE80:
      वापस IPV6_SCOPE_LINKLOCAL;
    हाल 0xFEC0:
      वापस IPV6_SCOPE_SITELOCAL;
    हाल 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      अगर(w || b[15] != 0x01)
        अवरोध;
      वापस IPV6_SCOPE_NODELOCAL;
    शेष:
      अवरोध;
    }
  }
  वापस IPV6_SCOPE_GLOBAL;
}
#पूर्ण_अगर

#अगर !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#अगर_घोषित HAVE_GETIFADDRS

if2ip_परिणाम_t Curl_if2ip(पूर्णांक af,
#अगर_घोषित USE_IPV6
                          अचिन्हित पूर्णांक remote_scope,
                          अचिन्हित पूर्णांक local_scope_id,
#पूर्ण_अगर
                          स्थिर अक्षर *interf,
                          अक्षर *buf, माप_प्रकार buf_आकार)
{
  काष्ठा ifaddrs *iface, *शीर्ष;
  if2ip_परिणाम_t res = IF2IP_NOT_FOUND;

#अगर defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (व्योम)local_scope_id;
#पूर्ण_अगर

  अगर(getifaddrs(&शीर्ष) >= 0) {
    क्रम(iface = शीर्ष; iface; iface = iface->ifa_अग्र) {
      अगर(iface->ifa_addr) {
        अगर(iface->ifa_addr->sa_family == af) {
          अगर(!म_भेद(iface->ifa_name, interf)) {
            व्योम *addr;
            अक्षर scope[12] = "";
            अक्षर ipstr[64];
            CURLcode परिणाम;
#अगर_घोषित USE_IPV6
            अगर(af == AF_INET6) {
#अगर_घोषित HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              अचिन्हित पूर्णांक scopeid = 0;
#पूर्ण_अगर
              अचिन्हित पूर्णांक ifscope = Curl_ipv6_scope(iface->ifa_addr);

              अगर(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                अगर(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                जारी;
              }

              addr =
                &((काष्ठा sockaddr_in6 *)(व्योम *)iface->ifa_addr)->sin6_addr;
#अगर_घोषित HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((काष्ठा sockaddr_in6 *)(व्योम *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              अगर(local_scope_id && scopeid != local_scope_id) {
                अगर(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                जारी;
              }

              अगर(scopeid)
                curl_msnprintf(scope, माप(scope), "%%%u", scopeid);
#पूर्ण_अगर
            }
            अन्यथा
#पूर्ण_अगर
              addr =
                &((काष्ठा sockaddr_in *)(व्योम *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            परिणाम = curlx_inet_ntop(af, addr, ipstr, माप(ipstr));
            curl_msnprintf(buf, buf_आकार, "%s%s", परिणाम ? "" : ipstr, scope);
            अवरोध;
          }
        }
        अन्यथा अगर((res == IF2IP_NOT_FOUND) &&
                !म_भेद(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(शीर्ष);
  }

  वापस res;
}

#या_अगर defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_परिणाम_t Curl_if2ip(पूर्णांक af,
#अगर_घोषित USE_IPV6
                          अचिन्हित पूर्णांक remote_scope,
                          अचिन्हित पूर्णांक local_scope_id,
#पूर्ण_अगर
                          स्थिर अक्षर *interf,
                          अक्षर *buf, माप_प्रकार buf_आकार)
{
  काष्ठा ifreq req;
  काष्ठा in_addr in;
  काष्ठा sockaddr_in *s;
  curl_socket_t dummy;
  माप_प्रकार len;
  CURLcode परिणाम;

#अगर_घोषित USE_IPV6
  (व्योम)remote_scope;
  (व्योम)local_scope_id;
#पूर्ण_अगर

  अगर(!interf || (af != AF_INET))
    वापस IF2IP_NOT_FOUND;

  len = म_माप(interf);
  अगर(len >= माप(req.ifr_name))
    वापस IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  अगर(dummy == CURL_SOCKET_BAD)
    वापस IF2IP_NOT_FOUND;

  स_रखो(&req, 0, माप(req));
  स_नकल(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#अगर defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wshift-sign-overflow"
#पूर्ण_अगर
  अगर(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#अगर defined(CURL_HAVE_DIAG) && defined(_AIX)
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    वापस IF2IP_NOT_FOUND;
  }

  s = (काष्ठा sockaddr_in *)(व्योम *)&req.ifr_addr;
  स_नकल(&in, &s->sin_addr, माप(in));
  परिणाम = curlx_inet_ntop(s->sin_family, &in, buf, buf_आकार);

  sclose(dummy);
  अगर(परिणाम)
    वापस IF2IP_NOT_FOUND;
  वापस IF2IP_FOUND;
}

#अन्यथा

if2ip_परिणाम_t Curl_if2ip(पूर्णांक af,
#अगर_घोषित USE_IPV6
                          अचिन्हित पूर्णांक remote_scope,
                          अचिन्हित पूर्णांक local_scope_id,
#पूर्ण_अगर
                          स्थिर अक्षर *interf,
                          अक्षर *buf, माप_प्रकार buf_आकार)
{
  (व्योम)af;
#अगर_घोषित USE_IPV6
  (व्योम)remote_scope;
  (व्योम)local_scope_id;
#पूर्ण_अगर
  (व्योम)interf;
  (व्योम)buf;
  (व्योम)buf_आकार;
  वापस IF2IP_NOT_FOUND;
}

#पूर्ण_अगर

#पूर्ण_अगर /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
