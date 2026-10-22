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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_OPENSSL
#شامل <openssl/crypto.h>
#اگر_ختم

/* DllMain() must only be defined for Windows DLL builds. */
#اگر defined(_WIN32) && !defined(CURL_STATICLIB)

#اگر defined(USE_OPENSSL) && \
  !defined(OPENSSL_IS_AWSLC) && !defined(OPENSSL_IS_BORINGSSL) && \
  !defined(LIBRESSL_نسخہ_نمبر)
#تعریف_کرو PREVENT_OPENSSL_MEMLEAK  /* non-fork OpenSSL */
#اگر_ختم

#اگر_تعریف_شدہ PREVENT_OPENSSL_MEMLEAK
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  (خالی)hinstDLL;
  (خالی)lpvReserved;

  انتخاب(fdwReason) {
  حالت DLL_PROCESS_ATTACH:
    توڑو;
  حالت DLL_PROCESS_DETACH:
    توڑو;
  حالت DLL_THREAD_ATTACH:
    توڑو;
  حالت DLL_THREAD_DETACH:
    /* Call OPENSSL_thread_stop to prevent a memory leak in case OpenSSL is
       linked statically.
       https://github.com/curl/curl/issues/12327#issuecomment-1826405944 */
    OPENSSL_thread_stop();
    توڑو;
  }
  واپس TRUE;
}
#اگر_ختم /* PREVENT_OPENSSL_MEMLEAK */

#اگر_ختم /* _WIN32 && !CURL_STATICLIB */
