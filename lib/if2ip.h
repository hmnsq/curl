#اگر_تعریف_نشده سرآیند_CURL_IF2_IP_H
#تعریف_کن سرآیند_CURL_IF2_IP_H
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
#تعریف_کن IPV6_SCOPE_GLOBAL       0       /* Global scope. */
#تعریف_کن IPV6_SCOPE_LINKLOCAL    1       /* Link-local scope. */
#تعریف_کن IPV6_SCOPE_SITELOCAL    2       /* Site-local scope (deprecated). */
#تعریف_کن IPV6_SCOPE_UNIQUELOCAL  3       /* Unique local */
#تعریف_کن IPV6_SCOPE_NODELOCAL    4       /* Loopback. */

#اگر_تعریف_شده USE_IPV6
بیعلامت صحيح Curl_ipv6_scope(ثابت بنية sockaddr *sa);
#وإلا
#تعریف_کن Curl_ipv6_scope(x) 0
#پایان_اگر

تعريفنوع تعداد {
  IF2_IP_NOT_یافت = 0, /* Interface not found */
  IF2IP_AF_NOT_SUPPORTED = 1, /* Int. exists but has no address for this af */
  IF2_IP_یافت = 2 /* The address has been stored in "buf" */
} if2ip_نتیجه_t;

if2ip_نتیجه_t Curl_if2ip(صحيح af,
#اگر_تعریف_شده USE_IPV6
                          بیعلامت صحيح remote_scope,
                          بیعلامت صحيح local_scope_id,
#پایان_اگر
                          ثابت حرف *interf,
                          حرف *buf, size_t buf_اندازه);

#اگر_تعریف_شده __INTERIX

/* Nedelcho Stanev's workaround for SFU 3.0 */
بنية ifreq {
#تعریف_کن IFNAMSIZ    16
#تعریف_کن IFHWADDRLEN 6
  اتحاد {
    حرف ifrn_name[IFNAMSIZ]; /* if name, e.g. "en0" */
  } ifr_ifrn;

  اتحاد {
    بنية sockaddr ifru_addr;
    بنية sockaddr ifru_broadaddr;
    بنية sockaddr ifru_netmask;
    بنية sockaddr ifru_hwaddr;
    کوتاه ifru_flags;
    صحيح ifru_metric;
    صحيح ifru_mtu;
  } ifr_ifru;
};

/* This define exists to avoid an extra #ifdef INTERIX in the C code. */

#تعریف_کن ifr_name      ifr_ifrn.ifrn_name /* interface name */
#تعریف_کن ifr_addr      ifr_ifru.ifru_addr /*آدرس*/
#تعریف_کن ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#تعریف_کن ifr_netmask   ifr_ifru.ifru_netmask /* interface net mask */
#تعریف_کن ifr_flags     ifr_ifru.ifru_flags /* flags */
#تعریف_کن ifr_hwaddr    ifr_ifru.ifru_hwaddr /* MAC address */
#تعریف_کن ifr_metric    ifr_ifru.ifru_metric /* metric */
#تعریف_کن ifr_mtu       ifr_ifru.ifru_mtu /* mtu */

#تعریف_کن SIOCGIFADDR _IOW('s', 102, بنية ifreq) /* Get if addr */

#پایان_اگر /* __INTERIX */

#پایان_اگر /* HEADER_CURL_IF2IP_H */
