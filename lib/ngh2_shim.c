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

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#ಅನ್ತರ್ಭಾವ <nghttp2/nghttp2.h>

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "ngh2_shim.h"


bool Curl_ngh2_valid(ಸಂರಚನಾ curl_ngh2 *ngh2)
{
  ಪ್ರತ್ಯಾಗಮ ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(ಸಂರಚನಾ curl_ngh2 *ngh2, ಪೂರ್ಣಾಙ್ಕ rv)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಯದಿ(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_fatal(ಸಂರಚನಾ curl_ngh2 *ngh2)
{
  ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ngh2_check(ಸಂರಚನಾ curl_ngh2 *ngh2, ಪೂರ್ಣಾಙ್ಕ rv)
{
  ಯದಿ(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  ಪ್ರತ್ಯಾಗಮ rv;
}

ಸ್ಥಿರ int32_t ngh2_check32(ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t rv)
{
  ಯದಿ((rv < 0) && nghttp2_is_fatal((ಪೂರ್ಣಾಙ್ಕ)rv))
    ngh2->fatal_rv = (ಪೂರ್ಣಾಙ್ಕ)rv;
  ಪ್ರತ್ಯಾಗಮ rv;
}

ಸ್ಥಿರ ssize_t ngh2_checks(ಸಂರಚನಾ curl_ngh2 *ngh2, ssize_t rv)
{
  ಯದಿ((rv < 0) && nghttp2_is_fatal((ಪೂರ್ಣಾಙ್ಕ)rv))
    ngh2->fatal_rv = (ಪೂರ್ಣಾಙ್ಕ)rv;
  ಪ್ರತ್ಯಾಗಮ rv;
}

ಶೂನ್ಯ Curl_ngh2_del(ಸಂರಚನಾ curl_ngh2 *ngh2)
{
  ಯದಿ(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = ರಿಕ್ತ;
  }
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_settings(ಸಂರಚನಾ curl_ngh2 *ngh2,
                              uint8_t flags,
                              ಧ್ರುವ nghttp2_settings_entry *iv,
                              size_t niv)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

ಶೂನ್ಯ *Curl_ngh2_session_get_stream_user_data(ಸಂರಚನಾ curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_set_stream_user_data(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id, ಶೂನ್ಯ *stream_user_data)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  ಸಂರಚನಾ curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  ಯದಿ(!Curl_ngh2_valid(ngh2))
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  ಸಂರಚನಾ curl_ngh2 *ngh2)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  ಸಂರಚನಾ curl_ngh2 *ngh2)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ nghttp2_session_get_remote_window_size(ngh2->s);
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_set_local_window_size(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_window_update(ಸಂರಚನಾ curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_rst_stream(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_send(ಸಂರಚನಾ curl_ngh2 *ngh2)
{
  ಪೂರ್ಣಾಙ್ಕ rv;
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_client_new3(
  ಸಂರಚನಾ curl_ngh2 *ngh2, ಧ್ರುವ nghttp2_session_callbacks *callbacks,
  ಶೂನ್ಯ *user_data, ಧ್ರುವ nghttp2_option *option, nghttp2_mem *mem)
{
  ಯದಿ(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_want_read(ಸಂರಚನಾ curl_ngh2 *ngh2)
{
  ಯದಿ(!Curl_ngh2_valid(ngh2))
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ nghttp2_session_want_read(ngh2->s);
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_want_write(ಸಂರಚನಾ curl_ngh2 *ngh2)
{
  ಯದಿ(!Curl_ngh2_valid(ngh2))
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  ಸಂರಚನಾ curl_ngh2 *ngh2, ಧ್ರುವ uint8_t *in, size_t inlen)
{
  ssize_t rv;
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_check_request_allowed(ಸಂರಚನಾ curl_ngh2 *ngh2)
{
  ಯದಿ(!Curl_ngh2_valid(ngh2))
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ nghttp2_session_check_request_allowed(ngh2->s);
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_ping(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags, ಧ್ರುವ uint8_t *opaque_data)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id)
{
  ಯದಿ(!Curl_ngh2_valid(ngh2))
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_resume_data(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_consume(
  ಸಂರಚನಾ curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_priority(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, ಧ್ರುವ nghttp2_priority_spec *pri_spec)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  ಸಂರಚನಾ curl_ngh2 *ngh2, ಧ್ರುವ nghttp2_priority_spec *pri_spec,
  ಧ್ರುವ nghttp2_nv *nva, size_t nvlen, ಧ್ರುವ nghttp2_data_provider *data_prd,
  ಶೂನ್ಯ *stream_user_data)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ (int32_t)ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_session_upgrade2(
  ಸಂರಚನಾ curl_ngh2 *ngh2, ಧ್ರುವ uint8_t *settings_payload,
  size_t settings_payloadlen, ಪೂರ್ಣಾಙ್ಕ head_request, ಶೂನ್ಯ *stream_user_data)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

ಪೂರ್ಣಾಙ್ಕ Curl_ngh2_submit_goaway(
  ಸಂರಚನಾ curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, ಧ್ರುವ uint8_t *opaque_data, size_t opaque_data_len)
{
  ಯದಿ(ngh2->fatal_rv)
    ಪ್ರತ್ಯಾಗಮ ngh2->fatal_rv;
  ಪ್ರತ್ಯಾಗಮ ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
