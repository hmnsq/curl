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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/crypto.h>
#ਜੇ_ਅੰਤ

/* DllMain() must only be defined for Windows DLL builds. */
#ਜੇ defined(_WIN32) && !defined(CURL_STATICLIB)

#ਜੇ defined(USE_OPENSSL) && \
  !defined(OPENSSL_IS_AWSLC) && !defined(OPENSSL_IS_BORINGSSL) && \
  !defined(LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PREVENT_OPENSSL_MEMLEAK  /* non-fork OpenSSL */
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PREVENT_OPENSSL_MEMLEAK
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  (ਖਾਲੀ)hinstDLL;
  (ਖਾਲੀ)lpvReserved;

  ਚੋਣ(fdwReason) {
  ਹਾਲਤ DLL_PROCESS_ATTACH:
    ਰੋਕੋ;
  ਹਾਲਤ DLL_PROCESS_DETACH:
    ਰੋਕੋ;
  ਹਾਲਤ DLL_THREAD_ATTACH:
    ਰੋਕੋ;
  ਹਾਲਤ DLL_THREAD_DETACH:
    /* Call OPENSSL_thread_stop to prevent a memory leak in case OpenSSL is
       linked statically.
       https://github.com/curl/curl/issues/12327#issuecomment-1826405944 */
    OPENSSL_thread_stop();
    ਰੋਕੋ;
  }
  ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ /* PREVENT_OPENSSL_MEMLEAK */

#ਜੇ_ਅੰਤ /* _WIN32 && !CURL_STATICLIB */
