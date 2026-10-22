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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#अन्तर्भाव <nghttp2/nghttp2.h>

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "ngh2_shim.h"


bool Curl_ngh2_valid(संरचना curl_ngh2 *ngh2)
{
  फर्काऊ ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(संरचना curl_ngh2 *ngh2, पूर्णाङ्क rv)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ TRUE;
  यदि(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    फर्काऊ TRUE;
  }
  फर्काऊ FALSE;
}

पूर्णाङ्क Curl_ngh2_fatal(संरचना curl_ngh2 *ngh2)
{
  फर्काऊ ngh2->fatal_rv;
}

स्थिर पूर्णाङ्क ngh2_check(संरचना curl_ngh2 *ngh2, पूर्णाङ्क rv)
{
  यदि(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  फर्काऊ rv;
}

स्थिर int32_t ngh2_check32(संरचना curl_ngh2 *ngh2, int32_t rv)
{
  यदि((rv < 0) && nghttp2_is_fatal((पूर्णाङ्क)rv))
    ngh2->fatal_rv = (पूर्णाङ्क)rv;
  फर्काऊ rv;
}

स्थिर ssize_t ngh2_checks(संरचना curl_ngh2 *ngh2, ssize_t rv)
{
  यदि((rv < 0) && nghttp2_is_fatal((पूर्णाङ्क)rv))
    ngh2->fatal_rv = (पूर्णाङ्क)rv;
  फर्काऊ rv;
}

शून्य Curl_ngh2_del(संरचना curl_ngh2 *ngh2)
{
  यदि(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = रिक्त;
  }
}

पूर्णाङ्क Curl_ngh2_submit_settings(संरचना curl_ngh2 *ngh2,
                              uint8_t flags,
                              अचर nghttp2_settings_entry *iv,
                              माप_प्रकार niv)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

शून्य *Curl_ngh2_session_get_stream_user_data(संरचना curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ रिक्त;
  फर्काऊ nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

पूर्णाङ्क Curl_ngh2_session_set_stream_user_data(
  संरचना curl_ngh2 *ngh2, int32_t stream_id, शून्य *stream_user_data)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  संरचना curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  यदि(!Curl_ngh2_valid(ngh2))
    फर्काऊ 0;
  फर्काऊ nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  संरचना curl_ngh2 *ngh2, int32_t stream_id)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ 0;
  फर्काऊ nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  संरचना curl_ngh2 *ngh2)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ 0;
  फर्काऊ nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  संरचना curl_ngh2 *ngh2, int32_t stream_id)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ 0;
  फर्काऊ nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  संरचना curl_ngh2 *ngh2)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ 0;
  फर्काऊ nghttp2_session_get_remote_window_size(ngh2->s);
}

पूर्णाङ्क Curl_ngh2_session_set_local_window_size(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

पूर्णाङ्क Curl_ngh2_submit_window_update(संरचना curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

पूर्णाङ्क Curl_ngh2_submit_rst_stream(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

पूर्णाङ्क Curl_ngh2_session_send(संरचना curl_ngh2 *ngh2)
{
  पूर्णाङ्क rv;
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  फर्काऊ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

पूर्णाङ्क Curl_ngh2_session_client_new3(
  संरचना curl_ngh2 *ngh2, अचर nghttp2_session_callbacks *callbacks,
  शून्य *user_data, अचर nghttp2_option *option, nghttp2_mem *mem)
{
  यदि(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

पूर्णाङ्क Curl_ngh2_session_want_read(संरचना curl_ngh2 *ngh2)
{
  यदि(!Curl_ngh2_valid(ngh2))
    फर्काऊ 0;
  फर्काऊ nghttp2_session_want_read(ngh2->s);
}

पूर्णाङ्क Curl_ngh2_session_want_write(संरचना curl_ngh2 *ngh2)
{
  यदि(!Curl_ngh2_valid(ngh2))
    फर्काऊ 0;
  फर्काऊ nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  संरचना curl_ngh2 *ngh2, अचर uint8_t *in, माप_प्रकार inlen)
{
  ssize_t rv;
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  फर्काऊ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

पूर्णाङ्क Curl_ngh2_session_check_request_allowed(संरचना curl_ngh2 *ngh2)
{
  यदि(!Curl_ngh2_valid(ngh2))
    फर्काऊ 0;
  फर्काऊ nghttp2_session_check_request_allowed(ngh2->s);
}

पूर्णाङ्क Curl_ngh2_submit_ping(
  संरचना curl_ngh2 *ngh2, uint8_t flags, अचर uint8_t *opaque_data)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  संरचना curl_ngh2 *ngh2, int32_t stream_id)
{
  यदि(!Curl_ngh2_valid(ngh2))
    फर्काऊ 0;
  फर्काऊ nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

पूर्णाङ्क Curl_ngh2_session_resume_data(
  संरचना curl_ngh2 *ngh2, int32_t stream_id)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

पूर्णाङ्क Curl_ngh2_session_consume(
  संरचना curl_ngh2 *ngh2, int32_t stream_id, माप_प्रकार size)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

पूर्णाङ्क Curl_ngh2_submit_priority(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, अचर nghttp2_priority_spec *pri_spec)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  संरचना curl_ngh2 *ngh2, अचर nghttp2_priority_spec *pri_spec,
  अचर nghttp2_nv *nva, माप_प्रकार nvlen, अचर nghttp2_data_provider *data_prd,
  शून्य *stream_user_data)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ (int32_t)ngh2->fatal_rv;
  फर्काऊ ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

पूर्णाङ्क Curl_ngh2_session_upgrade2(
  संरचना curl_ngh2 *ngh2, अचर uint8_t *settings_payload,
  माप_प्रकार settings_payloadlen, पूर्णाङ्क head_request, शून्य *stream_user_data)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

पूर्णाङ्क Curl_ngh2_submit_goaway(
  संरचना curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, अचर uint8_t *opaque_data, माप_प्रकार opaque_data_len)
{
  यदि(ngh2->fatal_rv)
    फर्काऊ ngh2->fatal_rv;
  फर्काऊ ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
