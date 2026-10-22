#నిర్వచితం_కాకపోతే శీర్షిక_CURLX_BASENAME_H
#నిర్వచించు శీర్షిక_CURLX_BASENAME_H
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

#నిర్వచితం_కాకపోతే HAVE_BASENAME
అక్షరం *curlx_basename(అక్షరం *మార్గం);
#లేకపోతే

#నిర్వచితమైతే HAVE_LIBGEN_H
#అన్తర్భావ <libgen.h>
#అయితే_ముగింపు

#నిర్వచించు curlx_basename(x) basename(x)
#అయితే_ముగింపు /* !HAVE_BASENAME */

#అయితే_ముగింపు /* HEADER_CURLX_BASENAME_H */
