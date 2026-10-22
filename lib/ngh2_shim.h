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
কাঠামো curl_ngh2 {
  nghttp2_session *s;
  পূর্ণসংখ্যা fatal_rv; /* != 0 once nghttp2 session has entered FATAL state */
};

/* True if nghttp2 session exists and is not in fatal state */
bool Curl_ngh2_valid(কাঠামো curl_ngh2 *ngh2);
/* True if `rv` is fatal or nghttp2 session already is in fatal state */
bool Curl_ngh2_is_fatal(কাঠামো curl_ngh2 *ngh2, পূর্ণসংখ্যা rv);
/* != 0 and the fatal error if nghttp2 is in fatal state */
পূর্ণসংখ্যা Curl_ngh2_fatal(কাঠামো curl_ngh2 *ngh2);

শূন্য Curl_ngh2_del(কাঠামো curl_ngh2 *ngh2);
পূর্ণসংখ্যা Curl_ngh2_submit_settings(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags,
  ধ্রুবক nghttp2_settings_entry *iv, size_t niv);
শূন্য *Curl_ngh2_session_get_stream_user_তথ্য(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id);
পূর্ণসংখ্যা Curl_ngh2_session_set_stream_user_তথ্য(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id, শূন্য *stream_user_তথ্য);
uint32_t Curl_ngh2_session_get_remote_settings(
  কাঠামো curl_ngh2 *ngh2, nghttp2_settings_id id);
int32_t Curl_ngh2_session_get_stream_effective_local_window_আকার(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id);
int32_t Curl_ngh2_session_get_local_window_আকার(
  কাঠামো curl_ngh2 *ngh2);
int32_t Curl_ngh2_session_get_stream_remote_window_আকার(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id);
int32_t Curl_ngh2_session_get_remote_window_আকার(
  কাঠামো curl_ngh2 *ngh2);
পূর্ণসংখ্যা Curl_ngh2_session_set_local_window_আকার(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_আকার);
পূর্ণসংখ্যা Curl_ngh2_submit_window_update(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_আকার_increment);
পূর্ণসংখ্যা Curl_ngh2_submit_rst_stream(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code);
পূর্ণসংখ্যা Curl_ngh2_session_send(কাঠামো curl_ngh2 *ngh2);
পূর্ণসংখ্যা Curl_ngh2_session_client_new3(
  কাঠামো curl_ngh2 *ngh2, ধ্রুবক nghttp2_session_callbacks *callbacks,
  শূন্য *user_তথ্য, ধ্রুবক nghttp2_option *option, nghttp2_mem *mem);
পূর্ণসংখ্যা Curl_ngh2_session_want_read(কাঠামো curl_ngh2 *ngh2);
পূর্ণসংখ্যা Curl_ngh2_session_want_write(কাঠামো curl_ngh2 *ngh2);
ssize_t Curl_ngh2_session_mem_recv(
  কাঠামো curl_ngh2 *ngh2, ধ্রুবক uint8_t *in, size_t inlen);
পূর্ণসংখ্যা Curl_ngh2_session_check_request_allowed(কাঠামো curl_ngh2 *ngh2);
পূর্ণসংখ্যা Curl_ngh2_submit_ping(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags, ধ্রুবক uint8_t *opaque_তথ্য);
int32_t Curl_ngh2_session_get_stream_effective_recv_তথ্য_দৈর্ঘ্য(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id);
পূর্ণসংখ্যা Curl_ngh2_session_resume_তথ্য(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id);
পূর্ণসংখ্যা Curl_ngh2_session_consume(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id, size_t আকার_2);
পূর্ণসংখ্যা Curl_ngh2_submit_priority(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, ধ্রুবক nghttp2_priority_spec *pri_spec);
int32_t Curl_ngh2_submit_request(
  কাঠামো curl_ngh2 *ngh2, ধ্রুবক nghttp2_priority_spec *pri_spec,
  ধ্রুবক nghttp2_nv *nva, size_t nvlen, ধ্রুবক nghttp2_তথ্য_provider *তথ্য_prd,
  শূন্য *stream_user_তথ্য);
পূর্ণসংখ্যা Curl_ngh2_session_upgrade2(
  কাঠামো curl_ngh2 *ngh2, ধ্রুবক uint8_t *settings_payload,
  size_t settings_payloadlen, পূর্ণসংখ্যা শীর্ষ_request, শূন্য *stream_user_তথ্য);
পূর্ণসংখ্যা Curl_ngh2_submit_goaway(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, ধ্রুবক uint8_t *opaque_তথ্য, size_t opaque_তথ্য_len);


#endif /* USE_NGHTTP2 */

#endif /* HEADER_CURL_NGH2_SHIM_H */
