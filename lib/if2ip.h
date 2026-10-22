#ifndef HEADER_CURL_IF2IP_H
#define HEADER_CURL_IF2IP_H
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
#અન્તર્ભાવ "curl_setup.h"

/* IPv6 address scopes. */
#define IPV6_SCOPE_GLOBAL       0       /* Global scope. */
#define IPV6_SCOPE_LINKLOCAL    1       /* Link-local scope. */
#define IPV6_SCOPE_SITELOCAL    2       /* Site-local scope (deprecated). */
#define IPV6_SCOPE_UNIQUELOCAL  3       /* Unique local */
#define IPV6_SCOPE_NODELOCAL    4       /* Loopback. */

#ifdef USE_IPV6
unsigned પૂર્ણાંક Curl_ipv6_scope(અચલ સંરચના sockaddr *sa);
#નહીંતર
#define Curl_ipv6_scope(x) 0
#endif

પ્રકારવ્યાખ્યા ગણના {
  IF2IP_NOT_FOUND = 0, /* Interface not found */
  IF2IP_AF_NOT_SUPPORTED = 1, /* Int. exists but has no address for this af */
  IF2IP_FOUND = 2 /* The address has been stored in "buf" */
} if2ip_result_t;

if2ip_result_t Curl_if2ip(પૂર્ણાંક af,
#ifdef USE_IPV6
                          unsigned પૂર્ણાંક remote_scope,
                          unsigned પૂર્ણાંક local_scope_id,
#endif
                          અચલ અક્ષર *interf,
                          અક્ષર *buf, size_t buf_size);

#ifdef __INTERIX

/* Nedelcho Stanev's workaround for SFU 3.0 */
સંરચના ifreq {
#define IFNAMSIZ    16
#define IFHWADDRLEN 6
  સંઘ {
    અક્ષર ifrn_name[IFNAMSIZ]; /* if name, e.g. "en0" */
  } ifr_ifrn;

  સંઘ {
    સંરચના sockaddr ifru_addr;
    સંરચના sockaddr ifru_broadaddr;
    સંરચના sockaddr ifru_netmask;
    સંરચના sockaddr ifru_hwaddr;
    short ifru_flags;
    પૂર્ણાંક ifru_metric;
    પૂર્ણાંક ifru_mtu;
  } ifr_ifru;
};

/* This define exists to avoid an extra #ifdef INTERIX in the C code. */

#define ifr_name      ifr_ifrn.ifrn_name /* interface name */
#define ifr_addr      ifr_ifru.ifru_addr /* address */
#define ifr_broadaddr ifr_ifru.ifru_broadaddr /* broadcast address */
#define ifr_netmask   ifr_ifru.ifru_netmask /* interface net mask */
#define ifr_flags     ifr_ifru.ifru_flags /* flags */
#define ifr_hwaddr    ifr_ifru.ifru_hwaddr /* MAC address */
#define ifr_metric    ifr_ifru.ifru_metric /* metric */
#define ifr_mtu       ifr_ifru.ifru_mtu /* mtu */

#define SIOCGIFADDR _IOW('s', 102, સંરચના ifreq) /* Get if addr */

#endif /* __INTERIX */

#endif /* HEADER_CURL_IF2IP_H */
