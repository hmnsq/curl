#اگر_تعریف_نشده سرآیند_CURL_RTSP_H
#تعریف_کن سرآیند_CURL_RTSP_H
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
#اگر_تعریف_نشده CURL_DISABLE_RTSP
CURLcode Curl_rtsp_parseheader(بنية Curl_easy *داده, ثابت حرف *سرآیند);
خارجي ثابت بنية Curl_protocol Curl_protocol_rtsp;
#وإلا
#تعریف_کن Curl_rtsp_parseheader(x, y) CURLE_NOT_BUILT_درون
#پایان_اگر

#تعریف_کن RTSPREQ_NONE CURL_RTSPREQ_NONE
#تعریف_کن RTSPREQ_OPTIONS CURL_RTSPREQ_OPTIONS
#تعریف_کن RTSPREQ_DESCRIBE CURL_RTSPREQ_DESCRIBE
#تعریف_کن RTSPREQ_ANNOUNCE CURL_RTSPREQ_ANNOUNCE
#تعریف_کن RTSPREQ_راهاندازی CURL_RTSPREQ_راهاندازی
#تعریف_کن RTSPREQ_PLAY CURL_RTSPREQ_PLAY
#تعریف_کن RTSPREQ_PAUSE CURL_RTSPREQ_PAUSE
#تعریف_کن RTSPREQ_TEARDOWN CURL_RTSPREQ_TEARDOWN
#تعریف_کن RTSPREQ_GET_PARAMETER CURL_RTSPREQ_GET_PARAMETER
#تعریف_کن RTSPREQ_مجموعه_PARAMETER CURL_RTSPREQ_مجموعه_PARAMETER
#تعریف_کن RTSPREQ_رکورد CURL_RTSPREQ_رکورد
#تعریف_کن RTSPREQ_RECEIVE CURL_RTSPREQ_RECEIVE
#تعریف_کن RTSPREQ_آخرین CURL_RTSPREQ_آخرین

#پایان_اگر /* HEADER_CURL_RTSP_H */
