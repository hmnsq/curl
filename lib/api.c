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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "api.h"
#అన్తర్భావ "curl_threads.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "vtls/vtls_scache.h"

రకనిర్వచనం అక్షరం mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_చివరి <= CURL_CBAPI_FN_ప్రారంభం) ? 1 : -1];
రకనిర్వచనం అక్షరం cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_చివరి - 1) <= UINT8_MAX) ? 1 : -1];
రకనిర్వచనం అక్షరం mapi_లోతు_సరిపోతుంది_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

నిర్మాణం Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t దత్తాంశం_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_తాళం; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

స్థిర స్థిరరాశి నిర్మాణం Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_చివరి] = {
  /* function                   kill rec  RO !sc */
  { CURL_EAPI_FN_easy_cleanup,     1,  0,  0,  0 },
  { CURL_EAPI_FN_easy_duphandle,   0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_getinfo,     0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_శీర్షిక,      0,  1,  1,  0 },
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
  { CURL_EAPI_FN_ws_ప్రారంభం_చట్రం,   0,  1,  0,  0 },
};

నిర్మాణం Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_తాళం;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

స్థిర స్థిరరాశి నిర్మాణం Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_చివరి] = {
  /* function                       kill rec ntfy !scach */
  { CURL_MAPI_FN_multi_కూడు_handle,     0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_కేటాయించు,         0,  1,   1,  0 },
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

నిర్మాణం Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t సమితి_readonly;    /* only allows readonly, recursive easy calls */
};

స్థిర స్థిరరాశి నిర్మాణం Curl_cbapi_fn_props
cbapi_fn_props[CURL_CBAPI_FN_చివరి - CURL_CBAPI_FN_ప్రారంభం] = {
  { CURL_CBAPI_FN_easy_chunk_bgn,             0 },
  { CURL_CBAPI_FN_easy_chunk_చివర,             0 },
  { CURL_CBAPI_FN_easy_closesocket,           0 },
  { CURL_CBAPI_FN_easy_cr_లోపల_read,            0 },
  { CURL_CBAPI_FN_easy_cr_లోపల_resume_from,     0 },
  { CURL_CBAPI_FN_easy_cw_బహిర్గతం_cb,             0 },
  { CURL_CBAPI_FN_easy_fdebug,                1 },
  { CURL_CBAPI_FN_easy_fnmatch_దత్తాంశం,          0 },
  { CURL_CBAPI_FN_easy_fopensocket,           0 },
  { CURL_CBAPI_FN_easy_fprereq,               0 },
  { CURL_CBAPI_FN_easy_fprogress,             0 },
  { CURL_CBAPI_FN_easy_fread_func,            0 },
  { CURL_CBAPI_FN_easy_fsockopt,              0 },
  { CURL_CBAPI_FN_easy_fsslctx,               0 },
  { CURL_CBAPI_FN_easy_fwrite_rtp,            0 },
  { CURL_CBAPI_FN_easy_fxferinfo,             0 },
  { CURL_CBAPI_FN_easy_ioctl_func,            0 },
  { CURL_CBAPI_FN_easy_resolver_ప్రారంభం,        0 },
  { CURL_CBAPI_FN_easy_seek_func,             0 },
  { CURL_CBAPI_FN_easy_ssh_hostkeyfunc,       0 },
  { CURL_CBAPI_FN_easy_ssh_keyfunc,           0 },
  { CURL_CBAPI_FN_easy_trailer_callback,      0 },

  { CURL_CBAPI_FN_multi_ntfy_cb,              0 },
  { CURL_CBAPI_FN_multi_నెట్టు_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

స్థిర bool eapi_లోపల_readonly_cb(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_multi *multi = దత్తాంశం->multi;
  ఒకవేళ(multi && multi->callstack.లెక్క) {
    size_t i;
    కొరకు(i = 0; i < multi->callstack.లెక్క; ++i) {
      ఒకవేళ(multi->callstack.calls[i] >= CURL_CBAPI_FN_ప్రారంభం) {
        uint8_t fn = multi->callstack.calls[i];
        ఒకవేళ((fn < CURL_CBAPI_FN_చివరి) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_ప్రారంభం].సమితి_readonly)
          తిరిగి TRUE;
      }
    }
  }
  తిరిగి FALSE;
}

స్థిర bool mapi_లోపల_ntfy_cb(నిర్మాణం Curl_multi *multi)
{
  ఒకవేళ(multi && multi->callstack.లెక్క) {
    size_t i;
    కొరకు(i = 0; i < multi->callstack.లెక్క; ++i) {
      ఒకవేళ(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        తిరిగి TRUE;
    }
  }
  తిరిగి FALSE;
}

bool Curl_api_multi_is_లోపల_callback(నిర్మాణం Curl_multi *multi)
{
  ఒకవేళ(multi && multi->callstack.లెక్క) {
    size_t i;
    కొరకు(i = 0; i < multi->callstack.లెక్క; ++i) {
      ఒకవేళ(multi->callstack.calls[i] >= CURL_CBAPI_FN_ప్రారంభం)
        తిరిగి TRUE;
    }
  }
  తిరిగి FALSE;
}

bool Curl_api_is_లోపల_callback(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(దత్తాంశం && దత్తాంశం->multi) {
    తిరిగి Curl_api_multi_is_లోపల_callback(దత్తాంశం->multi);
  }
  తిరిగి FALSE;
}

bool Curl_eapi_enter(నిర్మాణం Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  నిర్మాణం Curl_easy *దత్తాంశం = curl;
  స్థిరరాశి నిర్మాణం Curl_eapi_fn_props *fn_props;
  CURLcode ఫలితం = CURLE_OK;

  guard->లోతు = 0;

  /* Verify that we got an easy handle we can work with. */
  ఒకవేళ(!GOOD_EASY_HANDLE(దత్తాంశం) || దత్తాంశం->state.అంతర్గత) {
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  ఒకవేళ(దత్తాంశం->mid != UINT32_MAX) {
    ఒకవేళ(GOOD_MULTI_HANDLE(దత్తాంశం->multi)) {
      ఒకవేళ(!Curl_multi_knows_easy(దత్తాంశం->multi, దత్తాంశం)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
        వెళ్ళు బహిర్గతం;
      }
    }
    లేకపోతే {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  ఒకవేళ(fn >= CURL_EAPI_FN_చివరి) {
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  ఒకవేళ(!fn_props->recurse) {
    ఒకవేళ(దత్తాంశం->callstack.లెక్క) {
#నిర్వచితమైతే CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        దత్తాంశం->callstack.calls[దత్తాంశం->callstack.లెక్క - 1]));
#అయితే_ముగింపు
      ఫలితం = CURLE_RECURSIVE_API_CALL;
      వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ(దత్తాంశం->multi && దత్తాంశం->multi->callstack.లెక్క) {
#నిర్వచితమైతే CURLVERBOSE

      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        దత్తాంశం->multi->callstack.calls[దత్తాంశం->multi->callstack.లెక్క - 1]));
#అయితే_ముగింపు
      ఫలితం = CURLE_RECURSIVE_API_CALL;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(!fn_props->readonly && eapi_లోపల_readonly_cb(దత్తాంశం)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#నిర్వచితమైతే CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#అయితే_ముగింపు
    ఫలితం = CURLE_RECURSIVE_API_CALL;
    వెళ్ళు బహిర్గతం;
  }

#ఒకవేళ defined(USE_SSL) && defined(USE_MUTEX)
  ఒకవేళ(fn_props->no_scache_తాళం &&
     Curl_ssl_scache_is_locked_by_current_thread(దత్తాంశం)) {
#నిర్వచితమైతే CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#అయితే_ముగింపు
    ఫలితం = CURLE_RECURSIVE_API_CALL;
    వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు

  /* all fine, add to data's callstack */
  ఒకవేళ(దత్తాంశం->callstack.లెక్క >= CURL_EAPI_MAX_RECURSION) {
    ఫలితం = CURLE_RECURSIVE_API_CALL;
    వెళ్ళు బహిర్గతం;
  }
  దత్తాంశం->callstack.calls[దత్తాంశం->callstack.లెక్క] = (uint16_t)fn;
  ++దత్తాంశం->callstack.లెక్క;
  guard->లోతు = దత్తాంశం->callstack.లెక్క;
  guard->దత్తాంశం = fn_props->దత్తాంశం_is_killed ? ఖాళీ : దత్తాంశం;

బహిర్గతం:
  ఒకవేళ(presult)
    *presult = ఫలితం;
  తిరిగి guard->లోతు > 0;
}

శూన్య Curl_eapi_leave(నిర్మాణం Curl_eapi_guard *guard)
{
  ఒకవేళ(guard->లోతు) {
    /* guard->data is set when handle is supposed to stay alive during call */
    ఒకవేళ(guard->దత్తాంశం && GOOD_EASY_HANDLE(guard->దత్తాంశం)) {
      ఒకవేళ(guard->లోతు > guard->దత్తాంశం->callstack.లెక్క) {
        DEBUGASSERT(0); /* something very wrong */
      }
      లేకపోతే {
        ఒకవేళ(guard->లోతు < guard->దత్తాంశం->callstack.లెక్క) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->దత్తాంశం->callstack.లెక్క = (uint16_t)(guard->లోతు - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode ఫలితం)
{
  ఎంపిక(ఫలితం) {
  స్థితి CURLE_OK:
    తిరిగి CURLHE_OK;
  స్థితి CURLE_BAD_FUNCTION_వాదం:
    తిరిగి CURLHE_BAD_వాదం;
  స్థితి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం:
    తిరిగి CURLHE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  స్థితి CURLE_NOT_BUILT_లోపల:
    తిరిగి CURLHE_NOT_BUILT_లోపల;
  అప్రమేయ:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    తిరిగి CURLHE_BAD_వాదం;
  }
}

bool Curl_mapi_enter(నిర్మాణం Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  నిర్మాణం Curl_multi *multi = m;
  స్థిరరాశి నిర్మాణం Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->లోతు = 0;

  /* Verify that we got an easy handle we can work with. */
  ఒకవేళ(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(fn >= CURL_MAPI_FN_చివరి) {
    mresult = CURLM_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  ఒకవేళ(fn_props->allow_ntfy_cb && mapi_లోపల_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  లేకపోతే ఒకవేళ(!fn_props->recurse && multi->callstack.లెక్క) {
#నిర్వచితమైతే CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.లెక్క - 1]));
#అయితే_ముగింపు
    mresult = CURLM_RECURSIVE_API_CALL;
    వెళ్ళు బహిర్గతం;
  }

#ఒకవేళ defined(USE_SSL) && defined(USE_MUTEX)
  ఒకవేళ(fn_props->no_scache_తాళం && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#నిర్వచితమైతే CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#అయితే_ముగింపు
    mresult = CURLM_RECURSIVE_API_CALL;
    వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు

  /* all fine, add to data's callstack */
  ఒకవేళ(multi->callstack.లెక్క >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    వెళ్ళు బహిర్గతం;
  }
  multi->callstack.calls[multi->callstack.లెక్క] = (uint8_t)fn;
  ++multi->callstack.లెక్క;
  guard->లోతు = multi->callstack.లెక్క;
  guard->multi = fn_props->multi_is_killed ? ఖాళీ : multi;

బహిర్గతం:
  ఒకవేళ(pmresult)
    *pmresult = mresult;
  తిరిగి guard->లోతు > 0;
}

శూన్య Curl_mapi_leave(నిర్మాణం Curl_mapi_guard *guard)
{
  ఒకవేళ(guard->లోతు) {
    /* guard->data is set when handle is supposed to stay alive during call */
    ఒకవేళ(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      ఒకవేళ(guard->లోతు > guard->multi->callstack.లెక్క) {
        DEBUGASSERT(0); /* something very wrong */
      }
      లేకపోతే {
        ఒకవేళ(guard->లోతు < guard->multi->callstack.లెక్క) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.లెక్క = (uint8_t)(guard->లోతు - 1);
      }
    }
  }
}

శూన్య Curl_cbapi_enter(నిర్మాణం Curl_mapi_guard *guard,
                      నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->లోతు = 0;

  ఒకవేళ(!multi)
    multi = దత్తాంశం ? దత్తాంశం->multi : ఖాళీ;
  /* if not multi is involved here, just leave */
  ఒకవేళ(!multi)
    తిరిగి;
  /* invalid callback specifier? */
  ఒకవేళ((fn >= CURL_CBAPI_FN_చివరి) || (fn < CURL_CBAPI_FN_ప్రారంభం)) {
    DEBUGASSERT(0);
    తిరిగి;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_ప్రారంభం].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  ఒకవేళ(multi->callstack.లెక్క >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    తిరిగి;
  }
  multi->callstack.calls[multi->callstack.లెక్క] = (uint8_t)fn;
  ++multi->callstack.లెక్క;
  guard->లోతు = multi->callstack.లెక్క;
  guard->multi = multi;
}

శూన్య Curl_cbapi_leave(నిర్మాణం Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
