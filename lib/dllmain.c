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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_OPENSSL
#അന്തര്ഭാവ <openssl/crypto.h>
#endif

/* DllMain() must only be defined for Windows DLL builds. */
#എങ്കിൽ defined(_WIN32) && !defined(CURL_STATICLIB)

#എങ്കിൽ defined(USE_OPENSSL) && \
  !defined(OPENSSL_IS_AWSLC) && !defined(OPENSSL_IS_BORINGSSL) && \
  !defined(LIBRESSL_VERSION_സംഖ്യ)
#define PREVENT_OPENSSL_MEMLEAK  /* non-fork OpenSSL */
#endif

#ifdef PREVENT_OPENSSL_MEMLEAK
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  (ശൂന്യം)hinstDLL;
  (ശൂന്യം)lpvReserved;

  തിരഞ്ഞെടുപ്പ്(fdwReason) {
  അവസ്ഥ DLL_PROCESS_ATTACH:
    നിർത്തുക;
  അവസ്ഥ DLL_PROCESS_DETACH:
    നിർത്തുക;
  അവസ്ഥ DLL_THREAD_ATTACH:
    നിർത്തുക;
  അവസ്ഥ DLL_THREAD_DETACH:
    /* Call OPENSSL_thread_stop to prevent a memory leak in case OpenSSL is
       linked statically.
       https://github.com/curl/curl/issues/12327#issuecomment-1826405944 */
    OPENSSL_thread_stop();
    നിർത്തുക;
  }
  തിരികെ TRUE;
}
#endif /* PREVENT_OPENSSL_MEMLEAK */

#endif /* _WIN32 && !CURL_STATICLIB */
