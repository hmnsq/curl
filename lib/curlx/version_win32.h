#నిర్వచితం_కాకపోతే శీర్షిక_CURL_సంస్కరణ_WIN32_H
#నిర్వచించు శీర్షిక_CURL_సంస్కరణ_WIN32_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
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

#నిర్వచితమైతే _WIN32

/* Version condition */
రకనిర్వచనం గణన {
  సంస్కరణ_LESS_THAN,
  సంస్కరణ_LESS_THAN_EQUAL,
  సంస్కరణ_EQUAL,
  సంస్కరణ_GREATER_THAN_EQUAL,
  సంస్కరణ_GREATER_THAN
} సంస్కరణ_Condition;

/* Platform identifier */
రకనిర్వచనం గణన {
  PLATFORM_DONT_CARE,
  PLATFORM_WINDOWS,
  PLATFORM_WINNT
} PlatformIdentifier;

#నిర్వచితమైతే CURL_WINDOWS_UWP
#నిర్వచించు curlx_verify_windows_init() Curl_nop_stmt
#లేకపోతే
శూన్య curlx_verify_windows_init(శూన్య);
#అయితే_ముగింపు
/* This is used to verify if we are running on a specific Windows version */
bool curlx_verify_windows_సంస్కరణ(స్థిరరాశి గుర్తులేని పూర్ణాంకం major_సంస్కరణ,
                                  స్థిరరాశి గుర్తులేని పూర్ణాంకం minor_సంస్కరణ,
                                  స్థిరరాశి గుర్తులేని పూర్ణాంకం build_సంస్కరణ,
                                  స్థిరరాశి PlatformIdentifier platform,
                                  స్థిరరాశి సంస్కరణ_Condition condition);

#అయితే_ముగింపు /* _WIN32 */

#అయితే_ముగింపు /* HEADER_CURL_VERSION_WIN32_H */
