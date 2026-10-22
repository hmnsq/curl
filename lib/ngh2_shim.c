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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#समावेश <nghttp2/nghttp2.h>

#समावेश "urldata.h"
#समावेश "ngh2_shim.h"


bool Curl_ngh2_valid(काष्ठा curl_ngh2 *ngh2)
{
  वापस ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(काष्ठा curl_ngh2 *ngh2, पूर्णांक rv)
{
  अगर(ngh2->fatal_rv)
    वापस TRUE;
  अगर(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    वापस TRUE;
  }
  वापस FALSE;
}

पूर्णांक Curl_ngh2_fatal(काष्ठा curl_ngh2 *ngh2)
{
  वापस ngh2->fatal_rv;
}

अटल पूर्णांक ngh2_check(काष्ठा curl_ngh2 *ngh2, पूर्णांक rv)
{
  अगर(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  वापस rv;
}

अटल int32_t ngh2_check32(काष्ठा curl_ngh2 *ngh2, int32_t rv)
{
  अगर((rv < 0) && nghttp2_is_fatal((पूर्णांक)rv))
    ngh2->fatal_rv = (पूर्णांक)rv;
  वापस rv;
}

अटल ssize_t ngh2_checks(काष्ठा curl_ngh2 *ngh2, ssize_t rv)
{
  अगर((rv < 0) && nghttp2_is_fatal((पूर्णांक)rv))
    ngh2->fatal_rv = (पूर्णांक)rv;
  वापस rv;
}

व्योम Curl_ngh2_del(काष्ठा curl_ngh2 *ngh2)
{
  अगर(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = शून्य;
  }
}

पूर्णांक Curl_ngh2_submit_settings(काष्ठा curl_ngh2 *ngh2,
                              uint8_t flags,
                              स्थिर nghttp2_settings_entry *iv,
                              माप_प्रकार niv)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

व्योम *Curl_ngh2_session_get_stream_user_तथ्य(काष्ठा curl_ngh2 *ngh2,
                                             int32_t stream_id)
{
  अगर(ngh2->fatal_rv)
    वापस शून्य;
  वापस nghttp2_session_get_stream_user_तथ्य(ngh2->s, stream_id);
}

पूर्णांक Curl_ngh2_session_set_stream_user_तथ्य(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id, व्योम *stream_user_तथ्य)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2,
    nghttp2_session_set_stream_user_तथ्य(
      ngh2->s, stream_id, stream_user_तथ्य));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  काष्ठा curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  अगर(!Curl_ngh2_valid(ngh2))
    वापस 0;
  वापस nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_stream_effective_local_window_आकार(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id)
{
  अगर(ngh2->fatal_rv)
    वापस 0;
  वापस nghttp2_session_get_stream_effective_local_window_आकार(
    ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_local_window_आकार(
  काष्ठा curl_ngh2 *ngh2)
{
  अगर(ngh2->fatal_rv)
    वापस 0;
  वापस nghttp2_session_get_local_window_आकार(ngh2->s);
}

int32_t Curl_ngh2_session_get_stream_remote_window_आकार(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id)
{
  अगर(ngh2->fatal_rv)
    वापस 0;
  वापस nghttp2_session_get_stream_remote_window_आकार(ngh2->s, stream_id);
}

int32_t Curl_ngh2_session_get_remote_window_आकार(
  काष्ठा curl_ngh2 *ngh2)
{
  अगर(ngh2->fatal_rv)
    वापस 0;
  वापस nghttp2_session_get_remote_window_आकार(ngh2->s);
}

पूर्णांक Curl_ngh2_session_set_local_window_आकार(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, int32_t window_आकार)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2, nghttp2_session_set_local_window_आकार(
    ngh2->s, flags, stream_id, window_आकार));
}

पूर्णांक Curl_ngh2_submit_window_update(काष्ठा curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t stream_id,
  int32_t window_आकार_increment)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2, nghttp2_submit_window_update(
    ngh2->s, flags, stream_id, window_आकार_increment));
}

पूर्णांक Curl_ngh2_submit_rst_stream(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, uint32_t error_code)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2, nghttp2_submit_rst_stream(
    ngh2->s, flags, stream_id, error_code));
}

पूर्णांक Curl_ngh2_session_send(काष्ठा curl_ngh2 *ngh2)
{
  पूर्णांक rv;
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  वापस ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

पूर्णांक Curl_ngh2_session_client_new3(
  काष्ठा curl_ngh2 *ngh2, स्थिर nghttp2_session_callbacks *callbacks,
  व्योम *user_तथ्य, स्थिर nghttp2_option *option, nghttp2_mem *mem)
{
  अगर(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_तथ्य, option, mem));
}

पूर्णांक Curl_ngh2_session_want_read(काष्ठा curl_ngh2 *ngh2)
{
  अगर(!Curl_ngh2_valid(ngh2))
    वापस 0;
  वापस nghttp2_session_want_read(ngh2->s);
}

पूर्णांक Curl_ngh2_session_want_write(काष्ठा curl_ngh2 *ngh2)
{
  अगर(!Curl_ngh2_valid(ngh2))
    वापस 0;
  वापस nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  काष्ठा curl_ngh2 *ngh2, स्थिर uint8_t *in, माप_प्रकार inlen)
{
  ssize_t rv;
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, in, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  वापस ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

पूर्णांक Curl_ngh2_session_check_request_allowed(काष्ठा curl_ngh2 *ngh2)
{
  अगर(!Curl_ngh2_valid(ngh2))
    वापस 0;
  वापस nghttp2_session_check_request_allowed(ngh2->s);
}

पूर्णांक Curl_ngh2_submit_ping(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags, स्थिर uint8_t *opaque_तथ्य)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_तथ्य));
}

int32_t Curl_ngh2_session_get_stream_effective_recv_तथ्य_लंबाई(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id)
{
  अगर(!Curl_ngh2_valid(ngh2))
    वापस 0;
  वापस nghttp2_session_get_stream_effective_recv_तथ्य_लंबाई(
    ngh2->s, stream_id);
}

पूर्णांक Curl_ngh2_session_resume_तथ्य(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2,
    nghttp2_session_resume_तथ्य(ngh2->s, stream_id));
}

पूर्णांक Curl_ngh2_session_consume(
  काष्ठा curl_ngh2 *ngh2, int32_t stream_id, माप_प्रकार आकार)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, stream_id, आकार));
}

पूर्णांक Curl_ngh2_submit_priority(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags,
  int32_t stream_id, स्थिर nghttp2_priority_spec *pri_spec)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2,
    nghttp2_submit_priority(ngh2->s, flags, stream_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  काष्ठा curl_ngh2 *ngh2, स्थिर nghttp2_priority_spec *pri_spec,
  स्थिर nghttp2_nv *nva, माप_प्रकार nvlen, स्थिर nghttp2_तथ्य_provider *तथ्य_prd,
  व्योम *stream_user_तथ्य)
{
  अगर(ngh2->fatal_rv)
    वापस (int32_t)ngh2->fatal_rv;
  वापस ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      तथ्य_prd, stream_user_तथ्य));
}

पूर्णांक Curl_ngh2_session_upgrade2(
  काष्ठा curl_ngh2 *ngh2, स्थिर uint8_t *settings_payload,
  माप_प्रकार settings_payloadlen, पूर्णांक शीर्ष_request, व्योम *stream_user_तथ्य)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, शीर्ष_request, stream_user_तथ्य));
}

पूर्णांक Curl_ngh2_submit_goaway(
  काष्ठा curl_ngh2 *ngh2, uint8_t flags, int32_t last_stream_id,
  uint32_t error_code, स्थिर uint8_t *opaque_तथ्य, माप_प्रकार opaque_तथ्य_len)
{
  अगर(ngh2->fatal_rv)
    वापस ngh2->fatal_rv;
  वापस ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, last_stream_id, error_code,
    opaque_तथ्य, opaque_तथ्य_len));
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
