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

#如果 !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#include <nghttp2/nghttp2.h>

#include "urldata.h"
#include "ngh2_shim.h"


bool Curl_ngh2_valid(struct curl_ngh2 *ngh2)
{
  返回 ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(struct curl_ngh2 *ngh2, 整数 rv)
{
  如果(ngh2->fatal_rv)
    返回 TRUE;
  如果(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    返回 TRUE;
  }
  返回 FALSE;
}

整数 Curl_ngh2_fatal(struct curl_ngh2 *ngh2)
{
  返回 ngh2->fatal_rv;
}

static 整数 ngh2_check(struct curl_ngh2 *ngh2, 整数 rv)
{
  如果(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  返回 rv;
}

static int32_t ngh2_check32(struct curl_ngh2 *ngh2, int32_t rv)
{
  如果((rv < 0) && nghttp2_is_fatal((整数)rv))
    ngh2->fatal_rv = (整数)rv;
  返回 rv;
}

static ssize_t ngh2_checks(struct curl_ngh2 *ngh2, ssize_t rv)
{
  如果((rv < 0) && nghttp2_is_fatal((整数)rv))
    ngh2->fatal_rv = (整数)rv;
  返回 rv;
}

void Curl_ngh2_del(struct curl_ngh2 *ngh2)
{
  如果(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = NULL;
  }
}

整数 Curl_ngh2_submit_settings(struct curl_ngh2 *ngh2,
                              uint8_t flags,
                              const nghttp2_settings_entry *iv,
                              size_t niv)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

void *Curl_ngh2_session_get_stream_user_data(struct curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  如果(ngh2->fatal_rv)
    返回 NULL;
  返回 nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

整数 Curl_ngh2_session_set_stream_user_data(
  struct curl_ngh2 *ngh2, int32_t stream_id, void *stream_user_data)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  struct curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  如果(!Curl_ngh2_valid(ngh2))
    返回 0;
  返回 nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  struct curl_ngh2 *ngh2, int32_t stream_id)
{
  如果(ngh2->fatal_rv)
    返回 0;
  返回 nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  struct curl_ngh2 *ngh2)
{
  如果(ngh2->fatal_rv)
    返回 0;
  返回 nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  struct curl_ngh2 *ngh2, int32_t stream_id)
{
  如果(ngh2->fatal_rv)
    返回 0;
  返回 nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  struct curl_ngh2 *ngh2)
{
  如果(ngh2->fatal_rv)
    返回 0;
  返回 nghttp2_session_get_remote_window_size(ngh2->s);
}

整数 Curl_ngh2_session_set_local_window_size(
  struct curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

整数 Curl_ngh2_submit_window_update(struct curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

整数 Curl_ngh2_submit_rst_stream(
  struct curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

整数 Curl_ngh2_session_send(struct curl_ngh2 *ngh2)
{
  整数 rv;
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  返回 ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

整数 Curl_ngh2_session_client_new3(
  struct curl_ngh2 *ngh2, const nghttp2_session_callbacks *callbacks,
  void *user_data, const nghttp2_option *option, nghttp2_mem *mem)
{
  如果(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

整数 Curl_ngh2_session_want_read(struct curl_ngh2 *ngh2)
{
  如果(!Curl_ngh2_valid(ngh2))
    返回 0;
  返回 nghttp2_session_want_read(ngh2->s);
}

整数 Curl_ngh2_session_want_write(struct curl_ngh2 *ngh2)
{
  如果(!Curl_ngh2_valid(ngh2))
    返回 0;
  返回 nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  struct curl_ngh2 *ngh2, const uint8_t *in, size_t inlen)
{
  ssize_t rv;
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  返回 ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

整数 Curl_ngh2_session_check_request_allowed(struct curl_ngh2 *ngh2)
{
  如果(!Curl_ngh2_valid(ngh2))
    返回 0;
  返回 nghttp2_session_check_request_allowed(ngh2->s);
}

整数 Curl_ngh2_submit_ping(
  struct curl_ngh2 *ngh2, uint8_t flags, const uint8_t *opaque_data)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  struct curl_ngh2 *ngh2, int32_t stream_id)
{
  如果(!Curl_ngh2_valid(ngh2))
    返回 0;
  返回 nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

整数 Curl_ngh2_session_resume_data(
  struct curl_ngh2 *ngh2, int32_t stream_id)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

整数 Curl_ngh2_session_consume(
  struct curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

整数 Curl_ngh2_submit_priority(
  struct curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, const nghttp2_priority_spec *pri_spec)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  struct curl_ngh2 *ngh2, const nghttp2_priority_spec *pri_spec,
  const nghttp2_nv *nva, size_t nvlen, const nghttp2_data_provider *data_prd,
  void *stream_user_data)
{
  如果(ngh2->fatal_rv)
    返回 (int32_t)ngh2->fatal_rv;
  返回 ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

整数 Curl_ngh2_session_upgrade2(
  struct curl_ngh2 *ngh2, const uint8_t *settings_payload,
  size_t settings_payloadlen, 整数 head_request, void *stream_user_data)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

整数 Curl_ngh2_submit_goaway(
  struct curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, const uint8_t *opaque_data, size_t opaque_data_len)
{
  如果(ngh2->fatal_rv)
    返回 ngh2->fatal_rv;
  返回 ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
