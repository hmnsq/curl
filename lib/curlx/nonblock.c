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

#ifdef HAVE_SYS_IOCTL_H
#અન્તર્ભાવ <sys/ioctl.h>
#endif
#ifdef HAVE_FCNTL_H
#અન્તર્ભાવ <fcntl.h>
#endif

#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
પૂર્ણાંક curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   પૂર્ણાંક nonblock   /* TRUE or FALSE */)
{
#ifdef HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  પૂર્ણાંક flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  જો(flags < 0)
    પાછા -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  જો(!!(flags & O_NONBLOCK) == !!nonblock)
    પાછા 0;
  જો(nonblock)
    flags |= O_NONBLOCK;
  નહીંતર
    flags &= ~O_NONBLOCK;
  પાછા sfcntl(sockfd, F_SETFL, flags);

#અથવાજો defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  long flags = nonblock ? 1L : 0L;
  પાછા IoctlSocket(sockfd, FIONBIO, (અક્ષર *)&flags);

#અથવાજો defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  પૂર્ણાંક flags = nonblock ? 1 : 0;
  પાછા ioctl(sockfd, FIONBIO, &flags);

#અથવાજો defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  unsigned long flags = nonblock ? 1UL : 0UL;
  પાછા ioctlsocket(sockfd, (long)FIONBIO, &flags);

#અથવાજો defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  long b = nonblock ? 1L : 0L;
  પાછા setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, કદ(b));

#નહીંતર
#error "no non-blocking method was found/used/set"
#endif
}
