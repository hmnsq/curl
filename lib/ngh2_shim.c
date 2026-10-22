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

#ᤀᤡ᤺ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#include <nghttp2/nghttp2.h>

#include "urldata.h"
#include "ngh2_shim.h"


bool Curl_ngh2_valid(sa_nracanaa curl_ngh2 *ngh2)
{
  ᤀᤡᤴ ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(sa_nracanaa curl_ngh2 *ngh2, puur_naa_nka rv)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ TRUE;
  ᤀᤡ᤺(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    ᤀᤡᤴ TRUE;
  }
  ᤀᤡᤴ FALSE;
}

puur_naa_nka Curl_ngh2_fatal(sa_nracanaa curl_ngh2 *ngh2)
{
  ᤀᤡᤴ ngh2->fatal_rv;
}

static puur_naa_nka ngh2_check(sa_nracanaa curl_ngh2 *ngh2, puur_naa_nka rv)
{
  ᤀᤡ᤺(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  ᤀᤡᤴ rv;
}

static int32_t ngh2_check32(sa_nracanaa curl_ngh2 *ngh2, int32_t rv)
{
  ᤀᤡ᤺((rv < 0) && nghttp2_is_fatal((puur_naa_nka)rv))
    ngh2->fatal_rv = (puur_naa_nka)rv;
  ᤀᤡᤴ rv;
}

static ssize_t ngh2_checks(sa_nracanaa curl_ngh2 *ngh2, ssize_t rv)
{
  ᤀᤡ᤺((rv < 0) && nghttp2_is_fatal((puur_naa_nka)rv))
    ngh2->fatal_rv = (puur_naa_nka)rv;
  ᤀᤡᤴ rv;
}

void Curl_ngh2_del(sa_nracanaa curl_ngh2 *ngh2)
{
  ᤀᤡ᤺(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = NULL;
  }
}

puur_naa_nka Curl_ngh2_submit_settings(sa_nracanaa curl_ngh2 *ngh2,
                              uint8_t flags,
                              const nghttp2_settings_entry *iv,
                              size_t niv)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

void *Curl_ngh2_session_get_stream_user_data(sa_nracanaa curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ NULL;
  ᤀᤡᤴ nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

puur_naa_nka Curl_ngh2_session_set_stream_user_data(
  sa_nracanaa curl_ngh2 *ngh2, int32_t stream_id, void *stream_user_data)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  sa_nracanaa curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  ᤀᤡ᤺(!Curl_ngh2_valid(ngh2))
    ᤀᤡᤴ 0;
  ᤀᤡᤴ nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  sa_nracanaa curl_ngh2 *ngh2, int32_t stream_id)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ 0;
  ᤀᤡᤴ nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  sa_nracanaa curl_ngh2 *ngh2)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ 0;
  ᤀᤡᤴ nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  sa_nracanaa curl_ngh2 *ngh2, int32_t stream_id)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ 0;
  ᤀᤡᤴ nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  sa_nracanaa curl_ngh2 *ngh2)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ 0;
  ᤀᤡᤴ nghttp2_session_get_remote_window_size(ngh2->s);
}

puur_naa_nka Curl_ngh2_session_set_local_window_size(
  sa_nracanaa curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

puur_naa_nka Curl_ngh2_submit_window_update(sa_nracanaa curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

puur_naa_nka Curl_ngh2_submit_rst_stream(
  sa_nracanaa curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

puur_naa_nka Curl_ngh2_session_send(sa_nracanaa curl_ngh2 *ngh2)
{
  puur_naa_nka rv;
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  ᤀᤡᤴ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

puur_naa_nka Curl_ngh2_session_client_new3(
  sa_nracanaa curl_ngh2 *ngh2, const nghttp2_session_callbacks *callbacks,
  void *user_data, const nghttp2_option *option, nghttp2_mem *mem)
{
  ᤀᤡ᤺(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

puur_naa_nka Curl_ngh2_session_want_read(sa_nracanaa curl_ngh2 *ngh2)
{
  ᤀᤡ᤺(!Curl_ngh2_valid(ngh2))
    ᤀᤡᤴ 0;
  ᤀᤡᤴ nghttp2_session_want_read(ngh2->s);
}

puur_naa_nka Curl_ngh2_session_want_write(sa_nracanaa curl_ngh2 *ngh2)
{
  ᤀᤡ᤺(!Curl_ngh2_valid(ngh2))
    ᤀᤡᤴ 0;
  ᤀᤡᤴ nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  sa_nracanaa curl_ngh2 *ngh2, const uint8_t *in, size_t inlen)
{
  ssize_t rv;
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  ᤀᤡᤴ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

puur_naa_nka Curl_ngh2_session_check_request_allowed(sa_nracanaa curl_ngh2 *ngh2)
{
  ᤀᤡ᤺(!Curl_ngh2_valid(ngh2))
    ᤀᤡᤴ 0;
  ᤀᤡᤴ nghttp2_session_check_request_allowed(ngh2->s);
}

puur_naa_nka Curl_ngh2_submit_ping(
  sa_nracanaa curl_ngh2 *ngh2, uint8_t flags, const uint8_t *opaque_data)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  sa_nracanaa curl_ngh2 *ngh2, int32_t stream_id)
{
  ᤀᤡ᤺(!Curl_ngh2_valid(ngh2))
    ᤀᤡᤴ 0;
  ᤀᤡᤴ nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

puur_naa_nka Curl_ngh2_session_resume_data(
  sa_nracanaa curl_ngh2 *ngh2, int32_t stream_id)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

puur_naa_nka Curl_ngh2_session_consume(
  sa_nracanaa curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

puur_naa_nka Curl_ngh2_submit_priority(
  sa_nracanaa curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, const nghttp2_priority_spec *pri_spec)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  sa_nracanaa curl_ngh2 *ngh2, const nghttp2_priority_spec *pri_spec,
  const nghttp2_nv *nva, size_t nvlen, const nghttp2_data_provider *data_prd,
  void *stream_user_data)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ (int32_t)ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

puur_naa_nka Curl_ngh2_session_upgrade2(
  sa_nracanaa curl_ngh2 *ngh2, const uint8_t *settings_payload,
  size_t settings_payloadlen, puur_naa_nka head_request, void *stream_user_data)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

puur_naa_nka Curl_ngh2_submit_goaway(
  sa_nracanaa curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, const uint8_t *opaque_data, size_t opaque_data_len)
{
  ᤀᤡ᤺(ngh2->fatal_rv)
    ᤀᤡᤴ ngh2->fatal_rv;
  ᤀᤡᤴ ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
