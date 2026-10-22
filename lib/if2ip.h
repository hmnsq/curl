#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_IF2_IP_H
#وصف_ڪريو سرو_CURL_IF2_IP_H
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
#ضمن "curl_setup.h"

/* IPv6 address scopes. */
#وصف_ڪريو IPV6_SCOPE_GLOBAL       0       /* Global scope. */
#وصف_ڪريو IPV6_SCOPE_LINKLOCAL    1       /* Link-local scope. */
#وصف_ڪريو IPV6_SCOPE_SITELOCAL    2       /* Site-local scope (deprecated). */
#وصف_ڪريو IPV6_SCOPE_UNIQUELOCAL  3       /* Unique local */
#وصف_ڪريو IPV6_SCOPE_NODELOCAL    4       /* Loopback. */

#جيڪڏهن_وصف_ٿيل USE_IPV6
بنانشان صحيح Curl_ipv6_scope(ثابت بنية sockaddr *sa);
#وإلا
#وصف_ڪريو Curl_ipv6_scope(x) 0
#جيڪڏهن_پورو

تعريفنوع تعداد {
  IF2_IP_NOT_مليو = 0, /* Interface not found */
  IF2IP_AF_NOT_SUPPORTED = 1, /* Int. exists but has no address for this af */
  IF2_IP_مليو = 2 /* The address has been stored in "buf" */
} if2ip_نتيجو_t;

if2ip_نتيجو_t Curl_if2ip(صحيح af,
#جيڪڏهن_وصف_ٿيل USE_IPV6
                          بنانشان صحيح remote_scope,
                          بنانشان صحيح local_scope_id,
#جيڪڏهن_پورو
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_سائيز);

#جيڪڏهن_وصف_ٿيل __INTERIX

/* Nedelcho Stanev's workaround for SFU 3.0 */
بنية ifreq {
#وصف_ڪريو IFNAMSIZ    16
#وصف_ڪريو IFHWADDRLEN 6
  اتحاد {
    حرف ifrn_name[IFNAMSIZ]; /* if name, e.g. "en0" */
  } ifr_ifrn;

  اتحاد {
    بنية sockaddr ifru_addr;
    بنية sockaddr ifru_broadaddr;
    بنية sockaddr ifru_netmask;
    بنية sockaddr ifru_hwaddr;
    ننڍو ifru_flags;
    صحيح ifru_metric;
    صحيح ifru_mtu;
  } ifr_ifru;
};

/* This define exists to avoid an extra #ifdef INTERIX in the C code. */

#وصف_ڪريو ifr_name      ifr_ifrn.ifrn_name /* interface name */
#وصف_ڪريو ifr_addr      ifr_ifru.ifru_addr /*پتو*/
#وصف_ڪريو ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#وصف_ڪريو ifr_netmask   ifr_ifru.ifru_netmask /* interface net mask */
#وصف_ڪريو ifr_flags     ifr_ifru.ifru_flags /* flags */
#وصف_ڪريو ifr_hwaddr    ifr_ifru.ifru_hwaddr /* MAC address */
#وصف_ڪريو ifr_metric    ifr_ifru.ifru_metric /* metric */
#وصف_ڪريو ifr_mtu       ifr_ifru.ifru_mtu /* mtu */

#وصف_ڪريو SIOCGIFADDR _IOW('s', 102, بنية ifreq) /* Get if addr */

#جيڪڏهن_پورو /* __INTERIX */

#جيڪڏهن_پورو /* HEADER_CURL_IF2IP_H */
