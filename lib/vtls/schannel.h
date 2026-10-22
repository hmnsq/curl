#यदि_अपरिभाषितम् शीर्षकम्_CURL_SCHANNEL_H
#परिभाषय शीर्षकम्_CURL_SCHANNEL_H
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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_SCHANNEL

#अन्तर्भाव <schannel.h>

#अन्तर्भाव "curl_sspi.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "urldata.h"

बाह्य ध्रुव संरचना Curl_ssl Curl_ssl_schannel;

CURLcode Curl_verify_host(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त);

CURLcode Curl_verify_certificate(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त);

#यदि_अन्तः /* USE_SCHANNEL */
#यदि_अन्तः /* HEADER_CURL_SCHANNEL_H */
