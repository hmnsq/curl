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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif
#ifdef HAVE_FCNTL_H
#அந்தர்பாவ <fcntl.h>
#endif

#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
முழு curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   முழு nonblock   /* TRUE or FALSE */)
{
#ifdef HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  முழு flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  எனில்(flags < 0)
    திரும்பு -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  எனில்(!!(flags & O_NONBLOCK) == !!nonblock)
    திரும்பு 0;
  எனில்(nonblock)
    flags |= O_NONBLOCK;
  இல்லையெனில்
    flags &= ~O_NONBLOCK;
  திரும்பு sfcntl(sockfd, F_SETFL, flags);

#அல்லதெனில் defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  long flags = nonblock ? 1L : 0L;
  திரும்பு IoctlSocket(sockfd, FIONBIO, (எழுத்து *)&flags);

#அல்லதெனில் defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  முழு flags = nonblock ? 1 : 0;
  திரும்பு ioctl(sockfd, FIONBIO, &flags);

#அல்லதெனில் defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  unsigned long flags = nonblock ? 1UL : 0UL;
  திரும்பு ioctlsocket(sockfd, (long)FIONBIO, &flags);

#அல்லதெனில் defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  long b = nonblock ? 1L : 0L;
  திரும்பு setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, அளவு(b));

#இல்லையெனில்
#error "no non-blocking method was found/used/set"
#endif
}
