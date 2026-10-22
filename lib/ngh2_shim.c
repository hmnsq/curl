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
                              ثابت nghttp2_settings_مدخل *iv,
                              size_t niv)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

فراغ *Curl_ngh2_session_get_جریان_user_داده(بنية curl_ngh2 *ngh2,
                                             int32_t جریان_id)
{
  إذا(ngh2->fatal_rv)
    أرجع عدم;
  أرجع nghttp2_session_get_جریان_user_داده(ngh2->s, جریان_id);
}

صحيح Curl_ngh2_session_مجموعه_جریان_user_داده(
  بنية curl_ngh2 *ngh2, int32_t جریان_id, فراغ *جریان_user_داده)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_مجموعه_جریان_user_داده(
      ngh2->s, جریان_id, جریان_user_داده));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  بنية curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_جریان_effective_local_پنجره_اندازه(
  بنية curl_ngh2 *ngh2, int32_t جریان_id)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_جریان_effective_local_پنجره_اندازه(
    ngh2->s, جریان_id);
}

int32_t Curl_ngh2_session_get_local_پنجره_اندازه(
  بنية curl_ngh2 *ngh2)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_local_پنجره_اندازه(ngh2->s);
}

int32_t Curl_ngh2_session_get_جریان_remote_پنجره_اندازه(
  بنية curl_ngh2 *ngh2, int32_t جریان_id)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_جریان_remote_پنجره_اندازه(ngh2->s, جریان_id);
}

int32_t Curl_ngh2_session_get_remote_پنجره_اندازه(
  بنية curl_ngh2 *ngh2)
{
  إذا(ngh2->fatal_rv)
    أرجع 0;
  أرجع nghttp2_session_get_remote_پنجره_اندازه(ngh2->s);
}

صحيح Curl_ngh2_session_مجموعه_local_پنجره_اندازه(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t جریان_id, int32_t پنجره_اندازه)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_session_مجموعه_local_پنجره_اندازه(
    ngh2->s, flags, جریان_id, پنجره_اندازه));
}

صحيح Curl_ngh2_submit_پنجره_update(بنية curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t جریان_id,
  int32_t پنجره_اندازه_increment)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_submit_پنجره_update(
    ngh2->s, flags, جریان_id, پنجره_اندازه_increment));
}

صحيح Curl_ngh2_submit_rst_جریان(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t جریان_id, uint32_t error_کد)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2, nghttp2_submit_rst_جریان(
    ngh2->s, flags, جریان_id, error_کد));
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
  فراغ *user_داده, ثابت nghttp2_option *option, nghttp2_mem *mem)
{
  إذا(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_داده, option, mem));
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
  بنية curl_ngh2 *ngh2, ثابت uint8_t *درون, size_t inlen)
{
  ssize_t rv;
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, درون, inlen));
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
  بنية curl_ngh2 *ngh2, uint8_t flags, ثابت uint8_t *opaque_داده)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_داده));
}

int32_t Curl_ngh2_session_get_جریان_effective_recv_داده_درازی(
  بنية curl_ngh2 *ngh2, int32_t جریان_id)
{
  إذا(!Curl_ngh2_valid(ngh2))
    أرجع 0;
  أرجع nghttp2_session_get_جریان_effective_recv_داده_درازی(
    ngh2->s, جریان_id);
}

صحيح Curl_ngh2_session_resume_داده(
  بنية curl_ngh2 *ngh2, int32_t جریان_id)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_resume_داده(ngh2->s, جریان_id));
}

صحيح Curl_ngh2_session_consume(
  بنية curl_ngh2 *ngh2, int32_t جریان_id, size_t اندازه)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, جریان_id, اندازه));
}

صحيح Curl_ngh2_submit_اولویت(
  بنية curl_ngh2 *ngh2, uint8_t flags,
  int32_t جریان_id, ثابت nghttp2_اولویت_spec *pri_spec)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_submit_اولویت(ngh2->s, flags, جریان_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  بنية curl_ngh2 *ngh2, ثابت nghttp2_اولویت_spec *pri_spec,
  ثابت nghttp2_nv *nva, size_t nvlen, ثابت nghttp2_داده_provider *داده_prd,
  فراغ *جریان_user_داده)
{
  إذا(ngh2->fatal_rv)
    أرجع (int32_t)ngh2->fatal_rv;
  أرجع ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      داده_prd, جریان_user_داده));
}

صحيح Curl_ngh2_session_upgrade2(
  بنية curl_ngh2 *ngh2, ثابت uint8_t *settings_payload,
  size_t settings_payloadlen, صحيح سر_request, فراغ *جریان_user_داده)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, سر_request, جریان_user_داده));
}

صحيح Curl_ngh2_submit_goaway(
  بنية curl_ngh2 *ngh2, uint8_t flags, int32_t آخرین_جریان_id,
  uint32_t error_کد, ثابت uint8_t *opaque_داده, size_t opaque_داده_len)
{
  إذا(ngh2->fatal_rv)
    أرجع ngh2->fatal_rv;
  أرجع ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, آخرین_جریان_id, error_کد,
    opaque_داده, opaque_داده_len));
}

#پایان_اگر /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
