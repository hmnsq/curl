#यदि_अपरिभाषितम् शीर्षकम्_CURL_VTLS_SPACK_H
#परिभाषय शीर्षकम्_CURL_VTLS_SPACK_H
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

#यदि defined(USE_SSL) && defined(USE_SSLS_EXPORT)

संरचना dynbuf;
संरचना Curl_ssl_session;

CURLcode Curl_ssl_session_pack(संरचना Curl_easy *दत्त,
                               संरचना Curl_ssl_session *s,
                               संरचना dynbuf *buf);

CURLcode Curl_ssl_session_unpack(संरचना Curl_easy *दत्त,
                                 ध्रुव शून्य *bufv, size_t buflen,
                                 संरचना Curl_ssl_session **ps);

#यदि_अन्तः /* USE_SSL && USE_SSLS_EXPORT */

#यदि_अन्तः /* HEADER_CURL_VTLS_SPACK_H */
