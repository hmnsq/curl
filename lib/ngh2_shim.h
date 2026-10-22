#अगर_अघोषित HEADER_CURL_NGH2_SHIM_H
#घोषणा HEADER_CURL_NGH2_SHIM_H
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
#समावेश "curl_setup.h"

#अगर_घोषित USE_NGHTTP2

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
काष्ठा curl_ngh2 {
  nghttp2_session *s;
  पूर्णांक fatal_rv; /* != 0 once nghttp2 session has entered FATAL state */
};

/* True if nghttp2 session exists and is not in fatal state */
bool Curl_ngh2_valid(काष्ठा curl_ngh2 *ngh2);
/* True if `rv` is fatal or nghttp2 session already is in fatal state */
bool Curl_ngh2_is_fatal(काष्ठा curl_ngh2 *ngh2, पूर्णांक rv);
/* != 0 and the fatal error if nghttp2 is in fatal state */
पूर्णांक Curl_ngh2_fatal(काष्ठा curl_ngh2 *ngh2);

व्योम Curl_ngh2_del(काष्ठा curl_ngh2 *ngh2);
पूर्णांक Curl_ngh2_submit_settings(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags,
  स्थिर nghttp2_settings_entry *iv, माप_प्रकार niv);
व्योम *Curl_ngh2_session_get_stream_user_तथ्य(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id);
पूर्णांक Curl_ngh2_session_set_stream_user_तथ्य(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id, व्योम *stream_user_तथ्य);
uint32_t Curl_ngh2_session_get_remote_settings(
  काष्ठा curl_ngh2 *ngh2, nghttp2_settings_id id);
int32_t Curl_ngh2_session_get_stream_effective_local_window_आकार(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id);
int32_t Curl_ngh2_session_get_local_window_आकार(
  काष्ठा curl_ngh2 *ngh2);
int32_t Curl_ngh2_session_get_stream_remote_window_आकार(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id);
int32_t Curl_ngh2_session_get_remote_window_आकार(
  काष्ठा curl_ngh2 *ngh2);
पूर्णांक Curl_ngh2_session_set_local_window_आकार(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_आकार);
पूर्णांक Curl_ngh2_submit_window_update(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_आकार_increment);
पूर्णांक Curl_ngh2_submit_rst_stream(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code);
पूर्णांक Curl_ngh2_session_send(काष्ठा curl_ngh2 *ngh2);
पूर्णांक Curl_ngh2_session_client_new3(
  काष्ठा curl_ngh2 *ngh2, स्थिर nghttp2_session_callbacks *callbacks,
  व्योम *user_तथ्य, स्थिर nghttp2_option *option, nghttp2_mem *mem);
पूर्णांक Curl_ngh2_session_want_read(काष्ठा curl_ngh2 *ngh2);
पूर्णांक Curl_ngh2_session_want_write(काष्ठा curl_ngh2 *ngh2);
ssize_t Curl_ngh2_session_mem_recv(
  काष्ठा curl_ngh2 *ngh2, स्थिर uint8_t *in, माप_प्रकार inlen);
पूर्णांक Curl_ngh2_session_check_request_allowed(काष्ठा curl_ngh2 *ngh2);
पूर्णांक Curl_ngh2_submit_ping(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags, स्थिर uint8_t *opaque_तथ्य);
int32_t Curl_ngh2_session_get_stream_effective_recv_तथ्य_लंबाई(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id);
पूर्णांक Curl_ngh2_session_resume_तथ्य(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id);
पूर्णांक Curl_ngh2_session_consume(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id, माप_प्रकार आकार);
पूर्णांक Curl_ngh2_submit_priority(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, स्थिर nghttp2_priority_spec *pri_spec);
int32_t Curl_ngh2_submit_request(
  काष्ठा curl_ngh2 *ngh2, स्थिर nghttp2_priority_spec *pri_spec,
  स्थिर nghttp2_nv *nva, माप_प्रकार nvlen, स्थिर nghttp2_तथ्य_provider *तथ्य_prd,
  व्योम *stream_user_तथ्य);
पूर्णांक Curl_ngh2_session_upgrade2(
  काष्ठा curl_ngh2 *ngh2, स्थिर uint8_t *settings_payload,
  माप_प्रकार settings_payloadlen, पूर्णांक शीर्ष_request, व्योम *stream_user_तथ्य);
पूर्णांक Curl_ngh2_submit_goaway(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, स्थिर uint8_t *opaque_तथ्य, माप_प्रकार opaque_तथ्य_len);


#पूर्ण_अगर /* USE_NGHTTP2 */

#पूर्ण_अगर /* HEADER_CURL_NGH2_SHIM_H */
