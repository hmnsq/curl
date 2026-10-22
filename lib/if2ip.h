#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_IF2_IP_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_IF2_IP_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

/* IPv6 address scopes. */
#ವ್ಯಾಖ್ಯಾನಿಸು IPV6_SCOPE_GLOBAL       0       /* Global scope. */
#ವ್ಯಾಖ್ಯಾನಿಸು IPV6_SCOPE_LINKLOCAL    1       /* Link-local scope. */
#ವ್ಯಾಖ್ಯಾನಿಸು IPV6_SCOPE_SITELOCAL    2       /* Site-local scope (deprecated). */
#ವ್ಯಾಖ್ಯಾನಿಸು IPV6_SCOPE_UNIQUELOCAL  3       /* Unique local */
#ವ್ಯಾಖ್ಯಾನಿಸು IPV6_SCOPE_NODELOCAL    4       /* Loopback. */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ Curl_ipv6_scope(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *sa);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ipv6_scope(x) 0
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  IF2_IP_NOT_ಸಿಕ್ಕಿತು = 0, /* Interface not found */
  IF2IP_AF_NOT_SUPPORTED = 1, /* Int. exists but has no address for this af */
  IF2_IP_ಸಿಕ್ಕಿತು = 2 /* The address has been stored in "buf" */
} if2ip_ಫಲಿತಾಂಶ_t;

if2ip_ಫಲಿತಾಂಶ_t Curl_if2ip(ಪೂರ್ಣಾಂಕ af,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
                          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ remote_scope,
                          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ local_scope_id,
#ಆದರೆ_ಅಂತ್ಯ
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *interf,
                          ಅಕ್ಷರ *buf, size_t buf_ಗಾತ್ರ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __INTERIX

/* Nedelcho Stanev's workaround for SFU 3.0 */
ರಚನೆ ifreq {
#ವ್ಯಾಖ್ಯಾನಿಸು IFNAMSIZ    16
#ವ್ಯಾಖ್ಯಾನಿಸು IFHWADDRLEN 6
  ಒಕ್ಕೂಟ {
    ಅಕ್ಷರ ifrn_name[IFNAMSIZ]; /* if name, e.g. "en0" */
  } ifr_ifrn;

  ಒಕ್ಕೂಟ {
    ರಚನೆ sockaddr ifru_addr;
    ರಚನೆ sockaddr ifru_broadaddr;
    ರಚನೆ sockaddr ifru_netmask;
    ರಚನೆ sockaddr ifru_hwaddr;
    ಗಿಡ್ಡ ifru_flags;
    ಪೂರ್ಣಾಂಕ ifru_metric;
    ಪೂರ್ಣಾಂಕ ifru_mtu;
  } ifr_ifru;
};

/* This define exists to avoid an extra #ifdef INTERIX in the C code. */

#ವ್ಯಾಖ್ಯಾನಿಸು ifr_name      ifr_ifrn.ifrn_name /* interface name */
#ವ್ಯಾಖ್ಯಾನಿಸು ifr_addr      ifr_ifru.ifru_addr /*ವಿಳಾಸ*/
#ವ್ಯಾಖ್ಯಾನಿಸು ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#ವ್ಯಾಖ್ಯಾನಿಸು ifr_netmask   ifr_ifru.ifru_netmask /* interface net mask */
#ವ್ಯಾಖ್ಯಾನಿಸು ifr_flags     ifr_ifru.ifru_flags /* flags */
#ವ್ಯಾಖ್ಯಾನಿಸು ifr_hwaddr    ifr_ifru.ifru_hwaddr /* MAC address */
#ವ್ಯಾಖ್ಯಾನಿಸು ifr_metric    ifr_ifru.ifru_metric /* metric */
#ವ್ಯಾಖ್ಯಾನಿಸು ifr_mtu       ifr_ifru.ifru_mtu /* mtu */

#ವ್ಯಾಖ್ಯಾನಿಸು SIOCGIFADDR _IOW('s', 102, ರಚನೆ ifreq) /* Get if addr */

#ಆದರೆ_ಅಂತ್ಯ /* __INTERIX */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_IF2IP_H */
