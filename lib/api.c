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

#شامل "urldata.h"
#شامل "api.h"
#شامل "curl_threads.h"
#شامل "multiif.h"
#شامل "vtls/vtls_scache.h"

قسمتعریف حرف mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_آخری <= CURL_CBAPI_FN_آغاز) ? 1 : -1];
قسمتعریف حرف cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_آخری - 1) <= UINT8_MAX) ? 1 : -1];
قسمتعریف حرف mapi_گہرائی_سماتا_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

ساخت Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t ڈیٹا_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_تالا; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

ساکن ثابت ساخت Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_آخری] = {
  /* function                   kill rec  RO !sc */
  { CURL_EAPI_FN_easy_cleanup,     1,  0,  0,  0 },
  { CURL_EAPI_FN_easy_duphandle,   0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_getinfo,     0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_سرخی,      0,  1,  1,  0 },
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
  { CURL_EAPI_FN_ws_آغاز_چوکھٹا,   0,  1,  0,  0 },
};

ساخت Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_تالا;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

ساکن ثابت ساخت Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_آخری] = {
  /* function                       kill rec ntfy !scach */
  { CURL_MAPI_FN_multi_جوڑو_handle,     0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_تفویض,         0,  1,   1,  0 },
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

ساخت Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t سیٹ_readonly;    /* only allows readonly, recursive easy calls */
};

ساکن ثابت ساخت Curl_cbapi_fn_props
cbapi_fn_props[CURL_CBAPI_FN_آخری - CURL_CBAPI_FN_آغاز] = {
  { CURL_CBAPI_FN_easy_chunk_bgn,             0 },
  { CURL_CBAPI_FN_easy_chunk_انتہا,             0 },
  { CURL_CBAPI_FN_easy_closesocket,           0 },
  { CURL_CBAPI_FN_easy_cr_اندر_read,            0 },
  { CURL_CBAPI_FN_easy_cr_اندر_resume_from,     0 },
  { CURL_CBAPI_FN_easy_cw_برآمد_cb,             0 },
  { CURL_CBAPI_FN_easy_fdebug,                1 },
  { CURL_CBAPI_FN_easy_fnmatch_ڈیٹا,          0 },
  { CURL_CBAPI_FN_easy_fopensocket,           0 },
  { CURL_CBAPI_FN_easy_fprereq,               0 },
  { CURL_CBAPI_FN_easy_fprogress,             0 },
  { CURL_CBAPI_FN_easy_fread_func,            0 },
  { CURL_CBAPI_FN_easy_fsockopt,              0 },
  { CURL_CBAPI_FN_easy_fsslctx,               0 },
  { CURL_CBAPI_FN_easy_fwrite_rtp,            0 },
  { CURL_CBAPI_FN_easy_fxferinfo,             0 },
  { CURL_CBAPI_FN_easy_ioctl_func,            0 },
  { CURL_CBAPI_FN_easy_resolver_آغاز,        0 },
  { CURL_CBAPI_FN_easy_seek_func,             0 },
  { CURL_CBAPI_FN_easy_ssh_hostkeyfunc,       0 },
  { CURL_CBAPI_FN_easy_ssh_keyfunc,           0 },
  { CURL_CBAPI_FN_easy_trailer_callback,      0 },

  { CURL_CBAPI_FN_multi_ntfy_cb,              0 },
  { CURL_CBAPI_FN_multi_ڈالو_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

ساکن bool eapi_اندر_readonly_cb(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_multi *multi = ڈیٹا->multi;
  اگر(multi && multi->callstack.گنتی) {
    size_t i;
    برائے(i = 0; i < multi->callstack.گنتی; ++i) {
      اگر(multi->callstack.calls[i] >= CURL_CBAPI_FN_آغاز) {
        uint8_t fn = multi->callstack.calls[i];
        اگر((fn < CURL_CBAPI_FN_آخری) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_آغاز].سیٹ_readonly)
          واپس TRUE;
      }
    }
  }
  واپس FALSE;
}

ساکن bool mapi_اندر_ntfy_cb(ساخت Curl_multi *multi)
{
  اگر(multi && multi->callstack.گنتی) {
    size_t i;
    برائے(i = 0; i < multi->callstack.گنتی; ++i) {
      اگر(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        واپس TRUE;
    }
  }
  واپس FALSE;
}

bool Curl_api_multi_is_اندر_callback(ساخت Curl_multi *multi)
{
  اگر(multi && multi->callstack.گنتی) {
    size_t i;
    برائے(i = 0; i < multi->callstack.گنتی; ++i) {
      اگر(multi->callstack.calls[i] >= CURL_CBAPI_FN_آغاز)
        واپس TRUE;
    }
  }
  واپس FALSE;
}

bool Curl_api_is_اندر_callback(ساخت Curl_easy *ڈیٹا)
{
  اگر(ڈیٹا && ڈیٹا->multi) {
    واپس Curl_api_multi_is_اندر_callback(ڈیٹا->multi);
  }
  واپس FALSE;
}

bool Curl_eapi_enter(ساخت Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  ساخت Curl_easy *ڈیٹا = curl;
  ثابت ساخت Curl_eapi_fn_props *fn_props;
  CURLcode نتیجہ = CURLE_OK;

  guard->گہرائی = 0;

  /* Verify that we got an easy handle we can work with. */
  اگر(!GOOD_EASY_HANDLE(ڈیٹا) || ڈیٹا->state.اندرونی_2) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  اگر(ڈیٹا->mid != UINT32_MAX) {
    اگر(GOOD_MULTI_HANDLE(ڈیٹا->multi)) {
      اگر(!Curl_multi_knows_easy(ڈیٹا->multi, ڈیٹا)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        جاؤ برآمد;
      }
    }
    ورنہ {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }
  }
  ورنہ اگر(ڈیٹا->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  اگر(fn >= CURL_EAPI_FN_آخری) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  اگر(!fn_props->recurse) {
    اگر(ڈیٹا->callstack.گنتی) {
#اگر_تعریف_شدہ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        ڈیٹا->callstack.calls[ڈیٹا->callstack.گنتی - 1]));
#اگر_ختم
      نتیجہ = CURLE_RECURSIVE_API_CALL;
      جاؤ برآمد;
    }
    اگر(ڈیٹا->multi && ڈیٹا->multi->callstack.گنتی) {
#اگر_تعریف_شدہ CURLVERBOSE

      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        ڈیٹا->multi->callstack.calls[ڈیٹا->multi->callstack.گنتی - 1]));
#اگر_ختم
      نتیجہ = CURLE_RECURSIVE_API_CALL;
      جاؤ برآمد;
    }
  }

  اگر(!fn_props->readonly && eapi_اندر_readonly_cb(ڈیٹا)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#اگر_تعریف_شدہ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#اگر_ختم
    نتیجہ = CURLE_RECURSIVE_API_CALL;
    جاؤ برآمد;
  }

#اگر defined(USE_SSL) && defined(USE_MUTEX)
  اگر(fn_props->no_scache_تالا &&
     Curl_ssl_scache_is_locked_by_current_thread(ڈیٹا)) {
#اگر_تعریف_شدہ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#اگر_ختم
    نتیجہ = CURLE_RECURSIVE_API_CALL;
    جاؤ برآمد;
  }
#اگر_ختم

  /* all fine, add to data's callstack */
  اگر(ڈیٹا->callstack.گنتی >= CURL_EAPI_MAX_RECURSION) {
    نتیجہ = CURLE_RECURSIVE_API_CALL;
    جاؤ برآمد;
  }
  ڈیٹا->callstack.calls[ڈیٹا->callstack.گنتی] = (uint16_t)fn;
  ++ڈیٹا->callstack.گنتی;
  guard->گہرائی = ڈیٹا->callstack.گنتی;
  guard->ڈیٹا = fn_props->ڈیٹا_is_killed ? تہی : ڈیٹا;

برآمد:
  اگر(presult)
    *presult = نتیجہ;
  واپس guard->گہرائی > 0;
}

خالی Curl_eapi_leave(ساخت Curl_eapi_guard *guard)
{
  اگر(guard->گہرائی) {
    /* guard->data is set when handle is supposed to stay alive during call */
    اگر(guard->ڈیٹا && GOOD_EASY_HANDLE(guard->ڈیٹا)) {
      اگر(guard->گہرائی > guard->ڈیٹا->callstack.گنتی) {
        DEBUGASSERT(0); /* something very wrong */
      }
      ورنہ {
        اگر(guard->گہرائی < guard->ڈیٹا->callstack.گنتی) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->ڈیٹا->callstack.گنتی = (uint16_t)(guard->گہرائی - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode نتیجہ)
{
  انتخاب(نتیجہ) {
  حالت CURLE_OK:
    واپس CURLHE_OK;
  حالت CURLE_BAD_FUNCTION_دلیل:
    واپس CURLHE_BAD_دلیل;
  حالت CURLE_برآمد_کا_حافظہ:
    واپس CURLHE_برآمد_کا_حافظہ;
  حالت CURLE_NOT_BUILT_اندر:
    واپس CURLHE_NOT_BUILT_اندر;
  طےشدہ:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    واپس CURLHE_BAD_دلیل;
  }
}

bool Curl_mapi_enter(ساخت Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  ساخت Curl_multi *multi = m;
  ثابت ساخت Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->گہرائی = 0;

  /* Verify that we got an easy handle we can work with. */
  اگر(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    جاؤ برآمد;
  }
  اگر(fn >= CURL_MAPI_FN_آخری) {
    mresult = CURLM_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  اگر(fn_props->allow_ntfy_cb && mapi_اندر_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  ورنہ اگر(!fn_props->recurse && multi->callstack.گنتی) {
#اگر_تعریف_شدہ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.گنتی - 1]));
#اگر_ختم
    mresult = CURLM_RECURSIVE_API_CALL;
    جاؤ برآمد;
  }

#اگر defined(USE_SSL) && defined(USE_MUTEX)
  اگر(fn_props->no_scache_تالا && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#اگر_تعریف_شدہ CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#اگر_ختم
    mresult = CURLM_RECURSIVE_API_CALL;
    جاؤ برآمد;
  }
#اگر_ختم

  /* all fine, add to data's callstack */
  اگر(multi->callstack.گنتی >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    جاؤ برآمد;
  }
  multi->callstack.calls[multi->callstack.گنتی] = (uint8_t)fn;
  ++multi->callstack.گنتی;
  guard->گہرائی = multi->callstack.گنتی;
  guard->multi = fn_props->multi_is_killed ? تہی : multi;

برآمد:
  اگر(pmresult)
    *pmresult = mresult;
  واپس guard->گہرائی > 0;
}

خالی Curl_mapi_leave(ساخت Curl_mapi_guard *guard)
{
  اگر(guard->گہرائی) {
    /* guard->data is set when handle is supposed to stay alive during call */
    اگر(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      اگر(guard->گہرائی > guard->multi->callstack.گنتی) {
        DEBUGASSERT(0); /* something very wrong */
      }
      ورنہ {
        اگر(guard->گہرائی < guard->multi->callstack.گنتی) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.گنتی = (uint8_t)(guard->گہرائی - 1);
      }
    }
  }
}

خالی Curl_cbapi_enter(ساخت Curl_mapi_guard *guard,
                      ساخت Curl_easy *ڈیٹا,
                      ساخت Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->گہرائی = 0;

  اگر(!multi)
    multi = ڈیٹا ? ڈیٹا->multi : تہی;
  /* if not multi is involved here, just leave */
  اگر(!multi)
    واپس;
  /* invalid callback specifier? */
  اگر((fn >= CURL_CBAPI_FN_آخری) || (fn < CURL_CBAPI_FN_آغاز)) {
    DEBUGASSERT(0);
    واپس;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_آغاز].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  اگر(multi->callstack.گنتی >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    واپس;
  }
  multi->callstack.calls[multi->callstack.گنتی] = (uint8_t)fn;
  ++multi->callstack.گنتی;
  guard->گہرائی = multi->callstack.گنتی;
  guard->multi = multi;
}

خالی Curl_cbapi_leave(ساخت Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
