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

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "api.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಅಕ್ಷರ mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_ಕೊನೆಯ <= CURL_CBAPI_FN_ಆರಂಭ) ? 1 : -1];
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಅಕ್ಷರ cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_ಕೊನೆಯ - 1) <= UINT8_MAX) ? 1 : -1];
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಅಕ್ಷರ mapi_ಆಳ_ಹೊಂದುತ್ತದೆ_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

ರಚನೆ Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t ದತ್ತಾಂಶ_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_ಬೀಗ; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_ಕೊನೆಯ] = {
  /* function                   kill rec  RO !sc */
  { CURL_EAPI_FN_easy_cleanup,     1,  0,  0,  0 },
  { CURL_EAPI_FN_easy_duphandle,   0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_getinfo,     0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_ಶೀರ್ಷಿಕೆ,      0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_nextheader,  0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_pause,       0,  1,  0,  0 },
  { CURL_EAPI_FN_easy_perform_ev,  0,  0,  0,  1 },
  { CURL_EAPI_FN_easy_perform,     0,  0,  0,  1 },
  { CURL_EAPI_FN_easy_recv,        0,  0,  0,  1 },
  { CURL_EAPI_FN_easy_reset,       0,  0,  0,  0 },
  { CURL_EAPI_FN_easy_send,        0,  0,  0,  1 },
  { CURL_EAPI_FN_easy_setopt,      0,  1,  0,  0 },
  { CURL_EAPI_FN_easy_ssls_export, 0,  0,  0,  1 },
  { CURL_EAPI_FN_easy_ssls_import, 0,  0,  0,  1 },
  { CURL_EAPI_FN_easy_upkeep,      0,  0,  0,  1 },
  { CURL_EAPI_FN_ws_recv,          0,  1,  0,  1 },
  { CURL_EAPI_FN_ws_send,          0,  1,  0,  1 },
  { CURL_EAPI_FN_ws_ಆರಂಭ_ಚೌಕಟ್ಟು,   0,  1,  0,  0 },
};

ರಚನೆ Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_ಬೀಗ;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_ಕೊನೆಯ] = {
  /* function                       kill rec ntfy !scach */
  { CURL_MAPI_FN_multi_ಕೂಡಿಸು_handle,     0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_ನಿಯೋಜಿಸು,         0,  1,   1,  0 },
  { CURL_MAPI_FN_multi_cleanup,        1,  0,   0,  1 },
  { CURL_MAPI_FN_multi_fdset,          0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_get_handles,    0,  1,   1,  0 },
  { CURL_MAPI_FN_multi_get_offt,       0,  1,   1,  0 },
  { CURL_MAPI_FN_multi_info_read,      0,  1,   1,  0 },
  { CURL_MAPI_FN_multi_notify_disable, 0,  1,   1,  0 },
  { CURL_MAPI_FN_multi_notify_enable,  0,  1,   1,  0 },
  { CURL_MAPI_FN_multi_perform,        0,  0,   0,  1 },
  { CURL_MAPI_FN_multi_poll,           0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_remove_handle,  0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_setopt,         0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_socket_action,  0,  0,   0,  1 },
  { CURL_MAPI_FN_multi_socket_all,     0,  0,   0,  1 },
  { CURL_MAPI_FN_multi_socket,         0,  0,   0,  1 },
  { CURL_MAPI_FN_multi_timeout,        0,  0,   1,  1 },
  { CURL_MAPI_FN_multi_wait,           0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_waitfds,        0,  0,   1,  0 },
};

ರಚನೆ Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t ಗಣ_readonly;    /* only allows readonly, recursive easy calls */
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cbapi_fn_props
cbapi_fn_props[CURL_CBAPI_FN_ಕೊನೆಯ - CURL_CBAPI_FN_ಆರಂಭ] = {
  { CURL_CBAPI_FN_easy_chunk_bgn,             0 },
  { CURL_CBAPI_FN_easy_chunk_ಕೊನೆ,             0 },
  { CURL_CBAPI_FN_easy_closesocket,           0 },
  { CURL_CBAPI_FN_easy_cr_ಒಳ_read,            0 },
  { CURL_CBAPI_FN_easy_cr_ಒಳ_resume_from,     0 },
  { CURL_CBAPI_FN_easy_cw_ಹೊರಹೋಗು_cb,             0 },
  { CURL_CBAPI_FN_easy_fdebug,                1 },
  { CURL_CBAPI_FN_easy_fnmatch_ದತ್ತಾಂಶ,          0 },
  { CURL_CBAPI_FN_easy_fopensocket,           0 },
  { CURL_CBAPI_FN_easy_fprereq,               0 },
  { CURL_CBAPI_FN_easy_fprogress,             0 },
  { CURL_CBAPI_FN_easy_fread_func,            0 },
  { CURL_CBAPI_FN_easy_fsockopt,              0 },
  { CURL_CBAPI_FN_easy_fsslctx,               0 },
  { CURL_CBAPI_FN_easy_fwrite_rtp,            0 },
  { CURL_CBAPI_FN_easy_fxferinfo,             0 },
  { CURL_CBAPI_FN_easy_ioctl_func,            0 },
  { CURL_CBAPI_FN_easy_resolver_ಆರಂಭ,        0 },
  { CURL_CBAPI_FN_easy_seek_func,             0 },
  { CURL_CBAPI_FN_easy_ssh_hostkeyfunc,       0 },
  { CURL_CBAPI_FN_easy_ssh_keyfunc,           0 },
  { CURL_CBAPI_FN_easy_trailer_callback,      0 },

  { CURL_CBAPI_FN_multi_ntfy_cb,              0 },
  { CURL_CBAPI_FN_multi_ತಳ್ಳು_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

ಸ್ಥಿರ bool eapi_ಒಳ_readonly_cb(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ->multi;
  ಆದರೆ(multi && multi->callstack.ಎಣಿಕೆ_2) {
    size_t i;
    ಗಾಗಿ(i = 0; i < multi->callstack.ಎಣಿಕೆ_2; ++i) {
      ಆದರೆ(multi->callstack.calls[i] >= CURL_CBAPI_FN_ಆರಂಭ) {
        uint8_t fn = multi->callstack.calls[i];
        ಆದರೆ((fn < CURL_CBAPI_FN_ಕೊನೆಯ) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_ಆರಂಭ].ಗಣ_readonly)
          ಹಿಂತಿರುಗಿ TRUE;
      }
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool mapi_ಒಳ_ntfy_cb(ರಚನೆ Curl_multi *multi)
{
  ಆದರೆ(multi && multi->callstack.ಎಣಿಕೆ_2) {
    size_t i;
    ಗಾಗಿ(i = 0; i < multi->callstack.ಎಣಿಕೆ_2; ++i) {
      ಆದರೆ(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_api_multi_is_ಒಳ_callback(ರಚನೆ Curl_multi *multi)
{
  ಆದರೆ(multi && multi->callstack.ಎಣಿಕೆ_2) {
    size_t i;
    ಗಾಗಿ(i = 0; i < multi->callstack.ಎಣಿಕೆ_2; ++i) {
      ಆದರೆ(multi->callstack.calls[i] >= CURL_CBAPI_FN_ಆರಂಭ)
        ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_api_is_ಒಳ_callback(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ದತ್ತಾಂಶ && ದತ್ತಾಂಶ->multi) {
    ಹಿಂತಿರುಗಿ Curl_api_multi_is_ಒಳ_callback(ದತ್ತಾಂಶ->multi);
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_eapi_enter(ರಚನೆ Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_eapi_fn_props *fn_props;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  guard->ಆಳ = 0;

  /* Verify that we got an easy handle we can work with. */
  ಆದರೆ(!GOOD_EASY_HANDLE(ದತ್ತಾಂಶ) || ದತ್ತಾಂಶ->state.ಆಂತರಿಕ) {
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  ಆದರೆ(ದತ್ತಾಂಶ->mid != UINT32_MAX) {
    ಆದರೆ(GOOD_MULTI_HANDLE(ದತ್ತಾಂಶ->multi)) {
      ಆದರೆ(!Curl_multi_knows_easy(ದತ್ತಾಂಶ->multi, ದತ್ತಾಂಶ)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  ಆದರೆ(fn >= CURL_EAPI_FN_ಕೊನೆಯ) {
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  ಆದರೆ(!fn_props->recurse) {
    ಆದರೆ(ದತ್ತಾಂಶ->callstack.ಎಣಿಕೆ_2) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        ದತ್ತಾಂಶ->callstack.calls[ದತ್ತಾಂಶ->callstack.ಎಣಿಕೆ_2 - 1]));
#ಆದರೆ_ಅಂತ್ಯ
      ಫಲಿತಾಂಶ = CURLE_RECURSIVE_API_CALL;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ(ದತ್ತಾಂಶ->multi && ದತ್ತಾಂಶ->multi->callstack.ಎಣಿಕೆ_2) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE

      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        ದತ್ತಾಂಶ->multi->callstack.calls[ದತ್ತಾಂಶ->multi->callstack.ಎಣಿಕೆ_2 - 1]));
#ಆದರೆ_ಅಂತ್ಯ
      ಫಲಿತಾಂಶ = CURLE_RECURSIVE_API_CALL;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(!fn_props->readonly && eapi_ಒಳ_readonly_cb(ದತ್ತಾಂಶ)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#ಆದರೆ_ಅಂತ್ಯ
    ಫಲಿತಾಂಶ = CURLE_RECURSIVE_API_CALL;
    ಹೋಗು ಹೊರಹೋಗು;
  }

#ಆದರೆ defined(USE_SSL) && defined(USE_MUTEX)
  ಆದರೆ(fn_props->no_scache_ಬೀಗ &&
     Curl_ssl_scache_is_locked_by_current_thread(ದತ್ತಾಂಶ)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#ಆದರೆ_ಅಂತ್ಯ
    ಫಲಿತಾಂಶ = CURLE_RECURSIVE_API_CALL;
    ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* all fine, add to data's callstack */
  ಆದರೆ(ದತ್ತಾಂಶ->callstack.ಎಣಿಕೆ_2 >= CURL_EAPI_MAX_RECURSION) {
    ಫಲಿತಾಂಶ = CURLE_RECURSIVE_API_CALL;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ದತ್ತಾಂಶ->callstack.calls[ದತ್ತಾಂಶ->callstack.ಎಣಿಕೆ_2] = (uint16_t)fn;
  ++ದತ್ತಾಂಶ->callstack.ಎಣಿಕೆ_2;
  guard->ಆಳ = ದತ್ತಾಂಶ->callstack.ಎಣಿಕೆ_2;
  guard->ದತ್ತಾಂಶ = fn_props->ದತ್ತಾಂಶ_is_killed ? ಖಾಲಿ : ದತ್ತಾಂಶ;

ಹೊರಹೋಗು:
  ಆದರೆ(presult)
    *presult = ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ guard->ಆಳ > 0;
}

ಶೂನ್ಯ Curl_eapi_leave(ರಚನೆ Curl_eapi_guard *guard)
{
  ಆದರೆ(guard->ಆಳ) {
    /* guard->data is set when handle is supposed to stay alive during call */
    ಆದರೆ(guard->ದತ್ತಾಂಶ && GOOD_EASY_HANDLE(guard->ದತ್ತಾಂಶ)) {
      ಆದರೆ(guard->ಆಳ > guard->ದತ್ತಾಂಶ->callstack.ಎಣಿಕೆ_2) {
        DEBUGASSERT(0); /* something very wrong */
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(guard->ಆಳ < guard->ದತ್ತಾಂಶ->callstack.ಎಣಿಕೆ_2) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->ದತ್ತಾಂಶ->callstack.ಎಣಿಕೆ_2 = (uint16_t)(guard->ಆಳ - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode ಫಲಿತಾಂಶ)
{
  ಆಯ್ಕೆ(ಫಲಿತಾಂಶ) {
  ಸ್ಥಿತಿ CURLE_OK:
    ಹಿಂತಿರುಗಿ CURLHE_OK;
  ಸ್ಥಿತಿ CURLE_BAD_FUNCTION_ವಾದ:
    ಹಿಂತಿರುಗಿ CURLHE_BAD_ವಾದ;
  ಸ್ಥಿತಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ:
    ಹಿಂತಿರುಗಿ CURLHE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಸ್ಥಿತಿ CURLE_NOT_BUILT_ಒಳ:
    ಹಿಂತಿರುಗಿ CURLHE_NOT_BUILT_ಒಳ;
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    ಹಿಂತಿರುಗಿ CURLHE_BAD_ವಾದ;
  }
}

bool Curl_mapi_enter(ರಚನೆ Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  ರಚನೆ Curl_multi *multi = m;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->ಆಳ = 0;

  /* Verify that we got an easy handle we can work with. */
  ಆದರೆ(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(fn >= CURL_MAPI_FN_ಕೊನೆಯ) {
    mresult = CURLM_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  ಆದರೆ(fn_props->allow_ntfy_cb && mapi_ಒಳ_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!fn_props->recurse && multi->callstack.ಎಣಿಕೆ_2) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.ಎಣಿಕೆ_2 - 1]));
#ಆದರೆ_ಅಂತ್ಯ
    mresult = CURLM_RECURSIVE_API_CALL;
    ಹೋಗು ಹೊರಹೋಗು;
  }

#ಆದರೆ defined(USE_SSL) && defined(USE_MUTEX)
  ಆದರೆ(fn_props->no_scache_ಬೀಗ && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#ಆದರೆ_ಅಂತ್ಯ
    mresult = CURLM_RECURSIVE_API_CALL;
    ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* all fine, add to data's callstack */
  ಆದರೆ(multi->callstack.ಎಣಿಕೆ_2 >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  multi->callstack.calls[multi->callstack.ಎಣಿಕೆ_2] = (uint8_t)fn;
  ++multi->callstack.ಎಣಿಕೆ_2;
  guard->ಆಳ = multi->callstack.ಎಣಿಕೆ_2;
  guard->multi = fn_props->multi_is_killed ? ಖಾಲಿ : multi;

ಹೊರಹೋಗು:
  ಆದರೆ(pmresult)
    *pmresult = mresult;
  ಹಿಂತಿರುಗಿ guard->ಆಳ > 0;
}

ಶೂನ್ಯ Curl_mapi_leave(ರಚನೆ Curl_mapi_guard *guard)
{
  ಆದರೆ(guard->ಆಳ) {
    /* guard->data is set when handle is supposed to stay alive during call */
    ಆದರೆ(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      ಆದರೆ(guard->ಆಳ > guard->multi->callstack.ಎಣಿಕೆ_2) {
        DEBUGASSERT(0); /* something very wrong */
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(guard->ಆಳ < guard->multi->callstack.ಎಣಿಕೆ_2) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.ಎಣಿಕೆ_2 = (uint8_t)(guard->ಆಳ - 1);
      }
    }
  }
}

ಶೂನ್ಯ Curl_cbapi_enter(ರಚನೆ Curl_mapi_guard *guard,
                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->ಆಳ = 0;

  ಆದರೆ(!multi)
    multi = ದತ್ತಾಂಶ ? ದತ್ತಾಂಶ->multi : ಖಾಲಿ;
  /* if not multi is involved here, just leave */
  ಆದರೆ(!multi)
    ಹಿಂತಿರುಗಿ;
  /* invalid callback specifier? */
  ಆದರೆ((fn >= CURL_CBAPI_FN_ಕೊನೆಯ) || (fn < CURL_CBAPI_FN_ಆರಂಭ)) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_ಆರಂಭ].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  ಆದರೆ(multi->callstack.ಎಣಿಕೆ_2 >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ;
  }
  multi->callstack.calls[multi->callstack.ಎಣಿಕೆ_2] = (uint8_t)fn;
  ++multi->callstack.ಎಣಿಕೆ_2;
  guard->ಆಳ = multi->callstack.ಎಣಿಕೆ_2;
  guard->multi = multi;
}

ಶೂನ್ಯ Curl_cbapi_leave(ರಚನೆ Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
