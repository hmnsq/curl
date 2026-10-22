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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "api.h"
#समावेश "curl_threads.h"
#समावेश "multiif.h"
#समावेश "vtls/vtls_scache.h"

प्रकार अक्षर mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_LAST <= CURL_CBAPI_FN_START) ? 1 : -1];
प्रकार अक्षर cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_LAST - 1) <= UINT8_MAX) ? 1 : -1];
प्रकार अक्षर mapi_depth_fits_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

काष्ठा Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t तथ्य_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_lock; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

अटल स्थिर काष्ठा Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_LAST] = {
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

काष्ठा Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_lock;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

अटल स्थिर काष्ठा Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_LAST] = {
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

काष्ठा Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t set_readonly;    /* only allows readonly, recursive easy calls */
};

अटल स्थिर काष्ठा Curl_cbapi_fn_props
cbapi_fn_props[CURL_CBAPI_FN_LAST - CURL_CBAPI_FN_START] = {
  { CURL_CBAPI_FN_easy_chunk_bgn,             0 },
  { CURL_CBAPI_FN_easy_chunk_end,             0 },
  { CURL_CBAPI_FN_easy_closesocket,           0 },
  { CURL_CBAPI_FN_easy_cr_in_read,            0 },
  { CURL_CBAPI_FN_easy_cr_in_resume_from,     0 },
  { CURL_CBAPI_FN_easy_cw_out_cb,             0 },
  { CURL_CBAPI_FN_easy_fdebug,                1 },
  { CURL_CBAPI_FN_easy_fnmatch_तथ्य,          0 },
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
  { CURL_CBAPI_FN_multi_धकेलो_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

अटल bool eapi_in_readonly_cb(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_multi *multi = तथ्य->multi;
  अगर(multi && multi->callstack.गिनती) {
    माप_प्रकार i;
    क्रम(i = 0; i < multi->callstack.गिनती; ++i) {
      अगर(multi->callstack.calls[i] >= CURL_CBAPI_FN_START) {
        uint8_t fn = multi->callstack.calls[i];
        अगर((fn < CURL_CBAPI_FN_LAST) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_START].set_readonly)
          वापस TRUE;
      }
    }
  }
  वापस FALSE;
}

अटल bool mapi_in_ntfy_cb(काष्ठा Curl_multi *multi)
{
  अगर(multi && multi->callstack.गिनती) {
    माप_प्रकार i;
    क्रम(i = 0; i < multi->callstack.गिनती; ++i) {
      अगर(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        वापस TRUE;
    }
  }
  वापस FALSE;
}

bool Curl_api_multi_is_in_callback(काष्ठा Curl_multi *multi)
{
  अगर(multi && multi->callstack.गिनती) {
    माप_प्रकार i;
    क्रम(i = 0; i < multi->callstack.गिनती; ++i) {
      अगर(multi->callstack.calls[i] >= CURL_CBAPI_FN_START)
        वापस TRUE;
    }
  }
  वापस FALSE;
}

bool Curl_api_is_in_callback(काष्ठा Curl_easy *तथ्य)
{
  अगर(तथ्य && तथ्य->multi) {
    वापस Curl_api_multi_is_in_callback(तथ्य->multi);
  }
  वापस FALSE;
}

bool Curl_eapi_enter(काष्ठा Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  काष्ठा Curl_easy *तथ्य = curl;
  स्थिर काष्ठा Curl_eapi_fn_props *fn_props;
  CURLcode परिणाम = CURLE_OK;

  guard->depth = 0;

  /* Verify that we got an easy handle we can work with. */
  अगर(!GOOD_EASY_HANDLE(तथ्य) || तथ्य->state.internal) {
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  अगर(तथ्य->mid != UINT32_MAX) {
    अगर(GOOD_MULTI_HANDLE(तथ्य->multi)) {
      अगर(!Curl_multi_knows_easy(तथ्य->multi, तथ्य)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
        जाओ out;
      }
    }
    अन्यथा {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }
  }
  अन्यथा अगर(तथ्य->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  अगर(fn >= CURL_EAPI_FN_LAST) {
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  अगर(!fn_props->recurse) {
    अगर(तथ्य->callstack.गिनती) {
#अगर_घोषित CURLVERBOSE
      DEBUGF(curl_mfprintf(मानक_त्रुटि,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        तथ्य->callstack.calls[तथ्य->callstack.गिनती - 1]));
#पूर्ण_अगर
      परिणाम = CURLE_RECURSIVE_API_CALL;
      जाओ out;
    }
    अगर(तथ्य->multi && तथ्य->multi->callstack.गिनती) {
#अगर_घोषित CURLVERBOSE

      DEBUGF(curl_mfprintf(मानक_त्रुटि,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        तथ्य->multi->callstack.calls[तथ्य->multi->callstack.गिनती - 1]));
#पूर्ण_अगर
      परिणाम = CURLE_RECURSIVE_API_CALL;
      जाओ out;
    }
  }

  अगर(!fn_props->readonly && eapi_in_readonly_cb(तथ्य)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#अगर_घोषित CURLVERBOSE
      DEBUGF(curl_mfprintf(मानक_त्रुटि,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#पूर्ण_अगर
    परिणाम = CURLE_RECURSIVE_API_CALL;
    जाओ out;
  }

#अगर defined(USE_SSL) && defined(USE_MUTEX)
  अगर(fn_props->no_scache_lock &&
     Curl_ssl_scache_is_locked_by_current_thread(तथ्य)) {
#अगर_घोषित CURLVERBOSE
      DEBUGF(curl_mfprintf(मानक_त्रुटि,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#पूर्ण_अगर
    परिणाम = CURLE_RECURSIVE_API_CALL;
    जाओ out;
  }
#पूर्ण_अगर

  /* all fine, add to data's callstack */
  अगर(तथ्य->callstack.गिनती >= CURL_EAPI_MAX_RECURSION) {
    परिणाम = CURLE_RECURSIVE_API_CALL;
    जाओ out;
  }
  तथ्य->callstack.calls[तथ्य->callstack.गिनती] = (uint16_t)fn;
  ++तथ्य->callstack.गिनती;
  guard->depth = तथ्य->callstack.गिनती;
  guard->तथ्य = fn_props->तथ्य_is_killed ? शून्य : तथ्य;

out:
  अगर(presult)
    *presult = परिणाम;
  वापस guard->depth > 0;
}

व्योम Curl_eapi_leave(काष्ठा Curl_eapi_guard *guard)
{
  अगर(guard->depth) {
    /* guard->data is set when handle is supposed to stay alive during call */
    अगर(guard->तथ्य && GOOD_EASY_HANDLE(guard->तथ्य)) {
      अगर(guard->depth > guard->तथ्य->callstack.गिनती) {
        DEBUGASSERT(0); /* something very wrong */
      }
      अन्यथा {
        अगर(guard->depth < guard->तथ्य->callstack.गिनती) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->तथ्य->callstack.गिनती = (uint16_t)(guard->depth - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode परिणाम)
{
  चयन(परिणाम) {
  हाल CURLE_OK:
    वापस CURLHE_OK;
  हाल CURLE_BAD_FUNCTION_ARGUMENT:
    वापस CURLHE_BAD_ARGUMENT;
  हाल CURLE_OUT_OF_MEMORY:
    वापस CURLHE_OUT_OF_MEMORY;
  हाल CURLE_NOT_BUILT_IN:
    वापस CURLHE_NOT_BUILT_IN;
  शेष:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    वापस CURLHE_BAD_ARGUMENT;
  }
}

bool Curl_mapi_enter(काष्ठा Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  काष्ठा Curl_multi *multi = m;
  स्थिर काष्ठा Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->depth = 0;

  /* Verify that we got an easy handle we can work with. */
  अगर(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    जाओ out;
  }
  अगर(fn >= CURL_MAPI_FN_LAST) {
    mresult = CURLM_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  अगर(fn_props->allow_ntfy_cb && mapi_in_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  अन्यथा अगर(!fn_props->recurse && multi->callstack.गिनती) {
#अगर_घोषित CURLVERBOSE
      DEBUGF(curl_mfprintf(मानक_त्रुटि,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.गिनती - 1]));
#पूर्ण_अगर
    mresult = CURLM_RECURSIVE_API_CALL;
    जाओ out;
  }

#अगर defined(USE_SSL) && defined(USE_MUTEX)
  अगर(fn_props->no_scache_lock && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#अगर_घोषित CURLVERBOSE
      DEBUGF(curl_mfprintf(मानक_त्रुटि,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#पूर्ण_अगर
    mresult = CURLM_RECURSIVE_API_CALL;
    जाओ out;
  }
#पूर्ण_अगर

  /* all fine, add to data's callstack */
  अगर(multi->callstack.गिनती >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    जाओ out;
  }
  multi->callstack.calls[multi->callstack.गिनती] = (uint8_t)fn;
  ++multi->callstack.गिनती;
  guard->depth = multi->callstack.गिनती;
  guard->multi = fn_props->multi_is_killed ? शून्य : multi;

out:
  अगर(pmresult)
    *pmresult = mresult;
  वापस guard->depth > 0;
}

व्योम Curl_mapi_leave(काष्ठा Curl_mapi_guard *guard)
{
  अगर(guard->depth) {
    /* guard->data is set when handle is supposed to stay alive during call */
    अगर(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      अगर(guard->depth > guard->multi->callstack.गिनती) {
        DEBUGASSERT(0); /* something very wrong */
      }
      अन्यथा {
        अगर(guard->depth < guard->multi->callstack.गिनती) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.गिनती = (uint8_t)(guard->depth - 1);
      }
    }
  }
}

व्योम Curl_cbapi_enter(काष्ठा Curl_mapi_guard *guard,
                      काष्ठा Curl_easy *तथ्य,
                      काष्ठा Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->depth = 0;

  अगर(!multi)
    multi = तथ्य ? तथ्य->multi : शून्य;
  /* if not multi is involved here, just leave */
  अगर(!multi)
    वापस;
  /* invalid callback specifier? */
  अगर((fn >= CURL_CBAPI_FN_LAST) || (fn < CURL_CBAPI_FN_START)) {
    DEBUGASSERT(0);
    वापस;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_START].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  अगर(multi->callstack.गिनती >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    वापस;
  }
  multi->callstack.calls[multi->callstack.गिनती] = (uint8_t)fn;
  ++multi->callstack.गिनती;
  guard->depth = multi->callstack.गिनती;
  guard->multi = multi;
}

व्योम Curl_cbapi_leave(काष्ठा Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
