#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_NGH2_SHIM_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_NGH2_SHIM_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2

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
ರಚನೆ curl_ngh2 {
  nghttp2_session *s;
  ಪೂರ್ಣಾಂಕ fatal_rv; /* != 0 once nghttp2 session has entered FATAL state */
};

/* True if nghttp2 session exists and is not in fatal state */
bool Curl_ngh2_valid(ರಚನೆ curl_ngh2 *ngh2);
/* True if `rv` is fatal or nghttp2 session already is in fatal state */
bool Curl_ngh2_is_fatal(ರಚನೆ curl_ngh2 *ngh2, ಪೂರ್ಣಾಂಕ rv);
/* != 0 and the fatal error if nghttp2 is in fatal state */
ಪೂರ್ಣಾಂಕ Curl_ngh2_fatal(ರಚನೆ curl_ngh2 *ngh2);

ಶೂನ್ಯ Curl_ngh2_del(ರಚನೆ curl_ngh2 *ngh2);
ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_settings(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags,
  ಸ್ಥಿರಾಂಕ nghttp2_settings_ನಮೂದು *iv, size_t niv);
ಶೂನ್ಯ *Curl_ngh2_session_get_ಹರಿವು_user_ದತ್ತಾಂಶ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id);
ಪೂರ್ಣಾಂಕ Curl_ngh2_session_ಗಣ_ಹರಿವು_user_ದತ್ತಾಂಶ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ);
uint32_t Curl_ngh2_session_get_remote_settings(
  ರಚನೆ curl_ngh2 *ngh2, nghttp2_settings_id id);
int32_t Curl_ngh2_session_get_ಹರಿವು_effective_local_ಕಿಟಕಿ_ಗಾತ್ರ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id);
int32_t Curl_ngh2_session_get_local_ಕಿಟಕಿ_ಗಾತ್ರ(
  ರಚನೆ curl_ngh2 *ngh2);
int32_t Curl_ngh2_session_get_ಹರಿವು_remote_ಕಿಟಕಿ_ಗಾತ್ರ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id);
int32_t Curl_ngh2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(
  ರಚನೆ curl_ngh2 *ngh2);
ಪೂರ್ಣಾಂಕ Curl_ngh2_session_ಗಣ_local_ಕಿಟಕಿ_ಗಾತ್ರ(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ಹರಿವು_id, int32_t ಕಿಟಕಿ_ಗಾತ್ರ);
ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_ಕಿಟಕಿ_update(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ಹರಿವು_id, int32_t ಕಿಟಕಿ_ಗಾತ್ರ_increment);
ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_rst_ಹರಿವು(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ಹರಿವು_id, uint32_t error_ಸಂಕೇತ);
ಪೂರ್ಣಾಂಕ Curl_ngh2_session_send(ರಚನೆ curl_ngh2 *ngh2);
ಪೂರ್ಣಾಂಕ Curl_ngh2_session_client_new3(
  ರಚನೆ curl_ngh2 *ngh2, ಸ್ಥಿರಾಂಕ nghttp2_session_callbacks *callbacks,
  ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ nghttp2_option *option, nghttp2_mem *mem);
ಪೂರ್ಣಾಂಕ Curl_ngh2_session_want_read(ರಚನೆ curl_ngh2 *ngh2);
ಪೂರ್ಣಾಂಕ Curl_ngh2_session_want_write(ರಚನೆ curl_ngh2 *ngh2);
ssize_t Curl_ngh2_session_mem_recv(
  ರಚನೆ curl_ngh2 *ngh2, ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ, size_t inlen);
ಪೂರ್ಣಾಂಕ Curl_ngh2_session_check_request_allowed(ರಚನೆ curl_ngh2 *ngh2);
ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_ping(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags, ಸ್ಥಿರಾಂಕ uint8_t *opaque_ದತ್ತಾಂಶ);
int32_t Curl_ngh2_session_get_ಹರಿವು_effective_recv_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id);
ಪೂರ್ಣಾಂಕ Curl_ngh2_session_resume_ದತ್ತಾಂಶ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id);
ಪೂರ್ಣಾಂಕ Curl_ngh2_session_consume(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id, size_t ಗಾತ್ರ_2);
ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_ಆದ್ಯತೆ(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ಹರಿವು_id, ಸ್ಥಿರಾಂಕ nghttp2_ಆದ್ಯತೆ_spec *pri_spec);
int32_t Curl_ngh2_submit_request(
  ರಚನೆ curl_ngh2 *ngh2, ಸ್ಥಿರಾಂಕ nghttp2_ಆದ್ಯತೆ_spec *pri_spec,
  ಸ್ಥಿರಾಂಕ nghttp2_nv *nva, size_t nvlen, ಸ್ಥಿರಾಂಕ nghttp2_ದತ್ತಾಂಶ_provider *ದತ್ತಾಂಶ_prd,
  ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ);
ಪೂರ್ಣಾಂಕ Curl_ngh2_session_upgrade2(
  ರಚನೆ curl_ngh2 *ngh2, ಸ್ಥಿರಾಂಕ uint8_t *settings_payload,
  size_t settings_payloadlen, ಪೂರ್ಣಾಂಕ ತಲೆ_request, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ);
ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_goaway(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags, int32_t ಕೊನೆಯ_ಹರಿವು_id,
  uint32_t error_ಸಂಕೇತ, ಸ್ಥಿರಾಂಕ uint8_t *opaque_ದತ್ತಾಂಶ, size_t opaque_ದತ್ತಾಂಶ_len);


#ಆದರೆ_ಅಂತ್ಯ /* USE_NGHTTP2 */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_NGH2_SHIM_H */
