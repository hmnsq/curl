#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_API_H
#وصف_ڪريو سرو_CURL_API_H
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
#ضمن "curl_setup.h"

#وصف_ڪريو CURLEASY_MAGIC_عدد 0xc0dedbadU
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
/* On a debug build, we want to fail hard on easy handles that
 * are not NULL, but no longer have the MAGIC touch. This gives
 * us early warning on things only discovered by valgrind otherwise. */
#وصف_ڪريو GOOD_EASY_HANDLE(x) \
  (((x) && ((x)->magic == CURLEASY_MAGIC_عدد)) ? TRUE : \
   (DEBUGASSERT(!(x)), FALSE))
#وإلا
#وصف_ڪريو GOOD_EASY_HANDLE(x) \
  ((x) && ((x)->magic == CURLEASY_MAGIC_عدد))
#جيڪڏهن_پورو

#وصف_ڪريو CURLMULTI_MAGIC_عدد 0x000bab1e

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
/* On a debug build, we want to fail hard on multi handles that
 * are not NULL, but no longer have the MAGIC touch. This gives
 * us early warning on things only discovered by valgrind otherwise. */
#وصف_ڪريو GOOD_MULTI_HANDLE(x)                         \
  (((x) && (x)->magic == CURLMULTI_MAGIC_عدد) ? TRUE : \
  (DEBUGASSERT(!(x)), FALSE))
#وإلا
#وصف_ڪريو GOOD_MULTI_HANDLE(x) \
  ((x) && (x)->magic == CURLMULTI_MAGIC_عدد)
#جيڪڏهن_پورو

/* the API functions called on a CURL* */
تعريفنوع تعداد {
  CURL_EAPI_FN_easy_cleanup,
  CURL_EAPI_FN_easy_duphandle,
  CURL_EAPI_FN_easy_getinfo,
  CURL_EAPI_FN_easy_سرو,
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
  CURL_EAPI_FN_ws_شروعات_چوکٺو,
  CURL_EAPI_FN_آخري
} Curl_eapi_fn;

/* the API functions called on a CURLM* */
تعريفنوع تعداد {
  CURL_MAPI_FN_multi_جوڙيو_handle,
  CURL_MAPI_FN_multi_سونپيو,
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
  CURL_MAPI_FN_آخري
} Curl_mapi_fn;

#وصف_ڪريو CURL_CBAPI_FN_شروعات        128

/* the callback functions */
تعريفنوع تعداد {
  CURL_CBAPI_FN_easy_chunk_bgn = CURL_CBAPI_FN_شروعات,
  CURL_CBAPI_FN_easy_chunk_پڇاڙي,
  CURL_CBAPI_FN_easy_closesocket,
  CURL_CBAPI_FN_easy_cr_اندريون_read,
  CURL_CBAPI_FN_easy_cr_اندريون_resume_from,
  CURL_CBAPI_FN_easy_cw_ٻاهريون_cb,
  CURL_CBAPI_FN_easy_fdebug,
  CURL_CBAPI_FN_easy_fnmatch_معلومات,
  CURL_CBAPI_FN_easy_fopensocket,
  CURL_CBAPI_FN_easy_fprereq,
  CURL_CBAPI_FN_easy_fprogress,
  CURL_CBAPI_FN_easy_fread_func,
  CURL_CBAPI_FN_easy_fsockopt,
  CURL_CBAPI_FN_easy_fsslctx,
  CURL_CBAPI_FN_easy_fwrite_rtp,
  CURL_CBAPI_FN_easy_fxferinfo,
  CURL_CBAPI_FN_easy_ioctl_func,
  CURL_CBAPI_FN_easy_resolver_شروعات,
  CURL_CBAPI_FN_easy_seek_func,
  CURL_CBAPI_FN_easy_ssh_hostkeyfunc,
  CURL_CBAPI_FN_easy_ssh_keyfunc,
  CURL_CBAPI_FN_easy_trailer_callback,

  CURL_CBAPI_FN_multi_ntfy_cb,
  CURL_CBAPI_FN_multi_ڌڪو_cb,
  CURL_CBAPI_FN_multi_socket_cb,
  CURL_CBAPI_FN_multi_timer_cb,

  CURL_CBAPI_FN_آخري
} Curl_cbapi_fn;

/* EAPI */

#وصف_ڪريو CURL_EAPI_MAX_RECURSION       7

بنية Curl_eapi_ڍير {
  uint16_t ڳڻپ;
  uint16_t calls[CURL_EAPI_MAX_RECURSION];
};

بنية Curl_eapi_guard {
  بنية Curl_easy *معلومات;  /* != NULL if handle stays */
  uint16_t اونهائي;  /* > 0 if this guard was entered */
};

bool Curl_eapi_enter(بنية Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult);
فراغ Curl_eapi_leave(بنية Curl_eapi_guard *guard);

/* Convert an EAPI failure to a header API result */
CURLHcode Curl_eapi_hcode(CURLcode نتيجو);

/* Curl_eapi_enter() checks for curl being NULL, but windows compiler
 * analyzers do not realize this. *sigh* */
#وصف_ڪريو CURL_EAPI_ENTER(g, curl, fn, r) \
  Curl_eapi_enter((g), (curl), CURL_EAPI_FN_##fn, (r)) && (curl)
#وصف_ڪريو CURL_EAPI_LEAVE(g) \
  Curl_eapi_leave(g)

/* MAPI */

#وصف_ڪريو CURL_MAPI_MAX_RECURSION       15

بنية Curl_mapi_ڍير {
  uint8_t ڳڻپ;
  uint8_t calls[CURL_MAPI_MAX_RECURSION];
};

بنية Curl_mapi_guard {
  بنية Curl_multi *multi;  /* != NULL if handle stays */
  uint8_t اونهائي;  /* > 0 if this guard was entered */
};

bool Curl_mapi_enter(بنية Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult);
فراغ Curl_mapi_leave(بنية Curl_mapi_guard *guard);

/* Curl_mapi_enter() checks for m being NULL, but windows compiler
 * analyzers do not realize this. *sigh* */
#وصف_ڪريو CURL_MAPI_ENTER(g, m, fn, r) \
  Curl_mapi_enter((g), (m), CURL_MAPI_FN_##fn, (r)) && (m)
#وصف_ڪريو CURL_MAPI_LEAVE(g) \
  Curl_mapi_leave(g)

فراغ Curl_cbapi_enter(بنية Curl_mapi_guard *guard,
                      بنية Curl_easy *معلومات,
                      بنية Curl_multi *multi,
                      Curl_cbapi_fn fn);
فراغ Curl_cbapi_leave(بنية Curl_mapi_guard *guard);

#وصف_ڪريو CURL_CBAPI_شروعات(g, d, fn) \
  Curl_cbapi_enter((g), (d), عدم, CURL_CBAPI_FN_##fn)
#وصف_ڪريو CURL_CBAPI_MULTI_شروعات(g, m, fn) \
  Curl_cbapi_enter((g), عدم, (m), CURL_CBAPI_FN_##fn)
#وصف_ڪريو CURL_CBAPI_پڇاڙي(g) \
  Curl_cbapi_leave(g)
#وصف_ڪريو CURL_CBAPI_MULTI_پڇاڙي(g) \
  Curl_cbapi_leave(g)

bool Curl_api_is_اندريون_callback(بنية Curl_easy *معلومات);
bool Curl_api_multi_is_اندريون_callback(بنية Curl_multi *multi);

#جيڪڏهن_پورو /* HEADER_CURL_API_H */
