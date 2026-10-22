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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "api.h"
#अन्तर्भाव "curl_threads.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "vtls/vtls_scache.h"

प्रकारनिर्देश वर्ण mapi_fn_ids_fit_uint8[
  (CURL_MAPI_FN_चरमः <= CURL_CBAPI_FN_START) ? 1 : -1];
प्रकारनिर्देश वर्ण cbapi_fn_ids_fit_uint8[
  ((CURL_CBAPI_FN_चरमः - 1) <= UINT8_MAX) ? 1 : -1];
प्रकारनिर्देश वर्ण mapi_गभीरता_माति_uint8[
  (CURL_MAPI_MAX_RECURSION <= UINT8_MAX) ? 1 : -1];

संरचना Curl_eapi_fn_props {
  Curl_eapi_fn fn;
  uint8_t दत्त_is_killed; /* easy handle is killed in call */
  uint8_t recurse;        /* may be called when another call is in progress */
  uint8_t readonly;       /* call is not modifying easy handle */
  uint8_t no_scache_तालकम्; /* may not be called with easy's vtls_scache
                             locked by current thread */
};

स्थिर ध्रुव संरचना Curl_eapi_fn_props eapi_fn_props[CURL_EAPI_FN_चरमः] = {
  /* function                   kill rec  RO !sc */
  { CURL_EAPI_FN_easy_cleanup,     1,  0,  0,  0 },
  { CURL_EAPI_FN_easy_duphandle,   0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_getinfo,     0,  1,  1,  0 },
  { CURL_EAPI_FN_easy_शीर्षकम्,      0,  1,  1,  0 },
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

संरचना Curl_mapi_fn_props {
  Curl_mapi_fn fn;
  uint8_t multi_is_killed; /* multi handle is killed during call */
  uint8_t recurse;         /* may be called when another call is in progress */
  uint8_t allow_ntfy_cb;   /* may be called during a notify callback */
  uint8_t no_scache_तालकम्;  /* may not be called with multi's vtls_scache
                              locked by current thread */
};

स्थिर ध्रुव संरचना Curl_mapi_fn_props mapi_fn_props[CURL_MAPI_FN_चरमः] = {
  /* function                       kill rec ntfy !scach */
  { CURL_MAPI_FN_multi_सङ्कलय_handle,     0,  0,   1,  0 },
  { CURL_MAPI_FN_multi_नियोजय,         0,  1,   1,  0 },
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

संरचना Curl_cbapi_fn_props {
  Curl_cbapi_fn fn;
  uint8_t समुच्चयः_readonly;    /* only allows readonly, recursive easy calls */
};

स्थिर ध्रुव संरचना Curl_cbapi_fn_props
cbapi_fn_props[CURL_CBAPI_FN_चरमः - CURL_CBAPI_FN_START] = {
  { CURL_CBAPI_FN_easy_chunk_bgn,             0 },
  { CURL_CBAPI_FN_easy_chunk_अन्तः,             0 },
  { CURL_CBAPI_FN_easy_closesocket,           0 },
  { CURL_CBAPI_FN_easy_cr_अन्तर्_read,            0 },
  { CURL_CBAPI_FN_easy_cr_अन्तर्_resume_from,     0 },
  { CURL_CBAPI_FN_easy_cw_बहिर्गमनम्_cb,             0 },
  { CURL_CBAPI_FN_easy_fdebug,                1 },
  { CURL_CBAPI_FN_easy_fnmatch_दत्त,          0 },
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
  { CURL_CBAPI_FN_multi_प्रक्षिप_cb,              0 },
  { CURL_CBAPI_FN_multi_socket_cb,            1 },
  { CURL_CBAPI_FN_multi_timer_cb,             1 },
};

स्थिर bool eapi_अन्तर्_readonly_cb(संरचना Curl_easy *दत्त)
{
  संरचना Curl_multi *multi = दत्त->multi;
  यदि(multi && multi->callstack.गणनम्) {
    size_t i;
    प्रति(i = 0; i < multi->callstack.गणनम्; ++i) {
      यदि(multi->callstack.calls[i] >= CURL_CBAPI_FN_START) {
        uint8_t fn = multi->callstack.calls[i];
        यदि((fn < CURL_CBAPI_FN_चरमः) &&
           cbapi_fn_props[fn - CURL_CBAPI_FN_START].समुच्चयः_readonly)
          प्रत्यागम TRUE;
      }
    }
  }
  प्रत्यागम FALSE;
}

स्थिर bool mapi_अन्तर्_ntfy_cb(संरचना Curl_multi *multi)
{
  यदि(multi && multi->callstack.गणनम्) {
    size_t i;
    प्रति(i = 0; i < multi->callstack.गणनम्; ++i) {
      यदि(multi->callstack.calls[i] == CURL_CBAPI_FN_multi_ntfy_cb)
        प्रत्यागम TRUE;
    }
  }
  प्रत्यागम FALSE;
}

bool Curl_api_multi_is_अन्तर्_callback(संरचना Curl_multi *multi)
{
  यदि(multi && multi->callstack.गणनम्) {
    size_t i;
    प्रति(i = 0; i < multi->callstack.गणनम्; ++i) {
      यदि(multi->callstack.calls[i] >= CURL_CBAPI_FN_START)
        प्रत्यागम TRUE;
    }
  }
  प्रत्यागम FALSE;
}

bool Curl_api_is_अन्तर्_callback(संरचना Curl_easy *दत्त)
{
  यदि(दत्त && दत्त->multi) {
    प्रत्यागम Curl_api_multi_is_अन्तर्_callback(दत्त->multi);
  }
  प्रत्यागम FALSE;
}

bool Curl_eapi_enter(संरचना Curl_eapi_guard *guard,
                     CURL *curl,
                     Curl_eapi_fn fn,
                     CURLcode *presult)
{
  संरचना Curl_easy *दत्त = curl;
  ध्रुव संरचना Curl_eapi_fn_props *fn_props;
  CURLcode परिणाम = CURLE_OK;

  guard->गभीरता = 0;

  /* Verify that we got an easy handle we can work with. */
  यदि(!GOOD_EASY_HANDLE(दत्त) || दत्त->state.आन्तरिकम्) {
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }
  /* verify that is either not added to a multi handle OR has a
   * GOOD multi handle that knows `data` for `data->mid`. */
  यदि(दत्त->mid != UINT32_MAX) {
    यदि(GOOD_MULTI_HANDLE(दत्त->multi)) {
      यदि(!Curl_multi_knows_easy(दत्त->multi, दत्त)) {
        /* But multi does not know it, something is fishy, better deny call */
        DEBUGASSERT(0);
        परिणाम = CURLE_BAD_FUNCTION_तर्कः;
        गच्छ बहिर्गमनम्;
      }
    }
    अन्यथा {
      DEBUGASSERT(0); /* data needs to have a GOOD multi handle */
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }
  }
  अन्यथा यदि(दत्त->multi) {
    DEBUGASSERT(0); /* data should not have a multi handle */
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }
  /* verify that the call `fn` we're about to enter is known
   * and check call properties to be admitting. */
  यदि(fn >= CURL_EAPI_FN_चरमः) {
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }
  fn_props = &eapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  यदि(!fn_props->recurse) {
    यदि(दत्त->callstack.गणनम्) {
#यदि_परिभाषितम् CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        दत्त->callstack.calls[दत्त->callstack.गणनम् - 1]));
#यदि_अन्तः
      परिणाम = CURLE_RECURSIVE_API_CALL;
      गच्छ बहिर्गमनम्;
    }
    यदि(दत्त->multi && दत्त->multi->callstack.गणनम्) {
#यदि_परिभाषितम् CURLVERBOSE

      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu with multi call to %u ongoing\n",
        (uint16_t)fn,
        दत्त->multi->callstack.calls[दत्त->multi->callstack.गणनम् - 1]));
#यदि_अन्तः
      परिणाम = CURLE_RECURSIVE_API_CALL;
      गच्छ बहिर्गमनम्;
    }
  }

  यदि(!fn_props->readonly && eapi_अन्तर्_readonly_cb(दत्त)) {
    /* Not allowed to be invoked while an event cb is ongoing */
#यदि_परिभाषितम् CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu in readonly enforcing callback\n",
        (uint16_t)fn));
#यदि_अन्तः
    परिणाम = CURLE_RECURSIVE_API_CALL;
    गच्छ बहिर्गमनम्;
  }

#यदि defined(USE_SSL) && defined(USE_MUTEX)
  यदि(fn_props->no_scache_तालकम् &&
     Curl_ssl_scache_is_locked_by_current_thread(दत्त)) {
#यदि_परिभाषितम् CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "EAPI guard: calling %hu while vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#यदि_अन्तः
    परिणाम = CURLE_RECURSIVE_API_CALL;
    गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः

  /* all fine, add to data's callstack */
  यदि(दत्त->callstack.गणनम् >= CURL_EAPI_MAX_RECURSION) {
    परिणाम = CURLE_RECURSIVE_API_CALL;
    गच्छ बहिर्गमनम्;
  }
  दत्त->callstack.calls[दत्त->callstack.गणनम्] = (uint16_t)fn;
  ++दत्त->callstack.गणनम्;
  guard->गभीरता = दत्त->callstack.गणनम्;
  guard->दत्त = fn_props->दत्त_is_killed ? रिक्त : दत्त;

बहिर्गमनम्:
  यदि(presult)
    *presult = परिणाम;
  प्रत्यागम guard->गभीरता > 0;
}

शून्य Curl_eapi_leave(संरचना Curl_eapi_guard *guard)
{
  यदि(guard->गभीरता) {
    /* guard->data is set when handle is supposed to stay alive during call */
    यदि(guard->दत्त && GOOD_EASY_HANDLE(guard->दत्त)) {
      यदि(guard->गभीरता > guard->दत्त->callstack.गणनम्) {
        DEBUGASSERT(0); /* something very wrong */
      }
      अन्यथा {
        यदि(guard->गभीरता < guard->दत्त->callstack.गणनम्) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->दत्त->callstack.गणनम् = (uint16_t)(guard->गभीरता - 1);
      }
    }
  }
}

CURLHcode Curl_eapi_hcode(CURLcode परिणाम)
{
  विकल्प(परिणाम) {
  पक्ष CURLE_OK:
    प्रत्यागम CURLHE_OK;
  पक्ष CURLE_BAD_FUNCTION_तर्कः:
    प्रत्यागम CURLHE_BAD_तर्कः;
  पक्ष CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः:
    प्रत्यागम CURLHE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  पक्ष CURLE_NOT_BUILT_अन्तर्:
    प्रत्यागम CURLHE_NOT_BUILT_अन्तर्;
  स्वभाव:
    /* Unfortunately, we cannot convert RECURSIVE_API_CALL,
     * but since the header API is reentrant, this should not happen. */
    प्रत्यागम CURLHE_BAD_तर्कः;
  }
}

bool Curl_mapi_enter(संरचना Curl_mapi_guard *guard,
                     CURLM *m,
                     Curl_mapi_fn fn,
                     CURLMcode *pmresult)
{
  संरचना Curl_multi *multi = m;
  ध्रुव संरचना Curl_mapi_fn_props *fn_props;
  CURLMcode mresult = CURLM_OK;

  guard->गभीरता = 0;

  /* Verify that we got an easy handle we can work with. */
  यदि(!GOOD_MULTI_HANDLE(multi)) {
    mresult = CURLM_BAD_HANDLE;
    गच्छ बहिर्गमनम्;
  }
  यदि(fn >= CURL_MAPI_FN_चरमः) {
    mresult = CURLM_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }
  fn_props = &mapi_fn_props[fn];
  DEBUGASSERT(fn_props->fn == fn);
  यदि(fn_props->allow_ntfy_cb && mapi_अन्तर्_ntfy_cb(multi)) {
    /* explicitly allowed, even though normal recursion may not */
  }
  अन्यथा यदि(!fn_props->recurse && multi->callstack.गणनम्) {
#यदि_परिभाषितम् CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu with call to %u ongoing\n", (uint16_t)fn,
        multi->callstack.calls[multi->callstack.गणनम् - 1]));
#यदि_अन्तः
    mresult = CURLM_RECURSIVE_API_CALL;
    गच्छ बहिर्गमनम्;
  }

#यदि defined(USE_SSL) && defined(USE_MUTEX)
  यदि(fn_props->no_scache_तालकम् && multi->ssl_scache &&
     Curl_ssl_scache_is_locked_by_current_thread(multi->admin)) {
#यदि_परिभाषितम् CURLVERBOSE
      DEBUGF(curl_mfprintf(stderr,
        "MAPI guard: calling %hu while its vtls_scache is locked by "
        "current thread\n", (uint16_t)fn));
#यदि_अन्तः
    mresult = CURLM_RECURSIVE_API_CALL;
    गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः

  /* all fine, add to data's callstack */
  यदि(multi->callstack.गणनम् >= CURL_MAPI_MAX_RECURSION) {
    mresult = CURLM_RECURSIVE_API_CALL;
    गच्छ बहिर्गमनम्;
  }
  multi->callstack.calls[multi->callstack.गणनम्] = (uint8_t)fn;
  ++multi->callstack.गणनम्;
  guard->गभीरता = multi->callstack.गणनम्;
  guard->multi = fn_props->multi_is_killed ? रिक्त : multi;

बहिर्गमनम्:
  यदि(pmresult)
    *pmresult = mresult;
  प्रत्यागम guard->गभीरता > 0;
}

शून्य Curl_mapi_leave(संरचना Curl_mapi_guard *guard)
{
  यदि(guard->गभीरता) {
    /* guard->data is set when handle is supposed to stay alive during call */
    यदि(guard->multi && GOOD_MULTI_HANDLE(guard->multi)) {
      यदि(guard->गभीरता > guard->multi->callstack.गणनम्) {
        DEBUGASSERT(0); /* something very wrong */
      }
      अन्यथा {
        यदि(guard->गभीरता < guard->multi->callstack.गणनम्) {
          DEBUGASSERT(0); /* someone forgot to clean up */
        }
        /* reset to depth the guard was entered in */
        guard->multi->callstack.गणनम् = (uint8_t)(guard->गभीरता - 1);
      }
    }
  }
}

शून्य Curl_cbapi_enter(संरचना Curl_mapi_guard *guard,
                      संरचना Curl_easy *दत्त,
                      संरचना Curl_multi *multi,
                      Curl_cbapi_fn fn)
{
  guard->गभीरता = 0;

  यदि(!multi)
    multi = दत्त ? दत्त->multi : रिक्त;
  /* if not multi is involved here, just leave */
  यदि(!multi)
    प्रत्यागम;
  /* invalid callback specifier? */
  यदि((fn >= CURL_CBAPI_FN_चरमः) || (fn < CURL_CBAPI_FN_START)) {
    DEBUGASSERT(0);
    प्रत्यागम;
  }
  DEBUGASSERT(cbapi_fn_props[fn - CURL_CBAPI_FN_START].fn == fn);
  /* all fine, add to data's callstack */
  /* if multi callstack already at max depth, leave */
  यदि(multi->callstack.गणनम् >= CURL_MAPI_MAX_RECURSION) {
    DEBUGASSERT(0);
    प्रत्यागम;
  }
  multi->callstack.calls[multi->callstack.गणनम्] = (uint8_t)fn;
  ++multi->callstack.गणनम्;
  guard->गभीरता = multi->callstack.गणनम्;
  guard->multi = multi;
}

शून्य Curl_cbapi_leave(संरचना Curl_mapi_guard *guard)
{
  Curl_mapi_leave(guard);
}
