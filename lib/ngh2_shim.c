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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#ಅನ್ತರ್ಭಾವ <nghttp2/nghttp2.h>

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "ngh2_shim.h"


bool Curl_ngh2_valid(ರಚನೆ curl_ngh2 *ngh2)
{
  ಹಿಂತಿರುಗಿ ngh2->s && !ngh2->fatal_rv;
}

bool Curl_ngh2_is_fatal(ರಚನೆ curl_ngh2 *ngh2, ಪೂರ್ಣಾಂಕ rv)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ TRUE;
  ಆದರೆ(nghttp2_is_fatal(rv)) {
    ngh2->fatal_rv = rv;
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_fatal(ರಚನೆ curl_ngh2 *ngh2)
{
  ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ngh2_check(ರಚನೆ curl_ngh2 *ngh2, ಪೂರ್ಣಾಂಕ rv)
{
  ಆದರೆ(nghttp2_is_fatal(rv))
    ngh2->fatal_rv = rv;
  ಹಿಂತಿರುಗಿ rv;
}

ಸ್ಥಿರ int32_t ngh2_check32(ರಚನೆ curl_ngh2 *ngh2, int32_t rv)
{
  ಆದರೆ((rv < 0) && nghttp2_is_fatal((ಪೂರ್ಣಾಂಕ)rv))
    ngh2->fatal_rv = (ಪೂರ್ಣಾಂಕ)rv;
  ಹಿಂತಿರುಗಿ rv;
}

ಸ್ಥಿರ ssize_t ngh2_checks(ರಚನೆ curl_ngh2 *ngh2, ssize_t rv)
{
  ಆದರೆ((rv < 0) && nghttp2_is_fatal((ಪೂರ್ಣಾಂಕ)rv))
    ngh2->fatal_rv = (ಪೂರ್ಣಾಂಕ)rv;
  ಹಿಂತಿರುಗಿ rv;
}

ಶೂನ್ಯ Curl_ngh2_del(ರಚನೆ curl_ngh2 *ngh2)
{
  ಆದರೆ(ngh2->s) {
    nghttp2_session_del(ngh2->s);
    ngh2->s = ಖಾಲಿ;
  }
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_settings(ರಚನೆ curl_ngh2 *ngh2,
                              uint8_t flags,
                              ಸ್ಥಿರಾಂಕ nghttp2_settings_ನಮೂದು *iv,
                              size_t niv)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2, nghttp2_submit_settings(ngh2->s, flags, iv, niv));
}

ಶೂನ್ಯ *Curl_ngh2_session_get_ಹರಿವು_user_ದತ್ತಾಂಶ(ರಚನೆ curl_ngh2 *ngh2,
                                             int32_t ಹರಿವು_id)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ nghttp2_session_get_ಹರಿವು_user_ದತ್ತಾಂಶ(ngh2->s, ಹರಿವು_id);
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_session_ಗಣ_ಹರಿವು_user_ದತ್ತಾಂಶ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2,
    nghttp2_session_ಗಣ_ಹರಿವು_user_ದತ್ತಾಂಶ(
      ngh2->s, ಹರಿವು_id, ಹರಿವು_user_ದತ್ತಾಂಶ));
}

uint32_t Curl_ngh2_session_get_remote_settings(
  ರಚನೆ curl_ngh2 *ngh2, nghttp2_settings_id id)
{
  ಆದರೆ(!Curl_ngh2_valid(ngh2))
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ nghttp2_session_get_remote_settings(ngh2->s, id);
}

int32_t Curl_ngh2_session_get_ಹರಿವು_effective_local_ಕಿಟಕಿ_ಗಾತ್ರ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ nghttp2_session_get_ಹರಿವು_effective_local_ಕಿಟಕಿ_ಗಾತ್ರ(
    ngh2->s, ಹರಿವು_id);
}

int32_t Curl_ngh2_session_get_local_ಕಿಟಕಿ_ಗಾತ್ರ(
  ರಚನೆ curl_ngh2 *ngh2)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ nghttp2_session_get_local_ಕಿಟಕಿ_ಗಾತ್ರ(ngh2->s);
}

int32_t Curl_ngh2_session_get_ಹರಿವು_remote_ಕಿಟಕಿ_ಗಾತ್ರ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ nghttp2_session_get_ಹರಿವು_remote_ಕಿಟಕಿ_ಗಾತ್ರ(ngh2->s, ಹರಿವು_id);
}

int32_t Curl_ngh2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(
  ರಚನೆ curl_ngh2 *ngh2)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ nghttp2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(ngh2->s);
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_session_ಗಣ_local_ಕಿಟಕಿ_ಗಾತ್ರ(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ಹರಿವು_id, int32_t ಕಿಟಕಿ_ಗಾತ್ರ)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2, nghttp2_session_ಗಣ_local_ಕಿಟಕಿ_ಗಾತ್ರ(
    ngh2->s, flags, ಹರಿವು_id, ಕಿಟಕಿ_ಗಾತ್ರ));
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_ಕಿಟಕಿ_update(ರಚನೆ curl_ngh2 *ngh2,
  uint8_t flags,
  int32_t ಹರಿವು_id,
  int32_t ಕಿಟಕಿ_ಗಾತ್ರ_increment)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2, nghttp2_submit_ಕಿಟಕಿ_update(
    ngh2->s, flags, ಹರಿವು_id, ಕಿಟಕಿ_ಗಾತ್ರ_increment));
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_rst_ಹರಿವು(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ಹರಿವು_id, uint32_t error_ಸಂಕೇತ)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2, nghttp2_submit_rst_ಹರಿವು(
    ngh2->s, flags, ಹರಿವು_id, error_ಸಂಕೇತ));
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_session_send(ರಚನೆ curl_ngh2 *ngh2)
{
  ಪೂರ್ಣಾಂಕ rv;
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  rv = ngh2_check(ngh2, nghttp2_session_send(ngh2->s));
  /* a callback might trigger a fatal state without session_send()
   * noticing. Check again. */
  ಹಿಂತಿರುಗಿ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_session_client_new3(
  ರಚನೆ curl_ngh2 *ngh2, ಸ್ಥಿರಾಂಕ nghttp2_session_callbacks *callbacks,
  ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ nghttp2_option *option, nghttp2_mem *mem)
{
  ಆದರೆ(ngh2->s)
    ngh2->fatal_rv = NGHTTP2_ERR_FATAL;
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2,
    nghttp2_session_client_new3(
      &(ngh2->s), callbacks, user_ದತ್ತಾಂಶ, option, mem));
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_session_want_read(ರಚನೆ curl_ngh2 *ngh2)
{
  ಆದರೆ(!Curl_ngh2_valid(ngh2))
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ nghttp2_session_want_read(ngh2->s);
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_session_want_write(ರಚನೆ curl_ngh2 *ngh2)
{
  ಆದರೆ(!Curl_ngh2_valid(ngh2))
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ nghttp2_session_want_write(ngh2->s);
}

ssize_t Curl_ngh2_session_mem_recv(
  ರಚನೆ curl_ngh2 *ngh2, ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ, size_t inlen)
{
  ssize_t rv;
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  rv = ngh2_checks(ngh2, nghttp2_session_mem_recv(ngh2->s, ಒಳ, inlen));
  /* the session might encounter a fatal state in a callback,
   * but session_mem_rec() might not notice and return a non-error.
   * Check again. */
  ಹಿಂತಿರುಗಿ ngh2->fatal_rv ? ngh2->fatal_rv : rv;
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_session_check_request_allowed(ರಚನೆ curl_ngh2 *ngh2)
{
  ಆದರೆ(!Curl_ngh2_valid(ngh2))
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ nghttp2_session_check_request_allowed(ngh2->s);
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_ping(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags, ಸ್ಥಿರಾಂಕ uint8_t *opaque_ದತ್ತಾಂಶ)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2,
    nghttp2_submit_ping(ngh2->s, flags, opaque_ದತ್ತಾಂಶ));
}

int32_t Curl_ngh2_session_get_ಹರಿವು_effective_recv_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id)
{
  ಆದರೆ(!Curl_ngh2_valid(ngh2))
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ nghttp2_session_get_ಹರಿವು_effective_recv_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ(
    ngh2->s, ಹರಿವು_id);
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_session_resume_ದತ್ತಾಂಶ(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2,
    nghttp2_session_resume_ದತ್ತಾಂಶ(ngh2->s, ಹರಿವು_id));
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_session_consume(
  ರಚನೆ curl_ngh2 *ngh2, int32_t ಹರಿವು_id, size_t ಗಾತ್ರ_2)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2,
    nghttp2_session_consume(ngh2->s, ಹರಿವು_id, ಗಾತ್ರ_2));
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_ಆದ್ಯತೆ(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags,
  int32_t ಹರಿವು_id, ಸ್ಥಿರಾಂಕ nghttp2_ಆದ್ಯತೆ_spec *pri_spec)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2,
    nghttp2_submit_ಆದ್ಯತೆ(ngh2->s, flags, ಹರಿವು_id, pri_spec));
}

int32_t Curl_ngh2_submit_request(
  ರಚನೆ curl_ngh2 *ngh2, ಸ್ಥಿರಾಂಕ nghttp2_ಆದ್ಯತೆ_spec *pri_spec,
  ಸ್ಥಿರಾಂಕ nghttp2_nv *nva, size_t nvlen, ಸ್ಥಿರಾಂಕ nghttp2_ದತ್ತಾಂಶ_provider *ದತ್ತಾಂಶ_prd,
  ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ (int32_t)ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check32(ngh2,
    nghttp2_submit_request(ngh2->s, pri_spec, nva, nvlen,
      ದತ್ತಾಂಶ_prd, ಹರಿವು_user_ದತ್ತಾಂಶ));
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_session_upgrade2(
  ರಚನೆ curl_ngh2 *ngh2, ಸ್ಥಿರಾಂಕ uint8_t *settings_payload,
  size_t settings_payloadlen, ಪೂರ್ಣಾಂಕ ತಲೆ_request, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2,
    nghttp2_session_upgrade2(ngh2->s, settings_payload,
      settings_payloadlen, ತಲೆ_request, ಹರಿವು_user_ದತ್ತಾಂಶ));
}

ಪೂರ್ಣಾಂಕ Curl_ngh2_submit_goaway(
  ರಚನೆ curl_ngh2 *ngh2, uint8_t flags, int32_t ಕೊನೆಯ_ಹರಿವು_id,
  uint32_t error_ಸಂಕೇತ, ಸ್ಥಿರಾಂಕ uint8_t *opaque_ದತ್ತಾಂಶ, size_t opaque_ದತ್ತಾಂಶ_len)
{
  ಆದರೆ(ngh2->fatal_rv)
    ಹಿಂತಿರುಗಿ ngh2->fatal_rv;
  ಹಿಂತಿರುಗಿ ngh2_check(ngh2,
    nghttp2_submit_goaway(ngh2->s, flags, ಕೊನೆಯ_ಹರಿವು_id, error_ಸಂಕೇತ,
    opaque_ದತ್ತಾಂಶ, opaque_ದತ್ತಾಂಶ_len));
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
