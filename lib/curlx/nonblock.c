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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCNTL_H
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
ಪೂರ್ಣಾಂಕ curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   ಪೂರ್ಣಾಂಕ nonblock   /* TRUE or FALSE */)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  ಪೂರ್ಣಾಂಕ flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  ಆದರೆ(flags < 0)
    ಹಿಂತಿರುಗಿ -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  ಆದರೆ(!!(flags & O_NONBLOCK) == !!nonblock)
    ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ(nonblock)
    flags |= O_NONBLOCK;
  ಇಲ್ಲದಿದ್ದರೆ
    flags &= ~O_NONBLOCK;
  ಹಿಂತಿರುಗಿ sfcntl(sockfd, F_SETFL, flags);

#ಅಥವಾಆದರೆ defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  ದೀರ್ಘ flags = nonblock ? 1L : 0L;
  ಹಿಂತಿರುಗಿ IoctlSocket(sockfd, FIONBIO, (ಅಕ್ಷರ *)&flags);

#ಅಥವಾಆದರೆ defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  ಪೂರ್ಣಾಂಕ flags = nonblock ? 1 : 0;
  ಹಿಂತಿರುಗಿ ioctl(sockfd, FIONBIO, &flags);

#ಅಥವಾಆದರೆ defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags = nonblock ? 1UL : 0UL;
  ಹಿಂತಿರುಗಿ ioctlsocket(sockfd, (ದೀರ್ಘ)FIONBIO, &flags);

#ಅಥವಾಆದರೆ defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  ದೀರ್ಘ b = nonblock ? 1L : 0L;
  ಹಿಂತಿರುಗಿ setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, ಗಾತ್ರ(b));

#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "no non-blocking method was found/used/set"
#ಆದರೆ_ಅಂತ್ಯ
}
