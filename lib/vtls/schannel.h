#اگر_غیر_تعریف_شدہ سرخی_CURL_SCHANNEL_H
#تعریف_کرو سرخی_CURL_SCHANNEL_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_SCHANNEL

#شامل <schannel.h>

#شامل "curl_sspi.h"
#شامل "cfilters.h"
#شامل "urldata.h"

خارجی ثابت ساخت Curl_ssl Curl_ssl_schannel;

CURLcode Curl_verify_host(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا);

CURLcode Curl_verify_certificate(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا);

#اگر_ختم /* USE_SCHANNEL */
#اگر_ختم /* HEADER_CURL_SCHANNEL_H */
