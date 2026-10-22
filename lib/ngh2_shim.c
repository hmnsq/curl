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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#شامل <nghttp2/nghttp2.h>

#شامل "urldata.h"
#شامل "ngh2_shim.h"


bool Curl_ngh2_valid(ساختار curl_ngh2 *ngh2)
{
  بازگشت ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(ساختار curl_ngh2 *ngh2, صحیح rv)
{
  اگر(ngh2->fatal_rv)
    بازگشت TRUE;
  اگر(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

صحیح Curl_ngh2_fatal(ساختار curl_ngh2 *ngh2)
{
  بازگشت ngh2->fatal_rv;
}

ایستا صحیح ngh2_check(ساختار curl_ngh2 *ngh2, صحیح rv)
{
  اگر(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  بازگشت rv;
}

ایستا int32_t ngh2_check32(ساختار curl_ngh2 *ngh2, int32_t rv)
{
  اگر((rv < 0) && nghttp2_is_fatal((صحیح)rv))
    ngh2->fatal_rv = (صحیح)rv;
  بازگشت rv;
}

ایستا ssize_t ngh2_checks(ساختار curl_ngh2 *ngh2, ssize_t rv)
{
  اگر((rv < 0) && nghttp2_is_fatal((صحیح)rv))
    ngh2->fatal_rv = (صحیح)rv;
  بازگشت rv;
}

تهی Curl_ngh2_del(ساختار curl_ngh2 *ngh2)
{
  اگر(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = پوچ;
  }
}

صحیح Curl_ngh2_submit_settings(ساختار curl_ngh2 *ngh2,
                              uint8_t flags,
                              ثابت nghttp2_settings_entry *iv,
                              size_t niv)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

تهی *Curl_ngh2_session_get_stream_user_data(ساختار curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  اگر(ngh2->fatal_rv)
    بازگشت پوچ;
  بازگشت nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

صحیح Curl_ngh2_session_set_stream_user_data(
  ساختار curl_ngh2 *ngh2, int32_t stream_id, تهی *stream_user_data)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  ساختار curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  اگر(!Curl_ngh2_valid(ngh2))
    بازگشت 0;
  بازگشت nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  ساختار curl_ngh2 *ngh2, int32_t stream_id)
{
  اگر(ngh2->fatal_rv)
    بازگشت 0;
  بازگشت nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  ساختار curl_ngh2 *ngh2)
{
  اگر(ngh2->fatal_rv)
    بازگشت 0;
  بازگشت nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  ساختار curl_ngh2 *ngh2, int32_t stream_id)
{
  اگر(ngh2->fatal_rv)
    بازگشت 0;
  بازگشت nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  ساختار curl_ngh2 *ngh2)
{
  اگر(ngh2->fatal_rv)
    بازگشت 0;
  بازگشت nghttp2_session_get_remote_window_size(ngh2->s);
}

صحیح Curl_ngh2_session_set_local_window_size(
  ساختار curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

صحیح Curl_ngh2_submit_window_update(ساختار curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

صحیح Curl_ngh2_submit_rst_stream(
  ساختار curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

صحیح Curl_ngh2_session_send(ساختار curl_ngh2 *ngh2)
{
  صحیح rv;
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  بازگشت ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

صحیح Curl_ngh2_session_client_new3(
  ساختار curl_ngh2 *ngh2, ثابت nghttp2_session_callbacks *callbacks,
  تهی *user_data, ثابت nghttp2_option *option, nghttp2_mem *mem)
{
  اگر(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

صحیح Curl_ngh2_session_want_read(ساختار curl_ngh2 *ngh2)
{
  اگر(!Curl_ngh2_valid(ngh2))
    بازگشت 0;
  بازگشت nghttp2_session_want_read(ngh2->s);
}

صحیح Curl_ngh2_session_want_write(ساختار curl_ngh2 *ngh2)
{
  اگر(!Curl_ngh2_valid(ngh2))
    بازگشت 0;
  بازگشت nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  ساختار curl_ngh2 *ngh2, ثابت uint8_t *in, size_t inlen)
{
  ssize_t rv;
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  بازگشت ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

صحیح Curl_ngh2_session_check_request_allowed(ساختار curl_ngh2 *ngh2)
{
  اگر(!Curl_ngh2_valid(ngh2))
    بازگشت 0;
  بازگشت nghttp2_session_check_request_allowed(ngh2->s);
}

صحیح Curl_ngh2_submit_ping(
  ساختار curl_ngh2 *ngh2, uint8_t flags, ثابت uint8_t *opaque_data)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  ساختار curl_ngh2 *ngh2, int32_t stream_id)
{
  اگر(!Curl_ngh2_valid(ngh2))
    بازگشت 0;
  بازگشت nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

صحیح Curl_ngh2_session_resume_data(
  ساختار curl_ngh2 *ngh2, int32_t stream_id)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

صحیح Curl_ngh2_session_consume(
  ساختار curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

صحیح Curl_ngh2_submit_priority(
  ساختار curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, ثابت nghttp2_priority_spec *pri_spec)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  ساختار curl_ngh2 *ngh2, ثابت nghttp2_priority_spec *pri_spec,
  ثابت nghttp2_nv *nva, size_t nvlen, ثابت nghttp2_data_provider *data_prd,
  تهی *stream_user_data)
{
  اگر(ngh2->fatal_rv)
    بازگشت (int32_t)ngh2->fatal_rv;
  بازگشت ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

صحیح Curl_ngh2_session_upgrade2(
  ساختار curl_ngh2 *ngh2, ثابت uint8_t *settings_payload,
  size_t settings_payloadlen, صحیح head_request, تهی *stream_user_data)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

صحیح Curl_ngh2_submit_goaway(
  ساختار curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, ثابت uint8_t *opaque_data, size_t opaque_data_len)
{
  اگر(ngh2->fatal_rv)
    بازگشت ngh2->fatal_rv;
  بازگشت ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
