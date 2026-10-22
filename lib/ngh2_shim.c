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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#ضمن <nghttp2/nghttp2.h>

#ضمن "urldata.h"
#ضمن "ngh2_shim.h"


bool Curl_ngh2_valid(بنية curl_ngh2 *ngh2)
{
  أرجع ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(بنية curl_ngh2 *ngh2, صحيح rv)
{
  إذا(ngh2->fatal_rv)
    أرجع TRUE;
  إذا(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    أرجع TRUE;
  }
  أرجع FALSE;
}

صحيح Curl_ngh2_fatal(بنية curl_ngh2 *ngh2)
{
  أرجع ngh2->fatal_rv;
}

ساكن صحيح ngh2_check(بنية curl_ngh2 *ngh2, صحيح rv)
{
  إذا(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  أرجع rv;
}

ساكن int32_t ngh2_check32(بنية curl_ngh2 *ngh2, int32_t rv)
{
  إذا((rv < 0) && nghttp2_is_fatal((صحيح)rv))
    ngh2->fatal_rv = (صحيح)rv;
  أرجع rv;
}

ساكن ssize_t ngh2_checks(بنية curl_ngh2 *ngh2, ssize_t rv)
{
  إذا((rv < 0) && nghttp2_is_fatal((صحيح)rv))
    ngh2->fatal_rv = (صحيح)rv;
  أرجع rv;
}

فراغ Curl_ngh2_del(بنية curl_ngh2 *ngh2)
{
  إذا(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = عدم;
  }
}

صحيح Curl_ngh2_submit_settings(بنية curl_ngh2 *ngh2,
                              uint8_t flags,
                              ثابت nghttp2_settings_اندراج *iv,
                              size_t niv)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

فراغ *Curl_ngh2_session_get_دھارا_user_انکڑے(بنية curl_ngh2 *ngh2,
                                             int32_t دھارا_id)
{
  إذا(ngh2->fatal_rv)
    أرجع عدم;
  أرجع nghttp2_session_get_دھارا_user_انکڑے(ngh2->s, دھارا_id);
}

صحيح Curl_ngh2_session_سیٹ_دھارا_user_انکڑے(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id, فراغ *دھارا_user_انکڑے)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_سیٹ_دھارا_user_انکڑے(
      ngh2->s, دھارا_id, دھارا_user_انکڑے));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  بنية curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_دھارا_effective_local_کھڑکی_ناپ(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_دھارا_effective_local_کھڑکی_ناپ(
    ngh2->s, دھارا_id);
}

int32_t Curl_ngh2_session_get_local_کھڑکی_ناپ(
  بنية curl_ngh2 *ngh2)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_local_کھڑکی_ناپ(ngh2->s);
}

int32_t Curl_ngh2_session_get_دھارا_remote_کھڑکی_ناپ(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_دھارا_remote_کھڑکی_ناپ(ngh2->s, دھارا_id);
}

int32_t Curl_ngh2_session_get_remote_کھڑکی_ناپ(
  بنية curl_ngh2 *ngh2)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_remote_کھڑکی_ناپ(ngh2->s);
}

صحيح Curl_ngh2_session_سیٹ_local_کھڑکی_ناپ(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, int32_t کھڑکی_ناپ)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_session_سیٹ_local_کھڑکی_ناپ(
    ngh2->s, flags, دھارا_id, کھڑکی_ناپ));
}

صحيح Curl_ngh2_submit_کھڑکی_update(بنية curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t دھارا_id,
  int32_t کھڑکی_ناپ_increment)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_submit_کھڑکی_update(
    ngh2->s, flags, دھارا_id, کھڑکی_ناپ_increment));
}

صحيح Curl_ngh2_submit_rst_دھارا(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, uint32_t error_کوڈ)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_submit_rst_دھارا(
    ngh2->s, flags, دھارا_id, error_کوڈ));
}

صحيح Curl_ngh2_session_send(بنية curl_ngh2 *ngh2)
{
  صحيح rv;
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  أرجع ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

صحيح Curl_ngh2_session_client_new3(
  بنية curl_ngh2 *ngh2, ثابت nghttp2_session_callbacks *callbacks,
  فراغ *user_انکڑے, ثابت nghttp2_option *option, nghttp2_mem *mem)
{
  إذا(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_انکڑے, option, mem));
}

صحيح Curl_ngh2_session_want_read(بنية curl_ngh2 *ngh2)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_want_read(ngh2->s);
}

صحيح Curl_ngh2_session_want_write(بنية curl_ngh2 *ngh2)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  بنية curl_ngh2 *ngh2, ثابت uint8_t *وچ, size_t inlen)
{
  ssize_t rv;
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, وچ, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  أرجع ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

صحيح Curl_ngh2_session_check_request_allowed(بنية curl_ngh2 *ngh2)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_check_request_allowed(ngh2->s);
}

صحيح Curl_ngh2_submit_ping(
  بنية curl_ngh2 *ngh2, uint8_t flags, ثابت uint8_t *opaque_انکڑے)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_انکڑے));
}

int32_t Curl_ngh2_session_get_دھارا_effective_recv_انکڑے_لمائی(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_get_دھارا_effective_recv_انکڑے_لمائی(
    ngh2->s, دھارا_id);
}

صحيح Curl_ngh2_session_resume_انکڑے(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_resume_انکڑے(ngh2->s, دھارا_id));
}

صحيح Curl_ngh2_session_consume(
  بنية curl_ngh2 *ngh2, int32_t دھارا_id, size_t ناپ)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, دھارا_id, ناپ));
}

صحيح Curl_ngh2_submit_ترجیح(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t دھارا_id, ثابت nghttp2_ترجیح_spec *pri_spec)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_submit_ترجیح(ngh2->s, flags, دھارا_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  بنية curl_ngh2 *ngh2, ثابت nghttp2_ترجیح_spec *pri_spec,
  ثابت nghttp2_nv *nva, size_t nvlen, ثابت nghttp2_انکڑے_provider *انکڑے_prd,
  فراغ *دھارا_user_انکڑے)
{
  إذا(ngh2->fatal_rv)
    أرجع (int32_t)ngh2->fatal_rv;
  أرجع ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      انکڑے_prd, دھارا_user_انکڑے));
}

صحيح Curl_ngh2_session_upgrade2(
  بنية curl_ngh2 *ngh2, ثابت uint8_t *settings_payload,
  size_t settings_payloadlen, صحيح سر_request, فراغ *دھارا_user_انکڑے)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, سر_request, دھارا_user_انکڑے));
}

صحيح Curl_ngh2_submit_goaway(
  بنية curl_ngh2 *ngh2, uint8_t flags, int32_t آخری_دھارا_id,
  uint32_t error_کوڈ, ثابت uint8_t *opaque_انکڑے, size_t opaque_انکڑے_len)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, آخری_دھارا_id, error_کوڈ,
    opaque_انکڑے, opaque_انکڑے_len));
}

#جے_انت /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
