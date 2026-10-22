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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
ਪੂਰਨ curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   ਪੂਰਨ nonblock   /* TRUE or FALSE */)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  ਪੂਰਨ flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  ਜੇ(flags < 0)
    ਵਾਪਸ -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  ਜੇ(!!(flags & O_NONBLOCK) == !!nonblock)
    ਵਾਪਸ 0;
  ਜੇ(nonblock)
    flags |= O_NONBLOCK;
  ਨਹੀਂਤਾਂ
    flags &= ~O_NONBLOCK;
  ਵਾਪਸ sfcntl(sockfd, F_SETFL, flags);

#ਜਾਂਜੇ defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  ਲੰਮਾ flags = nonblock ? 1L : 0L;
  ਵਾਪਸ IoctlSocket(sockfd, FIONBIO, (ਅੱਖਰ *)&flags);

#ਜਾਂਜੇ defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  ਪੂਰਨ flags = nonblock ? 1 : 0;
  ਵਾਪਸ ioctl(sockfd, FIONBIO, &flags);

#ਜਾਂਜੇ defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags = nonblock ? 1UL : 0UL;
  ਵਾਪਸ ioctlsocket(sockfd, (ਲੰਮਾ)FIONBIO, &flags);

#ਜਾਂਜੇ defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  ਲੰਮਾ b = nonblock ? 1L : 0L;
  ਵਾਪਸ setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, ਆਕਾਰ(b));

#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "no non-blocking method was found/used/set"
#ਜੇ_ਅੰਤ
}
