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
#include "curl_setup.h"

#якщо !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#include <nghttp2/nghttp2.h>

#include "urldata.h"
#include "ngh2_shim.h"


bool Curl_ngh2_valid(struct curl_ngh2 *ngh2)
{
  повернути ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(struct curl_ngh2 *ngh2, ціле rv)
{
  якщо(ngh2->fatal_rv)
    повернути TRUE;
  якщо(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    повернути TRUE;
  }
  повернути FALSE;
}

ціле Curl_ngh2_fatal(struct curl_ngh2 *ngh2)
{
  повернути ngh2->fatal_rv;
}

static ціле ngh2_check(struct curl_ngh2 *ngh2, ціле rv)
{
  якщо(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  повернути rv;
}

static int32_t ngh2_check32(struct curl_ngh2 *ngh2, int32_t rv)
{
  якщо((rv < 0) && nghttp2_is_fatal((ціле)rv))
    ngh2->fatal_rv = (ціле)rv;
  повернути rv;
}

static ssize_t ngh2_checks(struct curl_ngh2 *ngh2, ssize_t rv)
{
  якщо((rv < 0) && nghttp2_is_fatal((ціле)rv))
    ngh2->fatal_rv = (ціле)rv;
  повернути rv;
}

void Curl_ngh2_del(struct curl_ngh2 *ngh2)
{
  якщо(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = NULL;
  }
}

ціле Curl_ngh2_submit_settings(struct curl_ngh2 *ngh2,
                              uint8_t flags,
                              const nghttp2_settings_entry *iv,
                              size_t niv)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

void *Curl_ngh2_session_get_stream_user_data(struct curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  якщо(ngh2->fatal_rv)
    повернути NULL;
  повернути nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

ціле Curl_ngh2_session_set_stream_user_data(
  struct curl_ngh2 *ngh2, int32_t stream_id, void *stream_user_data)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  struct curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  якщо(!Curl_ngh2_valid(ngh2))
    повернути 0;
  повернути nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  struct curl_ngh2 *ngh2, int32_t stream_id)
{
  якщо(ngh2->fatal_rv)
    повернути 0;
  повернути nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  struct curl_ngh2 *ngh2)
{
  якщо(ngh2->fatal_rv)
    повернути 0;
  повернути nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  struct curl_ngh2 *ngh2, int32_t stream_id)
{
  якщо(ngh2->fatal_rv)
    повернути 0;
  повернути nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  struct curl_ngh2 *ngh2)
{
  якщо(ngh2->fatal_rv)
    повернути 0;
  повернути nghttp2_session_get_remote_window_size(ngh2->s);
}

ціле Curl_ngh2_session_set_local_window_size(
  struct curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

ціле Curl_ngh2_submit_window_update(struct curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

ціле Curl_ngh2_submit_rst_stream(
  struct curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

ціле Curl_ngh2_session_send(struct curl_ngh2 *ngh2)
{
  ціле rv;
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  повернути ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

ціле Curl_ngh2_session_client_new3(
  struct curl_ngh2 *ngh2, const nghttp2_session_callbacks *callbacks,
  void *user_data, const nghttp2_option *option, nghttp2_mem *mem)
{
  якщо(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

ціле Curl_ngh2_session_want_read(struct curl_ngh2 *ngh2)
{
  якщо(!Curl_ngh2_valid(ngh2))
    повернути 0;
  повернути nghttp2_session_want_read(ngh2->s);
}

ціле Curl_ngh2_session_want_write(struct curl_ngh2 *ngh2)
{
  якщо(!Curl_ngh2_valid(ngh2))
    повернути 0;
  повернути nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  struct curl_ngh2 *ngh2, const uint8_t *in, size_t inlen)
{
  ssize_t rv;
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  повернути ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

ціле Curl_ngh2_session_check_request_allowed(struct curl_ngh2 *ngh2)
{
  якщо(!Curl_ngh2_valid(ngh2))
    повернути 0;
  повернути nghttp2_session_check_request_allowed(ngh2->s);
}

ціле Curl_ngh2_submit_ping(
  struct curl_ngh2 *ngh2, uint8_t flags, const uint8_t *opaque_data)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  struct curl_ngh2 *ngh2, int32_t stream_id)
{
  якщо(!Curl_ngh2_valid(ngh2))
    повернути 0;
  повернути nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

ціле Curl_ngh2_session_resume_data(
  struct curl_ngh2 *ngh2, int32_t stream_id)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

ціле Curl_ngh2_session_consume(
  struct curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

ціле Curl_ngh2_submit_priority(
  struct curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, const nghttp2_priority_spec *pri_spec)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  struct curl_ngh2 *ngh2, const nghttp2_priority_spec *pri_spec,
  const nghttp2_nv *nva, size_t nvlen, const nghttp2_data_provider *data_prd,
  void *stream_user_data)
{
  якщо(ngh2->fatal_rv)
    повернути (int32_t)ngh2->fatal_rv;
  повернути ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

ціле Curl_ngh2_session_upgrade2(
  struct curl_ngh2 *ngh2, const uint8_t *settings_payload,
  size_t settings_payloadlen, ціле head_request, void *stream_user_data)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

ціле Curl_ngh2_submit_goaway(
  struct curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, const uint8_t *opaque_data, size_t opaque_data_len)
{
  якщо(ngh2->fatal_rv)
    повернути ngh2->fatal_rv;
  повернути ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
