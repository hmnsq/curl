#ifndef HEADER_CURL_SCHANNEL_H
#define HEADER_CURL_SCHANNEL_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Marc Hoersken, <info@marc-hoersken.de>, et al.
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
#ضمن "curl_setup.h"

#ifdef USE_SCHANNEL

#ضمن <schannel.h>

#ضمن "curl_sspi.h"
#ضمن "cfilters.h"
#ضمن "urldata.h"

خارجي ثابت بنية Curl_ssl Curl_ssl_schannel;

CURLcode Curl_verify_host(بنية Curl_cfilter *cf, بنية Curl_easy *data);

CURLcode Curl_verify_certificate(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data);

#endif /* USE_SCHANNEL */
#endif /* HEADER_CURL_SCHANNEL_H */
