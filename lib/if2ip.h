#جے_بنا_تعریف سرخی_CURL_IF2_IP_H
#تعریف_کرو سرخی_CURL_IF2_IP_H
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
#تعریف_کرو IPV6_SCOPE_GLOBAL       0       /* Global scope. */
#تعریف_کرو IPV6_SCOPE_LINKLOCAL    1       /* Link-local scope. */
#تعریف_کرو IPV6_SCOPE_SITELOCAL    2       /* Site-local scope (deprecated). */
#تعریف_کرو IPV6_SCOPE_UNIQUELOCAL  3       /* Unique local */
#تعریف_کرو IPV6_SCOPE_NODELOCAL    4       /* Loopback. */

#جے_تعریف_شدہ USE_IPV6
بنانشانی صحيح Curl_ipv6_scope(ثابت بنية sockaddr *sa);
#وإلا
#تعریف_کرو Curl_ipv6_scope(x) 0
#جے_انت

تعريفنوع تعداد {
  IF2_IP_NOT_لبھیا = 0, /* Interface not found */
  IF2IP_AF_NOT_SUPPORTED = 1, /* Int. exists but has no address for this af */
  IF2_IP_لبھیا = 2 /* The address has been stored in "buf" */
} if2ip_نتیجہ_t;

if2ip_نتیجہ_t Curl_if2ip(صحيح af,
#جے_تعریف_شدہ USE_IPV6
                          بنانشانی صحيح remote_scope,
                          بنانشانی صحيح local_scope_id,
#جے_انت
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_ناپ);

#جے_تعریف_شدہ __INTERIX

/* Nedelcho Stanev's workaround for SFU 3.0 */
بنية ifreq {
#تعریف_کرو IFNAMSIZ    16
#تعریف_کرو IFHWADDRLEN 6
  اتحاد {
    حرف ifrn_name[IFNAMSIZ]; /* if name, e.g. "en0" */
  } ifr_ifrn;

  اتحاد {
    بنية sockaddr ifru_addr;
    بنية sockaddr ifru_broadaddr;
    بنية sockaddr ifru_netmask;
    بنية sockaddr ifru_hwaddr;
    چھوٹا ifru_flags;
    صحيح ifru_metric;
    صحيح ifru_mtu;
  } ifr_ifru;
};

/* This define exists to avoid an extra #ifdef INTERIX in the C code. */

#تعریف_کرو ifr_name      ifr_ifrn.ifrn_name /* interface name */
#تعریف_کرو ifr_addr      ifr_ifru.ifru_addr /*پتہ*/
#تعریف_کرو ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#تعریف_کرو ifr_netmask   ifr_ifru.ifru_netmask /* interface net mask */
#تعریف_کرو ifr_flags     ifr_ifru.ifru_flags /* flags */
#تعریف_کرو ifr_hwaddr    ifr_ifru.ifru_hwaddr /* MAC address */
#تعریف_کرو ifr_metric    ifr_ifru.ifru_metric /* metric */
#تعریف_کرو ifr_mtu       ifr_ifru.ifru_mtu /* mtu */

#تعریف_کرو SIOCGIFADDR _IOW('s', 102, بنية ifreq) /* Get if addr */

#جے_انت /* __INTERIX */

#جے_انت /* HEADER_CURL_IF2IP_H */
