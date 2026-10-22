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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#അന്തര്ഭാവ <nghttp2/nghttp2.h>

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "ngh2_shim.h"


bool Curl_ngh2_valid(ഘടന curl_ngh2 *ngh2)
{
  തിരികെ ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(ഘടന curl_ngh2 *ngh2, പൂർണ്ണസംഖ്യ rv)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ TRUE;
  എങ്കിൽ(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

പൂർണ്ണസംഖ്യ Curl_ngh2_fatal(ഘടന curl_ngh2 *ngh2)
{
  തിരികെ ngh2->fatal_rv;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ngh2_check(ഘടന curl_ngh2 *ngh2, പൂർണ്ണസംഖ്യ rv)
{
  എങ്കിൽ(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  തിരികെ rv;
}

സ്ഥിതം int32_t ngh2_check32(ഘടന curl_ngh2 *ngh2, int32_t rv)
{
  എങ്കിൽ((rv < 0) && nghttp2_is_fatal((പൂർണ്ണസംഖ്യ)rv))
    ngh2->fatal_rv = (പൂർണ്ണസംഖ്യ)rv;
  തിരികെ rv;
}

സ്ഥിതം ssize_t ngh2_checks(ഘടന curl_ngh2 *ngh2, ssize_t rv)
{
  എങ്കിൽ((rv < 0) && nghttp2_is_fatal((പൂർണ്ണസംഖ്യ)rv))
    ngh2->fatal_rv = (പൂർണ്ണസംഖ്യ)rv;
  തിരികെ rv;
}

ശൂന്യം Curl_ngh2_del(ഘടന curl_ngh2 *ngh2)
{
  എങ്കിൽ(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = ശൂന്യ;
  }
}

പൂർണ്ണസംഖ്യ Curl_ngh2_submit_settings(ഘടന curl_ngh2 *ngh2,
                              uint8_t flags,
                              സ്ഥിരം nghttp2_settings_entry *iv,
                              size_t niv)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

ശൂന്യം *Curl_ngh2_session_get_stream_user_data(ഘടന curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ശൂന്യ;
  തിരികെ nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

പൂർണ്ണസംഖ്യ Curl_ngh2_session_set_stream_user_data(
  ഘടന curl_ngh2 *ngh2, int32_t stream_id, ശൂന്യം *stream_user_data)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  ഘടന curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  എങ്കിൽ(!Curl_ngh2_valid(ngh2))
    തിരികെ 0;
  തിരികെ nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  ഘടന curl_ngh2 *ngh2, int32_t stream_id)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ 0;
  തിരികെ nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  ഘടന curl_ngh2 *ngh2)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ 0;
  തിരികെ nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  ഘടന curl_ngh2 *ngh2, int32_t stream_id)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ 0;
  തിരികെ nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  ഘടന curl_ngh2 *ngh2)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ 0;
  തിരികെ nghttp2_session_get_remote_window_size(ngh2->s);
}

പൂർണ്ണസംഖ്യ Curl_ngh2_session_set_local_window_size(
  ഘടന curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

പൂർണ്ണസംഖ്യ Curl_ngh2_submit_window_update(ഘടന curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

പൂർണ്ണസംഖ്യ Curl_ngh2_submit_rst_stream(
  ഘടന curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

പൂർണ്ണസംഖ്യ Curl_ngh2_session_send(ഘടന curl_ngh2 *ngh2)
{
  പൂർണ്ണസംഖ്യ rv;
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  തിരികെ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

പൂർണ്ണസംഖ്യ Curl_ngh2_session_client_new3(
  ഘടന curl_ngh2 *ngh2, സ്ഥിരം nghttp2_session_callbacks *callbacks,
  ശൂന്യം *user_data, സ്ഥിരം nghttp2_option *option, nghttp2_mem *mem)
{
  എങ്കിൽ(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

പൂർണ്ണസംഖ്യ Curl_ngh2_session_want_read(ഘടന curl_ngh2 *ngh2)
{
  എങ്കിൽ(!Curl_ngh2_valid(ngh2))
    തിരികെ 0;
  തിരികെ nghttp2_session_want_read(ngh2->s);
}

പൂർണ്ണസംഖ്യ Curl_ngh2_session_want_write(ഘടന curl_ngh2 *ngh2)
{
  എങ്കിൽ(!Curl_ngh2_valid(ngh2))
    തിരികെ 0;
  തിരികെ nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  ഘടന curl_ngh2 *ngh2, സ്ഥിരം uint8_t *in, size_t inlen)
{
  ssize_t rv;
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  തിരികെ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

പൂർണ്ണസംഖ്യ Curl_ngh2_session_check_request_allowed(ഘടന curl_ngh2 *ngh2)
{
  എങ്കിൽ(!Curl_ngh2_valid(ngh2))
    തിരികെ 0;
  തിരികെ nghttp2_session_check_request_allowed(ngh2->s);
}

പൂർണ്ണസംഖ്യ Curl_ngh2_submit_ping(
  ഘടന curl_ngh2 *ngh2, uint8_t flags, സ്ഥിരം uint8_t *opaque_data)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  ഘടന curl_ngh2 *ngh2, int32_t stream_id)
{
  എങ്കിൽ(!Curl_ngh2_valid(ngh2))
    തിരികെ 0;
  തിരികെ nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

പൂർണ്ണസംഖ്യ Curl_ngh2_session_resume_data(
  ഘടന curl_ngh2 *ngh2, int32_t stream_id)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

പൂർണ്ണസംഖ്യ Curl_ngh2_session_consume(
  ഘടന curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

പൂർണ്ണസംഖ്യ Curl_ngh2_submit_priority(
  ഘടന curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, സ്ഥിരം nghttp2_priority_spec *pri_spec)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  ഘടന curl_ngh2 *ngh2, സ്ഥിരം nghttp2_priority_spec *pri_spec,
  സ്ഥിരം nghttp2_nv *nva, size_t nvlen, സ്ഥിരം nghttp2_data_provider *data_prd,
  ശൂന്യം *stream_user_data)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ (int32_t)ngh2->fatal_rv;
  തിരികെ ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

പൂർണ്ണസംഖ്യ Curl_ngh2_session_upgrade2(
  ഘടന curl_ngh2 *ngh2, സ്ഥിരം uint8_t *settings_payload,
  size_t settings_payloadlen, പൂർണ്ണസംഖ്യ head_request, ശൂന്യം *stream_user_data)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

പൂർണ്ണസംഖ്യ Curl_ngh2_submit_goaway(
  ഘടന curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, സ്ഥിരം uint8_t *opaque_data, size_t opaque_data_len)
{
  എങ്കിൽ(ngh2->fatal_rv)
    തിരികെ ngh2->fatal_rv;
  തിരികെ ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
