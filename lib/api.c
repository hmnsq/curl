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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "api.h"
#ଅନ୍ତର୍ଭାଵ "curl_threads.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଅକ୍ଷର mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_LAST <= CURL_CBAPI_FN_START) ? 1 : -1];
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଅକ୍ଷର cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_LAST - 1) <= UINT8_MAX) ? 1 : -1];
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଅକ୍ଷର mapi_depth_fits_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

ସଂରଚନା Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t data_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_lock; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_LAST] = {
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

ସଂରଚନା Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_lock;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_LAST] = {
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

ସଂରଚନା Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t set_readonly;    /* only allows readonly, recursive easy calls */
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cbapi_fn_props
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
  { CURL_CBAPI_FN_multi_ଠେଲ_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

ସ୍ଥିର bool eapi_in_readonly_cb(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_multi *multi = data->multi;
  ଯଦି(multi && multi->callstack.ଗଣନା_2) {
    size_t i;
    ପାଇଁ(i = 0; i < multi->callstack.ଗଣନା_2; ++i) {
      ଯଦି(multi->callstack.calls[i] >= CURL_CBAPI_FN_START) {
        uint8_t fn = multi->callstack.calls[i];
        ଯଦି((fn < CURL_CBAPI_FN_LAST) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_START].set_readonly)
          ଫେରନ୍ତୁ TRUE;
      }
    }
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର bool mapi_in_ntfy_cb(ସଂରଚନା Curl_multi *multi)
{
  ଯଦି(multi && multi->callstack.ଗଣନା_2) {
    size_t i;
    ପାଇଁ(i = 0; i < multi->callstack.ଗଣନା_2; ++i) {
      ଯଦି(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        ଫେରନ୍ତୁ TRUE;
    }
  }
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_api_multi_is_in_callback(ସଂରଚନା Curl_multi *multi)
{
  ଯଦି(multi && multi->callstack.ଗଣନା_2) {
    size_t i;
    ପାଇଁ(i = 0; i < multi->callstack.ଗଣନା_2; ++i) {
      ଯଦି(multi->callstack.calls[i] >= CURL_CBAPI_FN_START)
        ଫେରନ୍ତୁ TRUE;
    }
  }
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_api_is_in_callback(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(data && data->multi) {
    ଫେରନ୍ତୁ Curl_api_multi_is_in_callback(data->multi);
  }
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_eapi_enter(ସଂରଚନା Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  ସଂରଚନା Curl_easy *data = curl;
  ଅଚଳ ସଂରଚନା Curl_eapi_fn_props *fn_props;
  CURLcode result = CURLE_OK;

  guard->depth = 0;

  /* Verify that we got an easy handle we can work with. */
  ଯଦି(!GOOD_EASY_HANDLE(data) || data->state.internal) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  ଯଦି(data->mid != UINT32_MAX) {
    ଯଦି(GOOD_MULTI_HANDLE(data->multi)) {
      ଯଦି(!Curl_multi_knows_easy(data->multi, data)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ଯାଅ out;
      }
    }
    ଅନ୍ୟଥା {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ଯାଅ out;
    }
  }
  ଅନ୍ୟଥା ଯଦି(data->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  ଯଦି(fn >= CURL_EAPI_FN_LAST) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  ଯଦି(!fn_props->recurse) {
    ଯଦି(data->callstack.ଗଣନା_2) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        data->callstack.calls[data->callstack.ଗଣନା_2 - 1]));
#endif
      result = CURLE_RECURSIVE_API_CALL;
      ଯାଅ out;
    }
    ଯଦି(data->multi && data->multi->callstack.ଗଣନା_2) {
#ifdef CURLVERBOSE

      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        data->multi->callstack.calls[data->multi->callstack.ଗଣନା_2 - 1]));
#endif
      result = CURLE_RECURSIVE_API_CALL;
      ଯାଅ out;
    }
  }

  ଯଦି(!fn_props->readonly && eapi_in_readonly_cb(data)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#endif
    result = CURLE_RECURSIVE_API_CALL;
    ଯାଅ out;
  }

#ଯଦି defined(USE_SSL) && defined(USE_MUTEX)
  ଯଦି(fn_props->no_scache_lock &&
     Curl_ssl_scache_is_locked_by_current_thread(data)) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#endif
    result = CURLE_RECURSIVE_API_CALL;
    ଯାଅ out;
  }
#endif

  /* all fine, add to data's callstack */
  ଯଦି(data->callstack.ଗଣନା_2 >= CURL_EAPI_MAX_RECURSION) {
    result = CURLE_RECURSIVE_API_CALL;
    ଯାଅ out;
  }
  data->callstack.calls[data->callstack.ଗଣନା_2] = (uint16_t)fn;
  ++data->callstack.ଗଣନା_2;
  guard->depth = data->callstack.ଗଣନା_2;
  guard->data = fn_props->data_is_killed ? ଖାଲି : data;

out:
  ଯଦି(presult)
    *presult = result;
  ଫେରନ୍ତୁ guard->depth > 0;
}

ଶୂନ୍ୟ Curl_eapi_leave(ସଂରଚନା Curl_eapi_guard *guard)
{
  ଯଦି(guard->depth) {
    /* guard->data is set when handle is supposed to stay alive during call */
    ଯଦି(guard->data && GOOD_EASY_HANDLE(guard->data)) {
      ଯଦି(guard->depth > guard->data->callstack.ଗଣନା_2) {
        DEBUGASSERT(0); /* something very wrong */
      }
      ଅନ୍ୟଥା {
        ଯଦି(guard->depth < guard->data->callstack.ଗଣନା_2) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->data->callstack.ଗଣନା_2 = (uint16_t)(guard->depth - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode result)
{
  ବାଛ(result) {
  ସ୍ଥିତି CURLE_OK:
    ଫେରନ୍ତୁ CURLHE_OK;
  ସ୍ଥିତି CURLE_BAD_FUNCTION_ARGUMENT:
    ଫେରନ୍ତୁ CURLHE_BAD_ARGUMENT;
  ସ୍ଥିତି CURLE_OUT_OF_MEMORY:
    ଫେରନ୍ତୁ CURLHE_OUT_OF_MEMORY;
  ସ୍ଥିତି CURLE_NOT_BUILT_IN:
    ଫେରନ୍ତୁ CURLHE_NOT_BUILT_IN;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    ଫେରନ୍ତୁ CURLHE_BAD_ARGUMENT;
  }
}

bool Curl_mapi_enter(ସଂରଚନା Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  ସଂରଚନା Curl_multi *multi = m;
  ଅଚଳ ସଂରଚନା Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->depth = 0;

  /* Verify that we got an easy handle we can work with. */
  ଯଦି(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    ଯାଅ out;
  }
  ଯଦି(fn >= CURL_MAPI_FN_LAST) {
    mresult = CURLM_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  ଯଦି(fn_props->allow_ntfy_cb && mapi_in_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  ଅନ୍ୟଥା ଯଦି(!fn_props->recurse && multi->callstack.ଗଣନା_2) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.ଗଣନା_2 - 1]));
#endif
    mresult = CURLM_RECURSIVE_API_CALL;
    ଯାଅ out;
  }

#ଯଦି defined(USE_SSL) && defined(USE_MUTEX)
  ଯଦି(fn_props->no_scache_lock && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#ifdef CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#endif
    mresult = CURLM_RECURSIVE_API_CALL;
    ଯାଅ out;
  }
#endif

  /* all fine, add to data's callstack */
  ଯଦି(multi->callstack.ଗଣନା_2 >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    ଯାଅ out;
  }
  multi->callstack.calls[multi->callstack.ଗଣନା_2] = (uint8_t)fn;
  ++multi->callstack.ଗଣନା_2;
  guard->depth = multi->callstack.ଗଣନା_2;
  guard->multi = fn_props->multi_is_killed ? ଖାଲି : multi;

out:
  ଯଦି(pmresult)
    *pmresult = mresult;
  ଫେରନ୍ତୁ guard->depth > 0;
}

ଶୂନ୍ୟ Curl_mapi_leave(ସଂରଚନା Curl_mapi_guard *guard)
{
  ଯଦି(guard->depth) {
    /* guard->data is set when handle is supposed to stay alive during call */
    ଯଦି(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      ଯଦି(guard->depth > guard->multi->callstack.ଗଣନା_2) {
        DEBUGASSERT(0); /* something very wrong */
      }
      ଅନ୍ୟଥା {
        ଯଦି(guard->depth < guard->multi->callstack.ଗଣନା_2) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.ଗଣନା_2 = (uint8_t)(guard->depth - 1);
      }
    }
  }
}

ଶୂନ୍ୟ Curl_cbapi_enter(ସଂରଚନା Curl_mapi_guard *guard,
                      ସଂରଚନା Curl_easy *data,
                      ସଂରଚନା Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->depth = 0;

  ଯଦି(!multi)
    multi = data ? data->multi : ଖାଲି;
  /* if not multi is involved here, just leave */
  ଯଦି(!multi)
    ଫେରନ୍ତୁ;
  /* invalid callback specifier? */
  ଯଦି((fn >= CURL_CBAPI_FN_LAST) || (fn < CURL_CBAPI_FN_START)) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_START].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  ଯଦି(multi->callstack.ଗଣନା_2 >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ;
  }
  multi->callstack.calls[multi->callstack.ଗଣନା_2] = (uint8_t)fn;
  ++multi->callstack.ଗଣନା_2;
  guard->depth = multi->callstack.ଗଣନା_2;
  guard->multi = multi;
}

ଶୂନ୍ୟ Curl_cbapi_leave(ସଂରଚନା Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
