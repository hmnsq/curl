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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif
#ifdef HAVE_FCNTL_H
#അന്തര്ഭാവ <fcntl.h>
#endif

#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
പൂർണ്ണസംഖ്യ curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   പൂർണ്ണസംഖ്യ nonblock   /* TRUE or FALSE */)
{
#ifdef HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  പൂർണ്ണസംഖ്യ flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  എങ്കിൽ(flags < 0)
    തിരികെ -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  എങ്കിൽ(!!(flags & O_NONBLOCK) == !!nonblock)
    തിരികെ 0;
  എങ്കിൽ(nonblock)
    flags |= O_NONBLOCK;
  അല്ലെങ്കിൽ
    flags &= ~O_NONBLOCK;
  തിരികെ sfcntl(sockfd, F_SETFL, flags);

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  long flags = nonblock ? 1L : 0L;
  തിരികെ IoctlSocket(sockfd, FIONBIO, (അക്ഷരം *)&flags);

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  പൂർണ്ണസംഖ്യ flags = nonblock ? 1 : 0;
  തിരികെ ioctl(sockfd, FIONBIO, &flags);

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  unsigned long flags = nonblock ? 1UL : 0UL;
  തിരികെ ioctlsocket(sockfd, (long)FIONBIO, &flags);

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  long b = nonblock ? 1L : 0L;
  തിരികെ setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, വലുപ്പം(b));

#അല്ലെങ്കിൽ
#error "no non-blocking method was found/used/set"
#endif
}
