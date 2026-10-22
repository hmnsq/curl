#నిర్వచితం_కాకపోతే శీర్షిక_CURL_IF2_IP_H
#నిర్వచించు శీర్షిక_CURL_IF2_IP_H
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

/* IPv6 address scopes. */
#నిర్వచించు IPV6_SCOPE_GLOBAL       0       /* Global scope. */
#నిర్వచించు IPV6_SCOPE_LINKLOCAL    1       /* Link-local scope. */
#నిర్వచించు IPV6_SCOPE_SITELOCAL    2       /* Site-local scope (deprecated). */
#నిర్వచించు IPV6_SCOPE_UNIQUELOCAL  3       /* Unique local */
#నిర్వచించు IPV6_SCOPE_NODELOCAL    4       /* Loopback. */

#నిర్వచితమైతే USE_IPV6
గుర్తులేని పూర్ణాంకం Curl_ipv6_scope(స్థిరరాశి నిర్మాణం sockaddr *sa);
#లేకపోతే
#నిర్వచించు Curl_ipv6_scope(x) 0
#అయితే_ముగింపు

రకనిర్వచనం గణన {
  IF2_IP_NOT_దొరికింది = 0, /* Interface not found */
  IF2IP_AF_NOT_SUPPORTED = 1, /* Int. exists but has no address for this af */
  IF2_IP_దొరికింది = 2 /* The address has been stored in "buf" */
} if2ip_ఫలితం_t;

if2ip_ఫలితం_t Curl_if2ip(పూర్ణాంకం af,
#నిర్వచితమైతే USE_IPV6
                          గుర్తులేని పూర్ణాంకం remote_scope,
                          గుర్తులేని పూర్ణాంకం local_scope_id,
#అయితే_ముగింపు
                          స్థిరరాశి అక్షరం *interf,
                          అక్షరం *buf, size_t buf_కొలత);

#నిర్వచితమైతే __INTERIX

/* Nedelcho Stanev's workaround for SFU 3.0 */
నిర్మాణం ifreq {
#నిర్వచించు IFNAMSIZ    16
#నిర్వచించు IFHWADDRLEN 6
  సంఘం {
    అక్షరం ifrn_name[IFNAMSIZ]; /* if name, e.g. "en0" */
  } ifr_ifrn;

  సంఘం {
    నిర్మాణం sockaddr ifru_addr;
    నిర్మాణం sockaddr ifru_broadaddr;
    నిర్మాణం sockaddr ifru_netmask;
    నిర్మాణం sockaddr ifru_hwaddr;
    పొట్టి ifru_flags;
    పూర్ణాంకం ifru_metric;
    పూర్ణాంకం ifru_mtu;
  } ifr_ifru;
};

/* This define exists to avoid an extra #ifdef INTERIX in the C code. */

#నిర్వచించు ifr_name      ifr_ifrn.ifrn_name /* interface name */
#నిర్వచించు ifr_addr      ifr_ifru.ifru_addr /*చిరునామా*/
#నిర్వచించు ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#నిర్వచించు ifr_netmask   ifr_ifru.ifru_netmask /* interface net mask */
#నిర్వచించు ifr_flags     ifr_ifru.ifru_flags /* flags */
#నిర్వచించు ifr_hwaddr    ifr_ifru.ifru_hwaddr /* MAC address */
#నిర్వచించు ifr_metric    ifr_ifru.ifru_metric /* metric */
#నిర్వచించు ifr_mtu       ifr_ifru.ifru_mtu /* mtu */

#నిర్వచించు SIOCGIFADDR _IOW('s', 102, నిర్మాణం ifreq) /* Get if addr */

#అయితే_ముగింపు /* __INTERIX */

#అయితే_ముగింపు /* HEADER_CURL_IF2IP_H */
