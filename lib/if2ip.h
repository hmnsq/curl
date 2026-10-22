#अगर_अघोषित HEADER_CURL_IF2IP_H
#घोषणा HEADER_CURL_IF2IP_H
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

/* IPv6 address scopes. */
#घोषणा IPV6_SCOPE_GLOBAL       0       /* Global scope. */
#घोषणा IPV6_SCOPE_LINKLOCAL    1       /* Link-local scope. */
#घोषणा IPV6_SCOPE_SITELOCAL    2       /* Site-local scope (deprecated). */
#घोषणा IPV6_SCOPE_UNIQUELOCAL  3       /* Unique local */
#घोषणा IPV6_SCOPE_NODELOCAL    4       /* Loopback. */

#अगर_घोषित USE_IPV6
अचिन्हित पूर्णांक Curl_ipv6_scope(स्थिर काष्ठा sockaddr *sa);
#अन्यथा
#घोषणा Curl_ipv6_scope(x) 0
#पूर्ण_अगर

प्रकार क्रमागत {
  IF2IP_NOT_FOUND = 0, /* Interface not found */
  IF2IP_AF_NOT_SUPPORTED = 1, /* Int. exists but has no address for this af */
  IF2IP_FOUND = 2 /* The address has been stored in "buf" */
} if2ip_परिणाम_t;

if2ip_परिणाम_t Curl_if2ip(पूर्णांक af,
#अगर_घोषित USE_IPV6
                          अचिन्हित पूर्णांक remote_scope,
                          अचिन्हित पूर्णांक local_scope_id,
#पूर्ण_अगर
                          स्थिर अक्षर *interf,
                          अक्षर *buf, माप_प्रकार buf_आकार);

#अगर_घोषित __INTERIX

/* Nedelcho Stanev's workaround for SFU 3.0 */
काष्ठा ifreq {
#घोषणा IFNAMSIZ    16
#घोषणा IFHWADDRLEN 6
  जोड़ {
    अक्षर ifrn_name[IFNAMSIZ]; /* if name, e.g. "en0" */
  } ifr_ifrn;

  जोड़ {
    काष्ठा sockaddr ifru_addr;
    काष्ठा sockaddr ifru_broadaddr;
    काष्ठा sockaddr ifru_netmask;
    काष्ठा sockaddr ifru_hwaddr;
    लघु ifru_flags;
    पूर्णांक ifru_metric;
    पूर्णांक ifru_mtu;
  } ifr_ifru;
};

/* This define exists to avoid an extra #ifdef INTERIX in the C code. */

#घोषणा ifr_name      ifr_ifrn.ifrn_name /* interface name */
#घोषणा ifr_addr      ifr_ifru.ifru_addr /* address */
#घोषणा ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#घोषणा ifr_netmask   ifr_ifru.ifru_netmask /* interface net mask */
#घोषणा ifr_flags     ifr_ifru.ifru_flags /* flags */
#घोषणा ifr_hwaddr    ifr_ifru.ifru_hwaddr /* MAC address */
#घोषणा ifr_metric    ifr_ifru.ifru_metric /* metric */
#घोषणा ifr_mtu       ifr_ifru.ifru_mtu /* mtu */

#घोषणा SIOCGIFADDR _IOW('s', 102, काष्ठा ifreq) /* Get if addr */

#पूर्ण_अगर /* __INTERIX */

#पूर्ण_अगर /* HEADER_CURL_IF2IP_H */
