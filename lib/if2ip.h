#که_نه_تعریف_شوی سرلیک_CURL_IF2_IP_H
#تعریف_کړه سرلیک_CURL_IF2_IP_H
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
#تعریف_کړه IPV6_SCOPE_GLOBAL       0       /* Global scope. */
#تعریف_کړه IPV6_SCOPE_LINKLOCAL    1       /* Link-local scope. */
#تعریف_کړه IPV6_SCOPE_SITELOCAL    2       /* Site-local scope (deprecated). */
#تعریف_کړه IPV6_SCOPE_UNIQUELOCAL  3       /* Unique local */
#تعریف_کړه IPV6_SCOPE_NODELOCAL    4       /* Loopback. */

#که_تعریف_شوی USE_IPV6
بینښه صحيح Curl_ipv6_scope(ثابت بنية sockaddr *sa);
#وإلا
#تعریف_کړه Curl_ipv6_scope(x) 0
#د_که_پای

تعريفنوع تعداد {
  IF2_IP_NOT_وموندل = 0, /* Interface not found */
  IF2IP_AF_NOT_SUPPORTED = 1, /* Int. exists but has no address for this af */
  IF2_IP_وموندل = 2 /* The address has been stored in "buf" */
} if2ip_پایله_t;

if2ip_پایله_t Curl_if2ip(صحيح af,
#که_تعریف_شوی USE_IPV6
                          بینښه صحيح remote_scope,
                          بینښه صحيح local_scope_id,
#د_که_پای
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_اندازه);

#که_تعریف_شوی __INTERIX

/* Nedelcho Stanev's workaround for SFU 3.0 */
بنية ifreq {
#تعریف_کړه IFNAMSIZ    16
#تعریف_کړه IFHWADDRLEN 6
  اتحاد {
    حرف ifrn_name[IFNAMSIZ]; /* if name, e.g. "en0" */
  } ifr_ifrn;

  اتحاد {
    بنية sockaddr ifru_addr;
    بنية sockaddr ifru_broadaddr;
    بنية sockaddr ifru_netmask;
    بنية sockaddr ifru_hwaddr;
    لنډ ifru_flags;
    صحيح ifru_metric;
    صحيح ifru_mtu;
  } ifr_ifru;
};

/* This define exists to avoid an extra #ifdef INTERIX in the C code. */

#تعریف_کړه ifr_name      ifr_ifrn.ifrn_name /* interface name */
#تعریف_کړه ifr_addr      ifr_ifru.ifru_addr /*پته*/
#تعریف_کړه ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#تعریف_کړه ifr_netmask   ifr_ifru.ifru_netmask /* interface net mask */
#تعریف_کړه ifr_flags     ifr_ifru.ifru_flags /* flags */
#تعریف_کړه ifr_hwaddr    ifr_ifru.ifru_hwaddr /* MAC address */
#تعریف_کړه ifr_metric    ifr_ifru.ifru_metric /* metric */
#تعریف_کړه ifr_mtu       ifr_ifru.ifru_mtu /* mtu */

#تعریف_کړه SIOCGIFADDR _IOW('s', 102, بنية ifreq) /* Get if addr */

#د_که_پای /* __INTERIX */

#د_که_پای /* HEADER_CURL_IF2IP_H */
