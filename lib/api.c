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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "api.h"
#অন্তর্ভাব "curl_threads.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "vtls/vtls_scache.h"

প্রকারলেখ অক্ষর mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_LAST <= CURL_CBAPI_FN_START) ? 1 : -1];
প্রকারলেখ অক্ষর cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_LAST - 1) <= UINT8_MAX) ? 1 : -1];
প্রকারলেখ অক্ষর mapi_depth_fits_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

কাঠামো Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t তথ্য_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_lock; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

স্থির ধ্রুবক কাঠামো Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_LAST] = {
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

কাঠামো Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_lock;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

স্থির ধ্রুবক কাঠামো Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_LAST] = {
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

কাঠামো Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t set_readonly;    /* only allows readonly, recursive easy calls */
};

স্থির ধ্রুবক কাঠামো Curl_cbapi_fn_props
cbapi_fn_props[CURL_CBAPI_FN_LAST - CURL_CBAPI_FN_START] = {
  { CURL_CBAPI_FN_easy_chunk_bgn,             0 },
  { CURL_CBAPI_FN_easy_chunk_end,             0 },
  { CURL_CBAPI_FN_easy_closesocket,           0 },
  { CURL_CBAPI_FN_easy_cr_in_read,            0 },
  { CURL_CBAPI_FN_easy_cr_in_resume_from,     0 },
  { CURL_CBAPI_FN_easy_cw_out_cb,             0 },
  { CURL_CBAPI_FN_easy_fdebug,                1 },
  { CURL_CBAPI_FN_easy_fnmatch_তথ্য,          0 },
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
  { CURL_CBAPI_FN_multi_ঠেলো_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

স্থির bool eapi_in_readonly_cb(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_multi *multi = তথ্য->multi;
  যদি(multi && multi->callstack.গুনতি) {
    size_t i;
    জন্য(i = 0; i < multi->callstack.গুনতি; ++i) {
      যদি(multi->callstack.calls[i] >= CURL_CBAPI_FN_START) {
        uint8_t fn = multi->callstack.calls[i];
        যদি((fn < CURL_CBAPI_FN_LAST) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_START].set_readonly)
          ফেরত TRUE;
      }
    }
  }
  ফেরত FALSE;
}

স্থির bool mapi_in_ntfy_cb(কাঠামো Curl_multi *multi)
{
  যদি(multi && multi->callstack.গুনতি) {
    size_t i;
    জন্য(i = 0; i < multi->callstack.গুনতি; ++i) {
      যদি(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        ফেরত TRUE;
    }
  }
  ফেরত FALSE;
}

bool Curl_api_multi_is_in_callback(কাঠামো Curl_multi *multi)
{
  যদি(multi && multi->callstack.গুনতি) {
    size_t i;
    জন্য(i = 0; i < multi->callstack.গুনতি; ++i) {
      যদি(multi->callstack.calls[i] >= CURL_CBAPI_FN_START)
        ফেরত TRUE;
    }
  }
  ফেরত FALSE;
}

bool Curl_api_is_in_callback(কাঠামো Curl_easy *তথ্য)
{
  যদি(তথ্য && তথ্য->multi) {
    ফেরত Curl_api_multi_is_in_callback(তথ্য->multi);
  }
  ফেরত FALSE;
}

bool Curl_eapi_enter(কাঠামো Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  কাঠামো Curl_easy *তথ্য = curl;
  ধ্রুবক কাঠামো Curl_eapi_fn_props *fn_props;
  CURLcode ফলাফল = CURLE_OK;

  guard->depth = 0;

  /* Verify that we got an easy handle we can work with. */
  যদি(!GOOD_EASY_HANDLE(তথ্য) || তথ্য->state.internal) {
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  যদি(তথ্য->mid != UINT32_MAX) {
    যদি(GOOD_MULTI_HANDLE(তথ্য->multi)) {
      যদি(!Curl_multi_knows_easy(তথ্য->multi, তথ্য)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
        যাও out;
      }
    }
    অন্যথা {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }
  }
  অন্যথা যদি(তথ্য->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  যদি(fn >= CURL_EAPI_FN_LAST) {
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  যদি(!fn_props->recurse) {
    যদি(তথ্য->callstack.গুনতি) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        তথ্য->callstack.calls[তথ্য->callstack.গুনতি - 1]));
#endif
      ফলাফল = CURLE_RECURSIVE_API_CALL;
      যাও out;
    }
    যদি(তথ্য->multi && তথ্য->multi->callstack.গুনতি) {
#ifdef CURLVERBOSE

      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        তথ্য->multi->callstack.calls[তথ্য->multi->callstack.গুনতি - 1]));
#endif
      ফলাফল = CURLE_RECURSIVE_API_CALL;
      যাও out;
    }
  }

  যদি(!fn_props->readonly && eapi_in_readonly_cb(তথ্য)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#endif
    ফলাফল = CURLE_RECURSIVE_API_CALL;
    যাও out;
  }

#যদি defined(USE_SSL) && defined(USE_MUTEX)
  যদি(fn_props->no_scache_lock &&
     Curl_ssl_scache_is_locked_by_current_thread(তথ্য)) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#endif
    ফলাফল = CURLE_RECURSIVE_API_CALL;
    যাও out;
  }
#endif

  /* all fine, add to data's callstack */
  যদি(তথ্য->callstack.গুনতি >= CURL_EAPI_MAX_RECURSION) {
    ফলাফল = CURLE_RECURSIVE_API_CALL;
    যাও out;
  }
  তথ্য->callstack.calls[তথ্য->callstack.গুনতি] = (uint16_t)fn;
  ++তথ্য->callstack.গুনতি;
  guard->depth = তথ্য->callstack.গুনতি;
  guard->তথ্য = fn_props->তথ্য_is_killed ? ফাঁকা : তথ্য;

out:
  যদি(presult)
    *presult = ফলাফল;
  ফেরত guard->depth > 0;
}

শূন্য Curl_eapi_leave(কাঠামো Curl_eapi_guard *guard)
{
  যদি(guard->depth) {
    /* guard->data is set when handle is supposed to stay alive during call */
    যদি(guard->তথ্য && GOOD_EASY_HANDLE(guard->তথ্য)) {
      যদি(guard->depth > guard->তথ্য->callstack.গুনতি) {
        DEBUGASSERT(0); /* something very wrong */
      }
      অন্যথা {
        যদি(guard->depth < guard->তথ্য->callstack.গুনতি) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->তথ্য->callstack.গুনতি = (uint16_t)(guard->depth - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode ফলাফল)
{
  নির্বাচন(ফলাফল) {
  ক্ষেত্র CURLE_OK:
    ফেরত CURLHE_OK;
  ক্ষেত্র CURLE_BAD_FUNCTION_ARGUMENT:
    ফেরত CURLHE_BAD_ARGUMENT;
  ক্ষেত্র CURLE_OUT_OF_MEMORY:
    ফেরত CURLHE_OUT_OF_MEMORY;
  ক্ষেত্র CURLE_NOT_BUILT_IN:
    ফেরত CURLHE_NOT_BUILT_IN;
  পূর্বনির্ধারিত:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    ফেরত CURLHE_BAD_ARGUMENT;
  }
}

bool Curl_mapi_enter(কাঠামো Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  কাঠামো Curl_multi *multi = m;
  ধ্রুবক কাঠামো Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->depth = 0;

  /* Verify that we got an easy handle we can work with. */
  যদি(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    যাও out;
  }
  যদি(fn >= CURL_MAPI_FN_LAST) {
    mresult = CURLM_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  যদি(fn_props->allow_ntfy_cb && mapi_in_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  অন্যথা যদি(!fn_props->recurse && multi->callstack.গুনতি) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.গুনতি - 1]));
#endif
    mresult = CURLM_RECURSIVE_API_CALL;
    যাও out;
  }

#যদি defined(USE_SSL) && defined(USE_MUTEX)
  যদি(fn_props->no_scache_lock && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#endif
    mresult = CURLM_RECURSIVE_API_CALL;
    যাও out;
  }
#endif

  /* all fine, add to data's callstack */
  যদি(multi->callstack.গুনতি >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    যাও out;
  }
  multi->callstack.calls[multi->callstack.গুনতি] = (uint8_t)fn;
  ++multi->callstack.গুনতি;
  guard->depth = multi->callstack.গুনতি;
  guard->multi = fn_props->multi_is_killed ? ফাঁকা : multi;

out:
  যদি(pmresult)
    *pmresult = mresult;
  ফেরত guard->depth > 0;
}

শূন্য Curl_mapi_leave(কাঠামো Curl_mapi_guard *guard)
{
  যদি(guard->depth) {
    /* guard->data is set when handle is supposed to stay alive during call */
    যদি(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      যদি(guard->depth > guard->multi->callstack.গুনতি) {
        DEBUGASSERT(0); /* something very wrong */
      }
      অন্যথা {
        যদি(guard->depth < guard->multi->callstack.গুনতি) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.গুনতি = (uint8_t)(guard->depth - 1);
      }
    }
  }
}

শূন্য Curl_cbapi_enter(কাঠামো Curl_mapi_guard *guard,
                      কাঠামো Curl_easy *তথ্য,
                      কাঠামো Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->depth = 0;

  যদি(!multi)
    multi = তথ্য ? তথ্য->multi : ফাঁকা;
  /* if not multi is involved here, just leave */
  যদি(!multi)
    ফেরত;
  /* invalid callback specifier? */
  যদি((fn >= CURL_CBAPI_FN_LAST) || (fn < CURL_CBAPI_FN_START)) {
    DEBUGASSERT(0);
    ফেরত;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_START].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  যদি(multi->callstack.গুনতি >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    ফেরত;
  }
  multi->callstack.calls[multi->callstack.গুনতি] = (uint8_t)fn;
  ++multi->callstack.গুনতি;
  guard->depth = multi->callstack.গুনতি;
  guard->multi = multi;
}

শূন্য Curl_cbapi_leave(কাঠামো Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
