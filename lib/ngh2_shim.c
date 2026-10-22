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


bool Curl_ngh2_valid(সংরচনা curl_ngh2 *ngh2)
{
  প্রত্যাগম ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(সংরচনা curl_ngh2 *ngh2, পূর্ণাঙ্ক rv)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম TRUE;
  যদি(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

পূর্ণাঙ্ক Curl_ngh2_fatal(সংরচনা curl_ngh2 *ngh2)
{
  প্রত্যাগম ngh2->fatal_rv;
}

স্থির পূর্ণাঙ্ক ngh2_check(সংরচনা curl_ngh2 *ngh2, পূর্ণাঙ্ক rv)
{
  যদি(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  প্রত্যাগম rv;
}

স্থির int32_t ngh2_check32(সংরচনা curl_ngh2 *ngh2, int32_t rv)
{
  যদি((rv < 0) && nghttp2_is_fatal((পূর্ণাঙ্ক)rv))
    ngh2->fatal_rv = (পূর্ণাঙ্ক)rv;
  প্রত্যাগম rv;
}

স্থির ssize_t ngh2_checks(সংরচনা curl_ngh2 *ngh2, ssize_t rv)
{
  যদি((rv < 0) && nghttp2_is_fatal((পূর্ণাঙ্ক)rv))
    ngh2->fatal_rv = (পূর্ণাঙ্ক)rv;
  প্রত্যাগম rv;
}

শূন্য Curl_ngh2_del(সংরচনা curl_ngh2 *ngh2)
{
  যদি(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = রিক্ত;
  }
}

পূর্ণাঙ্ক Curl_ngh2_submit_settings(সংরচনা curl_ngh2 *ngh2,
                              uint8_t flags,
                              ধ্রুব nghttp2_settings_entry *iv,
                              size_t niv)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

শূন্য *Curl_ngh2_session_get_stream_user_data(সংরচনা curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম রিক্ত;
  প্রত্যাগম nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

পূর্ণাঙ্ক Curl_ngh2_session_set_stream_user_data(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id, শূন্য *stream_user_data)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  সংরচনা curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  যদি(!Curl_ngh2_valid(ngh2))
    প্রত্যাগম 0;
  প্রত্যাগম nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম 0;
  প্রত্যাগম nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  সংরচনা curl_ngh2 *ngh2)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম 0;
  প্রত্যাগম nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম 0;
  প্রত্যাগম nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  সংরচনা curl_ngh2 *ngh2)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম 0;
  প্রত্যাগম nghttp2_session_get_remote_window_size(ngh2->s);
}

পূর্ণাঙ্ক Curl_ngh2_session_set_local_window_size(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

পূর্ণাঙ্ক Curl_ngh2_submit_window_update(সংরচনা curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

পূর্ণাঙ্ক Curl_ngh2_submit_rst_stream(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

পূর্ণাঙ্ক Curl_ngh2_session_send(সংরচনা curl_ngh2 *ngh2)
{
  পূর্ণাঙ্ক rv;
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  প্রত্যাগম ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

পূর্ণাঙ্ক Curl_ngh2_session_client_new3(
  সংরচনা curl_ngh2 *ngh2, ধ্রুব nghttp2_session_callbacks *callbacks,
  শূন্য *user_data, ধ্রুব nghttp2_option *option, nghttp2_mem *mem)
{
  যদি(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

পূর্ণাঙ্ক Curl_ngh2_session_want_read(সংরচনা curl_ngh2 *ngh2)
{
  যদি(!Curl_ngh2_valid(ngh2))
    প্রত্যাগম 0;
  প্রত্যাগম nghttp2_session_want_read(ngh2->s);
}

পূর্ণাঙ্ক Curl_ngh2_session_want_write(সংরচনা curl_ngh2 *ngh2)
{
  যদি(!Curl_ngh2_valid(ngh2))
    প্রত্যাগম 0;
  প্রত্যাগম nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  সংরচনা curl_ngh2 *ngh2, ধ্রুব uint8_t *in, size_t inlen)
{
  ssize_t rv;
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  প্রত্যাগম ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

পূর্ণাঙ্ক Curl_ngh2_session_check_request_allowed(সংরচনা curl_ngh2 *ngh2)
{
  যদি(!Curl_ngh2_valid(ngh2))
    প্রত্যাগম 0;
  প্রত্যাগম nghttp2_session_check_request_allowed(ngh2->s);
}

পূর্ণাঙ্ক Curl_ngh2_submit_ping(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags, ধ্রুব uint8_t *opaque_data)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id)
{
  যদি(!Curl_ngh2_valid(ngh2))
    প্রত্যাগম 0;
  প্রত্যাগম nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

পূর্ণাঙ্ক Curl_ngh2_session_resume_data(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

পূর্ণাঙ্ক Curl_ngh2_session_consume(
  সংরচনা curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

পূর্ণাঙ্ক Curl_ngh2_submit_priority(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, ধ্রুব nghttp2_priority_spec *pri_spec)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  সংরচনা curl_ngh2 *ngh2, ধ্রুব nghttp2_priority_spec *pri_spec,
  ধ্রুব nghttp2_nv *nva, size_t nvlen, ধ্রুব nghttp2_data_provider *data_prd,
  শূন্য *stream_user_data)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম (int32_t)ngh2->fatal_rv;
  প্রত্যাগম ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

পূর্ণাঙ্ক Curl_ngh2_session_upgrade2(
  সংরচনা curl_ngh2 *ngh2, ধ্রুব uint8_t *settings_payload,
  size_t settings_payloadlen, পূর্ণাঙ্ক head_request, শূন্য *stream_user_data)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

পূর্ণাঙ্ক Curl_ngh2_submit_goaway(
  সংরচনা curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, ধ্রুব uint8_t *opaque_data, size_t opaque_data_len)
{
  যদি(ngh2->fatal_rv)
    প্রত্যাগম ngh2->fatal_rv;
  প্রত্যাগম ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
