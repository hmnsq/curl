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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

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
ಸಂರಚನಾ curl_ngh2 {
  nghttp2_session *s;
  ಪೂರ್ಣಾಙ್ಕ fatal_rv; /* != 0 once nghttp2 session has entered FATAL state */
};

/* True if nghttp2 session exists and is not in fatal state */
bool Curl_ngh2_valid(ಸಂರಚನಾ curl_ngh2 *ngh2);
/* True if `rv` is fatal or nghttp2 session already is in fatal state */
bool Curl_ngh2_is_fatal(ಸಂರಚನಾ curl_ngh2 *ngh2, ಪೂರ್ಣಾಙ್ಕ rv);
/* != 0 and the fatal error if nghttp2 is in fatal state */
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_fatal(ಸಂರಚನಾ curl_ngh2 *ngh2);

ಶೂನ್ಯ Curl_ngh2_del(ಸಂರಚನಾ curl_ngh2 *ngh2);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_settings(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags,
  ಧ್ರುವ nghttp2_settings_entry *iv, size_t niv);
ಶೂನ್ಯ *Curl_ngh2_session_get_stream_user_data(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_set_stream_user_data(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id, ಶೂನ್ಯ *stream_user_data);
uint32_t Curl_ngh2_session_get_remote_settings(
  ಸಂರಚನಾ curl_ngh2 *ngh2, nghttp2_settings_id id);
int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id);
int32_t Curl_ngh2_session_get_local_window_size(
  ಸಂರಚನಾ curl_ngh2 *ngh2);
int32_t Curl_ngh2_session_get_stream_remote_window_size(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id);
int32_t Curl_ngh2_session_get_remote_window_size(
  ಸಂರಚನಾ curl_ngh2 *ngh2);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_set_local_window_size(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_window_update(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size_increment);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_rst_stream(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_send(ಸಂರಚನಾ curl_ngh2 *ngh2);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_client_new3(
  ಸಂರಚನಾ curl_ngh2 *ngh2, ಧ್ರುವ nghttp2_session_callbacks *callbacks,
  ಶೂನ್ಯ *user_data, ಧ್ರುವ nghttp2_option *option, nghttp2_mem *mem);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_want_read(ಸಂರಚನಾ curl_ngh2 *ngh2);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_want_write(ಸಂರಚನಾ curl_ngh2 *ngh2);
ssize_t Curl_ngh2_session_mem_recv(
  ಸಂರಚನಾ curl_ngh2 *ngh2, ಧ್ರುವ uint8_t *in, size_t inlen);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_check_request_allowed(ಸಂರಚನಾ curl_ngh2 *ngh2);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_ping(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags, ಧ್ರುವ uint8_t *opaque_data);
int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_resume_data(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_consume(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id, size_t size);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_priority(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, ಧ್ರುವ nghttp2_priority_spec *pri_spec);
int32_t Curl_ngh2_submit_request(
  ಸಂರಚನಾ curl_ngh2 *ngh2, ಧ್ರುವ nghttp2_priority_spec *pri_spec,
  ಧ್ರುವ nghttp2_nv *nva, size_t nvlen, ಧ್ರುವ nghttp2_data_provider *data_prd,
  ಶೂನ್ಯ *stream_user_data);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_upgrade2(
  ಸಂರಚನಾ curl_ngh2 *ngh2, ಧ್ರುವ uint8_t *settings_payload,
  size_t settings_payloadlen, ಪೂರ್ಣಾಙ್ಕ head_request, ಶೂನ್ಯ *stream_user_data);
ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_goaway(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, ಧ್ರುವ uint8_t *opaque_data, size_t opaque_data_len);


#endif /* USE_NGHTTP2 */

#endif /* HEADER_CURL_NGH2_SHIM_H */
