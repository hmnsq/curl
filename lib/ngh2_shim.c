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

#जर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#अन्तर्भाव <nghttp2/nghttp2.h>

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "ngh2_shim.h"


bool Curl_ngh2_valid(संरचना curl_ngh2 *ngh2)
{
  परत ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(संरचना curl_ngh2 *ngh2, पूर्णांक rv)
{
  जर(ngh2->fatal_rv)
    परत TRUE;
  जर(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    परत TRUE;
  }
  परत FALSE;
}

पूर्णांक Curl_ngh2_fatal(संरचना curl_ngh2 *ngh2)
{
  परत ngh2->fatal_rv;
}

स्थिर पूर्णांक ngh2_check(संरचना curl_ngh2 *ngh2, पूर्णांक rv)
{
  जर(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  परत rv;
}

स्थिर int32_t ngh2_check32(संरचना curl_ngh2 *ngh2, int32_t rv)
{
  जर((rv < 0) && nghttp2_is_fatal((पूर्णांक)rv))
    ngh2->fatal_rv = (पूर्णांक)rv;
  परत rv;
}

स्थिर ssize_t ngh2_checks(संरचना curl_ngh2 *ngh2, ssize_t rv)
{
  जर((rv < 0) && nghttp2_is_fatal((पूर्णांक)rv))
    ngh2->fatal_rv = (पूर्णांक)rv;
  परत rv;
}

शून्य Curl_ngh2_del(संरचना curl_ngh2 *ngh2)
{
  जर(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = रिक्त;
  }
}

पूर्णांक Curl_ngh2_submit_settings(संरचना curl_ngh2 *ngh2,
                              uint8_t flags,
                              स्थिरांक nghttp2_settings_entry *iv,
                              माप_प्रकार niv)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

शून्य *Curl_ngh2_session_get_stream_user_data(संरचना curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  जर(ngh2->fatal_rv)
    परत रिक्त;
  परत nghttp2_session_get_stream_user_data(ngh2->s, stream_id);
}

पूर्णांक Curl_ngh2_session_set_stream_user_data(
  संरचना curl_ngh2 *ngh2, int32_t stream_id, शून्य *stream_user_data)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2,
    nghttp2_session_set_stream_user_data(
      ngh2->s, stream_id, stream_user_data));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  संरचना curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  जर(!Curl_ngh2_valid(ngh2))
    परत 0;
  परत nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_size(
  संरचना curl_ngh2 *ngh2, int32_t stream_id)
{
  जर(ngh2->fatal_rv)
    परत 0;
  परत nghttp2_session_get_stream_effective_local_window_size(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_size(
  संरचना curl_ngh2 *ngh2)
{
  जर(ngh2->fatal_rv)
    परत 0;
  परत nghttp2_session_get_local_window_size(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_size(
  संरचना curl_ngh2 *ngh2, int32_t stream_id)
{
  जर(ngh2->fatal_rv)
    परत 0;
  परत nghttp2_session_get_stream_remote_window_size(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_size(
  संरचना curl_ngh2 *ngh2)
{
  जर(ngh2->fatal_rv)
    परत 0;
  परत nghttp2_session_get_remote_window_size(ngh2->s);
}

पूर्णांक Curl_ngh2_session_set_local_window_size(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_size)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2, nghttp2_session_set_local_window_size(
    ngh2->s, flags, stream_id, window_size));
}

पूर्णांक Curl_ngh2_submit_window_update(संरचना curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_size_increment)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_size_increment));
}

पूर्णांक Curl_ngh2_submit_rst_stream(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

पूर्णांक Curl_ngh2_session_send(संरचना curl_ngh2 *ngh2)
{
  पूर्णांक rv;
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  परत ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

पूर्णांक Curl_ngh2_session_client_new3(
  संरचना curl_ngh2 *ngh2, स्थिरांक nghttp2_session_callbacks *callbacks,
  शून्य *user_data, स्थिरांक nghttp2_option *option, nghttp2_mem *mem)
{
  जर(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_data, option, mem));
}

पूर्णांक Curl_ngh2_session_want_read(संरचना curl_ngh2 *ngh2)
{
  जर(!Curl_ngh2_valid(ngh2))
    परत 0;
  परत nghttp2_session_want_read(ngh2->s);
}

पूर्णांक Curl_ngh2_session_want_write(संरचना curl_ngh2 *ngh2)
{
  जर(!Curl_ngh2_valid(ngh2))
    परत 0;
  परत nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  संरचना curl_ngh2 *ngh2, स्थिरांक uint8_t *in, माप_प्रकार inlen)
{
  ssize_t rv;
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  परत ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

पूर्णांक Curl_ngh2_session_check_request_allowed(संरचना curl_ngh2 *ngh2)
{
  जर(!Curl_ngh2_valid(ngh2))
    परत 0;
  परत nghttp2_session_check_request_allowed(ngh2->s);
}

पूर्णांक Curl_ngh2_submit_ping(
  संरचना curl_ngh2 *ngh2, uint8_t flags, स्थिरांक uint8_t *opaque_data)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_data));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_data_length(
  संरचना curl_ngh2 *ngh2, int32_t stream_id)
{
  जर(!Curl_ngh2_valid(ngh2))
    परत 0;
  परत nghttp2_session_get_stream_effective_recv_data_length(
    ngh2->s, stream_id);
}

पूर्णांक Curl_ngh2_session_resume_data(
  संरचना curl_ngh2 *ngh2, int32_t stream_id)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2,
    nghttp2_session_resume_data(ngh2->s, stream_id));
}

पूर्णांक Curl_ngh2_session_consume(
  संरचना curl_ngh2 *ngh2, int32_t stream_id, माप_प्रकार size)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, size));
}

पूर्णांक Curl_ngh2_submit_priority(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, स्थिरांक nghttp2_priority_spec *pri_spec)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  संरचना curl_ngh2 *ngh2, स्थिरांक nghttp2_priority_spec *pri_spec,
  स्थिरांक nghttp2_nv *nva, माप_प्रकार nvlen, स्थिरांक nghttp2_data_provider *data_prd,
  शून्य *stream_user_data)
{
  जर(ngh2->fatal_rv)
    परत (int32_t)ngh2->fatal_rv;
  परत ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      data_prd, stream_user_data));
}

पूर्णांक Curl_ngh2_session_upgrade2(
  संरचना curl_ngh2 *ngh2, स्थिरांक uint8_t *settings_payload,
  माप_प्रकार settings_payloadlen, पूर्णांक head_request, शून्य *stream_user_data)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, head_request, stream_user_data));
}

पूर्णांक Curl_ngh2_submit_goaway(
  संरचना curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, स्थिरांक uint8_t *opaque_data, माप_प्रकार opaque_data_len)
{
  जर(ngh2->fatal_rv)
    परत ngh2->fatal_rv;
  परत ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_data, opaque_data_len));
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
