#నిర్వచితం_కాకపోతే శీర్షిక_FAKE_ADDRINFO_H
#నిర్వచించు శీర్షిక_FAKE_ADDRINFO_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_ARES
#అన్తర్భావ <ares.h>
#అయితే_ముగింపు

#ఒకవేళ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(USE_ARES) && (ARES_సంస్కరణ >= 0x011a00) /* >= 1.26.0 */
#నిర్వచించు USE_FAKE_GETADDRINFO 1
#అయితే_ముగింపు

#నిర్వచితమైతే USE_FAKE_GETADDRINFO

#నిర్వచితమైతే HAVE_NETDB_H
#  include <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#అయితే_ముగింపు

శూన్య r_freeaddrinfo(నిర్మాణం addrinfo *cahead);
పూర్ణాంకం r_getaddrinfo(స్థిరరాశి అక్షరం *కణుపు,
                  స్థిరరాశి అక్షరం *service,
                  స్థిరరాశి నిర్మాణం addrinfo *hints,
                  నిర్మాణం addrinfo **res);
#అయితే_ముగింపు /* USE_FAKE_GETADDRINFO */

#అయితే_ముగింపు /* HEADER_FAKE_ADDRINFO_H */
