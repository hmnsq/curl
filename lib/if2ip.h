#यदि_अपरिभाषितम् शीर्षकम्_CURL_IF2_IP_H
#परिभाषय शीर्षकम्_CURL_IF2_IP_H
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

/* IPv6 address scopes. */
#परिभाषय IPV6_SCOPE_GLOBAL       0       /* Global scope. */
#परिभाषय IPV6_SCOPE_LINKLOCAL    1       /* Link-local scope. */
#परिभाषय IPV6_SCOPE_SITELOCAL    2       /* Site-local scope (deprecated). */
#परिभाषय IPV6_SCOPE_UNIQUELOCAL  3       /* Unique local */
#परिभाषय IPV6_SCOPE_NODELOCAL    4       /* Loopback. */

#यदि_परिभाषितम् USE_IPV6
अचिह्नित पूर्णाङ्क Curl_ipv6_scope(ध्रुव संरचना sockaddr *sa);
#अन्यथा
#परिभाषय Curl_ipv6_scope(x) 0
#यदि_अन्तः

प्रकारनिर्देश गणना {
  IF2_IP_NOT_प्राप्तम् = 0, /* Interface not found */
  IF2IP_AF_NOT_SUPPORTED = 1, /* Int. exists but has no address for this af */
  IF2_IP_प्राप्तम् = 2 /* The address has been stored in "buf" */
} if2ip_परिणाम_t;

if2ip_परिणाम_t Curl_if2ip(पूर्णाङ्क af,
#यदि_परिभाषितम् USE_IPV6
                          अचिह्नित पूर्णाङ्क remote_scope,
                          अचिह्नित पूर्णाङ्क local_scope_id,
#यदि_अन्तः
                          ध्रुव वर्ण *interf,
                          वर्ण *buf, size_t buf_आकार);

#यदि_परिभाषितम् __INTERIX

/* Nedelcho Stanev's workaround for SFU 3.0 */
संरचना ifreq {
#परिभाषय IFNAMSIZ    16
#परिभाषय IFHWADDRLEN 6
  सङ्घ {
    वर्ण ifrn_name[IFNAMSIZ]; /* if name, e.g. "en0" */
  } ifr_ifrn;

  सङ्घ {
    संरचना sockaddr ifru_addr;
    संरचना sockaddr ifru_broadaddr;
    संरचना sockaddr ifru_netmask;
    संरचना sockaddr ifru_hwaddr;
    ह्रस्व ifru_flags;
    पूर्णाङ्क ifru_metric;
    पूर्णाङ्क ifru_mtu;
  } ifr_ifru;
};

/* This define exists to avoid an extra #ifdef INTERIX in the C code. */

#परिभाषय ifr_name      ifr_ifrn.ifrn_name /* interface name */
#परिभाषय ifr_addr      ifr_ifru.ifru_addr /*स्थानाङ्कः*/
#परिभाषय ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#परिभाषय ifr_netmask   ifr_ifru.ifru_netmask /* interface net mask */
#परिभाषय ifr_flags     ifr_ifru.ifru_flags /* flags */
#परिभाषय ifr_hwaddr    ifr_ifru.ifru_hwaddr /* MAC address */
#परिभाषय ifr_metric    ifr_ifru.ifru_metric /* metric */
#परिभाषय ifr_mtu       ifr_ifru.ifru_mtu /* mtu */

#परिभाषय SIOCGIFADDR _IOW('s', 102, संरचना ifreq) /* Get if addr */

#यदि_अन्तः /* __INTERIX */

#यदि_अन्तः /* HEADER_CURL_IF2IP_H */
