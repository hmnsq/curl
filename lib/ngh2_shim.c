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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#અન્તર્ભાવ <nghttp2/nghttp2.h>

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "ngh2_shim.h"


bool Curl_ngh2_valid(સંરચના curl_ngh2 *ngh2)
{
  પાછા ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(સંરચના curl_ngh2 *ngh2, પૂર્ણાંક rv)
{
  જો(ngh2->fatal_rv)
    પાછા TRUE;
  જો(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    પાછા TRUE;
  }
  પાછા FALSE;
}

પૂર્ણાંક Curl_ngh2_fatal(સંરચના curl_ngh2 *ngh2)
{
  પાછા ngh2->fatal_rv;
}

સ્થિર પૂર્ણાંક ngh2_check(સંરચના curl_ngh2 *ngh2, પૂર્ણાંક rv)
{
  જો(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  પાછા rv;
}

સ્થિર int32_t ngh2_check32(સંરચના curl_ngh2 *ngh2, int32_t rv)
{
  જો((rv < 0) && nghttp2_is_fatal((પૂર્ણાંક)rv))
    ngh2->fatal_rv = (પૂર્ણાંક)rv;
  પાછા rv;
}

સ્થિર ssize_t ngh2_checks(સંરચના curl_ngh2 *ngh2, ssize_t rv)
{
  જો((rv < 0) && nghttp2_is_fatal((પૂર્ણાંક)rv))
    ngh2->fatal_rv = (પૂર્ણાંક)rv;
  પાછા rv;
}

શૂન્ય Curl_ngh2_del(સંરચના curl_ngh2 *ngh2)
{
  જો(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = ખાલી;
  }
}

પૂર્ણાંક Curl_ngh2_submit_settings(સંરચના curl_ngh2 *ngh2,
                              uint8_t flags,
                              અચલ nghttp2_settings_entry *iv,
                              size_t niv)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

શૂન્ય *Curl_ngh2_session_get_stream_user_data(સંરચના curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  જો(ngh2->fatal_rv)
    પાછા ખાલી;
  પાછા nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

પૂર્ણાંક Curl_ngh2_session_set_stream_user_data(
  સંરચના curl_ngh2 *ngh2, int32_t stream_id, શૂન્ય *stream_user_data)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  સંરચના curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  જો(!Curl_ngh2_valid(ngh2))
    પાછા 0;
  પાછા nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  સંરચના curl_ngh2 *ngh2, int32_t stream_id)
{
  જો(ngh2->fatal_rv)
    પાછા 0;
  પાછા nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  સંરચના curl_ngh2 *ngh2)
{
  જો(ngh2->fatal_rv)
    પાછા 0;
  પાછા nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  સંરચના curl_ngh2 *ngh2, int32_t stream_id)
{
  જો(ngh2->fatal_rv)
    પાછા 0;
  પાછા nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  સંરચના curl_ngh2 *ngh2)
{
  જો(ngh2->fatal_rv)
    પાછા 0;
  પાછા nghttp2_session_get_remote_window_size(ngh2->s);
}

પૂર્ણાંક Curl_ngh2_session_set_local_window_size(
  સંરચના curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

પૂર્ણાંક Curl_ngh2_submit_window_update(સંરચના curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

પૂર્ણાંક Curl_ngh2_submit_rst_stream(
  સંરચના curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

પૂર્ણાંક Curl_ngh2_session_send(સંરચના curl_ngh2 *ngh2)
{
  પૂર્ણાંક rv;
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  પાછા ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

પૂર્ણાંક Curl_ngh2_session_client_new3(
  સંરચના curl_ngh2 *ngh2, અચલ nghttp2_session_callbacks *callbacks,
  શૂન્ય *user_data, અચલ nghttp2_option *option, nghttp2_mem *mem)
{
  જો(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

પૂર્ણાંક Curl_ngh2_session_want_read(સંરચના curl_ngh2 *ngh2)
{
  જો(!Curl_ngh2_valid(ngh2))
    પાછા 0;
  પાછા nghttp2_session_want_read(ngh2->s);
}

પૂર્ણાંક Curl_ngh2_session_want_write(સંરચના curl_ngh2 *ngh2)
{
  જો(!Curl_ngh2_valid(ngh2))
    પાછા 0;
  પાછા nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  સંરચના curl_ngh2 *ngh2, અચલ uint8_t *in, size_t inlen)
{
  ssize_t rv;
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  પાછા ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

પૂર્ણાંક Curl_ngh2_session_check_request_allowed(સંરચના curl_ngh2 *ngh2)
{
  જો(!Curl_ngh2_valid(ngh2))
    પાછા 0;
  પાછા nghttp2_session_check_request_allowed(ngh2->s);
}

પૂર્ણાંક Curl_ngh2_submit_ping(
  સંરચના curl_ngh2 *ngh2, uint8_t flags, અચલ uint8_t *opaque_data)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  સંરચના curl_ngh2 *ngh2, int32_t stream_id)
{
  જો(!Curl_ngh2_valid(ngh2))
    પાછા 0;
  પાછા nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

પૂર્ણાંક Curl_ngh2_session_resume_data(
  સંરચના curl_ngh2 *ngh2, int32_t stream_id)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

પૂર્ણાંક Curl_ngh2_session_consume(
  સંરચના curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

પૂર્ણાંક Curl_ngh2_submit_priority(
  સંરચના curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, અચલ nghttp2_priority_spec *pri_spec)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  સંરચના curl_ngh2 *ngh2, અચલ nghttp2_priority_spec *pri_spec,
  અચલ nghttp2_nv *nva, size_t nvlen, અચલ nghttp2_data_provider *data_prd,
  શૂન્ય *stream_user_data)
{
  જો(ngh2->fatal_rv)
    પાછા (int32_t)ngh2->fatal_rv;
  પાછા ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

પૂર્ણાંક Curl_ngh2_session_upgrade2(
  સંરચના curl_ngh2 *ngh2, અચલ uint8_t *settings_payload,
  size_t settings_payloadlen, પૂર્ણાંક head_request, શૂન્ય *stream_user_data)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

પૂર્ણાંક Curl_ngh2_submit_goaway(
  સંરચના curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, અચલ uint8_t *opaque_data, size_t opaque_data_len)
{
  જો(ngh2->fatal_rv)
    પાછા ngh2->fatal_rv;
  પાછા ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
