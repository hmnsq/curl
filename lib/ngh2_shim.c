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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#ضمن <nghttp2/nghttp2.h>

#ضمن "urldata.h"
#ضمن "ngh2_shim.h"


bool Curl_ngh2_valid(بنية curl_ngh2 *ngh2)
{
  أرجع ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(بنية curl_ngh2 *ngh2, صحيح rv)
{
  إذا(ngh2->fatal_rv)
    أرجع TRUE;
  إذا(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    أرجع TRUE;
  }
  أرجع FALSE;
}

صحيح Curl_ngh2_fatal(بنية curl_ngh2 *ngh2)
{
  أرجع ngh2->fatal_rv;
}

ساكن صحيح ngh2_check(بنية curl_ngh2 *ngh2, صحيح rv)
{
  إذا(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  أرجع rv;
}

ساكن int32_t ngh2_check32(بنية curl_ngh2 *ngh2, int32_t rv)
{
  إذا((rv < 0) && nghttp2_is_fatal((صحيح)rv))
    ngh2->fatal_rv = (صحيح)rv;
  أرجع rv;
}

ساكن ssize_t ngh2_checks(بنية curl_ngh2 *ngh2, ssize_t rv)
{
  إذا((rv < 0) && nghttp2_is_fatal((صحيح)rv))
    ngh2->fatal_rv = (صحيح)rv;
  أرجع rv;
}

فراغ Curl_ngh2_del(بنية curl_ngh2 *ngh2)
{
  إذا(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = عدم;
  }
}

صحيح Curl_ngh2_submit_settings(بنية curl_ngh2 *ngh2,
                              uint8_t flags,
                              ثابت nghttp2_settings_entry *iv,
                              size_t niv)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

فراغ *Curl_ngh2_session_get_stream_user_data(بنية curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  إذا(ngh2->fatal_rv)
    أرجع عدم;
  أرجع nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

صحيح Curl_ngh2_session_set_stream_user_data(
  بنية curl_ngh2 *ngh2, int32_t stream_id, فراغ *stream_user_data)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  بنية curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  بنية curl_ngh2 *ngh2, int32_t stream_id)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  بنية curl_ngh2 *ngh2)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  بنية curl_ngh2 *ngh2, int32_t stream_id)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  بنية curl_ngh2 *ngh2)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_remote_window_size(ngh2->s);
}

صحيح Curl_ngh2_session_set_local_window_size(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

صحيح Curl_ngh2_submit_window_update(بنية curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

صحيح Curl_ngh2_submit_rst_stream(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

صحيح Curl_ngh2_session_send(بنية curl_ngh2 *ngh2)
{
  صحيح rv;
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  أرجع ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

صحيح Curl_ngh2_session_client_new3(
  بنية curl_ngh2 *ngh2, ثابت nghttp2_session_callbacks *callbacks,
  فراغ *user_data, ثابت nghttp2_option *option, nghttp2_mem *mem)
{
  إذا(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

صحيح Curl_ngh2_session_want_read(بنية curl_ngh2 *ngh2)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_want_read(ngh2->s);
}

صحيح Curl_ngh2_session_want_write(بنية curl_ngh2 *ngh2)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  بنية curl_ngh2 *ngh2, ثابت uint8_t *in, size_t inlen)
{
  ssize_t rv;
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  أرجع ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

صحيح Curl_ngh2_session_check_request_allowed(بنية curl_ngh2 *ngh2)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_check_request_allowed(ngh2->s);
}

صحيح Curl_ngh2_submit_ping(
  بنية curl_ngh2 *ngh2, uint8_t flags, ثابت uint8_t *opaque_data)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  بنية curl_ngh2 *ngh2, int32_t stream_id)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

صحيح Curl_ngh2_session_resume_data(
  بنية curl_ngh2 *ngh2, int32_t stream_id)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

صحيح Curl_ngh2_session_consume(
  بنية curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

صحيح Curl_ngh2_submit_priority(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, ثابت nghttp2_priority_spec *pri_spec)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  بنية curl_ngh2 *ngh2, ثابت nghttp2_priority_spec *pri_spec,
  ثابت nghttp2_nv *nva, size_t nvlen, ثابت nghttp2_data_provider *data_prd,
  فراغ *stream_user_data)
{
  إذا(ngh2->fatal_rv)
    أرجع (int32_t)ngh2->fatal_rv;
  أرجع ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

صحيح Curl_ngh2_session_upgrade2(
  بنية curl_ngh2 *ngh2, ثابت uint8_t *settings_payload,
  size_t settings_payloadlen, صحيح head_request, فراغ *stream_user_data)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

صحيح Curl_ngh2_submit_goaway(
  بنية curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, ثابت uint8_t *opaque_data, size_t opaque_data_len)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
