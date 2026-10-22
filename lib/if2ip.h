#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_IF2_IP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_IF2_IP_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

/* IPv6 address scopes. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPV6_SCOPE_GLOBAL       0       /* Global scope. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPV6_SCOPE_LINKLOCAL    1       /* Link-local scope. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPV6_SCOPE_SITELOCAL    2       /* Site-local scope (deprecated). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPV6_SCOPE_UNIQUELOCAL  3       /* Unique local */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPV6_SCOPE_NODELOCAL    4       /* Loopback. */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ Curl_ipv6_scope(ਅਚਲ ਬਣਤਰ sockaddr *sa);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ipv6_scope(x) 0
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  IF2_IP_NOT_ਮਿਲਿਆ = 0, /* Interface not found */
  IF2IP_AF_NOT_SUPPORTED = 1, /* Int. exists but has no address for this af */
  IF2_IP_ਮਿਲਿਆ = 2 /* The address has been stored in "buf" */
} if2ip_ਨਤੀਜਾ_t;

if2ip_ਨਤੀਜਾ_t Curl_if2ip(ਪੂਰਨ af,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ remote_scope,
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ local_scope_id,
#ਜੇ_ਅੰਤ
                          ਅਚਲ ਅੱਖਰ *interf,
                          ਅੱਖਰ *buf, size_t buf_ਮਾਪ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __INTERIX

/* Nedelcho Stanev's workaround for SFU 3.0 */
ਬਣਤਰ ifreq {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IFNAMSIZ    16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IFHWADDRLEN 6
  ਸੰਘ {
    ਅੱਖਰ ifrn_name[IFNAMSIZ]; /* if name, e.g. "en0" */
  } ifr_ifrn;

  ਸੰਘ {
    ਬਣਤਰ sockaddr ifru_addr;
    ਬਣਤਰ sockaddr ifru_broadaddr;
    ਬਣਤਰ sockaddr ifru_netmask;
    ਬਣਤਰ sockaddr ifru_hwaddr;
    ਛੋਟਾ ifru_flags;
    ਪੂਰਨ ifru_metric;
    ਪੂਰਨ ifru_mtu;
  } ifr_ifru;
};

/* This define exists to avoid an extra #ifdef INTERIX in the C code. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ifr_name      ifr_ifrn.ifrn_name /* interface name */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ifr_addr      ifr_ifru.ifru_addr /*ਪਤਾ*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ifr_netmask   ifr_ifru.ifru_netmask /* interface net mask */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ifr_flags     ifr_ifru.ifru_flags /* flags */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ifr_hwaddr    ifr_ifru.ifru_hwaddr /* MAC address */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ifr_metric    ifr_ifru.ifru_metric /* metric */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ifr_mtu       ifr_ifru.ifru_mtu /* mtu */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIOCGIFADDR _IOW('s', 102, ਬਣਤਰ ifreq) /* Get if addr */

#ਜੇ_ਅੰਤ /* __INTERIX */

#ਜੇ_ਅੰਤ /* HEADER_CURL_IF2IP_H */
