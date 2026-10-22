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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_FCNTL_H
#شامل <fcntl.h>
#اگر_ختم

#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#شامل "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
صحیح curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   صحیح nonblock   /* TRUE or FALSE */)
{
#اگر_تعریف_شدہ HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  صحیح flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  اگر(flags < 0)
    واپس -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  اگر(!!(flags & O_NONBLOCK) == !!nonblock)
    واپس 0;
  اگر(nonblock)
    flags |= O_NONBLOCK;
  ورنہ
    flags &= ~O_NONBLOCK;
  واپس sfcntl(sockfd, F_SETFL, flags);

#وگرنہ_اگر defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  طویل flags = nonblock ? 1L : 0L;
  واپس IoctlSocket(sockfd, FIONBIO, (حرف *)&flags);

#وگرنہ_اگر defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  صحیح flags = nonblock ? 1 : 0;
  واپس ioctl(sockfd, FIONBIO, &flags);

#وگرنہ_اگر defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  غیرعلامتی طویل flags = nonblock ? 1UL : 0UL;
  واپس ioctlsocket(sockfd, (طویل)FIONBIO, &flags);

#وگرنہ_اگر defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  طویل b = nonblock ? 1L : 0L;
  واپس setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, حجم(b));

#ورنہ
#غلطی "no non-blocking method was found/used/set"
#اگر_ختم
}
