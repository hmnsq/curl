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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_SOCKIO_H
#అన్తర్భావ <sys/sockio.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_IFADDRS_H
#అన్తర్భావ <ifaddrs.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_STROPTS_H
#అన్తర్భావ <stropts.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "curlx/inet_ntop.h"
#అన్తర్భావ "if2ip.h"

/* ------------------------------------------------------------------ */

#నిర్వచితమైతే USE_IPV6
/* Return the scope of the given address. */
గుర్తులేని పూర్ణాంకం Curl_ipv6_scope(స్థిరరాశి నిర్మాణం sockaddr *sa)
{
  ఒకవేళ(sa->sa_family == AF_INET6) {
    స్థిరరాశి నిర్మాణం sockaddr_in6 *sa6 =
      (స్థిరరాశి నిర్మాణం sockaddr_in6 *)(స్థిరరాశి శూన్య *)sa;
    స్థిరరాశి గుర్తులేని అక్షరం *b = sa6->sin6_addr.s6_addr;
    గుర్తులేని పొట్టి w = (గుర్తులేని పొట్టి)((b[0] << 8) | b[1]);

    ఒకవేళ((b[0] & 0xFE) == 0xFC) /* Handle ULAs */
      తిరిగి IPV6_SCOPE_UNIQUELOCAL;
    ఎంపిక(w & 0xFFC0) {
    స్థితి 0xFE80:
      తిరిగి IPV6_SCOPE_LINKLOCAL;
    స్థితి 0xFEC0:
      తిరిగి IPV6_SCOPE_SITELOCAL;
    స్థితి 0x0000:
      w = b[1] | b[2] | b[3] | b[4] | b[5] | b[6] | b[7] | b[8] | b[9] |
          b[10] | b[11] | b[12] | b[13] | b[14];
      ఒకవేళ(w || b[15] != 0x01)
        ఆపు;
      తిరిగి IPV6_SCOPE_NODELOCAL;
    అప్రమేయ:
      ఆపు;
    }
  }
  తిరిగి IPV6_SCOPE_GLOBAL;
}
#అయితే_ముగింపు

#ఒకవేళ !defined(CURL_DISABLE_BINDLOCAL) || !defined(CURL_DISABLE_FTP)

#నిర్వచితమైతే HAVE_GETIFADDRS

if2ip_ఫలితం_t Curl_if2ip(పూర్ణాంకం af,
#నిర్వచితమైతే USE_IPV6
                          గుర్తులేని పూర్ణాంకం remote_scope,
                          గుర్తులేని పూర్ణాంకం local_scope_id,
#అయితే_ముగింపు
                          స్థిరరాశి అక్షరం *interf,
                          అక్షరం *buf, size_t buf_కొలత)
{
  నిర్మాణం ifaddrs *iface, *తల;
  if2ip_ఫలితం_t res = IF2_IP_NOT_దొరికింది;

#ఒకవేళ defined(USE_IPV6) && !defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  (శూన్య)local_scope_id;
#అయితే_ముగింపు

  ఒకవేళ(getifaddrs(&తల) >= 0) {
    కొరకు(iface = తల; iface; iface = iface->ifa_తదుపరి) {
      ఒకవేళ(iface->ifa_addr) {
        ఒకవేళ(iface->ifa_addr->sa_family == af) {
          ఒకవేళ(!strcmp(iface->ifa_name, interf)) {
            శూన్య *addr;
            అక్షరం scope[12] = "";
            అక్షరం ipstr[64];
            CURLcode ఫలితం;
#నిర్వచితమైతే USE_IPV6
            ఒకవేళ(af == AF_INET6) {
#నిర్వచితమైతే HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              గుర్తులేని పూర్ణాంకం scopeid = 0;
#అయితే_ముగింపు
              గుర్తులేని పూర్ణాంకం ifscope = Curl_ipv6_scope(iface->ifa_addr);

              ఒకవేళ(ifscope != remote_scope) {
                /* We are interested only in interface addresses whose scope
                   matches the remote address we want to connect to: global
                   for global, link-local for link-local, etc... */
                ఒకవేళ(res == IF2_IP_NOT_దొరికింది)
                  res = IF2IP_AF_NOT_SUPPORTED;
                కొనసాగు;
              }

              addr =
                &((నిర్మాణం sockaddr_in6 *)(శూన్య *)iface->ifa_addr)->sin6_addr;
#నిర్వచితమైతే HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
              /* Include the scope of this interface as part of the address */
              scopeid = ((నిర్మాణం sockaddr_in6 *)(శూన్య *)iface->ifa_addr)
                          ->sin6_scope_id;

              /* If given, scope id should match. */
              ఒకవేళ(local_scope_id && scopeid != local_scope_id) {
                ఒకవేళ(res == IF2_IP_NOT_దొరికింది)
                  res = IF2IP_AF_NOT_SUPPORTED;

                కొనసాగు;
              }

              ఒకవేళ(scopeid)
                curl_msnprintf(scope, పరిమాణం(scope), "%%%u", scopeid);
#అయితే_ముగింపు
            }
            లేకపోతే
#అయితే_ముగింపు
              addr =
                &((నిర్మాణం sockaddr_in *)(శూన్య *)iface->ifa_addr)->sin_addr;
            res = IF2_IP_దొరికింది;
            ఫలితం = curlx_inet_ntop(af, addr, ipstr, పరిమాణం(ipstr));
            curl_msnprintf(buf, buf_కొలత, "%s%s", ఫలితం ? "" : ipstr, scope);
            ఆపు;
          }
        }
        లేకపోతే ఒకవేళ((res == IF2_IP_NOT_దొరికింది) &&
                !strcmp(iface->ifa_name, interf)) {
          res = IF2IP_AF_NOT_SUPPORTED;
        }
      }
    }

    freeifaddrs(తల);
  }

  తిరిగి res;
}

#లేదావేళ defined(HAVE_IOCTL_SIOCGIFADDR)

if2ip_ఫలితం_t Curl_if2ip(పూర్ణాంకం af,
#నిర్వచితమైతే USE_IPV6
                          గుర్తులేని పూర్ణాంకం remote_scope,
                          గుర్తులేని పూర్ణాంకం local_scope_id,
#అయితే_ముగింపు
                          స్థిరరాశి అక్షరం *interf,
                          అక్షరం *buf, size_t buf_కొలత)
{
  నిర్మాణం ifreq req;
  నిర్మాణం in_addr లోపల;
  నిర్మాణం sockaddr_in *s;
  curl_socket_t dummy;
  size_t len;
  CURLcode ఫలితం;

#నిర్వచితమైతే USE_IPV6
  (శూన్య)remote_scope;
  (శూన్య)local_scope_id;
#అయితే_ముగింపు

  ఒకవేళ(!interf || (af != AF_INET))
    తిరిగి IF2_IP_NOT_దొరికింది;

  len = అక్షరశ్రేణి_పొడవు(interf);
  ఒకవేళ(len >= పరిమాణం(req.ifr_name))
    తిరిగి IF2_IP_NOT_దొరికింది;

  dummy = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ఒకవేళ(dummy == CURL_SOCKET_BAD)
    తిరిగి IF2_IP_NOT_దొరికింది;

  memset(&req, 0, పరిమాణం(req));
  జ్ఞాపకం_నకలు(req.ifr_name, interf, len + 1);
  req.ifr_addr.sa_family = AF_INET;

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(_AIX)
/* Suppress warning inside system headers */
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wshift-sign-overflow"
#అయితే_ముగింపు
  ఒకవేళ(ioctl(dummy, SIOCGIFADDR, &req) < 0) {
#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(_AIX)
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
    sclose(dummy);
    /* With SIOCGIFADDR, we cannot tell the difference between an interface
       that does not exist and an interface that has no address of the
       correct family. Assume the interface does not exist */
    తిరిగి IF2_IP_NOT_దొరికింది;
  }

  s = (నిర్మాణం sockaddr_in *)(శూన్య *)&req.ifr_addr;
  జ్ఞాపకం_నకలు(&లోపల, &s->sin_addr, పరిమాణం(లోపల));
  ఫలితం = curlx_inet_ntop(s->sin_family, &లోపల, buf, buf_కొలత);

  sclose(dummy);
  ఒకవేళ(ఫలితం)
    తిరిగి IF2_IP_NOT_దొరికింది;
  తిరిగి IF2_IP_దొరికింది;
}

#లేకపోతే

if2ip_ఫలితం_t Curl_if2ip(పూర్ణాంకం af,
#నిర్వచితమైతే USE_IPV6
                          గుర్తులేని పూర్ణాంకం remote_scope,
                          గుర్తులేని పూర్ణాంకం local_scope_id,
#అయితే_ముగింపు
                          స్థిరరాశి అక్షరం *interf,
                          అక్షరం *buf, size_t buf_కొలత)
{
  (శూన్య)af;
#నిర్వచితమైతే USE_IPV6
  (శూన్య)remote_scope;
  (శూన్య)local_scope_id;
#అయితే_ముగింపు
  (శూన్య)interf;
  (శూన్య)buf;
  (శూన్య)buf_కొలత;
  తిరిగి IF2_IP_NOT_దొరికింది;
}

#అయితే_ముగింపు

#అయితే_ముగింపు /* CURL_DISABLE_BINDLOCAL && CURL_DISABLE_FTP */
