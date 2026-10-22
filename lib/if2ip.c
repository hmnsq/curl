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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#અન્તર્ભાવ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#અન્તર્ભાવ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#અન્તર્ભાવ <sys/sockio.h>
#endif
#ifdef HAVE_IFADDRS_H
#અન્તર્ભાવ <ifaddrs.h>
#endif
#ifdef HAVE_STROPTS_H
#અન્તર્ભાવ <stropts.h>
#endif
#ifdef __VMS
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ "curlx/inet_ntop.h"
#અન્તર્ભાવ "if2ip.h"

/* ------------------------------------------------------------------ */

#ifdef USE_IPV6
/* Return the scope of the given address. */
unsigned પૂર્ણાંક Curl_ipv6_scope(અચલ સંરચના sockaddr *sa)
{
  જો(sa->sa_family == AF_INET6) {
    અચલ સંરચના sockaddr_in6 *sa6 =
      (અચલ સંરચના sockaddr_in6 *)(અચલ શૂન્ય *)sa;
    અચલ unsigned અક્ષર *b = sa6->sin6_addr.s6_addr;
    unsigned short w = (unsigned short)((b[0] << 8) | b[1]);

    જો((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      પાછા IPV6_SCOPE_UNIQUELOCAL;
    પસંદગી(w & 0xFFC0) {
    સ્થિતિ 0xFE80:
      પાછા IPV6_SCOPE_LINKLOCAL;
    સ્થિતિ 0xFEC0:
      પાછા IPV6_SCOPE_SITELOCAL;
    સ્થિતિ 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      જો(w || b[15] != 0x01)
        તોડો;
      પાછા IPV6_SCOPE_NODELOCAL;
    મૂળભૂત:
      તોડો;
    }
  }
  પાછા IPV6_SCOPE_GLOBAL;
}
#endif

#જો !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#ifdef HAVE_GETIFADDRS

if2ip_result_t Curl_if2ip(પૂર્ણાંક af,
#ifdef USE_IPV6
                          unsigned પૂર્ણાંક remote_scope,
                          unsigned પૂર્ણાંક local_scope_id,
#endif
                          અચલ અક્ષર *interf,
                          અક્ષર *buf, size_t buf_size)
{
  સંરચના ifaddrs *iface, *head;
  if2ip_result_t res = IF2IP_NOT_FOUND;

#જો defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (શૂન્ય)local_scope_id;
#endif

  જો(getifaddrs(&head) >= 0) {
    માટે(iface = head; iface; iface = iface->ifa_next) {
      જો(iface->ifa_addr) {
        જો(iface->ifa_addr->sa_family == af) {
          જો(!strcmp(iface->ifa_name, interf)) {
            શૂન્ય *addr;
            અક્ષર scope[12] = "";
            અક્ષર ipstr[64];
            CURLcode result;
#ifdef USE_IPV6
            જો(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              unsigned પૂર્ણાંક scopeid = 0;
#endif
              unsigned પૂર્ણાંક ifscope = Curl_ipv6_scope(iface->ifa_addr);

              જો(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                જો(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;
                ચાલુ;
              }

              addr =
                &((સંરચના sockaddr_in6 *)(શૂન્ય *)iface->ifa_addr)->sin6_addr;
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((સંરચના sockaddr_in6 *)(શૂન્ય *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              જો(local_scope_id && scopeid != local_scope_id) {
                જો(res == IF2IP_NOT_FOUND)
                  res = IF2IP_AF_NOT_SUPPORTED;

                ચાલુ;
              }

              જો(scopeid)
                curl_msnprintf(scope, કદ(scope), "%%%u", scopeid);
#endif
            }
            નહીંતર
#endif
              addr =
                &((સંરચના sockaddr_in *)(શૂન્ય *)iface->ifa_addr)->sin_addr;
            res = IF2IP_FOUND;
            result = curlx_inet_ntop(af, addr, ipstr, કદ(ipstr));
            curl_msnprintf(buf, buf_size, "%s%s", result ? "" : ipstr, scope);
            તોડો;
          }
        }
        નહીંતર જો((res == IF2IP_NOT_FOUND) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(head);
  }

  પાછા res;
}

#અથવાજો defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_result_t Curl_if2ip(પૂર્ણાંક af,
#ifdef USE_IPV6
                          unsigned પૂર્ણાંક remote_scope,
                          unsigned પૂર્ણાંક local_scope_id,
#endif
                          અચલ અક્ષર *interf,
                          અક્ષર *buf, size_t buf_size)
{
  સંરચના ifreq req;
  સંરચના in_addr in;
  સંરચના sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode result;

#ifdef USE_IPV6
  (શૂન્ય)remote_scope;
  (શૂન્ય)local_scope_id;
#endif

  જો(!interf || (af != AF_INET))
    પાછા IF2IP_NOT_FOUND;

  len = strlen(interf);
  જો(len >= કદ(req.ifr_name))
    પાછા IF2IP_NOT_FOUND;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  જો(dummy == CURL_SOCKET_BAD)
    પાછા IF2IP_NOT_FOUND;

  memset(&req, 0, કદ(req));
  memcpy(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#જો defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif
  જો(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#જો defined(CURL_HAVE_DIAG) && defined(_AIX)
#pragma GCC diagnostic કાઢો
#endif
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    પાછા IF2IP_NOT_FOUND;
  }

  s = (સંરચના sockaddr_in *)(શૂન્ય *)&req.ifr_addr;
  memcpy(&in, &s->sin_addr, કદ(in));
  result = curlx_inet_ntop(s->sin_family, &in, buf, buf_size);

  sclose(dummy);
  જો(result)
    પાછા IF2IP_NOT_FOUND;
  પાછા IF2IP_FOUND;
}

#નહીંતર

if2ip_result_t Curl_if2ip(પૂર્ણાંક af,
#ifdef USE_IPV6
                          unsigned પૂર્ણાંક remote_scope,
                          unsigned પૂર્ણાંક local_scope_id,
#endif
                          અચલ અક્ષર *interf,
                          અક્ષર *buf, size_t buf_size)
{
  (શૂન્ય)af;
#ifdef USE_IPV6
  (શૂન્ય)remote_scope;
  (શૂન્ય)local_scope_id;
#endif
  (શૂન્ય)interf;
  (શૂન્ય)buf;
  (શૂન્ય)buf_size;
  પાછા IF2IP_NOT_FOUND;
}

#endif

#endif /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
