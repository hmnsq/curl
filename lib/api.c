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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "api.h"
#அந்தர்பாவ "curl_threads.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "vtls/vtls_scache.h"

வகைவரை எழுத்து mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_LAST <= CURL_CBAPI_FN_START) ? 1 : -1];
வகைவரை எழுத்து cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_LAST - 1) <= UINT8_MAX) ? 1 : -1];
வகைவரை எழுத்து mapi_depth_fits_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

கட்டமைப்பு Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t data_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_lock; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

நிலையான மாறிலி கட்டமைப்பு Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_LAST] = {
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

கட்டமைப்பு Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_lock;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

நிலையான மாறிலி கட்டமைப்பு Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_LAST] = {
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

கட்டமைப்பு Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t set_readonly;    /* only allows readonly, recursive easy calls */
};

நிலையான மாறிலி கட்டமைப்பு Curl_cbapi_fn_props
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
  { CURL_CBAPI_FN_multi_தள்ளு_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

நிலையான bool eapi_in_readonly_cb(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_multi *multi = data->multi;
  எனில்(multi && multi->callstack.மொத்தம்) {
    size_t i;
    க்கு(i = 0; i < multi->callstack.மொத்தம்; ++i) {
      எனில்(multi->callstack.calls[i] >= CURL_CBAPI_FN_START) {
        uint8_t fn = multi->callstack.calls[i];
        எனில்((fn < CURL_CBAPI_FN_LAST) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_START].set_readonly)
          திரும்பு TRUE;
      }
    }
  }
  திரும்பு FALSE;
}

நிலையான bool mapi_in_ntfy_cb(கட்டமைப்பு Curl_multi *multi)
{
  எனில்(multi && multi->callstack.மொத்தம்) {
    size_t i;
    க்கு(i = 0; i < multi->callstack.மொத்தம்; ++i) {
      எனில்(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        திரும்பு TRUE;
    }
  }
  திரும்பு FALSE;
}

bool Curl_api_multi_is_in_callback(கட்டமைப்பு Curl_multi *multi)
{
  எனில்(multi && multi->callstack.மொத்தம்) {
    size_t i;
    க்கு(i = 0; i < multi->callstack.மொத்தம்; ++i) {
      எனில்(multi->callstack.calls[i] >= CURL_CBAPI_FN_START)
        திரும்பு TRUE;
    }
  }
  திரும்பு FALSE;
}

bool Curl_api_is_in_callback(கட்டமைப்பு Curl_easy *data)
{
  எனில்(data && data->multi) {
    திரும்பு Curl_api_multi_is_in_callback(data->multi);
  }
  திரும்பு FALSE;
}

bool Curl_eapi_enter(கட்டமைப்பு Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  கட்டமைப்பு Curl_easy *data = curl;
  மாறிலி கட்டமைப்பு Curl_eapi_fn_props *fn_props;
  CURLcode result = CURLE_OK;

  guard->depth = 0;

  /* Verify that we got an easy handle we can work with. */
  எனில்(!GOOD_EASY_HANDLE(data) || data->state.internal) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  எனில்(data->mid != UINT32_MAX) {
    எனில்(GOOD_MULTI_HANDLE(data->multi)) {
      எனில்(!Curl_multi_knows_easy(data->multi, data)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        செல் out;
      }
    }
    இல்லையெனில் {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      செல் out;
    }
  }
  இல்லையெனில் எனில்(data->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  எனில்(fn >= CURL_EAPI_FN_LAST) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  எனில்(!fn_props->recurse) {
    எனில்(data->callstack.மொத்தம்) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        data->callstack.calls[data->callstack.மொத்தம் - 1]));
#endif
      result = CURLE_RECURSIVE_API_CALL;
      செல் out;
    }
    எனில்(data->multi && data->multi->callstack.மொத்தம்) {
#ifdef CURLVERBOSE

      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        data->multi->callstack.calls[data->multi->callstack.மொத்தம் - 1]));
#endif
      result = CURLE_RECURSIVE_API_CALL;
      செல் out;
    }
  }

  எனில்(!fn_props->readonly && eapi_in_readonly_cb(data)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#endif
    result = CURLE_RECURSIVE_API_CALL;
    செல் out;
  }

#எனில் defined(USE_SSL) && defined(USE_MUTEX)
  எனில்(fn_props->no_scache_lock &&
     Curl_ssl_scache_is_locked_by_current_thread(data)) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#endif
    result = CURLE_RECURSIVE_API_CALL;
    செல் out;
  }
#endif

  /* all fine, add to data's callstack */
  எனில்(data->callstack.மொத்தம் >= CURL_EAPI_MAX_RECURSION) {
    result = CURLE_RECURSIVE_API_CALL;
    செல் out;
  }
  data->callstack.calls[data->callstack.மொத்தம்] = (uint16_t)fn;
  ++data->callstack.மொத்தம்;
  guard->depth = data->callstack.மொத்தம்;
  guard->data = fn_props->data_is_killed ? வெற்று : data;

out:
  எனில்(presult)
    *presult = result;
  திரும்பு guard->depth > 0;
}

வெறுமை Curl_eapi_leave(கட்டமைப்பு Curl_eapi_guard *guard)
{
  எனில்(guard->depth) {
    /* guard->data is set when handle is supposed to stay alive during call */
    எனில்(guard->data && GOOD_EASY_HANDLE(guard->data)) {
      எனில்(guard->depth > guard->data->callstack.மொத்தம்) {
        DEBUGASSERT(0); /* something very wrong */
      }
      இல்லையெனில் {
        எனில்(guard->depth < guard->data->callstack.மொத்தம்) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->data->callstack.மொத்தம் = (uint16_t)(guard->depth - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode result)
{
  தேர்வு(result) {
  நிலை CURLE_OK:
    திரும்பு CURLHE_OK;
  நிலை CURLE_BAD_FUNCTION_ARGUMENT:
    திரும்பு CURLHE_BAD_ARGUMENT;
  நிலை CURLE_OUT_OF_MEMORY:
    திரும்பு CURLHE_OUT_OF_MEMORY;
  நிலை CURLE_NOT_BUILT_IN:
    திரும்பு CURLHE_NOT_BUILT_IN;
  இயல்பு:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    திரும்பு CURLHE_BAD_ARGUMENT;
  }
}

bool Curl_mapi_enter(கட்டமைப்பு Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  கட்டமைப்பு Curl_multi *multi = m;
  மாறிலி கட்டமைப்பு Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->depth = 0;

  /* Verify that we got an easy handle we can work with. */
  எனில்(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    செல் out;
  }
  எனில்(fn >= CURL_MAPI_FN_LAST) {
    mresult = CURLM_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  எனில்(fn_props->allow_ntfy_cb && mapi_in_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  இல்லையெனில் எனில்(!fn_props->recurse && multi->callstack.மொத்தம்) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.மொத்தம் - 1]));
#endif
    mresult = CURLM_RECURSIVE_API_CALL;
    செல் out;
  }

#எனில் defined(USE_SSL) && defined(USE_MUTEX)
  எனில்(fn_props->no_scache_lock && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#endif
    mresult = CURLM_RECURSIVE_API_CALL;
    செல் out;
  }
#endif

  /* all fine, add to data's callstack */
  எனில்(multi->callstack.மொத்தம் >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    செல் out;
  }
  multi->callstack.calls[multi->callstack.மொத்தம்] = (uint8_t)fn;
  ++multi->callstack.மொத்தம்;
  guard->depth = multi->callstack.மொத்தம்;
  guard->multi = fn_props->multi_is_killed ? வெற்று : multi;

out:
  எனில்(pmresult)
    *pmresult = mresult;
  திரும்பு guard->depth > 0;
}

வெறுமை Curl_mapi_leave(கட்டமைப்பு Curl_mapi_guard *guard)
{
  எனில்(guard->depth) {
    /* guard->data is set when handle is supposed to stay alive during call */
    எனில்(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      எனில்(guard->depth > guard->multi->callstack.மொத்தம்) {
        DEBUGASSERT(0); /* something very wrong */
      }
      இல்லையெனில் {
        எனில்(guard->depth < guard->multi->callstack.மொத்தம்) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.மொத்தம் = (uint8_t)(guard->depth - 1);
      }
    }
  }
}

வெறுமை Curl_cbapi_enter(கட்டமைப்பு Curl_mapi_guard *guard,
                      கட்டமைப்பு Curl_easy *data,
                      கட்டமைப்பு Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->depth = 0;

  எனில்(!multi)
    multi = data ? data->multi : வெற்று;
  /* if not multi is involved here, just leave */
  எனில்(!multi)
    திரும்பு;
  /* invalid callback specifier? */
  எனில்((fn >= CURL_CBAPI_FN_LAST) || (fn < CURL_CBAPI_FN_START)) {
    DEBUGASSERT(0);
    திரும்பு;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_START].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  எனில்(multi->callstack.மொத்தம் >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    திரும்பு;
  }
  multi->callstack.calls[multi->callstack.மொத்தம்] = (uint8_t)fn;
  ++multi->callstack.மொத்தம்;
  guard->depth = multi->callstack.மொத்தம்;
  guard->multi = multi;
}

வெறுமை Curl_cbapi_leave(கட்டமைப்பு Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
