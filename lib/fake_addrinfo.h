#यदि_अपरिभाषितम् शीर्षकम्_FAKE_ADDRINFO_H
#परिभाषय शीर्षकम्_FAKE_ADDRINFO_H
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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_ARES
#अन्तर्भाव <ares.h>
#यदि_अन्तः

#यदि defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(USE_ARES) && (ARES_संस्करणम् >= 0x011a00) /* >= 1.26.0 */
#परिभाषय USE_FAKE_GETADDRINFO 1
#यदि_अन्तः

#यदि_परिभाषितम् USE_FAKE_GETADDRINFO

#यदि_परिभाषितम् HAVE_NETDB_H
#  include <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#यदि_अन्तः

शून्य r_freeaddrinfo(संरचना addrinfo *cahead);
पूर्णाङ्क r_getaddrinfo(ध्रुव वर्ण *ग्रन्थिः,
                  ध्रुव वर्ण *service,
                  ध्रुव संरचना addrinfo *hints,
                  संरचना addrinfo **res);
#यदि_अन्तः /* USE_FAKE_GETADDRINFO */

#यदि_अन्तः /* HEADER_FAKE_ADDRINFO_H */
