#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_NGH2_SHIM_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_NGH2_SHIM_H
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2

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
ਬਣਤਰ curl_ngh2 {
  nghttp2_session *s;
  ਪੂਰਨ fatal_rv; /* != 0 once nghttp2 session has entered FATAL state */
};

/* True if nghttp2 session exists and is not in fatal state */
bool Curl_ngh2_valid(ਬਣਤਰ curl_ngh2 *ngh2);
/* True if `rv` is fatal or nghttp2 session already is in fatal state */
bool Curl_ngh2_is_fatal(ਬਣਤਰ curl_ngh2 *ngh2, ਪੂਰਨ rv);
/* != 0 and the fatal error if nghttp2 is in fatal state */
ਪੂਰਨ Curl_ngh2_fatal(ਬਣਤਰ curl_ngh2 *ngh2);

ਖਾਲੀ Curl_ngh2_del(ਬਣਤਰ curl_ngh2 *ngh2);
ਪੂਰਨ Curl_ngh2_submit_settings(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags,
  ਅਚਲ nghttp2_settings_ਇੰਦਰਾਜ *iv, size_t niv);
ਖਾਲੀ *Curl_ngh2_session_get_ਧਾਰਾ_user_ਅੰਕੜੇ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id);
ਪੂਰਨ Curl_ngh2_session_ਸੈੱਟ_ਧਾਰਾ_user_ਅੰਕੜੇ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ);
uint32_t Curl_ngh2_session_get_remote_settings(
  ਬਣਤਰ curl_ngh2 *ngh2, nghttp2_settings_id id);
int32_t Curl_ngh2_session_get_ਧਾਰਾ_effective_local_ਖਿੜਕੀ_ਮਾਪ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id);
int32_t Curl_ngh2_session_get_local_ਖਿੜਕੀ_ਮਾਪ(
  ਬਣਤਰ curl_ngh2 *ngh2);
int32_t Curl_ngh2_session_get_ਧਾਰਾ_remote_ਖਿੜਕੀ_ਮਾਪ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id);
int32_t Curl_ngh2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(
  ਬਣਤਰ curl_ngh2 *ngh2);
ਪੂਰਨ Curl_ngh2_session_ਸੈੱਟ_local_ਖਿੜਕੀ_ਮਾਪ(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ਧਾਰਾ_id, int32_t ਖਿੜਕੀ_ਮਾਪ);
ਪੂਰਨ Curl_ngh2_submit_ਖਿੜਕੀ_update(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ਧਾਰਾ_id, int32_t ਖਿੜਕੀ_ਮਾਪ_increment);
ਪੂਰਨ Curl_ngh2_submit_rst_ਧਾਰਾ(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ਧਾਰਾ_id, uint32_t error_ਕੋਡ);
ਪੂਰਨ Curl_ngh2_session_send(ਬਣਤਰ curl_ngh2 *ngh2);
ਪੂਰਨ Curl_ngh2_session_client_new3(
  ਬਣਤਰ curl_ngh2 *ngh2, ਅਚਲ nghttp2_session_callbacks *callbacks,
  ਖਾਲੀ *user_ਅੰਕੜੇ, ਅਚਲ nghttp2_option *option, nghttp2_mem *mem);
ਪੂਰਨ Curl_ngh2_session_want_read(ਬਣਤਰ curl_ngh2 *ngh2);
ਪੂਰਨ Curl_ngh2_session_want_write(ਬਣਤਰ curl_ngh2 *ngh2);
ssize_t Curl_ngh2_session_mem_recv(
  ਬਣਤਰ curl_ngh2 *ngh2, ਅਚਲ uint8_t *ਵਿੱਚ, size_t inlen);
ਪੂਰਨ Curl_ngh2_session_check_request_allowed(ਬਣਤਰ curl_ngh2 *ngh2);
ਪੂਰਨ Curl_ngh2_submit_ping(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags, ਅਚਲ uint8_t *opaque_ਅੰਕੜੇ);
int32_t Curl_ngh2_session_get_ਧਾਰਾ_effective_recv_ਅੰਕੜੇ_ਲੰਮਾਈ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id);
ਪੂਰਨ Curl_ngh2_session_resume_ਅੰਕੜੇ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id);
ਪੂਰਨ Curl_ngh2_session_consume(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id, size_t ਮਾਪ);
ਪੂਰਨ Curl_ngh2_submit_ਤਰਜੀਹ(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ਧਾਰਾ_id, ਅਚਲ nghttp2_ਤਰਜੀਹ_spec *pri_spec);
int32_t Curl_ngh2_submit_request(
  ਬਣਤਰ curl_ngh2 *ngh2, ਅਚਲ nghttp2_ਤਰਜੀਹ_spec *pri_spec,
  ਅਚਲ nghttp2_nv *nva, size_t nvlen, ਅਚਲ nghttp2_ਅੰਕੜੇ_provider *ਅੰਕੜੇ_prd,
  ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ);
ਪੂਰਨ Curl_ngh2_session_upgrade2(
  ਬਣਤਰ curl_ngh2 *ngh2, ਅਚਲ uint8_t *settings_payload,
  size_t settings_payloadlen, ਪੂਰਨ ਸਿਰ_request, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ);
ਪੂਰਨ Curl_ngh2_submit_goaway(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags, int32_t ਆਖਰੀ_ਧਾਰਾ_id,
  uint32_t error_ਕੋਡ, ਅਚਲ uint8_t *opaque_ਅੰਕੜੇ, size_t opaque_ਅੰਕੜੇ_len);


#ਜੇ_ਅੰਤ /* USE_NGHTTP2 */

#ਜੇ_ਅੰਤ /* HEADER_CURL_NGH2_SHIM_H */
