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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#شامل <nghttp2/nghttp2.h>

#شامل "urldata.h"
#شامل "ngh2_shim.h"


bool Curl_ngh2_valid(ساخت curl_ngh2 *ngh2)
{
  واپس ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(ساخت curl_ngh2 *ngh2, صحیح rv)
{
  اگر(ngh2->fatal_rv)
    واپس TRUE;
  اگر(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    واپس TRUE;
  }
  واپس FALSE;
}

صحیح Curl_ngh2_fatal(ساخت curl_ngh2 *ngh2)
{
  واپس ngh2->fatal_rv;
}

ساکن صحیح ngh2_check(ساخت curl_ngh2 *ngh2, صحیح rv)
{
  اگر(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  واپس rv;
}

ساکن int32_t ngh2_check32(ساخت curl_ngh2 *ngh2, int32_t rv)
{
  اگر((rv < 0) && nghttp2_is_fatal((صحیح)rv))
    ngh2->fatal_rv = (صحیح)rv;
  واپس rv;
}

ساکن ssize_t ngh2_checks(ساخت curl_ngh2 *ngh2, ssize_t rv)
{
  اگر((rv < 0) && nghttp2_is_fatal((صحیح)rv))
    ngh2->fatal_rv = (صحیح)rv;
  واپس rv;
}

خالی Curl_ngh2_del(ساخت curl_ngh2 *ngh2)
{
  اگر(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = تہی;
  }
}

صحیح Curl_ngh2_submit_settings(ساخت curl_ngh2 *ngh2,
                              uint8_t flags,
                              ثابت nghttp2_settings_اندراج *iv,
                              size_t niv)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

خالی *Curl_ngh2_session_get_دھارا_user_ڈیٹا(ساخت curl_ngh2 *ngh2,
                                             int32_t دھارا_id)
{
  اگر(ngh2->fatal_rv)
    واپس تہی;
  واپس nghttp2_session_get_دھارا_user_ڈیٹا(ngh2->s, دھارا_id);
}

صحیح Curl_ngh2_session_سیٹ_دھارا_user_ڈیٹا(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id, خالی *دھارا_user_ڈیٹا)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2,
    nghttp2_session_سیٹ_دھارا_user_ڈیٹا(
      ngh2->s, دھارا_id, دھارا_user_ڈیٹا));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  ساخت curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  اگر(!Curl_ngh2_valid(ngh2))
    واپس 0;
  واپس nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_دھارا_effective_local_کھڑکی_سائز(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id)
{
  اگر(ngh2->fatal_rv)
    واپس 0;
  واپس nghttp2_session_get_دھارا_effective_local_کھڑکی_سائز(
    ngh2->s, دھارا_id);
}

int32_t Curl_ngh2_session_get_local_کھڑکی_سائز(
  ساخت curl_ngh2 *ngh2)
{
  اگر(ngh2->fatal_rv)
    واپس 0;
  واپس nghttp2_session_get_local_کھڑکی_سائز(ngh2->s);
}

int32_t Curl_ngh2_session_get_دھارا_remote_کھڑکی_سائز(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id)
{
  اگر(ngh2->fatal_rv)
    واپس 0;
  واپس nghttp2_session_get_دھارا_remote_کھڑکی_سائز(ngh2->s, دھارا_id);
}

int32_t Curl_ngh2_session_get_remote_کھڑکی_سائز(
  ساخت curl_ngh2 *ngh2)
{
  اگر(ngh2->fatal_rv)
    واپس 0;
  واپس nghttp2_session_get_remote_کھڑکی_سائز(ngh2->s);
}

صحیح Curl_ngh2_session_سیٹ_local_کھڑکی_سائز(
  ساخت curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, int32_t کھڑکی_سائز)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2, nghttp2_session_سیٹ_local_کھڑکی_سائز(
    ngh2->s, flags, دھارا_id, کھڑکی_سائز));
}

صحیح Curl_ngh2_submit_کھڑکی_update(ساخت curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t دھارا_id,
  int32_t کھڑکی_سائز_increment)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2, nghttp2_submit_کھڑکی_update(
    ngh2->s, flags, دھارا_id, کھڑکی_سائز_increment));
}

صحیح Curl_ngh2_submit_rst_دھارا(
  ساخت curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, uint32_t error_رمز)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2, nghttp2_submit_rst_دھارا(
    ngh2->s, flags, دھارا_id, error_رمز));
}

صحیح Curl_ngh2_session_send(ساخت curl_ngh2 *ngh2)
{
  صحیح rv;
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  واپس ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

صحیح Curl_ngh2_session_client_new3(
  ساخت curl_ngh2 *ngh2, ثابت nghttp2_session_callbacks *callbacks,
  خالی *user_ڈیٹا, ثابت nghttp2_option *option, nghttp2_mem *mem)
{
  اگر(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_ڈیٹا, option, mem));
}

صحیح Curl_ngh2_session_want_read(ساخت curl_ngh2 *ngh2)
{
  اگر(!Curl_ngh2_valid(ngh2))
    واپس 0;
  واپس nghttp2_session_want_read(ngh2->s);
}

صحیح Curl_ngh2_session_want_write(ساخت curl_ngh2 *ngh2)
{
  اگر(!Curl_ngh2_valid(ngh2))
    واپس 0;
  واپس nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  ساخت curl_ngh2 *ngh2, ثابت uint8_t *اندر, size_t inlen)
{
  ssize_t rv;
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, اندر, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  واپس ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

صحیح Curl_ngh2_session_check_request_allowed(ساخت curl_ngh2 *ngh2)
{
  اگر(!Curl_ngh2_valid(ngh2))
    واپس 0;
  واپس nghttp2_session_check_request_allowed(ngh2->s);
}

صحیح Curl_ngh2_submit_ping(
  ساخت curl_ngh2 *ngh2, uint8_t flags, ثابت uint8_t *opaque_ڈیٹا)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_ڈیٹا));
}

int32_t Curl_ngh2_session_get_دھارا_effective_recv_ڈیٹا_لمبائی(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id)
{
  اگر(!Curl_ngh2_valid(ngh2))
    واپس 0;
  واپس nghttp2_session_get_دھارا_effective_recv_ڈیٹا_لمبائی(
    ngh2->s, دھارا_id);
}

صحیح Curl_ngh2_session_resume_ڈیٹا(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2,
    nghttp2_session_resume_ڈیٹا(ngh2->s, دھارا_id));
}

صحیح Curl_ngh2_session_consume(
  ساخت curl_ngh2 *ngh2, int32_t دھارا_id, size_t سائز)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, دھارا_id, سائز));
}

صحیح Curl_ngh2_submit_ترجیح(
  ساخت curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, ثابت nghttp2_ترجیح_spec *pri_spec)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2,
    nghttp2_submit_ترجیح(ngh2->s, flags, دھارا_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  ساخت curl_ngh2 *ngh2, ثابت nghttp2_ترجیح_spec *pri_spec,
  ثابت nghttp2_nv *nva, size_t nvlen, ثابت nghttp2_ڈیٹا_provider *ڈیٹا_prd,
  خالی *دھارا_user_ڈیٹا)
{
  اگر(ngh2->fatal_rv)
    واپس (int32_t)ngh2->fatal_rv;
  واپس ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      ڈیٹا_prd, دھارا_user_ڈیٹا));
}

صحیح Curl_ngh2_session_upgrade2(
  ساخت curl_ngh2 *ngh2, ثابت uint8_t *settings_payload,
  size_t settings_payloadlen, صحیح سر_request, خالی *دھارا_user_ڈیٹا)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, سر_request, دھارا_user_ڈیٹا));
}

صحیح Curl_ngh2_submit_goaway(
  ساخت curl_ngh2 *ngh2, uint8_t flags, int32_t آخری_دھارا_id,
  uint32_t error_رمز, ثابت uint8_t *opaque_ڈیٹا, size_t opaque_ڈیٹا_len)
{
  اگر(ngh2->fatal_rv)
    واپس ngh2->fatal_rv;
  واپس ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, آخری_دھارا_id, error_رمز,
    opaque_ڈیٹا, opaque_ڈیٹا_len));
}

#اگر_ختم /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
