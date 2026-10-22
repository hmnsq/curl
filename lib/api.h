#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_API_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_API_H
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLEASY_MAGIC_ਸੰਖਿਆ 0xc0dedbadU
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
/* On a debug build, we want to fail hard on easy handles that
 * are not NULL, but no longer have the MAGIC touch. This gives
 * us early warning on things only discovered by valgrind otherwise. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GOOD_EASY_HANDLE(x) \
  (((x) && ((x)->magic == CURLEASY_MAGIC_ਸੰਖਿਆ)) ? TRUE : \
   (DEBUGASSERT(!(x)), FALSE))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GOOD_EASY_HANDLE(x) \
  ((x) && ((x)->magic == CURLEASY_MAGIC_ਸੰਖਿਆ))
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLMULTI_MAGIC_ਸੰਖਿਆ 0x000bab1e

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
/* On a debug build, we want to fail hard on multi handles that
 * are not NULL, but no longer have the MAGIC touch. This gives
 * us early warning on things only discovered by valgrind otherwise. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GOOD_MULTI_HANDLE(x)                         \
  (((x) && (x)->magic == CURLMULTI_MAGIC_ਸੰਖਿਆ) ? TRUE : \
  (DEBUGASSERT(!(x)), FALSE))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GOOD_MULTI_HANDLE(x) \
  ((x) && (x)->magic == CURLMULTI_MAGIC_ਸੰਖਿਆ)
#ਜੇ_ਅੰਤ

/* the API functions called on a CURL* */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_EAPI_FN_easy_cleanup,
  CURL_EAPI_FN_easy_duphandle,
  CURL_EAPI_FN_easy_getinfo,
  CURL_EAPI_FN_easy_ਸਿਰਲੇਖ,
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
  CURL_EAPI_FN_ws_ਸ਼ੁਰੂ_ਚੌਖਟਾ,
  CURL_EAPI_FN_ਆਖਰੀ
} Curl_eapi_fn;

/* the API functions called on a CURLM* */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_MAPI_FN_multi_ਜੋੜੋ_handle,
  CURL_MAPI_FN_multi_ਸੌਂਪੋ,
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
  CURL_MAPI_FN_ਆਖਰੀ
} Curl_mapi_fn;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CBAPI_FN_ਸ਼ੁਰੂ        128

/* the callback functions */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_CBAPI_FN_easy_chunk_bgn = CURL_CBAPI_FN_ਸ਼ੁਰੂ,
  CURL_CBAPI_FN_easy_chunk_ਸਿਰਾ,
  CURL_CBAPI_FN_easy_closesocket,
  CURL_CBAPI_FN_easy_cr_ਵਿੱਚ_read,
  CURL_CBAPI_FN_easy_cr_ਵਿੱਚ_resume_from,
  CURL_CBAPI_FN_easy_cw_ਬਾਹਰਲਾ_cb,
  CURL_CBAPI_FN_easy_fdebug,
  CURL_CBAPI_FN_easy_fnmatch_ਅੰਕੜੇ,
  CURL_CBAPI_FN_easy_fopensocket,
  CURL_CBAPI_FN_easy_fprereq,
  CURL_CBAPI_FN_easy_fprogress,
  CURL_CBAPI_FN_easy_fread_func,
  CURL_CBAPI_FN_easy_fsockopt,
  CURL_CBAPI_FN_easy_fsslctx,
  CURL_CBAPI_FN_easy_fwrite_rtp,
  CURL_CBAPI_FN_easy_fxferinfo,
  CURL_CBAPI_FN_easy_ioctl_func,
  CURL_CBAPI_FN_easy_resolver_ਸ਼ੁਰੂ,
  CURL_CBAPI_FN_easy_seek_func,
  CURL_CBAPI_FN_easy_ssh_hostkeyfunc,
  CURL_CBAPI_FN_easy_ssh_keyfunc,
  CURL_CBAPI_FN_easy_trailer_callback,

  CURL_CBAPI_FN_multi_ntfy_cb,
  CURL_CBAPI_FN_multi_ਧੱਕੋ_cb,
  CURL_CBAPI_FN_multi_socket_cb,
  CURL_CBAPI_FN_multi_timer_cb,

  CURL_CBAPI_FN_ਆਖਰੀ
} Curl_cbapi_fn;

/* EAPI */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_EAPI_MAX_RECURSION       7

ਬਣਤਰ Curl_eapi_ਢੇਰ {
  uint16_t ਗਿਣਤੀ_2;
  uint16_t calls[CURL_EAPI_MAX_RECURSION];
};

ਬਣਤਰ Curl_eapi_guard {
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;  /* != NULL if handle stays */
  uint16_t ਡੂੰਘਾਈ;  /* > 0 if this guard was entered */
};

bool Curl_eapi_enter(ਬਣਤਰ Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult);
ਖਾਲੀ Curl_eapi_leave(ਬਣਤਰ Curl_eapi_guard *guard);

/* Convert an EAPI failure to a header API result */
CURLHcode Curl_eapi_hcode(CURLcode ਨਤੀਜਾ);

/* Curl_eapi_enter() checks for curl being NULL, but windows compiler
 * analyzers do not realize this. *sigh* */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_EAPI_ENTER(g, curl, fn, r) \
  Curl_eapi_enter((g), (curl), CURL_EAPI_FN_##fn, (r)) && (curl)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_EAPI_LEAVE(g) \
  Curl_eapi_leave(g)

/* MAPI */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MAPI_MAX_RECURSION       15

ਬਣਤਰ Curl_mapi_ਢੇਰ {
  uint8_t ਗਿਣਤੀ_2;
  uint8_t calls[CURL_MAPI_MAX_RECURSION];
};

ਬਣਤਰ Curl_mapi_guard {
  ਬਣਤਰ Curl_multi *multi;  /* != NULL if handle stays */
  uint8_t ਡੂੰਘਾਈ;  /* > 0 if this guard was entered */
};

bool Curl_mapi_enter(ਬਣਤਰ Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult);
ਖਾਲੀ Curl_mapi_leave(ਬਣਤਰ Curl_mapi_guard *guard);

/* Curl_mapi_enter() checks for m being NULL, but windows compiler
 * analyzers do not realize this. *sigh* */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MAPI_ENTER(g, m, fn, r) \
  Curl_mapi_enter((g), (m), CURL_MAPI_FN_##fn, (r)) && (m)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MAPI_LEAVE(g) \
  Curl_mapi_leave(g)

ਖਾਲੀ Curl_cbapi_enter(ਬਣਤਰ Curl_mapi_guard *guard,
                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ Curl_multi *multi,
                      Curl_cbapi_fn fn);
ਖਾਲੀ Curl_cbapi_leave(ਬਣਤਰ Curl_mapi_guard *guard);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CBAPI_ਸ਼ੁਰੂ(g, d, fn) \
  Curl_cbapi_enter((g), (d), ਨਿਰਾ, CURL_CBAPI_FN_##fn)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CBAPI_MULTI_ਸ਼ੁਰੂ(g, m, fn) \
  Curl_cbapi_enter((g), ਨਿਰਾ, (m), CURL_CBAPI_FN_##fn)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CBAPI_ਸਿਰਾ(g) \
  Curl_cbapi_leave(g)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CBAPI_MULTI_ਸਿਰਾ(g) \
  Curl_cbapi_leave(g)

bool Curl_api_is_ਵਿੱਚ_callback(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
bool Curl_api_multi_is_ਵਿੱਚ_callback(ਬਣਤਰ Curl_multi *multi);

#ਜੇ_ਅੰਤ /* HEADER_CURL_API_H */
