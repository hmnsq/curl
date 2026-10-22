#अगर_अघोषित HEADER_CURLX_BASENAME_H
#घोषणा HEADER_CURLX_BASENAME_H
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
#समावेश "curl_setup.h"

#अगर_अघोषित HAVE_BASENAME
अक्षर *curlx_basename(अक्षर *path);
#अन्यथा

#अगर_घोषित HAVE_LIBGEN_H
#समावेश <libgen.h>
#पूर्ण_अगर

#घोषणा curlx_basename(x) basename(x)
#पूर्ण_अगर /* !HAVE_BASENAME */

#पूर्ण_अगर /* HEADER_CURLX_BASENAME_H */
