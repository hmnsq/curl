#यदि_अपरिभाषितम् शीर्षकम्_CURL_NGH2_SHIM_H
#परिभाषय शीर्षकम्_CURL_NGH2_SHIM_H
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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_NGHTTP2

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
संरचना curl_ngh2 {
  nghttp2_session *s;
  पूर्णाङ्क fatal_rv; /* != 0 once nghttp2 session has entered FATAL state */
};

/* True if nghttp2 session exists and is not in fatal state */
bool Curl_ngh2_valid(संरचना curl_ngh2 *ngh2);
/* True if `rv` is fatal or nghttp2 session already is in fatal state */
bool Curl_ngh2_is_fatal(संरचना curl_ngh2 *ngh2, पूर्णाङ्क rv);
/* != 0 and the fatal error if nghttp2 is in fatal state */
पूर्णाङ्क Curl_ngh2_fatal(संरचना curl_ngh2 *ngh2);

शून्य Curl_ngh2_del(संरचना curl_ngh2 *ngh2);
पूर्णाङ्क Curl_ngh2_submit_settings(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  ध्रुव nghttp2_settings_प्रविष्टिः *iv, size_t niv);
शून्य *Curl_ngh2_session_get_प्रवाहः_user_दत्त(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id);
पूर्णाङ्क Curl_ngh2_session_समुच्चयः_प्रवाहः_user_दत्त(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id, शून्य *प्रवाहः_user_दत्त);
uint32_t Curl_ngh2_session_get_remote_settings(
  संरचना curl_ngh2 *ngh2, nghttp2_settings_id id);
int32_t Curl_ngh2_session_get_प्रवाहः_effective_local_गवाक्षः_आकार(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id);
int32_t Curl_ngh2_session_get_local_गवाक्षः_आकार(
  संरचना curl_ngh2 *ngh2);
int32_t Curl_ngh2_session_get_प्रवाहः_remote_गवाक्षः_आकार(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id);
int32_t Curl_ngh2_session_get_remote_गवाक्षः_आकार(
  संरचना curl_ngh2 *ngh2);
पूर्णाङ्क Curl_ngh2_session_समुच्चयः_local_गवाक्षः_आकार(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t प्रवाहः_id, int32_t गवाक्षः_आकार);
पूर्णाङ्क Curl_ngh2_submit_गवाक्षः_update(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t प्रवाहः_id, int32_t गवाक्षः_आकार_increment);
पूर्णाङ्क Curl_ngh2_submit_rst_प्रवाहः(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t प्रवाहः_id, uint32_t error_संहिता);
पूर्णाङ्क Curl_ngh2_session_send(संरचना curl_ngh2 *ngh2);
पूर्णाङ्क Curl_ngh2_session_client_new3(
  संरचना curl_ngh2 *ngh2, ध्रुव nghttp2_session_callbacks *callbacks,
  शून्य *user_दत्त, ध्रुव nghttp2_option *option, nghttp2_mem *mem);
पूर्णाङ्क Curl_ngh2_session_want_read(संरचना curl_ngh2 *ngh2);
पूर्णाङ्क Curl_ngh2_session_want_write(संरचना curl_ngh2 *ngh2);
ssize_t Curl_ngh2_session_mem_recv(
  संरचना curl_ngh2 *ngh2, ध्रुव uint8_t *अन्तर्, size_t inlen);
पूर्णाङ्क Curl_ngh2_session_check_request_allowed(संरचना curl_ngh2 *ngh2);
पूर्णाङ्क Curl_ngh2_submit_ping(
  संरचना curl_ngh2 *ngh2, uint8_t flags, ध्रुव uint8_t *opaque_दत्त);
int32_t Curl_ngh2_session_get_प्रवाहः_effective_recv_दत्त_दीर्घता(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id);
पूर्णाङ्क Curl_ngh2_session_resume_दत्त(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id);
पूर्णाङ्क Curl_ngh2_session_consume(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id, size_t आकार);
पूर्णाङ्क Curl_ngh2_submit_प्राथमिकता(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t प्रवाहः_id, ध्रुव nghttp2_प्राथमिकता_spec *pri_spec);
int32_t Curl_ngh2_submit_request(
  संरचना curl_ngh2 *ngh2, ध्रुव nghttp2_प्राथमिकता_spec *pri_spec,
  ध्रुव nghttp2_nv *nva, size_t nvlen, ध्रुव nghttp2_दत्त_provider *दत्त_prd,
  शून्य *प्रवाहः_user_दत्त);
पूर्णाङ्क Curl_ngh2_session_upgrade2(
  संरचना curl_ngh2 *ngh2, ध्रुव uint8_t *settings_payload,
  size_t settings_payloadlen, पूर्णाङ्क शीर्ष_request, शून्य *प्रवाहः_user_दत्त);
पूर्णाङ्क Curl_ngh2_submit_goaway(
  संरचना curl_ngh2 *ngh2, uint8_t flags, int32_t चरमः_प्रवाहः_id,
  uint32_t error_संहिता, ध्रुव uint8_t *opaque_दत्त, size_t opaque_दत्त_len);


#यदि_अन्तः /* USE_NGHTTP2 */

#यदि_अन्तः /* HEADER_CURL_NGH2_SHIM_H */
