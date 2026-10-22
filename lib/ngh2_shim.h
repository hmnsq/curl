#اگر_غیر_تعریف_شدہ سرخی_CURL_NGH2_SHIM_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_NGHTTP2

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
ساخت curl_ngh2 {
  nghttp2_session *s;
  صحیح fatal_rv; /* != 0 once nghttp2 session has entered FATAL state */
};

/* True if nghttp2 session exists and is not in fatal state */
bool Curl_ngh2_valid(ساخت curl_ngh2 *ngh2);
/* True if `rv` is fatal or nghttp2 session already is in fatal state */
bool Curl_ngh2_is_fatal(ساخت curl_ngh2 *ngh2, صحیح rv);
/* != 0 and the fatal error if nghttp2 is in fatal state */
صحیح Curl_ngh2_fatal(ساخت curl_ngh2 *ngh2);

خالی Curl_ngh2_del(ساخت curl_ngh2 *ngh2);
صحیح Curl_ngh2_submit_settings(
  ساخت curl_ngh2 *ngh2, uint8_t flags,
  ثابت nghttp2_settings_اندراج *iv, size_t niv);
خالی *Curl_ngh2_session_get_دھارا_user_ڈیٹا(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id);
صحیح Curl_ngh2_session_سیٹ_دھارا_user_ڈیٹا(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id, خالی *دھارا_user_ڈیٹا);
uint32_t Curl_ngh2_session_get_remote_settings(
  ساخت curl_ngh2 *ngh2, nghttp2_settings_id id);
int32_t Curl_ngh2_session_get_دھارا_effective_local_کھڑکی_سائز(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id);
int32_t Curl_ngh2_session_get_local_کھڑکی_سائز(
  ساخت curl_ngh2 *ngh2);
int32_t Curl_ngh2_session_get_دھارا_remote_کھڑکی_سائز(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id);
int32_t Curl_ngh2_session_get_remote_کھڑکی_سائز(
  ساخت curl_ngh2 *ngh2);
صحیح Curl_ngh2_session_سیٹ_local_کھڑکی_سائز(
  ساخت curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, int32_t کھڑکی_سائز);
صحیح Curl_ngh2_submit_کھڑکی_update(
  ساخت curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, int32_t کھڑکی_سائز_increment);
صحیح Curl_ngh2_submit_rst_دھارا(
  ساخت curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, uint32_t error_رمز);
صحیح Curl_ngh2_session_send(ساخت curl_ngh2 *ngh2);
صحیح Curl_ngh2_session_client_new3(
  ساخت curl_ngh2 *ngh2, ثابت nghttp2_session_callbacks *callbacks,
  خالی *user_ڈیٹا, ثابت nghttp2_option *option, nghttp2_mem *mem);
صحیح Curl_ngh2_session_want_read(ساخت curl_ngh2 *ngh2);
صحیح Curl_ngh2_session_want_write(ساخت curl_ngh2 *ngh2);
ssize_t Curl_ngh2_session_mem_recv(
  ساخت curl_ngh2 *ngh2, ثابت uint8_t *اندر, size_t inlen);
صحیح Curl_ngh2_session_check_request_allowed(ساخت curl_ngh2 *ngh2);
صحیح Curl_ngh2_submit_ping(
  ساخت curl_ngh2 *ngh2, uint8_t flags, ثابت uint8_t *opaque_ڈیٹا);
int32_t Curl_ngh2_session_get_دھارا_effective_recv_ڈیٹا_لمبائی(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id);
صحیح Curl_ngh2_session_resume_ڈیٹا(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id);
صحیح Curl_ngh2_session_consume(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id, size_t سائز);
صحیح Curl_ngh2_submit_ترجیح(
  ساخت curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, ثابت nghttp2_ترجیح_spec *pri_spec);
int32_t Curl_ngh2_submit_request(
  ساخت curl_ngh2 *ngh2, ثابت nghttp2_ترجیح_spec *pri_spec,
  ثابت nghttp2_nv *nva, size_t nvlen, ثابت nghttp2_ڈیٹا_provider *ڈیٹا_prd,
  خالی *دھارا_user_ڈیٹا);
صحیح Curl_ngh2_session_upgrade2(
  ساخت curl_ngh2 *ngh2, ثابت uint8_t *settings_payload,
  size_t settings_payloadlen, صحیح سر_request, خالی *دھارا_user_ڈیٹا);
صحیح Curl_ngh2_submit_goaway(
  ساخت curl_ngh2 *ngh2, uint8_t flags, int32_t آخری_دھارا_id,
  uint32_t error_رمز, ثابت uint8_t *opaque_ڈیٹا, size_t opaque_ڈیٹا_len);


#اگر_ختم /* USE_NGHTTP2 */

#اگر_ختم /* HEADER_CURL_NGH2_SHIM_H */
