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
số_nguyên curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   số_nguyên nonblock   /* TRUE or FALSE */)
{
#ifdef HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  số_nguyên flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  nếu(flags < 0)
    trả_về -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  nếu(!!(flags & O_NONBLOCK) == !!nonblock)
    trả_về 0;
  nếu(nonblock)
    flags |= O_NONBLOCK;
  không_thì
    flags &= ~O_NONBLOCK;
  trả_về sfcntl(sockfd, F_SETFL, flags);

#elif defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  long flags = nonblock ? 1L : 0L;
  trả_về IoctlSocket(sockfd, FIONBIO, (char *)&flags);

#elif defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  số_nguyên flags = nonblock ? 1 : 0;
  trả_về ioctl(sockfd, FIONBIO, &flags);

#elif defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  unsigned long flags = nonblock ? 1UL : 0UL;
  trả_về ioctlsocket(sockfd, (long)FIONBIO, &flags);

#elif defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  long b = nonblock ? 1L : 0L;
  trả_về setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, sizeof(b));

#không_thì
#error "no non-blocking method was found/used/set"
#endif
}
