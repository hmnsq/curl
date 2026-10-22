#اگر_تعریف_نشده سرآیند_CURL_MQTT_H
#تعریف_کن سرآیند_CURL_MQTT_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Björn Stenberg, <bjorn@haxx.se>
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
#اگر_تعریف_نشده CURL_DISABLE_MQTT
خارجي ثابت بنية Curl_protocol Curl_protocol_mqtt;
#اگر_تعریف_شده USE_SSL
خارجي ثابت بنية Curl_protocol Curl_protocol_mqtts;
#پایان_اگر
#پایان_اگر

#پایان_اگر /* HEADER_CURL_MQTT_H */
