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
  प्रत्यागम ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(संरचना curl_ngh2 *ngh2, पूर्णाङ्क rv)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम TRUE;
  यदि(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

पूर्णाङ्क Curl_ngh2_fatal(संरचना curl_ngh2 *ngh2)
{
  प्रत्यागम ngh2->fatal_rv;
}

स्थिर पूर्णाङ्क ngh2_check(संरचना curl_ngh2 *ngh2, पूर्णाङ्क rv)
{
  यदि(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  प्रत्यागम rv;
}

स्थिर int32_t ngh2_check32(संरचना curl_ngh2 *ngh2, int32_t rv)
{
  यदि((rv < 0) && nghttp2_is_fatal((पूर्णाङ्क)rv))
    ngh2->fatal_rv = (पूर्णाङ्क)rv;
  प्रत्यागम rv;
}

स्थिर ssize_t ngh2_checks(संरचना curl_ngh2 *ngh2, ssize_t rv)
{
  यदि((rv < 0) && nghttp2_is_fatal((पूर्णाङ्क)rv))
    ngh2->fatal_rv = (पूर्णाङ्क)rv;
  प्रत्यागम rv;
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
                              ध्रुव nghttp2_settings_प्रविष्टिः *iv,
                              size_t niv)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

शून्य *Curl_ngh2_session_get_प्रवाहः_user_दत्त(संरचना curl_ngh2 *ngh2,
                                             int32_t प्रवाहः_id)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम रिक्त;
  प्रत्यागम nghttp2_session_get_प्रवाहः_user_दत्त(ngh2->s, प्रवाहः_id);
}

पूर्णाङ्क Curl_ngh2_session_समुच्चयः_प्रवाहः_user_दत्त(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id, शून्य *प्रवाहः_user_दत्त)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2,
    nghttp2_session_समुच्चयः_प्रवाहः_user_दत्त(
      ngh2->s, प्रवाहः_id, प्रवाहः_user_दत्त));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  संरचना curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  यदि(!Curl_ngh2_valid(ngh2))
    प्रत्यागम 0;
  प्रत्यागम nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_प्रवाहः_effective_local_गवाक्षः_आकार(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम 0;
  प्रत्यागम nghttp2_session_get_प्रवाहः_effective_local_गवाक्षः_आकार(
    ngh2->s, प्रवाहः_id);
}

int32_t Curl_ngh2_session_get_local_गवाक्षः_आकार(
  संरचना curl_ngh2 *ngh2)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम 0;
  प्रत्यागम nghttp2_session_get_local_गवाक्षः_आकार(ngh2->s);
}

int32_t Curl_ngh2_session_get_प्रवाहः_remote_गवाक्षः_आकार(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम 0;
  प्रत्यागम nghttp2_session_get_प्रवाहः_remote_गवाक्षः_आकार(ngh2->s, प्रवाहः_id);
}

int32_t Curl_ngh2_session_get_remote_गवाक्षः_आकार(
  संरचना curl_ngh2 *ngh2)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम 0;
  प्रत्यागम nghttp2_session_get_remote_गवाक्षः_आकार(ngh2->s);
}

पूर्णाङ्क Curl_ngh2_session_समुच्चयः_local_गवाक्षः_आकार(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t प्रवाहः_id, int32_t गवाक्षः_आकार)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2, nghttp2_session_समुच्चयः_local_गवाक्षः_आकार(
    ngh2->s, flags, प्रवाहः_id, गवाक्षः_आकार));
}

पूर्णाङ्क Curl_ngh2_submit_गवाक्षः_update(संरचना curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t प्रवाहः_id,
  int32_t गवाक्षः_आकार_increment)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2, nghttp2_submit_गवाक्षः_update(
    ngh2->s, flags, प्रवाहः_id, गवाक्षः_आकार_increment));
}

पूर्णाङ्क Curl_ngh2_submit_rst_प्रवाहः(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t प्रवाहः_id, uint32_t error_संहिता)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2, nghttp2_submit_rst_प्रवाहः(
    ngh2->s, flags, प्रवाहः_id, error_संहिता));
}

पूर्णाङ्क Curl_ngh2_session_send(संरचना curl_ngh2 *ngh2)
{
  पूर्णाङ्क rv;
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  प्रत्यागम ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

पूर्णाङ्क Curl_ngh2_session_client_new3(
  संरचना curl_ngh2 *ngh2, ध्रुव nghttp2_session_callbacks *callbacks,
  शून्य *user_दत्त, ध्रुव nghttp2_option *option, nghttp2_mem *mem)
{
  यदि(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_दत्त, option, mem));
}

पूर्णाङ्क Curl_ngh2_session_want_read(संरचना curl_ngh2 *ngh2)
{
  यदि(!Curl_ngh2_valid(ngh2))
    प्रत्यागम 0;
  प्रत्यागम nghttp2_session_want_read(ngh2->s);
}

पूर्णाङ्क Curl_ngh2_session_want_write(संरचना curl_ngh2 *ngh2)
{
  यदि(!Curl_ngh2_valid(ngh2))
    प्रत्यागम 0;
  प्रत्यागम nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  संरचना curl_ngh2 *ngh2, ध्रुव uint8_t *अन्तर्, size_t inlen)
{
  ssize_t rv;
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, अन्तर्, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  प्रत्यागम ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

पूर्णाङ्क Curl_ngh2_session_check_request_allowed(संरचना curl_ngh2 *ngh2)
{
  यदि(!Curl_ngh2_valid(ngh2))
    प्रत्यागम 0;
  प्रत्यागम nghttp2_session_check_request_allowed(ngh2->s);
}

पूर्णाङ्क Curl_ngh2_submit_ping(
  संरचना curl_ngh2 *ngh2, uint8_t flags, ध्रुव uint8_t *opaque_दत्त)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_दत्त));
}

int32_t Curl_ngh2_session_get_प्रवाहः_effective_recv_दत्त_दीर्घता(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id)
{
  यदि(!Curl_ngh2_valid(ngh2))
    प्रत्यागम 0;
  प्रत्यागम nghttp2_session_get_प्रवाहः_effective_recv_दत्त_दीर्घता(
    ngh2->s, प्रवाहः_id);
}

पूर्णाङ्क Curl_ngh2_session_resume_दत्त(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2,
    nghttp2_session_resume_दत्त(ngh2->s, प्रवाहः_id));
}

पूर्णाङ्क Curl_ngh2_session_consume(
  संरचना curl_ngh2 *ngh2, int32_t प्रवाहः_id, size_t आकार)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, प्रवाहः_id, आकार));
}

पूर्णाङ्क Curl_ngh2_submit_प्राथमिकता(
  संरचना curl_ngh2 *ngh2, uint8_t flags,
  int32_t प्रवाहः_id, ध्रुव nghttp2_प्राथमिकता_spec *pri_spec)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2,
    nghttp2_submit_प्राथमिकता(ngh2->s, flags, प्रवाहः_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  संरचना curl_ngh2 *ngh2, ध्रुव nghttp2_प्राथमिकता_spec *pri_spec,
  ध्रुव nghttp2_nv *nva, size_t nvlen, ध्रुव nghttp2_दत्त_provider *दत्त_prd,
  शून्य *प्रवाहः_user_दत्त)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम (int32_t)ngh2->fatal_rv;
  प्रत्यागम ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      दत्त_prd, प्रवाहः_user_दत्त));
}

पूर्णाङ्क Curl_ngh2_session_upgrade2(
  संरचना curl_ngh2 *ngh2, ध्रुव uint8_t *settings_payload,
  size_t settings_payloadlen, पूर्णाङ्क शीर्ष_request, शून्य *प्रवाहः_user_दत्त)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, शीर्ष_request, प्रवाहः_user_दत्त));
}

पूर्णाङ्क Curl_ngh2_submit_goaway(
  संरचना curl_ngh2 *ngh2, uint8_t flags, int32_t चरमः_प्रवाहः_id,
  uint32_t error_संहिता, ध्रुव uint8_t *opaque_दत्त, size_t opaque_दत्त_len)
{
  यदि(ngh2->fatal_rv)
    प्रत्यागम ngh2->fatal_rv;
  प्रत्यागम ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, चरमः_प्रवाहः_id, error_संहिता,
    opaque_दत्त, opaque_दत्त_len));
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
