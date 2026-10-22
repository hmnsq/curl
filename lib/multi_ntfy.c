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
#समावेश "curl_trc.h"
#समावेश "multihandle.h"
#समावेश "multiif.h"
#समावेश "multi_ntfy.h"

#अगर CURLMNOTIFY_LAST > 31
#त्रुटि "notification types must fit below the curl_multi_ntfy high flag bit"
#पूर्ण_अगर

#घोषणा CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

काष्ठा mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#घोषणा CURL_MNTFY_CHUNK_आकार 128

काष्ठा mntfy_chunk {
  काष्ठा mntfy_chunk *अग्र;
  माप_प्रकार r_offset;
  माप_प्रकार w_offset;
  काष्ठा mntfy_entry entries[CURL_MNTFY_CHUNK_आकार];
};

अटल काष्ठा mntfy_chunk *mnfty_chunk_create(व्योम)
{
  वापस curlx_calloc(1, माप(काष्ठा mntfy_chunk));
}

अटल व्योम mnfty_chunk_destroy(काष्ठा mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

अटल व्योम mnfty_chunk_reset(काष्ठा mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

अटल bool mntfy_chunk_append(काष्ठा mntfy_chunk *chunk,
                               काष्ठा Curl_easy *तथ्य,
                               uint32_t type)
{
  काष्ठा mntfy_entry *e;

  अगर(chunk->w_offset >= CURL_MNTFY_CHUNK_आकार)
    वापस FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = तथ्य->mid;
  e->type = type;
  वापस TRUE;
}

अटल काष्ठा mntfy_chunk *mntfy_non_full_पुच्छ(काष्ठा curl_multi_ntfy *mntfy)
{
  काष्ठा mntfy_chunk *chunk;
  अगर(!mntfy->पुच्छ) {
    chunk = mnfty_chunk_create();
    अगर(!chunk)
      वापस शून्य;
    DEBUGASSERT(!mntfy->शीर्ष);
    mntfy->शीर्ष = mntfy->पुच्छ = chunk;
    वापस chunk;
  }
  अन्यथा अगर(mntfy->पुच्छ->w_offset < CURL_MNTFY_CHUNK_आकार)
    वापस mntfy->पुच्छ;
  अन्यथा { /* tail is full. */
    chunk = mnfty_chunk_create();
    अगर(!chunk)
      वापस शून्य;
    DEBUGASSERT(mntfy->शीर्ष);
    mntfy->पुच्छ->अग्र = chunk;
    mntfy->पुच्छ = chunk;
    वापस chunk;
  }
}

अटल व्योम mntfy_chunk_dispatch_all(काष्ठा Curl_multi *multi,
                                     काष्ठा mntfy_chunk *chunk)
{
  काष्ठा mntfy_entry *e;
  काष्ठा Curl_easy *तथ्य;

  जबतक(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    तथ्य = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    अगर(तथ्य && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, तथ्य, multi->ntfy.ntfy_cb_तथ्य);
    }
    /* once dispatched, safe to increment */
    chunk->r_offset++;
  }
  mnfty_chunk_reset(chunk);
}

व्योम Curl_mntfy_init(काष्ठा Curl_multi *multi)
{
  स_रखो(&multi->ntfy, 0, माप(multi->ntfy));
}

व्योम Curl_mntfy_cleanup(काष्ठा Curl_multi *multi)
{
  जबतक(multi->ntfy.शीर्ष) {
    काष्ठा mntfy_chunk *chunk = multi->ntfy.शीर्ष;
    multi->ntfy.शीर्ष = chunk->अग्र;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.पुच्छ = शून्य;
}

CURLMcode Curl_mntfy_enable(काष्ठा Curl_multi *multi, अचिन्हित पूर्णांक type)
{
  अगर(type >= CURLMNOTIFY_LAST)
    वापस CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  वापस CURLM_OK;
}

CURLMcode Curl_mntfy_disable(काष्ठा Curl_multi *multi, अचिन्हित पूर्णांक type)
{
  अगर(type >= CURLMNOTIFY_LAST)
    वापस CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  वापस CURLM_OK;
}

व्योम Curl_mntfy_add(काष्ठा Curl_easy *तथ्य, अचिन्हित पूर्णांक type)
{
  काष्ठा Curl_multi *multi = तथ्य ? तथ्य->multi : शून्य;
  अगर(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    काष्ठा mntfy_chunk *पुच्छ = mntfy_non_full_पुच्छ(&multi->ntfy);
    CURL_TRC_M(तथ्य, "[NTFY] add %u for xfer %u", type, तथ्य->mid);
    अगर(पुच्छ)
      mntfy_chunk_append(पुच्छ, तथ्य, (uint32_t)type);
    अन्यथा
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(काष्ठा Curl_multi *multi)
{
  काष्ठा Curl_mapi_guard guard;

  अगर(!multi)
    वापस CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  जबतक(multi->ntfy.शीर्ष && !multi->ntfy.failure) {
    काष्ठा mntfy_chunk *chunk = multi->ntfy.शीर्ष;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    अगर(chunk == multi->ntfy.पुच्छ) /* last one, keep */
      अवरोध;
    DEBUGASSERT(chunk->अग्र);
    DEBUGASSERT(multi->ntfy.शीर्ष != multi->ntfy.पुच्छ);
    multi->ntfy.शीर्ष = chunk->अग्र;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  अगर(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    वापस mresult;
  }
  अन्यथा
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  वापस CURLM_OK;
}
