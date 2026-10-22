#ifndef HEADER_CURL_NGH2_SHIM_H
#define HEADER_CURL_NGH2_SHIM_H
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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_NGHTTP2

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
সংরচনা curl_ngh2 {
  nghttp2_session *s;
  পূর্ণাঙ্ক fatal_rv; /* != 0 once nghttp2 session has entered FATAL state */
};

/* True if nghttp2 session exists and is not in fatal state */
bool Curl_ngh2_valid(সংরচনা curl_ngh2 *ngh2);
/* True if `rv` is fatal or nghttp2 session already is in fatal state */
bool Curl_ngh2_is_fatal(সংরচনা curl_ngh2 *ngh2, পূর্ণাঙ্ক rv);
/* != 0 and the fatal error if nghttp2 is in fatal state */
পূর্ণাঙ্ক Curl_ngh2_fatal(সংরচনা curl_ngh2 *ngh2);

শূন্য Curl_ngh2_del(সংরচনা curl_ngh2 *ngh2);
পূর্ণাঙ্ক Curl_ngh2_submit_settings(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags,
  ধ্রুব nghttp2_settings_entry *iv, size_t niv);
শূন্য *Curl_ngh2_session_get_stream_user_data(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id);
পূর্ণাঙ্ক Curl_ngh2_session_set_stream_user_data(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id, শূন্য *stream_user_data);
uint32_t Curl_ngh2_session_get_remote_settings(
  সংরচনা curl_ngh2 *ngh2, nghttp2_settings_id id);
int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id);
int32_t Curl_ngh2_session_get_local_window_size(
  সংরচনা curl_ngh2 *ngh2);
int32_t Curl_ngh2_session_get_stream_remote_window_size(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id);
int32_t Curl_ngh2_session_get_remote_window_size(
  সংরচনা curl_ngh2 *ngh2);
পূর্ণাঙ্ক Curl_ngh2_session_set_local_window_size(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size);
পূর্ণাঙ্ক Curl_ngh2_submit_window_update(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size_increment);
পূর্ণাঙ্ক Curl_ngh2_submit_rst_stream(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code);
পূর্ণাঙ্ক Curl_ngh2_session_send(সংরচনা curl_ngh2 *ngh2);
পূর্ণাঙ্ক Curl_ngh2_session_client_new3(
  সংরচনা curl_ngh2 *ngh2, ধ্রুব nghttp2_session_callbacks *callbacks,
  শূন্য *user_data, ধ্রুব nghttp2_option *option, nghttp2_mem *mem);
পূর্ণাঙ্ক Curl_ngh2_session_want_read(সংরচনা curl_ngh2 *ngh2);
পূর্ণাঙ্ক Curl_ngh2_session_want_write(সংরচনা curl_ngh2 *ngh2);
ssize_t Curl_ngh2_session_mem_recv(
  সংরচনা curl_ngh2 *ngh2, ধ্রুব uint8_t *in, size_t inlen);
পূর্ণাঙ্ক Curl_ngh2_session_check_request_allowed(সংরচনা curl_ngh2 *ngh2);
পূর্ণাঙ্ক Curl_ngh2_submit_ping(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags, ধ্রুব uint8_t *opaque_data);
int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id);
পূর্ণাঙ্ক Curl_ngh2_session_resume_data(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id);
পূর্ণাঙ্ক Curl_ngh2_session_consume(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id, size_t size);
পূর্ণাঙ্ক Curl_ngh2_submit_priority(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, ধ্রুব nghttp2_priority_spec *pri_spec);
int32_t Curl_ngh2_submit_request(
  সংরচনা curl_ngh2 *ngh2, ধ্রুব nghttp2_priority_spec *pri_spec,
  ধ্রুব nghttp2_nv *nva, size_t nvlen, ধ্রুব nghttp2_data_provider *data_prd,
  শূন্য *stream_user_data);
পূর্ণাঙ্ক Curl_ngh2_session_upgrade2(
  সংরচনা curl_ngh2 *ngh2, ধ্রুব uint8_t *settings_payload,
  size_t settings_payloadlen, পূর্ণাঙ্ক head_request, শূন্য *stream_user_data);
পূর্ণাঙ্ক Curl_ngh2_submit_goaway(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, ধ্রুব uint8_t *opaque_data, size_t opaque_data_len);


#endif /* USE_NGHTTP2 */

#endif /* HEADER_CURL_NGH2_SHIM_H */
