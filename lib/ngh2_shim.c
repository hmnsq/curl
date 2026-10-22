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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#அந்தர்பாவ <nghttp2/nghttp2.h>

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "ngh2_shim.h"


bool Curl_ngh2_valid(கட்டமைப்பு curl_ngh2 *ngh2)
{
  திரும்பு ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(கட்டமைப்பு curl_ngh2 *ngh2, முழு rv)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு TRUE;
  எனில்(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

முழு Curl_ngh2_fatal(கட்டமைப்பு curl_ngh2 *ngh2)
{
  திரும்பு ngh2->fatal_rv;
}

நிலையான முழு ngh2_check(கட்டமைப்பு curl_ngh2 *ngh2, முழு rv)
{
  எனில்(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  திரும்பு rv;
}

நிலையான int32_t ngh2_check32(கட்டமைப்பு curl_ngh2 *ngh2, int32_t rv)
{
  எனில்((rv < 0) && nghttp2_is_fatal((முழு)rv))
    ngh2->fatal_rv = (முழு)rv;
  திரும்பு rv;
}

நிலையான ssize_t ngh2_checks(கட்டமைப்பு curl_ngh2 *ngh2, ssize_t rv)
{
  எனில்((rv < 0) && nghttp2_is_fatal((முழு)rv))
    ngh2->fatal_rv = (முழு)rv;
  திரும்பு rv;
}

வெறுமை Curl_ngh2_del(கட்டமைப்பு curl_ngh2 *ngh2)
{
  எனில்(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = வெற்று;
  }
}

முழு Curl_ngh2_submit_settings(கட்டமைப்பு curl_ngh2 *ngh2,
                              uint8_t flags,
                              மாறிலி nghttp2_settings_entry *iv,
                              size_t niv)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

வெறுமை *Curl_ngh2_session_get_stream_user_data(கட்டமைப்பு curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு வெற்று;
  திரும்பு nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

முழு Curl_ngh2_session_set_stream_user_data(
  கட்டமைப்பு curl_ngh2 *ngh2, int32_t stream_id, வெறுமை *stream_user_data)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  கட்டமைப்பு curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  எனில்(!Curl_ngh2_valid(ngh2))
    திரும்பு 0;
  திரும்பு nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  கட்டமைப்பு curl_ngh2 *ngh2, int32_t stream_id)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு 0;
  திரும்பு nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  கட்டமைப்பு curl_ngh2 *ngh2)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு 0;
  திரும்பு nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  கட்டமைப்பு curl_ngh2 *ngh2, int32_t stream_id)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு 0;
  திரும்பு nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  கட்டமைப்பு curl_ngh2 *ngh2)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு 0;
  திரும்பு nghttp2_session_get_remote_window_size(ngh2->s);
}

முழு Curl_ngh2_session_set_local_window_size(
  கட்டமைப்பு curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

முழு Curl_ngh2_submit_window_update(கட்டமைப்பு curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

முழு Curl_ngh2_submit_rst_stream(
  கட்டமைப்பு curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

முழு Curl_ngh2_session_send(கட்டமைப்பு curl_ngh2 *ngh2)
{
  முழு rv;
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  திரும்பு ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

முழு Curl_ngh2_session_client_new3(
  கட்டமைப்பு curl_ngh2 *ngh2, மாறிலி nghttp2_session_callbacks *callbacks,
  வெறுமை *user_data, மாறிலி nghttp2_option *option, nghttp2_mem *mem)
{
  எனில்(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

முழு Curl_ngh2_session_want_read(கட்டமைப்பு curl_ngh2 *ngh2)
{
  எனில்(!Curl_ngh2_valid(ngh2))
    திரும்பு 0;
  திரும்பு nghttp2_session_want_read(ngh2->s);
}

முழு Curl_ngh2_session_want_write(கட்டமைப்பு curl_ngh2 *ngh2)
{
  எனில்(!Curl_ngh2_valid(ngh2))
    திரும்பு 0;
  திரும்பு nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  கட்டமைப்பு curl_ngh2 *ngh2, மாறிலி uint8_t *in, size_t inlen)
{
  ssize_t rv;
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  திரும்பு ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

முழு Curl_ngh2_session_check_request_allowed(கட்டமைப்பு curl_ngh2 *ngh2)
{
  எனில்(!Curl_ngh2_valid(ngh2))
    திரும்பு 0;
  திரும்பு nghttp2_session_check_request_allowed(ngh2->s);
}

முழு Curl_ngh2_submit_ping(
  கட்டமைப்பு curl_ngh2 *ngh2, uint8_t flags, மாறிலி uint8_t *opaque_data)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  கட்டமைப்பு curl_ngh2 *ngh2, int32_t stream_id)
{
  எனில்(!Curl_ngh2_valid(ngh2))
    திரும்பு 0;
  திரும்பு nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

முழு Curl_ngh2_session_resume_data(
  கட்டமைப்பு curl_ngh2 *ngh2, int32_t stream_id)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

முழு Curl_ngh2_session_consume(
  கட்டமைப்பு curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

முழு Curl_ngh2_submit_priority(
  கட்டமைப்பு curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, மாறிலி nghttp2_priority_spec *pri_spec)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  கட்டமைப்பு curl_ngh2 *ngh2, மாறிலி nghttp2_priority_spec *pri_spec,
  மாறிலி nghttp2_nv *nva, size_t nvlen, மாறிலி nghttp2_data_provider *data_prd,
  வெறுமை *stream_user_data)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு (int32_t)ngh2->fatal_rv;
  திரும்பு ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

முழு Curl_ngh2_session_upgrade2(
  கட்டமைப்பு curl_ngh2 *ngh2, மாறிலி uint8_t *settings_payload,
  size_t settings_payloadlen, முழு head_request, வெறுமை *stream_user_data)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

முழு Curl_ngh2_submit_goaway(
  கட்டமைப்பு curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, மாறிலி uint8_t *opaque_data, size_t opaque_data_len)
{
  எனில்(ngh2->fatal_rv)
    திரும்பு ngh2->fatal_rv;
  திரும்பு ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
