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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_MACOS_CALL_COPYPROXIES

#ಅನ್ತರ್ಭಾವ "macos.h"

#ಅನ್ತರ್ಭಾವ <SystemConfiguration/SCDynamicStoreCopySpecific.h>

CURLcode Curl_macos_init(ಶೂನ್ಯ)
{
  /*
   * The automagic conversion from IPv4 literals to IPv6 literals only
   * works if the SCDynamicStoreCopyProxies system function gets called
   * first. As curl currently does not support system-wide HTTP proxies, we
   * therefore do not use any value this function might return.
   *
   * This function is only available on macOS and is not needed for
   * IPv4-only builds, hence the conditions for defining
   * CURL_MACOS_CALL_COPYPROXIES in curl_setup.h.
   */
  CFDictionaryRef dict = SCDynamicStoreCopyProxies(ಖಾಲಿ);
  ಆದರೆ(dict)
    CFRelease(dict);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ_ಅಂತ್ಯ
