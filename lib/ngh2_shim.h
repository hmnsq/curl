#నిర్వచితం_కాకపోతే శీర్షిక_CURL_NGH2_SHIM_H
#నిర్వచించు శీర్షిక_CURL_NGH2_SHIM_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_NGHTTP2

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
నిర్మాణం curl_ngh2 {
  nghttp2_session *s;
  పూర్ణాంకం fatal_rv; /* != 0 once nghttp2 session has entered FATAL state */
};

/* True if nghttp2 session exists and is not in fatal state */
bool Curl_ngh2_valid(నిర్మాణం curl_ngh2 *ngh2);
/* True if `rv` is fatal or nghttp2 session already is in fatal state */
bool Curl_ngh2_is_fatal(నిర్మాణం curl_ngh2 *ngh2, పూర్ణాంకం rv);
/* != 0 and the fatal error if nghttp2 is in fatal state */
పూర్ణాంకం Curl_ngh2_fatal(నిర్మాణం curl_ngh2 *ngh2);

శూన్య Curl_ngh2_del(నిర్మాణం curl_ngh2 *ngh2);
పూర్ణాంకం Curl_ngh2_submit_settings(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags,
  స్థిరరాశి nghttp2_settings_నమోదు *iv, size_t niv);
శూన్య *Curl_ngh2_session_get_ప్రవాహం_user_దత్తాంశం(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id);
పూర్ణాంకం Curl_ngh2_session_సమితి_ప్రవాహం_user_దత్తాంశం(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id, శూన్య *ప్రవాహం_user_దత్తాంశం);
uint32_t Curl_ngh2_session_get_remote_settings(
  నిర్మాణం curl_ngh2 *ngh2, nghttp2_settings_id id);
int32_t Curl_ngh2_session_get_ప్రవాహం_effective_local_కిటికీ_కొలత(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id);
int32_t Curl_ngh2_session_get_local_కిటికీ_కొలత(
  నిర్మాణం curl_ngh2 *ngh2);
int32_t Curl_ngh2_session_get_ప్రవాహం_remote_కిటికీ_కొలత(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id);
int32_t Curl_ngh2_session_get_remote_కిటికీ_కొలత(
  నిర్మాణం curl_ngh2 *ngh2);
పూర్ణాంకం Curl_ngh2_session_సమితి_local_కిటికీ_కొలత(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags,
  int32_t ప్రవాహం_id, int32_t కిటికీ_కొలత);
పూర్ణాంకం Curl_ngh2_submit_కిటికీ_update(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags,
  int32_t ప్రవాహం_id, int32_t కిటికీ_కొలత_increment);
పూర్ణాంకం Curl_ngh2_submit_rst_ప్రవాహం(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags,
  int32_t ప్రవాహం_id, uint32_t error_సంకేతం);
పూర్ణాంకం Curl_ngh2_session_send(నిర్మాణం curl_ngh2 *ngh2);
పూర్ణాంకం Curl_ngh2_session_client_new3(
  నిర్మాణం curl_ngh2 *ngh2, స్థిరరాశి nghttp2_session_callbacks *callbacks,
  శూన్య *user_దత్తాంశం, స్థిరరాశి nghttp2_option *option, nghttp2_mem *mem);
పూర్ణాంకం Curl_ngh2_session_want_read(నిర్మాణం curl_ngh2 *ngh2);
పూర్ణాంకం Curl_ngh2_session_want_write(నిర్మాణం curl_ngh2 *ngh2);
ssize_t Curl_ngh2_session_mem_recv(
  నిర్మాణం curl_ngh2 *ngh2, స్థిరరాశి uint8_t *లోపల, size_t inlen);
పూర్ణాంకం Curl_ngh2_session_check_request_allowed(నిర్మాణం curl_ngh2 *ngh2);
పూర్ణాంకం Curl_ngh2_submit_ping(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags, స్థిరరాశి uint8_t *opaque_దత్తాంశం);
int32_t Curl_ngh2_session_get_ప్రవాహం_effective_recv_దత్తాంశం_నిడివి(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id);
పూర్ణాంకం Curl_ngh2_session_resume_దత్తాంశం(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id);
పూర్ణాంకం Curl_ngh2_session_consume(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id, size_t కొలత);
పూర్ణాంకం Curl_ngh2_submit_ప్రాధాన్యత(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags,
  int32_t ప్రవాహం_id, స్థిరరాశి nghttp2_ప్రాధాన్యత_spec *pri_spec);
int32_t Curl_ngh2_submit_request(
  నిర్మాణం curl_ngh2 *ngh2, స్థిరరాశి nghttp2_ప్రాధాన్యత_spec *pri_spec,
  స్థిరరాశి nghttp2_nv *nva, size_t nvlen, స్థిరరాశి nghttp2_దత్తాంశం_provider *దత్తాంశం_prd,
  శూన్య *ప్రవాహం_user_దత్తాంశం);
పూర్ణాంకం Curl_ngh2_session_upgrade2(
  నిర్మాణం curl_ngh2 *ngh2, స్థిరరాశి uint8_t *settings_payload,
  size_t settings_payloadlen, పూర్ణాంకం తల_request, శూన్య *ప్రవాహం_user_దత్తాంశం);
పూర్ణాంకం Curl_ngh2_submit_goaway(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags, int32_t చివరి_ప్రవాహం_id,
  uint32_t error_సంకేతం, స్థిరరాశి uint8_t *opaque_దత్తాంశం, size_t opaque_దత్తాంశం_len);


#అయితే_ముగింపు /* USE_NGHTTP2 */

#అయితే_ముగింపు /* HEADER_CURL_NGH2_SHIM_H */
