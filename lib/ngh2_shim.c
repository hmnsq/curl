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

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#అన్తర్భావ <nghttp2/nghttp2.h>

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "ngh2_shim.h"


bool Curl_ngh2_valid(నిర్మాణం curl_ngh2 *ngh2)
{
  తిరిగి ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(నిర్మాణం curl_ngh2 *ngh2, పూర్ణాంకం rv)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి TRUE;
  ఒకవేళ(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

పూర్ణాంకం Curl_ngh2_fatal(నిర్మాణం curl_ngh2 *ngh2)
{
  తిరిగి ngh2->fatal_rv;
}

స్థిర పూర్ణాంకం ngh2_check(నిర్మాణం curl_ngh2 *ngh2, పూర్ణాంకం rv)
{
  ఒకవేళ(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  తిరిగి rv;
}

స్థిర int32_t ngh2_check32(నిర్మాణం curl_ngh2 *ngh2, int32_t rv)
{
  ఒకవేళ((rv < 0) && nghttp2_is_fatal((పూర్ణాంకం)rv))
    ngh2->fatal_rv = (పూర్ణాంకం)rv;
  తిరిగి rv;
}

స్థిర ssize_t ngh2_checks(నిర్మాణం curl_ngh2 *ngh2, ssize_t rv)
{
  ఒకవేళ((rv < 0) && nghttp2_is_fatal((పూర్ణాంకం)rv))
    ngh2->fatal_rv = (పూర్ణాంకం)rv;
  తిరిగి rv;
}

శూన్య Curl_ngh2_del(నిర్మాణం curl_ngh2 *ngh2)
{
  ఒకవేళ(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = ఖాళీ;
  }
}

పూర్ణాంకం Curl_ngh2_submit_settings(నిర్మాణం curl_ngh2 *ngh2,
                              uint8_t flags,
                              స్థిరరాశి nghttp2_settings_నమోదు *iv,
                              size_t niv)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

శూన్య *Curl_ngh2_session_get_ప్రవాహం_user_దత్తాంశం(నిర్మాణం curl_ngh2 *ngh2,
                                             int32_t ప్రవాహం_id)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ఖాళీ;
  తిరిగి nghttp2_session_get_ప్రవాహం_user_దత్తాంశం(ngh2->s, ప్రవాహం_id);
}

పూర్ణాంకం Curl_ngh2_session_సమితి_ప్రవాహం_user_దత్తాంశం(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2,
    nghttp2_session_సమితి_ప్రవాహం_user_దత్తాంశం(
      ngh2->s, ప్రవాహం_id, ప్రవాహం_user_దత్తాంశం));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  నిర్మాణం curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  ఒకవేళ(!Curl_ngh2_valid(ngh2))
    తిరిగి 0;
  తిరిగి nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_ప్రవాహం_effective_local_కిటికీ_కొలత(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి 0;
  తిరిగి nghttp2_session_get_ప్రవాహం_effective_local_కిటికీ_కొలత(
    ngh2->s, ప్రవాహం_id);
}

int32_t Curl_ngh2_session_get_local_కిటికీ_కొలత(
  నిర్మాణం curl_ngh2 *ngh2)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి 0;
  తిరిగి nghttp2_session_get_local_కిటికీ_కొలత(ngh2->s);
}

int32_t Curl_ngh2_session_get_ప్రవాహం_remote_కిటికీ_కొలత(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి 0;
  తిరిగి nghttp2_session_get_ప్రవాహం_remote_కిటికీ_కొలత(ngh2->s, ప్రవాహం_id);
}

int32_t Curl_ngh2_session_get_remote_కిటికీ_కొలత(
  నిర్మాణం curl_ngh2 *ngh2)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి 0;
  తిరిగి nghttp2_session_get_remote_కిటికీ_కొలత(ngh2->s);
}

పూర్ణాంకం Curl_ngh2_session_సమితి_local_కిటికీ_కొలత(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags,
  int32_t ప్రవాహం_id, int32_t కిటికీ_కొలత)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2, nghttp2_session_సమితి_local_కిటికీ_కొలత(
    ngh2->s, flags, ప్రవాహం_id, కిటికీ_కొలత));
}

పూర్ణాంకం Curl_ngh2_submit_కిటికీ_update(నిర్మాణం curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t ప్రవాహం_id,
  int32_t కిటికీ_కొలత_increment)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2, nghttp2_submit_కిటికీ_update(
    ngh2->s, flags, ప్రవాహం_id, కిటికీ_కొలత_increment));
}

పూర్ణాంకం Curl_ngh2_submit_rst_ప్రవాహం(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags,
  int32_t ప్రవాహం_id, uint32_t error_సంకేతం)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2, nghttp2_submit_rst_ప్రవాహం(
    ngh2->s, flags, ప్రవాహం_id, error_సంకేతం));
}

పూర్ణాంకం Curl_ngh2_session_send(నిర్మాణం curl_ngh2 *ngh2)
{
  పూర్ణాంకం rv;
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  తిరిగి ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

పూర్ణాంకం Curl_ngh2_session_client_new3(
  నిర్మాణం curl_ngh2 *ngh2, స్థిరరాశి nghttp2_session_callbacks *callbacks,
  శూన్య *user_దత్తాంశం, స్థిరరాశి nghttp2_option *option, nghttp2_mem *mem)
{
  ఒకవేళ(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_దత్తాంశం, option, mem));
}

పూర్ణాంకం Curl_ngh2_session_want_read(నిర్మాణం curl_ngh2 *ngh2)
{
  ఒకవేళ(!Curl_ngh2_valid(ngh2))
    తిరిగి 0;
  తిరిగి nghttp2_session_want_read(ngh2->s);
}

పూర్ణాంకం Curl_ngh2_session_want_write(నిర్మాణం curl_ngh2 *ngh2)
{
  ఒకవేళ(!Curl_ngh2_valid(ngh2))
    తిరిగి 0;
  తిరిగి nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  నిర్మాణం curl_ngh2 *ngh2, స్థిరరాశి uint8_t *లోపల, size_t inlen)
{
  ssize_t rv;
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, లోపల, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  తిరిగి ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

పూర్ణాంకం Curl_ngh2_session_check_request_allowed(నిర్మాణం curl_ngh2 *ngh2)
{
  ఒకవేళ(!Curl_ngh2_valid(ngh2))
    తిరిగి 0;
  తిరిగి nghttp2_session_check_request_allowed(ngh2->s);
}

పూర్ణాంకం Curl_ngh2_submit_ping(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags, స్థిరరాశి uint8_t *opaque_దత్తాంశం)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_దత్తాంశం));
}

int32_t Curl_ngh2_session_get_ప్రవాహం_effective_recv_దత్తాంశం_నిడివి(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id)
{
  ఒకవేళ(!Curl_ngh2_valid(ngh2))
    తిరిగి 0;
  తిరిగి nghttp2_session_get_ప్రవాహం_effective_recv_దత్తాంశం_నిడివి(
    ngh2->s, ప్రవాహం_id);
}

పూర్ణాంకం Curl_ngh2_session_resume_దత్తాంశం(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2,
    nghttp2_session_resume_దత్తాంశం(ngh2->s, ప్రవాహం_id));
}

పూర్ణాంకం Curl_ngh2_session_consume(
  నిర్మాణం curl_ngh2 *ngh2, int32_t ప్రవాహం_id, size_t కొలత)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, ప్రవాహం_id, కొలత));
}

పూర్ణాంకం Curl_ngh2_submit_ప్రాధాన్యత(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags,
  int32_t ప్రవాహం_id, స్థిరరాశి nghttp2_ప్రాధాన్యత_spec *pri_spec)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2,
    nghttp2_submit_ప్రాధాన్యత(ngh2->s, flags, ప్రవాహం_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  నిర్మాణం curl_ngh2 *ngh2, స్థిరరాశి nghttp2_ప్రాధాన్యత_spec *pri_spec,
  స్థిరరాశి nghttp2_nv *nva, size_t nvlen, స్థిరరాశి nghttp2_దత్తాంశం_provider *దత్తాంశం_prd,
  శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి (int32_t)ngh2->fatal_rv;
  తిరిగి ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      దత్తాంశం_prd, ప్రవాహం_user_దత్తాంశం));
}

పూర్ణాంకం Curl_ngh2_session_upgrade2(
  నిర్మాణం curl_ngh2 *ngh2, స్థిరరాశి uint8_t *settings_payload,
  size_t settings_payloadlen, పూర్ణాంకం తల_request, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, తల_request, ప్రవాహం_user_దత్తాంశం));
}

పూర్ణాంకం Curl_ngh2_submit_goaway(
  నిర్మాణం curl_ngh2 *ngh2, uint8_t flags, int32_t చివరి_ప్రవాహం_id,
  uint32_t error_సంకేతం, స్థిరరాశి uint8_t *opaque_దత్తాంశం, size_t opaque_దత్తాంశం_len)
{
  ఒకవేళ(ngh2->fatal_rv)
    తిరిగి ngh2->fatal_rv;
  తిరిగి ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, చివరి_ప్రవాహం_id, error_సంకేతం,
    opaque_దత్తాంశం, opaque_దత్తాంశం_len));
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
