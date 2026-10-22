#جے_بنا_تعریف سرخی_CURL_NGH2_SHIM_H
#تعریف_کرو سرخی_CURL_NGH2_SHIM_H
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

#جے_تعریف_شدہ USE_NGHTTP2

/* From the nghttp2 documentation:
 * > The errors < :enum:`nghttp2_error.NGHTTP2_ERR_FATAL` mean that
 * > the library is under unexpected condition and processing was
 * > terminated (e.g., out of memory).  If application receives this
 * > error code, it must stop using that :type:`nghttp2_session`
 * > object and only allowed operation for that object is deallocate
 * > it using `nghttp2_session_del()`.
 *
 * This shim protects further calls into a nghttp2 session once
 * a fatal error occurred.
 */
بنية curl_ngh2 {
  nghttp2_session *s;
  صحيح fatal_rv; /* != 0 once nghttp2 session has entered FATAL state */
};

/* True if nghttp2 session exists and is not in fatal state */
bool Curl_ngh2_valid(بنية curl_ngh2 *ngh2);
/* True if `rv` is fatal or nghttp2 session already is in fatal state */
bool Curl_ngh2_is_fatal(بنية curl_ngh2 *ngh2, صحيح rv);
/* != 0 and the fatal error if nghttp2 is in fatal state */
صحيح Curl_ngh2_fatal(بنية curl_ngh2 *ngh2);

فراغ Curl_ngh2_del(بنية curl_ngh2 *ngh2);
صحيح Curl_ngh2_submit_settings(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  ثابت nghttp2_settings_اندراج *iv, size_t niv);
فراغ *Curl_ngh2_session_get_دھارا_user_انکڑے(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id);
صحيح Curl_ngh2_session_سیٹ_دھارا_user_انکڑے(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id, فراغ *دھارا_user_انکڑے);
uint32_t Curl_ngh2_session_get_remote_settings(
  بنية curl_ngh2 *ngh2, nghttp2_settings_id id);
int32_t Curl_ngh2_session_get_دھارا_effective_local_کھڑکی_ناپ(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id);
int32_t Curl_ngh2_session_get_local_کھڑکی_ناپ(
  بنية curl_ngh2 *ngh2);
int32_t Curl_ngh2_session_get_دھارا_remote_کھڑکی_ناپ(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id);
int32_t Curl_ngh2_session_get_remote_کھڑکی_ناپ(
  بنية curl_ngh2 *ngh2);
صحيح Curl_ngh2_session_سیٹ_local_کھڑکی_ناپ(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, int32_t کھڑکی_ناپ);
صحيح Curl_ngh2_submit_کھڑکی_update(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, int32_t کھڑکی_ناپ_increment);
صحيح Curl_ngh2_submit_rst_دھارا(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, uint32_t error_کوڈ);
صحيح Curl_ngh2_session_send(بنية curl_ngh2 *ngh2);
صحيح Curl_ngh2_session_client_new3(
  بنية curl_ngh2 *ngh2, ثابت nghttp2_session_callbacks *callbacks,
  فراغ *user_انکڑے, ثابت nghttp2_option *option, nghttp2_mem *mem);
صحيح Curl_ngh2_session_want_read(بنية curl_ngh2 *ngh2);
صحيح Curl_ngh2_session_want_write(بنية curl_ngh2 *ngh2);
ssize_t Curl_ngh2_session_mem_recv(
  بنية curl_ngh2 *ngh2, ثابت uint8_t *وچ, size_t inlen);
صحيح Curl_ngh2_session_check_request_allowed(بنية curl_ngh2 *ngh2);
صحيح Curl_ngh2_submit_ping(
  بنية curl_ngh2 *ngh2, uint8_t flags, ثابت uint8_t *opaque_انکڑے);
int32_t Curl_ngh2_session_get_دھارا_effective_recv_انکڑے_لمائی(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id);
صحيح Curl_ngh2_session_resume_انکڑے(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id);
صحيح Curl_ngh2_session_consume(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id, size_t ناپ);
صحيح Curl_ngh2_submit_ترجیح(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, ثابت nghttp2_ترجیح_spec *pri_spec);
int32_t Curl_ngh2_submit_request(
  بنية curl_ngh2 *ngh2, ثابت nghttp2_ترجیح_spec *pri_spec,
  ثابت nghttp2_nv *nva, size_t nvlen, ثابت nghttp2_انکڑے_provider *انکڑے_prd,
  فراغ *دھارا_user_انکڑے);
صحيح Curl_ngh2_session_upgrade2(
  بنية curl_ngh2 *ngh2, ثابت uint8_t *settings_payload,
  size_t settings_payloadlen, صحيح سر_request, فراغ *دھارا_user_انکڑے);
صحيح Curl_ngh2_submit_goaway(
  بنية curl_ngh2 *ngh2, uint8_t flags, int32_t آخری_دھارا_id,
  uint32_t error_کوڈ, ثابت uint8_t *opaque_انکڑے, size_t opaque_انکڑے_len);


#جے_انت /* USE_NGHTTP2 */

#جے_انت /* HEADER_CURL_NGH2_SHIM_H */
