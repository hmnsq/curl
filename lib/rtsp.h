#अगर_अघोषित HEADER_CURL_RTSP_H
#घोषणा HEADER_CURL_RTSP_H
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
#अगर_अघोषित CURL_DISABLE_RTSP
CURLcode Curl_rtsp_parseheader(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *header);
बाह्य स्थिर काष्ठा Curl_protocol Curl_protocol_rtsp;
#अन्यथा
#घोषणा Curl_rtsp_parseheader(x, y) CURLE_NOT_BUILT_IN
#पूर्ण_अगर

#घोषणा RTSPREQ_NONE CURL_RTSPREQ_NONE
#घोषणा RTSPREQ_OPTIONS CURL_RTSPREQ_OPTIONS
#घोषणा RTSPREQ_DESCRIBE CURL_RTSPREQ_DESCRIBE
#घोषणा RTSPREQ_ANNOUNCE CURL_RTSPREQ_ANNOUNCE
#घोषणा RTSPREQ_SETUP CURL_RTSPREQ_SETUP
#घोषणा RTSPREQ_PLAY CURL_RTSPREQ_PLAY
#घोषणा RTSPREQ_PAUSE CURL_RTSPREQ_PAUSE
#घोषणा RTSPREQ_TEARDOWN CURL_RTSPREQ_TEARDOWN
#घोषणा RTSPREQ_GET_PARAMETER CURL_RTSPREQ_GET_PARAMETER
#घोषणा RTSPREQ_SET_PARAMETER CURL_RTSPREQ_SET_PARAMETER
#घोषणा RTSPREQ_RECORD CURL_RTSPREQ_RECORD
#घोषणा RTSPREQ_RECEIVE CURL_RTSPREQ_RECEIVE
#घोषणा RTSPREQ_LAST CURL_RTSPREQ_LAST

#पूर्ण_अगर /* HEADER_CURL_RTSP_H */
