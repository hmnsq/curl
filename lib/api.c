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

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "api.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_threads.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅੱਖਰ mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_ਆਖਰੀ <= CURL_CBAPI_FN_ਸ਼ੁਰੂ) ? 1 : -1];
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅੱਖਰ cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_ਆਖਰੀ - 1) <= UINT8_MAX) ? 1 : -1];
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅੱਖਰ mapi_ਡੂੰਘਾਈ_ਸਮਾਉਂਦਾ_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

ਬਣਤਰ Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t ਅੰਕੜੇ_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_ਤਾਲਾ; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_ਆਖਰੀ] = {
  /* function                   kill rec  RO !sc */
  { CURL_EAPI_FN_easy_cleanup,     1,  0,  0,  0 },
  { CURL_EAPI_FN_easy_duphandle,   0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_getinfo,     0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_ਸਿਰਲੇਖ,      0,  1,  1,  0 },
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
  { CURL_EAPI_FN_ws_ਸ਼ੁਰੂ_ਚੌਖਟਾ,   0,  1,  0,  0 },
};

ਬਣਤਰ Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_ਤਾਲਾ;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_ਆਖਰੀ] = {
  /* function                       kill rec ntfy !scach */
  { CURL_MAPI_FN_multi_ਜੋੜੋ_handle,     0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_ਸੌਂਪੋ,         0,  1,   1,  0 },
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

ਬਣਤਰ Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t ਸੈੱਟ_readonly;    /* only allows readonly, recursive easy calls */
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cbapi_fn_props
cbapi_fn_props[CURL_CBAPI_FN_ਆਖਰੀ - CURL_CBAPI_FN_ਸ਼ੁਰੂ] = {
  { CURL_CBAPI_FN_easy_chunk_bgn,             0 },
  { CURL_CBAPI_FN_easy_chunk_ਸਿਰਾ,             0 },
  { CURL_CBAPI_FN_easy_closesocket,           0 },
  { CURL_CBAPI_FN_easy_cr_ਵਿੱਚ_read,            0 },
  { CURL_CBAPI_FN_easy_cr_ਵਿੱਚ_resume_from,     0 },
  { CURL_CBAPI_FN_easy_cw_ਬਾਹਰਲਾ_cb,             0 },
  { CURL_CBAPI_FN_easy_fdebug,                1 },
  { CURL_CBAPI_FN_easy_fnmatch_ਅੰਕੜੇ,          0 },
  { CURL_CBAPI_FN_easy_fopensocket,           0 },
  { CURL_CBAPI_FN_easy_fprereq,               0 },
  { CURL_CBAPI_FN_easy_fprogress,             0 },
  { CURL_CBAPI_FN_easy_fread_func,            0 },
  { CURL_CBAPI_FN_easy_fsockopt,              0 },
  { CURL_CBAPI_FN_easy_fsslctx,               0 },
  { CURL_CBAPI_FN_easy_fwrite_rtp,            0 },
  { CURL_CBAPI_FN_easy_fxferinfo,             0 },
  { CURL_CBAPI_FN_easy_ioctl_func,            0 },
  { CURL_CBAPI_FN_easy_resolver_ਸ਼ੁਰੂ,        0 },
  { CURL_CBAPI_FN_easy_seek_func,             0 },
  { CURL_CBAPI_FN_easy_ssh_hostkeyfunc,       0 },
  { CURL_CBAPI_FN_easy_ssh_keyfunc,           0 },
  { CURL_CBAPI_FN_easy_trailer_callback,      0 },

  { CURL_CBAPI_FN_multi_ntfy_cb,              0 },
  { CURL_CBAPI_FN_multi_ਧੱਕੋ_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

ਸਥਿਰ bool eapi_ਵਿੱਚ_readonly_cb(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ->multi;
  ਜੇ(multi && multi->callstack.ਗਿਣਤੀ_2) {
    size_t i;
    ਲਈ(i = 0; i < multi->callstack.ਗਿਣਤੀ_2; ++i) {
      ਜੇ(multi->callstack.calls[i] >= CURL_CBAPI_FN_ਸ਼ੁਰੂ) {
        uint8_t fn = multi->callstack.calls[i];
        ਜੇ((fn < CURL_CBAPI_FN_ਆਖਰੀ) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_ਸ਼ੁਰੂ].ਸੈੱਟ_readonly)
          ਵਾਪਸ TRUE;
      }
    }
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool mapi_ਵਿੱਚ_ntfy_cb(ਬਣਤਰ Curl_multi *multi)
{
  ਜੇ(multi && multi->callstack.ਗਿਣਤੀ_2) {
    size_t i;
    ਲਈ(i = 0; i < multi->callstack.ਗਿਣਤੀ_2; ++i) {
      ਜੇ(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        ਵਾਪਸ TRUE;
    }
  }
  ਵਾਪਸ FALSE;
}

bool Curl_api_multi_is_ਵਿੱਚ_callback(ਬਣਤਰ Curl_multi *multi)
{
  ਜੇ(multi && multi->callstack.ਗਿਣਤੀ_2) {
    size_t i;
    ਲਈ(i = 0; i < multi->callstack.ਗਿਣਤੀ_2; ++i) {
      ਜੇ(multi->callstack.calls[i] >= CURL_CBAPI_FN_ਸ਼ੁਰੂ)
        ਵਾਪਸ TRUE;
    }
  }
  ਵਾਪਸ FALSE;
}

bool Curl_api_is_ਵਿੱਚ_callback(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ਅੰਕੜੇ && ਅੰਕੜੇ->multi) {
    ਵਾਪਸ Curl_api_multi_is_ਵਿੱਚ_callback(ਅੰਕੜੇ->multi);
  }
  ਵਾਪਸ FALSE;
}

bool Curl_eapi_enter(ਬਣਤਰ Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
  ਅਚਲ ਬਣਤਰ Curl_eapi_fn_props *fn_props;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  guard->ਡੂੰਘਾਈ = 0;

  /* Verify that we got an easy handle we can work with. */
  ਜੇ(!GOOD_EASY_HANDLE(ਅੰਕੜੇ) || ਅੰਕੜੇ->state.ਅੰਦਰੂਨੀ) {
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  ਜੇ(ਅੰਕੜੇ->mid != UINT32_MAX) {
    ਜੇ(GOOD_MULTI_HANDLE(ਅੰਕੜੇ->multi)) {
      ਜੇ(!Curl_multi_knows_easy(ਅੰਕੜੇ->multi, ਅੰਕੜੇ)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    ਨਹੀਂਤਾਂ {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  ਜੇ(fn >= CURL_EAPI_FN_ਆਖਰੀ) {
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  ਜੇ(!fn_props->recurse) {
    ਜੇ(ਅੰਕੜੇ->callstack.ਗਿਣਤੀ_2) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        ਅੰਕੜੇ->callstack.calls[ਅੰਕੜੇ->callstack.ਗਿਣਤੀ_2 - 1]));
#ਜੇ_ਅੰਤ
      ਨਤੀਜਾ = CURLE_RECURSIVE_API_CALL;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ(ਅੰਕੜੇ->multi && ਅੰਕੜੇ->multi->callstack.ਗਿਣਤੀ_2) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE

      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        ਅੰਕੜੇ->multi->callstack.calls[ਅੰਕੜੇ->multi->callstack.ਗਿਣਤੀ_2 - 1]));
#ਜੇ_ਅੰਤ
      ਨਤੀਜਾ = CURLE_RECURSIVE_API_CALL;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(!fn_props->readonly && eapi_ਵਿੱਚ_readonly_cb(ਅੰਕੜੇ)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#ਜੇ_ਅੰਤ
    ਨਤੀਜਾ = CURLE_RECURSIVE_API_CALL;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਜੇ defined(USE_SSL) && defined(USE_MUTEX)
  ਜੇ(fn_props->no_scache_ਤਾਲਾ &&
     Curl_ssl_scache_is_locked_by_current_thread(ਅੰਕੜੇ)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#ਜੇ_ਅੰਤ
    ਨਤੀਜਾ = CURLE_RECURSIVE_API_CALL;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ

  /* all fine, add to data's callstack */
  ਜੇ(ਅੰਕੜੇ->callstack.ਗਿਣਤੀ_2 >= CURL_EAPI_MAX_RECURSION) {
    ਨਤੀਜਾ = CURLE_RECURSIVE_API_CALL;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਅੰਕੜੇ->callstack.calls[ਅੰਕੜੇ->callstack.ਗਿਣਤੀ_2] = (uint16_t)fn;
  ++ਅੰਕੜੇ->callstack.ਗਿਣਤੀ_2;
  guard->ਡੂੰਘਾਈ = ਅੰਕੜੇ->callstack.ਗਿਣਤੀ_2;
  guard->ਅੰਕੜੇ = fn_props->ਅੰਕੜੇ_is_killed ? ਨਿਰਾ : ਅੰਕੜੇ;

ਬਾਹਰਲਾ:
  ਜੇ(presult)
    *presult = ਨਤੀਜਾ;
  ਵਾਪਸ guard->ਡੂੰਘਾਈ > 0;
}

ਖਾਲੀ Curl_eapi_leave(ਬਣਤਰ Curl_eapi_guard *guard)
{
  ਜੇ(guard->ਡੂੰਘਾਈ) {
    /* guard->data is set when handle is supposed to stay alive during call */
    ਜੇ(guard->ਅੰਕੜੇ && GOOD_EASY_HANDLE(guard->ਅੰਕੜੇ)) {
      ਜੇ(guard->ਡੂੰਘਾਈ > guard->ਅੰਕੜੇ->callstack.ਗਿਣਤੀ_2) {
        DEBUGASSERT(0); /* something very wrong */
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(guard->ਡੂੰਘਾਈ < guard->ਅੰਕੜੇ->callstack.ਗਿਣਤੀ_2) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->ਅੰਕੜੇ->callstack.ਗਿਣਤੀ_2 = (uint16_t)(guard->ਡੂੰਘਾਈ - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode ਨਤੀਜਾ)
{
  ਚੋਣ(ਨਤੀਜਾ) {
  ਹਾਲਤ CURLE_OK:
    ਵਾਪਸ CURLHE_OK;
  ਹਾਲਤ CURLE_BAD_FUNCTION_ਦਲੀਲ:
    ਵਾਪਸ CURLHE_BAD_ਦਲੀਲ;
  ਹਾਲਤ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ:
    ਵਾਪਸ CURLHE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਹਾਲਤ CURLE_NOT_BUILT_ਵਿੱਚ:
    ਵਾਪਸ CURLHE_NOT_BUILT_ਵਿੱਚ;
  ਮੂਲ:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    ਵਾਪਸ CURLHE_BAD_ਦਲੀਲ;
  }
}

bool Curl_mapi_enter(ਬਣਤਰ Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  ਬਣਤਰ Curl_multi *multi = m;
  ਅਚਲ ਬਣਤਰ Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->ਡੂੰਘਾਈ = 0;

  /* Verify that we got an easy handle we can work with. */
  ਜੇ(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(fn >= CURL_MAPI_FN_ਆਖਰੀ) {
    mresult = CURLM_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  ਜੇ(fn_props->allow_ntfy_cb && mapi_ਵਿੱਚ_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  ਨਹੀਂਤਾਂ ਜੇ(!fn_props->recurse && multi->callstack.ਗਿਣਤੀ_2) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.ਗਿਣਤੀ_2 - 1]));
#ਜੇ_ਅੰਤ
    mresult = CURLM_RECURSIVE_API_CALL;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਜੇ defined(USE_SSL) && defined(USE_MUTEX)
  ਜੇ(fn_props->no_scache_ਤਾਲਾ && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#ਜੇ_ਅੰਤ
    mresult = CURLM_RECURSIVE_API_CALL;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ

  /* all fine, add to data's callstack */
  ਜੇ(multi->callstack.ਗਿਣਤੀ_2 >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  multi->callstack.calls[multi->callstack.ਗਿਣਤੀ_2] = (uint8_t)fn;
  ++multi->callstack.ਗਿਣਤੀ_2;
  guard->ਡੂੰਘਾਈ = multi->callstack.ਗਿਣਤੀ_2;
  guard->multi = fn_props->multi_is_killed ? ਨਿਰਾ : multi;

ਬਾਹਰਲਾ:
  ਜੇ(pmresult)
    *pmresult = mresult;
  ਵਾਪਸ guard->ਡੂੰਘਾਈ > 0;
}

ਖਾਲੀ Curl_mapi_leave(ਬਣਤਰ Curl_mapi_guard *guard)
{
  ਜੇ(guard->ਡੂੰਘਾਈ) {
    /* guard->data is set when handle is supposed to stay alive during call */
    ਜੇ(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      ਜੇ(guard->ਡੂੰਘਾਈ > guard->multi->callstack.ਗਿਣਤੀ_2) {
        DEBUGASSERT(0); /* something very wrong */
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(guard->ਡੂੰਘਾਈ < guard->multi->callstack.ਗਿਣਤੀ_2) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.ਗਿਣਤੀ_2 = (uint8_t)(guard->ਡੂੰਘਾਈ - 1);
      }
    }
  }
}

ਖਾਲੀ Curl_cbapi_enter(ਬਣਤਰ Curl_mapi_guard *guard,
                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->ਡੂੰਘਾਈ = 0;

  ਜੇ(!multi)
    multi = ਅੰਕੜੇ ? ਅੰਕੜੇ->multi : ਨਿਰਾ;
  /* if not multi is involved here, just leave */
  ਜੇ(!multi)
    ਵਾਪਸ;
  /* invalid callback specifier? */
  ਜੇ((fn >= CURL_CBAPI_FN_ਆਖਰੀ) || (fn < CURL_CBAPI_FN_ਸ਼ੁਰੂ)) {
    DEBUGASSERT(0);
    ਵਾਪਸ;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_ਸ਼ੁਰੂ].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  ਜੇ(multi->callstack.ਗਿਣਤੀ_2 >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    ਵਾਪਸ;
  }
  multi->callstack.calls[multi->callstack.ਗਿਣਤੀ_2] = (uint8_t)fn;
  ++multi->callstack.ਗਿਣਤੀ_2;
  guard->ਡੂੰਘਾਈ = multi->callstack.ਗਿਣਤੀ_2;
  guard->multi = multi;
}

ਖਾਲੀ Curl_cbapi_leave(ਬਣਤਰ Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
