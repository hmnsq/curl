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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#ଅନ୍ତର୍ଭାଵ <nghttp2/nghttp2.h>

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "ngh2_shim.h"


bool Curl_ngh2_valid(ସଂରଚନା curl_ngh2 *ngh2)
{
  ଫେରନ୍ତୁ ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(ସଂରଚନା curl_ngh2 *ngh2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ TRUE;
  ଯଦି(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_fatal(ସଂରଚନା curl_ngh2 *ngh2)
{
  ଫେରନ୍ତୁ ngh2->fatal_rv;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ngh2_check(ସଂରଚନା curl_ngh2 *ngh2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv)
{
  ଯଦି(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  ଫେରନ୍ତୁ rv;
}

ସ୍ଥିର int32_t ngh2_check32(ସଂରଚନା curl_ngh2 *ngh2, int32_t rv)
{
  ଯଦି((rv < 0) && nghttp2_is_fatal((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rv))
    ngh2->fatal_rv = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rv;
  ଫେରନ୍ତୁ rv;
}

ସ୍ଥିର ssize_t ngh2_checks(ସଂରଚନା curl_ngh2 *ngh2, ssize_t rv)
{
  ଯଦି((rv < 0) && nghttp2_is_fatal((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rv))
    ngh2->fatal_rv = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rv;
  ଫେରନ୍ତୁ rv;
}

ଶୂନ୍ୟ Curl_ngh2_del(ସଂରଚନା curl_ngh2 *ngh2)
{
  ଯଦି(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = ଖାଲି;
  }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_submit_settings(ସଂରଚନା curl_ngh2 *ngh2,
                              uint8_t flags,
                              ଅଚଳ nghttp2_settings_entry *iv,
                              size_t niv)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

ଶୂନ୍ୟ *Curl_ngh2_session_get_stream_user_data(ସଂରଚନା curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ଖାଲି;
  ଫେରନ୍ତୁ nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_session_set_stream_user_data(
  ସଂରଚନା curl_ngh2 *ngh2, int32_t stream_id, ଶୂନ୍ୟ *stream_user_data)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  ସଂରଚନା curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  ଯଦି(!Curl_ngh2_valid(ngh2))
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  ସଂରଚନା curl_ngh2 *ngh2, int32_t stream_id)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  ସଂରଚନା curl_ngh2 *ngh2)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  ସଂରଚନା curl_ngh2 *ngh2, int32_t stream_id)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  ସଂରଚନା curl_ngh2 *ngh2)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ nghttp2_session_get_remote_window_size(ngh2->s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_session_set_local_window_size(
  ସଂରଚନା curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_submit_window_update(ସଂରଚନା curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_submit_rst_stream(
  ସଂରଚନା curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_session_send(ସଂରଚନା curl_ngh2 *ngh2)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  ଫେରନ୍ତୁ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_session_client_new3(
  ସଂରଚନା curl_ngh2 *ngh2, ଅଚଳ nghttp2_session_callbacks *callbacks,
  ଶୂନ୍ୟ *user_data, ଅଚଳ nghttp2_option *option, nghttp2_mem *mem)
{
  ଯଦି(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_session_want_read(ସଂରଚନା curl_ngh2 *ngh2)
{
  ଯଦି(!Curl_ngh2_valid(ngh2))
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ nghttp2_session_want_read(ngh2->s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_session_want_write(ସଂରଚନା curl_ngh2 *ngh2)
{
  ଯଦି(!Curl_ngh2_valid(ngh2))
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  ସଂରଚନା curl_ngh2 *ngh2, ଅଚଳ uint8_t *in, size_t inlen)
{
  ssize_t rv;
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  ଫେରନ୍ତୁ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_session_check_request_allowed(ସଂରଚନା curl_ngh2 *ngh2)
{
  ଯଦି(!Curl_ngh2_valid(ngh2))
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ nghttp2_session_check_request_allowed(ngh2->s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_submit_ping(
  ସଂରଚନା curl_ngh2 *ngh2, uint8_t flags, ଅଚଳ uint8_t *opaque_data)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  ସଂରଚନା curl_ngh2 *ngh2, int32_t stream_id)
{
  ଯଦି(!Curl_ngh2_valid(ngh2))
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_session_resume_data(
  ସଂରଚନା curl_ngh2 *ngh2, int32_t stream_id)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_session_consume(
  ସଂରଚନା curl_ngh2 *ngh2, int32_t stream_id, size_t size)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_submit_priority(
  ସଂରଚନା curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, ଅଚଳ nghttp2_priority_spec *pri_spec)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  ସଂରଚନା curl_ngh2 *ngh2, ଅଚଳ nghttp2_priority_spec *pri_spec,
  ଅଚଳ nghttp2_nv *nva, size_t nvlen, ଅଚଳ nghttp2_data_provider *data_prd,
  ଶୂନ୍ୟ *stream_user_data)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ (int32_t)ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_session_upgrade2(
  ସଂରଚନା curl_ngh2 *ngh2, ଅଚଳ uint8_t *settings_payload,
  size_t settings_payloadlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା head_request, ଶୂନ୍ୟ *stream_user_data)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ngh2_submit_goaway(
  ସଂରଚନା curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, ଅଚଳ uint8_t *opaque_data, size_t opaque_data_len)
{
  ଯଦି(ngh2->fatal_rv)
    ଫେରନ୍ତୁ ngh2->fatal_rv;
  ଫେରନ୍ତୁ ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
