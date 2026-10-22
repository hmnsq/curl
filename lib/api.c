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

ಪ್ರಕಾರನಿರ್ದೇಶ ವರ್ಣ mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_LAST <= CURL_CBAPI_FN_START) ? 1 : -1];
ಪ್ರಕಾರನಿರ್ದೇಶ ವರ್ಣ cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_LAST - 1) <= UINT8_MAX) ? 1 : -1];
ಪ್ರಕಾರನಿರ್ದೇಶ ವರ್ಣ mapi_depth_fits_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

ಸಂರಚನಾ Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t data_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_lock; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_LAST] = {
  /* function                   kill rec  RO !sc */
  { CURL_EAPI_FN_easy_cleanup,     1,  0,  0,  0 },
  { CURL_EAPI_FN_easy_duphandle,   0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_getinfo,     0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_header,      0,  1,  1,  0 },
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
  { CURL_EAPI_FN_ws_start_frame,   0,  1,  0,  0 },
};

ಸಂರಚನಾ Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_lock;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_LAST] = {
  /* function                       kill rec ntfy !scach */
  { CURL_MAPI_FN_multi_add_handle,     0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_assign,         0,  1,   1,  0 },
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

ಸಂರಚನಾ Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t set_readonly;    /* only allows readonly, recursive easy calls */
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cbapi_fn_props
cbapi_fn_props[CURL_CBAPI_FN_LAST - CURL_CBAPI_FN_START] = {
  { CURL_CBAPI_FN_easy_chunk_bgn,             0 },
  { CURL_CBAPI_FN_easy_chunk_end,             0 },
  { CURL_CBAPI_FN_easy_closesocket,           0 },
  { CURL_CBAPI_FN_easy_cr_in_read,            0 },
  { CURL_CBAPI_FN_easy_cr_in_resume_from,     0 },
  { CURL_CBAPI_FN_easy_cw_out_cb,             0 },
  { CURL_CBAPI_FN_easy_fdebug,                1 },
  { CURL_CBAPI_FN_easy_fnmatch_data,          0 },
  { CURL_CBAPI_FN_easy_fopensocket,           0 },
  { CURL_CBAPI_FN_easy_fprereq,               0 },
  { CURL_CBAPI_FN_easy_fprogress,             0 },
  { CURL_CBAPI_FN_easy_fread_func,            0 },
  { CURL_CBAPI_FN_easy_fsockopt,              0 },
  { CURL_CBAPI_FN_easy_fsslctx,               0 },
  { CURL_CBAPI_FN_easy_fwrite_rtp,            0 },
  { CURL_CBAPI_FN_easy_fxferinfo,             0 },
  { CURL_CBAPI_FN_easy_ioctl_func,            0 },
  { CURL_CBAPI_FN_easy_resolver_start,        0 },
  { CURL_CBAPI_FN_easy_seek_func,             0 },
  { CURL_CBAPI_FN_easy_ssh_hostkeyfunc,       0 },
  { CURL_CBAPI_FN_easy_ssh_keyfunc,           0 },
  { CURL_CBAPI_FN_easy_trailer_callback,      0 },

  { CURL_CBAPI_FN_multi_ntfy_cb,              0 },
  { CURL_CBAPI_FN_multi_ತಳ್ಳು_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

ಸ್ಥಿರ bool eapi_in_readonly_cb(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_multi *multi = data->multi;
  ಯದಿ(multi && multi->callstack.ಎಣಿಕೆ) {
    size_t i;
    ಪ್ರತಿ(i = 0; i < multi->callstack.ಎಣಿಕೆ; ++i) {
      ಯದಿ(multi->callstack.calls[i] >= CURL_CBAPI_FN_START) {
        uint8_t fn = multi->callstack.calls[i];
        ಯದಿ((fn < CURL_CBAPI_FN_LAST) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_START].set_readonly)
          ಪ್ರತ್ಯಾಗಮ TRUE;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ bool mapi_in_ntfy_cb(ಸಂರಚನಾ Curl_multi *multi)
{
  ಯದಿ(multi && multi->callstack.ಎಣಿಕೆ) {
    size_t i;
    ಪ್ರತಿ(i = 0; i < multi->callstack.ಎಣಿಕೆ; ++i) {
      ಯದಿ(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_api_multi_is_in_callback(ಸಂರಚನಾ Curl_multi *multi)
{
  ಯದಿ(multi && multi->callstack.ಎಣಿಕೆ) {
    size_t i;
    ಪ್ರತಿ(i = 0; i < multi->callstack.ಎಣಿಕೆ; ++i) {
      ಯದಿ(multi->callstack.calls[i] >= CURL_CBAPI_FN_START)
        ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_api_is_in_callback(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(data && data->multi) {
    ಪ್ರತ್ಯಾಗಮ Curl_api_multi_is_in_callback(data->multi);
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_eapi_enter(ಸಂರಚನಾ Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  ಸಂರಚನಾ Curl_easy *data = curl;
  ಧ್ರುವ ಸಂರಚನಾ Curl_eapi_fn_props *fn_props;
  CURLcode result = CURLE_OK;

  guard->depth = 0;

  /* Verify that we got an easy handle we can work with. */
  ಯದಿ(!GOOD_EASY_HANDLE(data) || data->state.internal) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  ಯದಿ(data->mid != UINT32_MAX) {
    ಯದಿ(GOOD_MULTI_HANDLE(data->multi)) {
      ಯದಿ(!Curl_multi_knows_easy(data->multi, data)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ಗಚ್ಛ out;
      }
    }
    ಅನ್ಯಥಾ {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ಗಚ್ಛ out;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(data->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  ಯದಿ(fn >= CURL_EAPI_FN_LAST) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  ಯದಿ(!fn_props->recurse) {
    ಯದಿ(data->callstack.ಎಣಿಕೆ) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        data->callstack.calls[data->callstack.ಎಣಿಕೆ - 1]));
#endif
      result = CURLE_RECURSIVE_API_CALL;
      ಗಚ್ಛ out;
    }
    ಯದಿ(data->multi && data->multi->callstack.ಎಣಿಕೆ) {
#ifdef CURLVERBOSE

      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        data->multi->callstack.calls[data->multi->callstack.ಎಣಿಕೆ - 1]));
#endif
      result = CURLE_RECURSIVE_API_CALL;
      ಗಚ್ಛ out;
    }
  }

  ಯದಿ(!fn_props->readonly && eapi_in_readonly_cb(data)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#endif
    result = CURLE_RECURSIVE_API_CALL;
    ಗಚ್ಛ out;
  }

#ಯದಿ defined(USE_SSL) && defined(USE_MUTEX)
  ಯದಿ(fn_props->no_scache_lock &&
     Curl_ssl_scache_is_locked_by_current_thread(data)) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#endif
    result = CURLE_RECURSIVE_API_CALL;
    ಗಚ್ಛ out;
  }
#endif

  /* all fine, add to data's callstack */
  ಯದಿ(data->callstack.ಎಣಿಕೆ >= CURL_EAPI_MAX_RECURSION) {
    result = CURLE_RECURSIVE_API_CALL;
    ಗಚ್ಛ out;
  }
  data->callstack.calls[data->callstack.ಎಣಿಕೆ] = (uint16_t)fn;
  ++data->callstack.ಎಣಿಕೆ;
  guard->depth = data->callstack.ಎಣಿಕೆ;
  guard->data = fn_props->data_is_killed ? ರಿಕ್ತ : data;

out:
  ಯದಿ(presult)
    *presult = result;
  ಪ್ರತ್ಯಾಗಮ guard->depth > 0;
}

ಶೂನ್ಯ Curl_eapi_leave(ಸಂರಚನಾ Curl_eapi_guard *guard)
{
  ಯದಿ(guard->depth) {
    /* guard->data is set when handle is supposed to stay alive during call */
    ಯದಿ(guard->data && GOOD_EASY_HANDLE(guard->data)) {
      ಯದಿ(guard->depth > guard->data->callstack.ಎಣಿಕೆ) {
        DEBUGASSERT(0); /* something very wrong */
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(guard->depth < guard->data->callstack.ಎಣಿಕೆ) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->data->callstack.ಎಣಿಕೆ = (uint16_t)(guard->depth - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode result)
{
  ವಿಕಲ್ಪ(result) {
  ಪಕ್ಷ CURLE_OK:
    ಪ್ರತ್ಯಾಗಮ CURLHE_OK;
  ಪಕ್ಷ CURLE_BAD_FUNCTION_ARGUMENT:
    ಪ್ರತ್ಯಾಗಮ CURLHE_BAD_ARGUMENT;
  ಪಕ್ಷ CURLE_OUT_OF_MEMORY:
    ಪ್ರತ್ಯಾಗಮ CURLHE_OUT_OF_MEMORY;
  ಪಕ್ಷ CURLE_NOT_BUILT_IN:
    ಪ್ರತ್ಯಾಗಮ CURLHE_NOT_BUILT_IN;
  ಸ್ವಭಾವ:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    ಪ್ರತ್ಯಾಗಮ CURLHE_BAD_ARGUMENT;
  }
}

bool Curl_mapi_enter(ಸಂರಚನಾ Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  ಸಂರಚನಾ Curl_multi *multi = m;
  ಧ್ರುವ ಸಂರಚನಾ Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->depth = 0;

  /* Verify that we got an easy handle we can work with. */
  ಯದಿ(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    ಗಚ್ಛ out;
  }
  ಯದಿ(fn >= CURL_MAPI_FN_LAST) {
    mresult = CURLM_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  ಯದಿ(fn_props->allow_ntfy_cb && mapi_in_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  ಅನ್ಯಥಾ ಯದಿ(!fn_props->recurse && multi->callstack.ಎಣಿಕೆ) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.ಎಣಿಕೆ - 1]));
#endif
    mresult = CURLM_RECURSIVE_API_CALL;
    ಗಚ್ಛ out;
  }

#ಯದಿ defined(USE_SSL) && defined(USE_MUTEX)
  ಯದಿ(fn_props->no_scache_lock && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#endif
    mresult = CURLM_RECURSIVE_API_CALL;
    ಗಚ್ಛ out;
  }
#endif

  /* all fine, add to data's callstack */
  ಯದಿ(multi->callstack.ಎಣಿಕೆ >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    ಗಚ್ಛ out;
  }
  multi->callstack.calls[multi->callstack.ಎಣಿಕೆ] = (uint8_t)fn;
  ++multi->callstack.ಎಣಿಕೆ;
  guard->depth = multi->callstack.ಎಣಿಕೆ;
  guard->multi = fn_props->multi_is_killed ? ರಿಕ್ತ : multi;

out:
  ಯದಿ(pmresult)
    *pmresult = mresult;
  ಪ್ರತ್ಯಾಗಮ guard->depth > 0;
}

ಶೂನ್ಯ Curl_mapi_leave(ಸಂರಚನಾ Curl_mapi_guard *guard)
{
  ಯದಿ(guard->depth) {
    /* guard->data is set when handle is supposed to stay alive during call */
    ಯದಿ(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      ಯದಿ(guard->depth > guard->multi->callstack.ಎಣಿಕೆ) {
        DEBUGASSERT(0); /* something very wrong */
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(guard->depth < guard->multi->callstack.ಎಣಿಕೆ) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.ಎಣಿಕೆ = (uint8_t)(guard->depth - 1);
      }
    }
  }
}

ಶೂನ್ಯ Curl_cbapi_enter(ಸಂರಚನಾ Curl_mapi_guard *guard,
                      ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->depth = 0;

  ಯದಿ(!multi)
    multi = data ? data->multi : ರಿಕ್ತ;
  /* if not multi is involved here, just leave */
  ಯದಿ(!multi)
    ಪ್ರತ್ಯಾಗಮ;
  /* invalid callback specifier? */
  ಯದಿ((fn >= CURL_CBAPI_FN_LAST) || (fn < CURL_CBAPI_FN_START)) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_START].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  ಯದಿ(multi->callstack.ಎಣಿಕೆ >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ;
  }
  multi->callstack.calls[multi->callstack.ಎಣಿಕೆ] = (uint8_t)fn;
  ++multi->callstack.ಎಣಿಕೆ;
  guard->depth = multi->callstack.ಎಣಿಕೆ;
  guard->multi = multi;
}

ಶೂನ್ಯ Curl_cbapi_leave(ಸಂರಚನಾ Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
