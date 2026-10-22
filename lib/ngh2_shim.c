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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#ਅਨ੍ਤਰ੍ਭਾਵ <nghttp2/nghttp2.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ngh2_shim.h"


bool Curl_ngh2_valid(ਬਣਤਰ curl_ngh2 *ngh2)
{
  ਵਾਪਸ ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(ਬਣਤਰ curl_ngh2 *ngh2, ਪੂਰਨ rv)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ TRUE;
  ਜੇ(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

ਪੂਰਨ Curl_ngh2_fatal(ਬਣਤਰ curl_ngh2 *ngh2)
{
  ਵਾਪਸ ngh2->fatal_rv;
}

ਸਥਿਰ ਪੂਰਨ ngh2_check(ਬਣਤਰ curl_ngh2 *ngh2, ਪੂਰਨ rv)
{
  ਜੇ(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  ਵਾਪਸ rv;
}

ਸਥਿਰ int32_t ngh2_check32(ਬਣਤਰ curl_ngh2 *ngh2, int32_t rv)
{
  ਜੇ((rv < 0) && nghttp2_is_fatal((ਪੂਰਨ)rv))
    ngh2->fatal_rv = (ਪੂਰਨ)rv;
  ਵਾਪਸ rv;
}

ਸਥਿਰ ssize_t ngh2_checks(ਬਣਤਰ curl_ngh2 *ngh2, ssize_t rv)
{
  ਜੇ((rv < 0) && nghttp2_is_fatal((ਪੂਰਨ)rv))
    ngh2->fatal_rv = (ਪੂਰਨ)rv;
  ਵਾਪਸ rv;
}

ਖਾਲੀ Curl_ngh2_del(ਬਣਤਰ curl_ngh2 *ngh2)
{
  ਜੇ(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = ਨਿਰਾ;
  }
}

ਪੂਰਨ Curl_ngh2_submit_settings(ਬਣਤਰ curl_ngh2 *ngh2,
                              uint8_t flags,
                              ਅਚਲ nghttp2_settings_ਇੰਦਰਾਜ *iv,
                              size_t niv)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

ਖਾਲੀ *Curl_ngh2_session_get_ਧਾਰਾ_user_ਅੰਕੜੇ(ਬਣਤਰ curl_ngh2 *ngh2,
                                             int32_t ਧਾਰਾ_id)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ਨਿਰਾ;
  ਵਾਪਸ nghttp2_session_get_ਧਾਰਾ_user_ਅੰਕੜੇ(ngh2->s, ਧਾਰਾ_id);
}

ਪੂਰਨ Curl_ngh2_session_ਸੈੱਟ_ਧਾਰਾ_user_ਅੰਕੜੇ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2,
    nghttp2_session_ਸੈੱਟ_ਧਾਰਾ_user_ਅੰਕੜੇ(
      ngh2->s, ਧਾਰਾ_id, ਧਾਰਾ_user_ਅੰਕੜੇ));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  ਬਣਤਰ curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  ਜੇ(!Curl_ngh2_valid(ngh2))
    ਵਾਪਸ 0;
  ਵਾਪਸ nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_ਧਾਰਾ_effective_local_ਖਿੜਕੀ_ਮਾਪ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ 0;
  ਵਾਪਸ nghttp2_session_get_ਧਾਰਾ_effective_local_ਖਿੜਕੀ_ਮਾਪ(
    ngh2->s, ਧਾਰਾ_id);
}

int32_t Curl_ngh2_session_get_local_ਖਿੜਕੀ_ਮਾਪ(
  ਬਣਤਰ curl_ngh2 *ngh2)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ 0;
  ਵਾਪਸ nghttp2_session_get_local_ਖਿੜਕੀ_ਮਾਪ(ngh2->s);
}

int32_t Curl_ngh2_session_get_ਧਾਰਾ_remote_ਖਿੜਕੀ_ਮਾਪ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ 0;
  ਵਾਪਸ nghttp2_session_get_ਧਾਰਾ_remote_ਖਿੜਕੀ_ਮਾਪ(ngh2->s, ਧਾਰਾ_id);
}

int32_t Curl_ngh2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(
  ਬਣਤਰ curl_ngh2 *ngh2)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ 0;
  ਵਾਪਸ nghttp2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(ngh2->s);
}

ਪੂਰਨ Curl_ngh2_session_ਸੈੱਟ_local_ਖਿੜਕੀ_ਮਾਪ(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ਧਾਰਾ_id, int32_t ਖਿੜਕੀ_ਮਾਪ)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2, nghttp2_session_ਸੈੱਟ_local_ਖਿੜਕੀ_ਮਾਪ(
    ngh2->s, flags, ਧਾਰਾ_id, ਖਿੜਕੀ_ਮਾਪ));
}

ਪੂਰਨ Curl_ngh2_submit_ਖਿੜਕੀ_update(ਬਣਤਰ curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t ਧਾਰਾ_id,
  int32_t ਖਿੜਕੀ_ਮਾਪ_increment)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2, nghttp2_submit_ਖਿੜਕੀ_update(
    ngh2->s, flags, ਧਾਰਾ_id, ਖਿੜਕੀ_ਮਾਪ_increment));
}

ਪੂਰਨ Curl_ngh2_submit_rst_ਧਾਰਾ(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ਧਾਰਾ_id, uint32_t error_ਕੋਡ)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2, nghttp2_submit_rst_ਧਾਰਾ(
    ngh2->s, flags, ਧਾਰਾ_id, error_ਕੋਡ));
}

ਪੂਰਨ Curl_ngh2_session_send(ਬਣਤਰ curl_ngh2 *ngh2)
{
  ਪੂਰਨ rv;
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  ਵਾਪਸ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

ਪੂਰਨ Curl_ngh2_session_client_new3(
  ਬਣਤਰ curl_ngh2 *ngh2, ਅਚਲ nghttp2_session_callbacks *callbacks,
  ਖਾਲੀ *user_ਅੰਕੜੇ, ਅਚਲ nghttp2_option *option, nghttp2_mem *mem)
{
  ਜੇ(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_ਅੰਕੜੇ, option, mem));
}

ਪੂਰਨ Curl_ngh2_session_want_read(ਬਣਤਰ curl_ngh2 *ngh2)
{
  ਜੇ(!Curl_ngh2_valid(ngh2))
    ਵਾਪਸ 0;
  ਵਾਪਸ nghttp2_session_want_read(ngh2->s);
}

ਪੂਰਨ Curl_ngh2_session_want_write(ਬਣਤਰ curl_ngh2 *ngh2)
{
  ਜੇ(!Curl_ngh2_valid(ngh2))
    ਵਾਪਸ 0;
  ਵਾਪਸ nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  ਬਣਤਰ curl_ngh2 *ngh2, ਅਚਲ uint8_t *ਵਿੱਚ, size_t inlen)
{
  ssize_t rv;
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, ਵਿੱਚ, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  ਵਾਪਸ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

ਪੂਰਨ Curl_ngh2_session_check_request_allowed(ਬਣਤਰ curl_ngh2 *ngh2)
{
  ਜੇ(!Curl_ngh2_valid(ngh2))
    ਵਾਪਸ 0;
  ਵਾਪਸ nghttp2_session_check_request_allowed(ngh2->s);
}

ਪੂਰਨ Curl_ngh2_submit_ping(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags, ਅਚਲ uint8_t *opaque_ਅੰਕੜੇ)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_ਅੰਕੜੇ));
}

int32_t Curl_ngh2_session_get_ਧਾਰਾ_effective_recv_ਅੰਕੜੇ_ਲੰਮਾਈ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id)
{
  ਜੇ(!Curl_ngh2_valid(ngh2))
    ਵਾਪਸ 0;
  ਵਾਪਸ nghttp2_session_get_ਧਾਰਾ_effective_recv_ਅੰਕੜੇ_ਲੰਮਾਈ(
    ngh2->s, ਧਾਰਾ_id);
}

ਪੂਰਨ Curl_ngh2_session_resume_ਅੰਕੜੇ(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2,
    nghttp2_session_resume_ਅੰਕੜੇ(ngh2->s, ਧਾਰਾ_id));
}

ਪੂਰਨ Curl_ngh2_session_consume(
  ਬਣਤਰ curl_ngh2 *ngh2, int32_t ਧਾਰਾ_id, size_t ਮਾਪ)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, ਧਾਰਾ_id, ਮਾਪ));
}

ਪੂਰਨ Curl_ngh2_submit_ਤਰਜੀਹ(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ਧਾਰਾ_id, ਅਚਲ nghttp2_ਤਰਜੀਹ_spec *pri_spec)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2,
    nghttp2_submit_ਤਰਜੀਹ(ngh2->s, flags, ਧਾਰਾ_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  ਬਣਤਰ curl_ngh2 *ngh2, ਅਚਲ nghttp2_ਤਰਜੀਹ_spec *pri_spec,
  ਅਚਲ nghttp2_nv *nva, size_t nvlen, ਅਚਲ nghttp2_ਅੰਕੜੇ_provider *ਅੰਕੜੇ_prd,
  ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ (int32_t)ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      ਅੰਕੜੇ_prd, ਧਾਰਾ_user_ਅੰਕੜੇ));
}

ਪੂਰਨ Curl_ngh2_session_upgrade2(
  ਬਣਤਰ curl_ngh2 *ngh2, ਅਚਲ uint8_t *settings_payload,
  size_t settings_payloadlen, ਪੂਰਨ ਸਿਰ_request, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, ਸਿਰ_request, ਧਾਰਾ_user_ਅੰਕੜੇ));
}

ਪੂਰਨ Curl_ngh2_submit_goaway(
  ਬਣਤਰ curl_ngh2 *ngh2, uint8_t flags, int32_t ਆਖਰੀ_ਧਾਰਾ_id,
  uint32_t error_ਕੋਡ, ਅਚਲ uint8_t *opaque_ਅੰਕੜੇ, size_t opaque_ਅੰਕੜੇ_len)
{
  ਜੇ(ngh2->fatal_rv)
    ਵਾਪਸ ngh2->fatal_rv;
  ਵਾਪਸ ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, ਆਖਰੀ_ਧਾਰਾ_id, error_ਕੋਡ,
    opaque_ਅੰਕੜੇ, opaque_ਅੰਕੜੇ_len));
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
