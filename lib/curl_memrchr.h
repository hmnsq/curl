#నిర్వచితం_కాకపోతే శీర్షిక_CURL_MEMRCHR_H
#నిర్వచించు శీర్షిక_CURL_MEMRCHR_H
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

#నిర్వచితమైతే HAVE_MEMRCHR

#నిర్వచితమైతే HAVE_అక్షరశ్రేణులు_H
#  include <strings.h>
#అయితే_ముగింపు

#లేకపోతే /* HAVE_MEMRCHR */
శూన్య *Curl_memrchr(స్థిరరాశి శూన్య *s, పూర్ణాంకం c, size_t n);
#నిర్వచించు memrchr(x, y, z)  Curl_memrchr(x, y, z)

#అయితే_ముగింపు /* HAVE_MEMRCHR */

#అయితే_ముగింపు /* HEADER_CURL_MEMRCHR_H */
