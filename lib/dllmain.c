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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/crypto.h>
#ಆದರೆ_ಅಂತ್ಯ

/* DllMain() must only be defined for Windows DLL builds. */
#ಆದರೆ defined(_WIN32) && !defined(CURL_STATICLIB)

#ಆದರೆ defined(USE_OPENSSL) && \
  !defined(OPENSSL_IS_AWSLC) && !defined(OPENSSL_IS_BORINGSSL) && \
  !defined(LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು PREVENT_OPENSSL_MEMLEAK  /* non-fork OpenSSL */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PREVENT_OPENSSL_MEMLEAK
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  (ಶೂನ್ಯ)hinstDLL;
  (ಶೂನ್ಯ)lpvReserved;

  ಆಯ್ಕೆ(fdwReason) {
  ಸ್ಥಿತಿ DLL_PROCESS_ATTACH:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ DLL_PROCESS_DETACH:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ DLL_THREAD_ATTACH:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ DLL_THREAD_DETACH:
    /* Call OPENSSL_thread_stop to prevent a memory leak in case OpenSSL is
       linked statically.
       https://github.com/curl/curl/issues/12327#issuecomment-1826405944 */
    OPENSSL_thread_stop();
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ /* PREVENT_OPENSSL_MEMLEAK */

#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 && !CURL_STATICLIB */
