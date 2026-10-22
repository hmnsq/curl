#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_API_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_API_H
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

#ವ್ಯಾಖ್ಯಾನಿಸು CURLEASY_MAGIC_ಸಂಖ್ಯೆ 0xc0dedbadU
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
/* On a debug build, we want to fail hard on easy handles that
 * are not NULL, but no longer have the MAGIC touch. This gives
 * us early warning on things only discovered by valgrind otherwise. */
#ವ್ಯಾಖ್ಯಾನಿಸು GOOD_EASY_HANDLE(x) \
  (((x) && ((x)->magic == CURLEASY_MAGIC_ಸಂಖ್ಯೆ)) ? TRUE : \
   (DEBUGASSERT(!(x)), FALSE))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು GOOD_EASY_HANDLE(x) \
  ((x) && ((x)->magic == CURLEASY_MAGIC_ಸಂಖ್ಯೆ))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CURLMULTI_MAGIC_ಸಂಖ್ಯೆ 0x000bab1e

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
/* On a debug build, we want to fail hard on multi handles that
 * are not NULL, but no longer have the MAGIC touch. This gives
 * us early warning on things only discovered by valgrind otherwise. */
#ವ್ಯಾಖ್ಯಾನಿಸು GOOD_MULTI_HANDLE(x)                         \
  (((x) && (x)->magic == CURLMULTI_MAGIC_ಸಂಖ್ಯೆ) ? TRUE : \
  (DEBUGASSERT(!(x)), FALSE))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು GOOD_MULTI_HANDLE(x) \
  ((x) && (x)->magic == CURLMULTI_MAGIC_ಸಂಖ್ಯೆ)
#ಆದರೆ_ಅಂತ್ಯ

/* the API functions called on a CURL* */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURL_EAPI_FN_easy_cleanup,
  CURL_EAPI_FN_easy_duphandle,
  CURL_EAPI_FN_easy_getinfo,
  CURL_EAPI_FN_easy_ಶೀರ್ಷಿಕೆ,
  CURL_EAPI_FN_easy_nextheader,
  CURL_EAPI_FN_easy_pause,
  CURL_EAPI_FN_easy_perform_ev,
  CURL_EAPI_FN_easy_perform,
  CURL_EAPI_FN_easy_recv,
  CURL_EAPI_FN_easy_reset,
  CURL_EAPI_FN_easy_send,
  CURL_EAPI_FN_easy_setopt,
  CURL_EAPI_FN_easy_ssls_export,
  CURL_EAPI_FN_easy_ssls_import,
  CURL_EAPI_FN_easy_upkeep,
  CURL_EAPI_FN_ws_recv,
  CURL_EAPI_FN_ws_send,
  CURL_EAPI_FN_ws_ಆರಂಭ_ಚೌಕಟ್ಟು,
  CURL_EAPI_FN_ಕೊನೆಯ
} Curl_eapi_fn;

/* the API functions called on a CURLM* */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURL_MAPI_FN_multi_ಕೂಡಿಸು_handle,
  CURL_MAPI_FN_multi_ನಿಯೋಜಿಸು,
  CURL_MAPI_FN_multi_cleanup,
  CURL_MAPI_FN_multi_fdset,
  CURL_MAPI_FN_multi_get_handles,
  CURL_MAPI_FN_multi_get_offt,
  CURL_MAPI_FN_multi_info_read,
  CURL_MAPI_FN_multi_notify_disable,
  CURL_MAPI_FN_multi_notify_enable,
  CURL_MAPI_FN_multi_perform,
  CURL_MAPI_FN_multi_poll,
  CURL_MAPI_FN_multi_remove_handle,
  CURL_MAPI_FN_multi_setopt,
  CURL_MAPI_FN_multi_socket_action,
  CURL_MAPI_FN_multi_socket_all,
  CURL_MAPI_FN_multi_socket,
  CURL_MAPI_FN_multi_timeout,
  CURL_MAPI_FN_multi_wait,
  CURL_MAPI_FN_multi_waitfds,
  CURL_MAPI_FN_ಕೊನೆಯ
} Curl_mapi_fn;

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CBAPI_FN_ಆರಂಭ        128

/* the callback functions */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURL_CBAPI_FN_easy_chunk_bgn = CURL_CBAPI_FN_ಆರಂಭ,
  CURL_CBAPI_FN_easy_chunk_ಕೊನೆ,
  CURL_CBAPI_FN_easy_closesocket,
  CURL_CBAPI_FN_easy_cr_ಒಳ_read,
  CURL_CBAPI_FN_easy_cr_ಒಳ_resume_from,
  CURL_CBAPI_FN_easy_cw_ಹೊರಹೋಗು_cb,
  CURL_CBAPI_FN_easy_fdebug,
  CURL_CBAPI_FN_easy_fnmatch_ದತ್ತಾಂಶ,
  CURL_CBAPI_FN_easy_fopensocket,
  CURL_CBAPI_FN_easy_fprereq,
  CURL_CBAPI_FN_easy_fprogress,
  CURL_CBAPI_FN_easy_fread_func,
  CURL_CBAPI_FN_easy_fsockopt,
  CURL_CBAPI_FN_easy_fsslctx,
  CURL_CBAPI_FN_easy_fwrite_rtp,
  CURL_CBAPI_FN_easy_fxferinfo,
  CURL_CBAPI_FN_easy_ioctl_func,
  CURL_CBAPI_FN_easy_resolver_ಆರಂಭ,
  CURL_CBAPI_FN_easy_seek_func,
  CURL_CBAPI_FN_easy_ssh_hostkeyfunc,
  CURL_CBAPI_FN_easy_ssh_keyfunc,
  CURL_CBAPI_FN_easy_trailer_callback,

  CURL_CBAPI_FN_multi_ntfy_cb,
  CURL_CBAPI_FN_multi_ತಳ್ಳು_cb,
  CURL_CBAPI_FN_multi_socket_cb,
  CURL_CBAPI_FN_multi_timer_cb,

  CURL_CBAPI_FN_ಕೊನೆಯ
} Curl_cbapi_fn;

/* EAPI */

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_EAPI_MAX_RECURSION       7

ರಚನೆ Curl_eapi_ರಾಶಿ {
  uint16_t ಎಣಿಕೆ_2;
  uint16_t calls[CURL_EAPI_MAX_RECURSION];
};

ರಚನೆ Curl_eapi_guard {
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;  /* != NULL if handle stays */
  uint16_t ಆಳ;  /* > 0 if this guard was entered */
};

bool Curl_eapi_enter(ರಚನೆ Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult);
ಶೂನ್ಯ Curl_eapi_leave(ರಚನೆ Curl_eapi_guard *guard);

/* Convert an EAPI failure to a header API result */
CURLHcode Curl_eapi_hcode(CURLcode ಫಲಿತಾಂಶ);

/* Curl_eapi_enter() checks for curl being NULL, but windows compiler
 * analyzers do not realize this. *sigh* */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_EAPI_ENTER(g, curl, fn, r) \
  Curl_eapi_enter((g), (curl), CURL_EAPI_FN_##fn, (r)) && (curl)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_EAPI_LEAVE(g) \
  Curl_eapi_leave(g)

/* MAPI */

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MAPI_MAX_RECURSION       15

ರಚನೆ Curl_mapi_ರಾಶಿ {
  uint8_t ಎಣಿಕೆ_2;
  uint8_t calls[CURL_MAPI_MAX_RECURSION];
};

ರಚನೆ Curl_mapi_guard {
  ರಚನೆ Curl_multi *multi;  /* != NULL if handle stays */
  uint8_t ಆಳ;  /* > 0 if this guard was entered */
};

bool Curl_mapi_enter(ರಚನೆ Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult);
ಶೂನ್ಯ Curl_mapi_leave(ರಚನೆ Curl_mapi_guard *guard);

/* Curl_mapi_enter() checks for m being NULL, but windows compiler
 * analyzers do not realize this. *sigh* */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MAPI_ENTER(g, m, fn, r) \
  Curl_mapi_enter((g), (m), CURL_MAPI_FN_##fn, (r)) && (m)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MAPI_LEAVE(g) \
  Curl_mapi_leave(g)

ಶೂನ್ಯ Curl_cbapi_enter(ರಚನೆ Curl_mapi_guard *guard,
                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ Curl_multi *multi,
                      Curl_cbapi_fn fn);
ಶೂನ್ಯ Curl_cbapi_leave(ರಚನೆ Curl_mapi_guard *guard);

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CBAPI_ಆರಂಭ(g, d, fn) \
  Curl_cbapi_enter((g), (d), ಖಾಲಿ, CURL_CBAPI_FN_##fn)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CBAPI_MULTI_ಆರಂಭ(g, m, fn) \
  Curl_cbapi_enter((g), ಖಾಲಿ, (m), CURL_CBAPI_FN_##fn)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CBAPI_ಕೊನೆ(g) \
  Curl_cbapi_leave(g)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CBAPI_MULTI_ಕೊನೆ(g) \
  Curl_cbapi_leave(g)

bool Curl_api_is_ಒಳ_callback(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
bool Curl_api_multi_is_ಒಳ_callback(ರಚನೆ Curl_multi *multi);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_API_H */
