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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif
#ifdef HAVE_FCNTL_H
#অন্তর্ভাব <fcntl.h>
#endif

#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
পূর্ণসংখ্যা curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   পূর্ণসংখ্যা nonblock   /* TRUE or FALSE */)
{
#ifdef HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  পূর্ণসংখ্যা flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  যদি(flags < 0)
    ফেরত -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  যদি(!!(flags & O_NONBLOCK) == !!nonblock)
    ফেরত 0;
  যদি(nonblock)
    flags |= O_NONBLOCK;
  অন্যথা
    flags &= ~O_NONBLOCK;
  ফেরত sfcntl(sockfd, F_SETFL, flags);

#নতুবা_যদি defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  long flags = nonblock ? 1L : 0L;
  ফেরত IoctlSocket(sockfd, FIONBIO, (অক্ষর *)&flags);

#নতুবা_যদি defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  পূর্ণসংখ্যা flags = nonblock ? 1 : 0;
  ফেরত ioctl(sockfd, FIONBIO, &flags);

#নতুবা_যদি defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  unsigned long flags = nonblock ? 1UL : 0UL;
  ফেরত ioctlsocket(sockfd, (long)FIONBIO, &flags);

#নতুবা_যদি defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  long b = nonblock ? 1L : 0L;
  ফেরত setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, আকার(b));

#অন্যথা
#error "no non-blocking method was found/used/set"
#endif
}
