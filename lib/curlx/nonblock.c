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
#include "curl_setup.h"

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
целое curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   целое nonblock   /* TRUE or FALSE */)
{
#ifdef HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  целое flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  если(flags < 0)
    вернуть -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  если(!!(flags & O_NONBLOCK) == !!nonblock)
    вернуть 0;
  если(nonblock)
    flags |= O_NONBLOCK;
  иначе
    flags &= ~O_NONBLOCK;
  вернуть sfcntl(sockfd, F_SETFL, flags);

#elif defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  long flags = nonblock ? 1L : 0L;
  вернуть IoctlSocket(sockfd, FIONBIO, (char *)&flags);

#elif defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  целое flags = nonblock ? 1 : 0;
  вернуть ioctl(sockfd, FIONBIO, &flags);

#elif defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  unsigned long flags = nonblock ? 1UL : 0UL;
  вернуть ioctlsocket(sockfd, (long)FIONBIO, &flags);

#elif defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  long b = nonblock ? 1L : 0L;
  вернуть setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, sizeof(b));

#иначе
#error "no non-blocking method was found/used/set"
#endif
}
