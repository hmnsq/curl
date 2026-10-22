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

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#অন্তর্ভাব <nghttp2/nghttp2.h>

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "ngh2_shim.h"


bool Curl_ngh2_valid(কাঠামো curl_ngh2 *ngh2)
{
  ফেরত ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(কাঠামো curl_ngh2 *ngh2, পূর্ণসংখ্যা rv)
{
  যদি(ngh2->fatal_rv)
    ফেরত TRUE;
  যদি(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

পূর্ণসংখ্যা Curl_ngh2_fatal(কাঠামো curl_ngh2 *ngh2)
{
  ফেরত ngh2->fatal_rv;
}

স্থির পূর্ণসংখ্যা ngh2_check(কাঠামো curl_ngh2 *ngh2, পূর্ণসংখ্যা rv)
{
  যদি(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  ফেরত rv;
}

স্থির int32_t ngh2_check32(কাঠামো curl_ngh2 *ngh2, int32_t rv)
{
  যদি((rv < 0) && nghttp2_is_fatal((পূর্ণসংখ্যা)rv))
    ngh2->fatal_rv = (পূর্ণসংখ্যা)rv;
  ফেরত rv;
}

স্থির ssize_t ngh2_checks(কাঠামো curl_ngh2 *ngh2, ssize_t rv)
{
  যদি((rv < 0) && nghttp2_is_fatal((পূর্ণসংখ্যা)rv))
    ngh2->fatal_rv = (পূর্ণসংখ্যা)rv;
  ফেরত rv;
}

শূন্য Curl_ngh2_del(কাঠামো curl_ngh2 *ngh2)
{
  যদি(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = ফাঁকা;
  }
}

পূর্ণসংখ্যা Curl_ngh2_submit_settings(কাঠামো curl_ngh2 *ngh2,
                              uint8_t flags,
                              ধ্রুবক nghttp2_settings_entry *iv,
                              size_t niv)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

শূন্য *Curl_ngh2_session_get_stream_user_তথ্য(কাঠামো curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  যদি(ngh2->fatal_rv)
    ফেরত ফাঁকা;
  ফেরত nghttp2_session_get_stream_user_তথ্য(ngh2->s, stream_id);
}

পূর্ণসংখ্যা Curl_ngh2_session_set_stream_user_তথ্য(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id, শূন্য *stream_user_তথ্য)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2,
    nghttp2_session_set_stream_user_তথ্য(
      ngh2->s, stream_id, stream_user_তথ্য));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  কাঠামো curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  যদি(!Curl_ngh2_valid(ngh2))
    ফেরত 0;
  ফেরত nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_আকার(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id)
{
  যদি(ngh2->fatal_rv)
    ফেরত 0;
  ফেরত nghttp2_session_get_stream_effective_local_window_আকার(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_আকার(
  কাঠামো curl_ngh2 *ngh2)
{
  যদি(ngh2->fatal_rv)
    ফেরত 0;
  ফেরত nghttp2_session_get_local_window_আকার(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_আকার(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id)
{
  যদি(ngh2->fatal_rv)
    ফেরত 0;
  ফেরত nghttp2_session_get_stream_remote_window_আকার(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_আকার(
  কাঠামো curl_ngh2 *ngh2)
{
  যদি(ngh2->fatal_rv)
    ফেরত 0;
  ফেরত nghttp2_session_get_remote_window_আকার(ngh2->s);
}

পূর্ণসংখ্যা Curl_ngh2_session_set_local_window_আকার(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_আকার)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2, nghttp2_session_set_local_window_আকার(
    ngh2->s, flags, stream_id, window_আকার));
}

পূর্ণসংখ্যা Curl_ngh2_submit_window_update(কাঠামো curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_আকার_increment)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_আকার_increment));
}

পূর্ণসংখ্যা Curl_ngh2_submit_rst_stream(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

পূর্ণসংখ্যা Curl_ngh2_session_send(কাঠামো curl_ngh2 *ngh2)
{
  পূর্ণসংখ্যা rv;
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  ফেরত ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

পূর্ণসংখ্যা Curl_ngh2_session_client_new3(
  কাঠামো curl_ngh2 *ngh2, ধ্রুবক nghttp2_session_callbacks *callbacks,
  শূন্য *user_তথ্য, ধ্রুবক nghttp2_option *option, nghttp2_mem *mem)
{
  যদি(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_তথ্য, option, mem));
}

পূর্ণসংখ্যা Curl_ngh2_session_want_read(কাঠামো curl_ngh2 *ngh2)
{
  যদি(!Curl_ngh2_valid(ngh2))
    ফেরত 0;
  ফেরত nghttp2_session_want_read(ngh2->s);
}

পূর্ণসংখ্যা Curl_ngh2_session_want_write(কাঠামো curl_ngh2 *ngh2)
{
  যদি(!Curl_ngh2_valid(ngh2))
    ফেরত 0;
  ফেরত nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  কাঠামো curl_ngh2 *ngh2, ধ্রুবক uint8_t *in, size_t inlen)
{
  ssize_t rv;
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  ফেরত ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

পূর্ণসংখ্যা Curl_ngh2_session_check_request_allowed(কাঠামো curl_ngh2 *ngh2)
{
  যদি(!Curl_ngh2_valid(ngh2))
    ফেরত 0;
  ফেরত nghttp2_session_check_request_allowed(ngh2->s);
}

পূর্ণসংখ্যা Curl_ngh2_submit_ping(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags, ধ্রুবক uint8_t *opaque_তথ্য)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_তথ্য));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_তথ্য_দৈর্ঘ্য(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id)
{
  যদি(!Curl_ngh2_valid(ngh2))
    ফেরত 0;
  ফেরত nghttp2_session_get_stream_effective_recv_তথ্য_দৈর্ঘ্য(
    ngh2->s, stream_id);
}

পূর্ণসংখ্যা Curl_ngh2_session_resume_তথ্য(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2,
    nghttp2_session_resume_তথ্য(ngh2->s, stream_id));
}

পূর্ণসংখ্যা Curl_ngh2_session_consume(
  কাঠামো curl_ngh2 *ngh2, int32_t stream_id, size_t আকার_2)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, আকার_2));
}

পূর্ণসংখ্যা Curl_ngh2_submit_priority(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, ধ্রুবক nghttp2_priority_spec *pri_spec)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  কাঠামো curl_ngh2 *ngh2, ধ্রুবক nghttp2_priority_spec *pri_spec,
  ধ্রুবক nghttp2_nv *nva, size_t nvlen, ধ্রুবক nghttp2_তথ্য_provider *তথ্য_prd,
  শূন্য *stream_user_তথ্য)
{
  যদি(ngh2->fatal_rv)
    ফেরত (int32_t)ngh2->fatal_rv;
  ফেরত ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      তথ্য_prd, stream_user_তথ্য));
}

পূর্ণসংখ্যা Curl_ngh2_session_upgrade2(
  কাঠামো curl_ngh2 *ngh2, ধ্রুবক uint8_t *settings_payload,
  size_t settings_payloadlen, পূর্ণসংখ্যা শীর্ষ_request, শূন্য *stream_user_তথ্য)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, শীর্ষ_request, stream_user_তথ্য));
}

পূর্ণসংখ্যা Curl_ngh2_submit_goaway(
  কাঠামো curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, ধ্রুবক uint8_t *opaque_তথ্য, size_t opaque_তথ্য_len)
{
  যদি(ngh2->fatal_rv)
    ফেরত ngh2->fatal_rv;
  ফেরত ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_তথ্য, opaque_তথ্য_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
