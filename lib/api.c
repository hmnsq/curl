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

#ضمن "urldata.h"
#ضمن "api.h"
#ضمن "curl_threads.h"
#ضمن "multiif.h"
#ضمن "vtls/vtls_scache.h"

تعريفنوع حرف mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_وروستی <= CURL_CBAPI_FN_پیل) ? 1 : -1];
تعريفنوع حرف cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_وروستی - 1) <= UINT8_MAX) ? 1 : -1];
تعريفنوع حرف mapi_ژوروالی_ځاییږي_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

بنية Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t معلومات_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_قلف; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

ساكن ثابت بنية Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_وروستی] = {
  /* function                   kill rec  RO !sc */
  { CURL_EAPI_FN_easy_cleanup,     1,  0,  0,  0 },
  { CURL_EAPI_FN_easy_duphandle,   0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_getinfo,     0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_سرلیک,      0,  1,  1,  0 },
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
  { CURL_EAPI_FN_ws_پیل_چوکاټ,   0,  1,  0,  0 },
};

بنية Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_قلف;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

ساكن ثابت بنية Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_وروستی] = {
  /* function                       kill rec ntfy !scach */
  { CURL_MAPI_FN_multi_جمع_کړه_handle,     0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_ټاکل,         0,  1,   1,  0 },
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

بنية Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t سېټ_readonly;    /* only allows readonly, recursive easy calls */
};

ساكن ثابت بنية Curl_cbapi_fn_props
cbapi_fn_props[CURL_CBAPI_FN_وروستی - CURL_CBAPI_FN_پیل] = {
  { CURL_CBAPI_FN_easy_chunk_bgn,             0 },
  { CURL_CBAPI_FN_easy_chunk_پای_ټکی,             0 },
  { CURL_CBAPI_FN_easy_closesocket,           0 },
  { CURL_CBAPI_FN_easy_cr_دننه_read,            0 },
  { CURL_CBAPI_FN_easy_cr_دننه_resume_from,     0 },
  { CURL_CBAPI_FN_easy_cw_بهر_cb,             0 },
  { CURL_CBAPI_FN_easy_fdebug,                1 },
  { CURL_CBAPI_FN_easy_fnmatch_معلومات,          0 },
  { CURL_CBAPI_FN_easy_fopensocket,           0 },
  { CURL_CBAPI_FN_easy_fprereq,               0 },
  { CURL_CBAPI_FN_easy_fprogress,             0 },
  { CURL_CBAPI_FN_easy_fread_func,            0 },
  { CURL_CBAPI_FN_easy_fsockopt,              0 },
  { CURL_CBAPI_FN_easy_fsslctx,               0 },
  { CURL_CBAPI_FN_easy_fwrite_rtp,            0 },
  { CURL_CBAPI_FN_easy_fxferinfo,             0 },
  { CURL_CBAPI_FN_easy_ioctl_func,            0 },
  { CURL_CBAPI_FN_easy_resolver_پیل,        0 },
  { CURL_CBAPI_FN_easy_seek_func,             0 },
  { CURL_CBAPI_FN_easy_ssh_hostkeyfunc,       0 },
  { CURL_CBAPI_FN_easy_ssh_keyfunc,           0 },
  { CURL_CBAPI_FN_easy_trailer_callback,      0 },

  { CURL_CBAPI_FN_multi_ntfy_cb,              0 },
  { CURL_CBAPI_FN_multi_ټېل_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

ساكن bool eapi_دننه_readonly_cb(بنية Curl_easy *معلومات)
{
  بنية Curl_multi *multi = معلومات->multi;
  إذا(multi && multi->callstack.شمېر) {
    size_t i;
    لكل(i = 0; i < multi->callstack.شمېر; ++i) {
      إذا(multi->callstack.calls[i] >= CURL_CBAPI_FN_پیل) {
        uint8_t fn = multi->callstack.calls[i];
        إذا((fn < CURL_CBAPI_FN_وروستی) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_پیل].سېټ_readonly)
          أرجع TRUE;
      }
    }
  }
  أرجع FALSE;
}

ساكن bool mapi_دننه_ntfy_cb(بنية Curl_multi *multi)
{
  إذا(multi && multi->callstack.شمېر) {
    size_t i;
    لكل(i = 0; i < multi->callstack.شمېر; ++i) {
      إذا(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        أرجع TRUE;
    }
  }
  أرجع FALSE;
}

bool Curl_api_multi_is_دننه_callback(بنية Curl_multi *multi)
{
  إذا(multi && multi->callstack.شمېر) {
    size_t i;
    لكل(i = 0; i < multi->callstack.شمېر; ++i) {
      إذا(multi->callstack.calls[i] >= CURL_CBAPI_FN_پیل)
        أرجع TRUE;
    }
  }
  أرجع FALSE;
}

bool Curl_api_is_دننه_callback(بنية Curl_easy *معلومات)
{
  إذا(معلومات && معلومات->multi) {
    أرجع Curl_api_multi_is_دننه_callback(معلومات->multi);
  }
  أرجع FALSE;
}

bool Curl_eapi_enter(بنية Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  بنية Curl_easy *معلومات = curl;
  ثابت بنية Curl_eapi_fn_props *fn_props;
  CURLcode پایله = CURLE_OK;

  guard->ژوروالی = 0;

  /* Verify that we got an easy handle we can work with. */
  إذا(!GOOD_EASY_HANDLE(معلومات) || معلومات->state.دننی) {
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  إذا(معلومات->mid != UINT32_MAX) {
    إذا(GOOD_MULTI_HANDLE(معلومات->multi)) {
      إذا(!Curl_multi_knows_easy(معلومات->multi, معلومات)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        پایله = CURLE_BAD_FUNCTION_دلیل;
        اذهب بهر;
      }
    }
    وإلا {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      پایله = CURLE_BAD_FUNCTION_دلیل;
      اذهب بهر;
    }
  }
  وإلا إذا(معلومات->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  إذا(fn >= CURL_EAPI_FN_وروستی) {
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  إذا(!fn_props->recurse) {
    إذا(معلومات->callstack.شمېر) {
#که_تعریف_شوی CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        معلومات->callstack.calls[معلومات->callstack.شمېر - 1]));
#د_که_پای
      پایله = CURLE_RECURSIVE_API_CALL;
      اذهب بهر;
    }
    إذا(معلومات->multi && معلومات->multi->callstack.شمېر) {
#که_تعریف_شوی CURLVERBOSE

      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        معلومات->multi->callstack.calls[معلومات->multi->callstack.شمېر - 1]));
#د_که_پای
      پایله = CURLE_RECURSIVE_API_CALL;
      اذهب بهر;
    }
  }

  إذا(!fn_props->readonly && eapi_دننه_readonly_cb(معلومات)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#که_تعریف_شوی CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#د_که_پای
    پایله = CURLE_RECURSIVE_API_CALL;
    اذهب بهر;
  }

#إذا defined(USE_SSL) && defined(USE_MUTEX)
  إذا(fn_props->no_scache_قلف &&
     Curl_ssl_scache_is_locked_by_current_thread(معلومات)) {
#که_تعریف_شوی CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#د_که_پای
    پایله = CURLE_RECURSIVE_API_CALL;
    اذهب بهر;
  }
#د_که_پای

  /* all fine, add to data's callstack */
  إذا(معلومات->callstack.شمېر >= CURL_EAPI_MAX_RECURSION) {
    پایله = CURLE_RECURSIVE_API_CALL;
    اذهب بهر;
  }
  معلومات->callstack.calls[معلومات->callstack.شمېر] = (uint16_t)fn;
  ++معلومات->callstack.شمېر;
  guard->ژوروالی = معلومات->callstack.شمېر;
  guard->معلومات = fn_props->معلومات_is_killed ? عدم : معلومات;

بهر:
  إذا(presult)
    *presult = پایله;
  أرجع guard->ژوروالی > 0;
}

فراغ Curl_eapi_leave(بنية Curl_eapi_guard *guard)
{
  إذا(guard->ژوروالی) {
    /* guard->data is set when handle is supposed to stay alive during call */
    إذا(guard->معلومات && GOOD_EASY_HANDLE(guard->معلومات)) {
      إذا(guard->ژوروالی > guard->معلومات->callstack.شمېر) {
        DEBUGASSERT(0); /* something very wrong */
      }
      وإلا {
        إذا(guard->ژوروالی < guard->معلومات->callstack.شمېر) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->معلومات->callstack.شمېر = (uint16_t)(guard->ژوروالی - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode پایله)
{
  بدل(پایله) {
  حالة CURLE_OK:
    أرجع CURLHE_OK;
  حالة CURLE_BAD_FUNCTION_دلیل:
    أرجع CURLHE_BAD_دلیل;
  حالة CURLE_بهر_د_حافظه:
    أرجع CURLHE_بهر_د_حافظه;
  حالة CURLE_NOT_BUILT_دننه:
    أرجع CURLHE_NOT_BUILT_دننه;
  افتراضي:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    أرجع CURLHE_BAD_دلیل;
  }
}

bool Curl_mapi_enter(بنية Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  بنية Curl_multi *multi = m;
  ثابت بنية Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->ژوروالی = 0;

  /* Verify that we got an easy handle we can work with. */
  إذا(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    اذهب بهر;
  }
  إذا(fn >= CURL_MAPI_FN_وروستی) {
    mresult = CURLM_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  إذا(fn_props->allow_ntfy_cb && mapi_دننه_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  وإلا إذا(!fn_props->recurse && multi->callstack.شمېر) {
#که_تعریف_شوی CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.شمېر - 1]));
#د_که_پای
    mresult = CURLM_RECURSIVE_API_CALL;
    اذهب بهر;
  }

#إذا defined(USE_SSL) && defined(USE_MUTEX)
  إذا(fn_props->no_scache_قلف && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#که_تعریف_شوی CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#د_که_پای
    mresult = CURLM_RECURSIVE_API_CALL;
    اذهب بهر;
  }
#د_که_پای

  /* all fine, add to data's callstack */
  إذا(multi->callstack.شمېر >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    اذهب بهر;
  }
  multi->callstack.calls[multi->callstack.شمېر] = (uint8_t)fn;
  ++multi->callstack.شمېر;
  guard->ژوروالی = multi->callstack.شمېر;
  guard->multi = fn_props->multi_is_killed ? عدم : multi;

بهر:
  إذا(pmresult)
    *pmresult = mresult;
  أرجع guard->ژوروالی > 0;
}

فراغ Curl_mapi_leave(بنية Curl_mapi_guard *guard)
{
  إذا(guard->ژوروالی) {
    /* guard->data is set when handle is supposed to stay alive during call */
    إذا(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      إذا(guard->ژوروالی > guard->multi->callstack.شمېر) {
        DEBUGASSERT(0); /* something very wrong */
      }
      وإلا {
        إذا(guard->ژوروالی < guard->multi->callstack.شمېر) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.شمېر = (uint8_t)(guard->ژوروالی - 1);
      }
    }
  }
}

فراغ Curl_cbapi_enter(بنية Curl_mapi_guard *guard,
                      بنية Curl_easy *معلومات,
                      بنية Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->ژوروالی = 0;

  إذا(!multi)
    multi = معلومات ? معلومات->multi : عدم;
  /* if not multi is involved here, just leave */
  إذا(!multi)
    أرجع;
  /* invalid callback specifier? */
  إذا((fn >= CURL_CBAPI_FN_وروستی) || (fn < CURL_CBAPI_FN_پیل)) {
    DEBUGASSERT(0);
    أرجع;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_پیل].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  إذا(multi->callstack.شمېر >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    أرجع;
  }
  multi->callstack.calls[multi->callstack.شمېر] = (uint8_t)fn;
  ++multi->callstack.شمېر;
  guard->ژوروالی = multi->callstack.شمېر;
  guard->multi = multi;
}

فراغ Curl_cbapi_leave(بنية Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
