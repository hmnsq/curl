#جے_بنا_تعریف سرخی_CURL_API_H
#تعریف_کرو سرخی_CURL_API_H
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

#تعریف_کرو CURLEASY_MAGIC_نمبر 0xc0dedbadU
#جے_تعریف_شدہ DEBUGBUILD
/* On a debug build, we want to fail hard on easy handles that
 * are not NULL, but no longer have the MAGIC touch. This gives
 * us early warning on things only discovered by valgrind otherwise. */
#تعریف_کرو GOOD_EASY_HANDLE(x) \
  (((x) && ((x)->magic == CURLEASY_MAGIC_نمبر)) ? TRUE : \
   (DEBUGASSERT(!(x)), FALSE))
#وإلا
#تعریف_کرو GOOD_EASY_HANDLE(x) \
  ((x) && ((x)->magic == CURLEASY_MAGIC_نمبر))
#جے_انت

#تعریف_کرو CURLMULTI_MAGIC_نمبر 0x000bab1e

#جے_تعریف_شدہ DEBUGBUILD
/* On a debug build, we want to fail hard on multi handles that
 * are not NULL, but no longer have the MAGIC touch. This gives
 * us early warning on things only discovered by valgrind otherwise. */
#تعریف_کرو GOOD_MULTI_HANDLE(x)                         \
  (((x) && (x)->magic == CURLMULTI_MAGIC_نمبر) ? TRUE : \
  (DEBUGASSERT(!(x)), FALSE))
#وإلا
#تعریف_کرو GOOD_MULTI_HANDLE(x) \
  ((x) && (x)->magic == CURLMULTI_MAGIC_نمبر)
#جے_انت

/* the API functions called on a CURL* */
تعريفنوع تعداد {
  CURL_EAPI_FN_easy_cleanup,
  CURL_EAPI_FN_easy_duphandle,
  CURL_EAPI_FN_easy_getinfo,
  CURL_EAPI_FN_easy_سرخی,
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
  CURL_EAPI_FN_ws_شروع_چوکھٹا,
  CURL_EAPI_FN_آخری
} Curl_eapi_fn;

/* the API functions called on a CURLM* */
تعريفنوع تعداد {
  CURL_MAPI_FN_multi_جوڑو_handle,
  CURL_MAPI_FN_multi_سونپو,
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
  CURL_MAPI_FN_آخری
} Curl_mapi_fn;

#تعریف_کرو CURL_CBAPI_FN_شروع        128

/* the callback functions */
تعريفنوع تعداد {
  CURL_CBAPI_FN_easy_chunk_bgn = CURL_CBAPI_FN_شروع,
  CURL_CBAPI_FN_easy_chunk_سرا,
  CURL_CBAPI_FN_easy_closesocket,
  CURL_CBAPI_FN_easy_cr_وچ_read,
  CURL_CBAPI_FN_easy_cr_وچ_resume_from,
  CURL_CBAPI_FN_easy_cw_باہرلا_cb,
  CURL_CBAPI_FN_easy_fdebug,
  CURL_CBAPI_FN_easy_fnmatch_انکڑے,
  CURL_CBAPI_FN_easy_fopensocket,
  CURL_CBAPI_FN_easy_fprereq,
  CURL_CBAPI_FN_easy_fprogress,
  CURL_CBAPI_FN_easy_fread_func,
  CURL_CBAPI_FN_easy_fsockopt,
  CURL_CBAPI_FN_easy_fsslctx,
  CURL_CBAPI_FN_easy_fwrite_rtp,
  CURL_CBAPI_FN_easy_fxferinfo,
  CURL_CBAPI_FN_easy_ioctl_func,
  CURL_CBAPI_FN_easy_resolver_شروع,
  CURL_CBAPI_FN_easy_seek_func,
  CURL_CBAPI_FN_easy_ssh_hostkeyfunc,
  CURL_CBAPI_FN_easy_ssh_keyfunc,
  CURL_CBAPI_FN_easy_trailer_callback,

  CURL_CBAPI_FN_multi_ntfy_cb,
  CURL_CBAPI_FN_multi_ڈالو_cb,
  CURL_CBAPI_FN_multi_socket_cb,
  CURL_CBAPI_FN_multi_timer_cb,

  CURL_CBAPI_FN_آخری
} Curl_cbapi_fn;

/* EAPI */

#تعریف_کرو CURL_EAPI_MAX_RECURSION       7

بنية Curl_eapi_ڈھیر {
  uint16_t گنتی;
  uint16_t calls[CURL_EAPI_MAX_RECURSION];
};

بنية Curl_eapi_guard {
  بنية Curl_easy *انکڑے;  /* != NULL if handle stays */
  uint16_t ڈونگھائی;  /* > 0 if this guard was entered */
};

bool Curl_eapi_enter(بنية Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult);
فراغ Curl_eapi_leave(بنية Curl_eapi_guard *guard);

/* Convert an EAPI failure to a header API result */
CURLHcode Curl_eapi_hcode(CURLcode نتیجہ);

/* Curl_eapi_enter() checks for curl being NULL, but windows compiler
 * analyzers do not realize this. *sigh* */
#تعریف_کرو CURL_EAPI_ENTER(g, curl, fn, r) \
  Curl_eapi_enter((g), (curl), CURL_EAPI_FN_##fn, (r)) && (curl)
#تعریف_کرو CURL_EAPI_LEAVE(g) \
  Curl_eapi_leave(g)

/* MAPI */

#تعریف_کرو CURL_MAPI_MAX_RECURSION       15

بنية Curl_mapi_ڈھیر {
  uint8_t گنتی;
  uint8_t calls[CURL_MAPI_MAX_RECURSION];
};

بنية Curl_mapi_guard {
  بنية Curl_multi *multi;  /* != NULL if handle stays */
  uint8_t ڈونگھائی;  /* > 0 if this guard was entered */
};

bool Curl_mapi_enter(بنية Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult);
فراغ Curl_mapi_leave(بنية Curl_mapi_guard *guard);

/* Curl_mapi_enter() checks for m being NULL, but windows compiler
 * analyzers do not realize this. *sigh* */
#تعریف_کرو CURL_MAPI_ENTER(g, m, fn, r) \
  Curl_mapi_enter((g), (m), CURL_MAPI_FN_##fn, (r)) && (m)
#تعریف_کرو CURL_MAPI_LEAVE(g) \
  Curl_mapi_leave(g)

فراغ Curl_cbapi_enter(بنية Curl_mapi_guard *guard,
                      بنية Curl_easy *انکڑے,
                      بنية Curl_multi *multi,
                      Curl_cbapi_fn fn);
فراغ Curl_cbapi_leave(بنية Curl_mapi_guard *guard);

#تعریف_کرو CURL_CBAPI_شروع(g, d, fn) \
  Curl_cbapi_enter((g), (d), عدم, CURL_CBAPI_FN_##fn)
#تعریف_کرو CURL_CBAPI_MULTI_شروع(g, m, fn) \
  Curl_cbapi_enter((g), عدم, (m), CURL_CBAPI_FN_##fn)
#تعریف_کرو CURL_CBAPI_سرا(g) \
  Curl_cbapi_leave(g)
#تعریف_کرو CURL_CBAPI_MULTI_سرا(g) \
  Curl_cbapi_leave(g)

bool Curl_api_is_وچ_callback(بنية Curl_easy *انکڑے);
bool Curl_api_multi_is_وچ_callback(بنية Curl_multi *multi);

#جے_انت /* HEADER_CURL_API_H */
