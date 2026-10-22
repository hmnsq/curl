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
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multihandle.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "multi_ntfy.h"

#यदि CURLMNOTIFY_चरमः > 31
#दोषः "notification types must fit below the curl_multi_ntfy high flag bit"
#यदि_अन्तः

#परिभाषय CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

संरचना mntfy_प्रविष्टिः {
  uint32_t mid;
  uint32_t type;
};

#परिभाषय CURL_MNTFY_CHUNK_आकार 128

संरचना mntfy_chunk {
  संरचना mntfy_chunk *अग्र;
  size_t r_विस्थापनम्;
  size_t w_विस्थापनम्;
  संरचना mntfy_प्रविष्टिः entries[CURL_MNTFY_CHUNK_आकार];
};

स्थिर संरचना mntfy_chunk *mnfty_chunk_create(शून्य)
{
  प्रत्यागम curlx_calloc(1, परिमाण(संरचना mntfy_chunk));
}

स्थिर शून्य mnfty_chunk_destroy(संरचना mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

स्थिर शून्य mnfty_chunk_reset(संरचना mntfy_chunk *chunk)
{
  chunk->r_विस्थापनम् = chunk->w_विस्थापनम् = 0;
}

स्थिर bool mntfy_chunk_append(संरचना mntfy_chunk *chunk,
                               संरचना Curl_easy *दत्त,
                               uint32_t type)
{
  संरचना mntfy_प्रविष्टिः *e;

  यदि(chunk->w_विस्थापनम् >= CURL_MNTFY_CHUNK_आकार)
    प्रत्यागम FALSE;
  e = &chunk->entries[chunk->w_विस्थापनम्++];
  e->mid = दत्त->mid;
  e->type = type;
  प्रत्यागम TRUE;
}

स्थिर संरचना mntfy_chunk *mntfy_non_पूर्णम्_पुच्छ(संरचना curl_multi_ntfy *mntfy)
{
  संरचना mntfy_chunk *chunk;
  यदि(!mntfy->पुच्छ) {
    chunk = mnfty_chunk_create();
    यदि(!chunk)
      प्रत्यागम रिक्त;
    DEBUGASSERT(!mntfy->शीर्ष);
    mntfy->शीर्ष = mntfy->पुच्छ = chunk;
    प्रत्यागम chunk;
  }
  अन्यथा यदि(mntfy->पुच्छ->w_विस्थापनम् < CURL_MNTFY_CHUNK_आकार)
    प्रत्यागम mntfy->पुच्छ;
  अन्यथा { /* tail is full. */
    chunk = mnfty_chunk_create();
    यदि(!chunk)
      प्रत्यागम रिक्त;
    DEBUGASSERT(mntfy->शीर्ष);
    mntfy->पुच्छ->अग्र = chunk;
    mntfy->पुच्छ = chunk;
    प्रत्यागम chunk;
  }
}

स्थिर शून्य mntfy_chunk_dispatch_all(संरचना Curl_multi *multi,
                                     संरचना mntfy_chunk *chunk)
{
  संरचना mntfy_प्रविष्टिः *e;
  संरचना Curl_easy *दत्त;

  यावत्(multi->ntfy.ntfy_cb &&
        (chunk->r_विस्थापनम् < chunk->w_विस्थापनम्) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_विस्थापनम्];
    दत्त = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    यदि(दत्त && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, दत्त, multi->ntfy.ntfy_cb_दत्त);
    }
    /* once dispatched, safe to increment */
    chunk->r_विस्थापनम्++;
  }
  mnfty_chunk_reset(chunk);
}

शून्य Curl_mntfy_init(संरचना Curl_multi *multi)
{
  स्मृतिं_पूरय(&multi->ntfy, 0, परिमाण(multi->ntfy));
}

शून्य Curl_mntfy_cleanup(संरचना Curl_multi *multi)
{
  यावत्(multi->ntfy.शीर्ष) {
    संरचना mntfy_chunk *chunk = multi->ntfy.शीर्ष;
    multi->ntfy.शीर्ष = chunk->अग्र;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.पुच्छ = रिक्त;
}

CURLMcode Curl_mntfy_enable(संरचना Curl_multi *multi, अचिह्नित पूर्णाङ्क type)
{
  यदि(type >= CURLMNOTIFY_चरमः)
    प्रत्यागम CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  प्रत्यागम CURLM_OK;
}

CURLMcode Curl_mntfy_disable(संरचना Curl_multi *multi, अचिह्नित पूर्णाङ्क type)
{
  यदि(type >= CURLMNOTIFY_चरमः)
    प्रत्यागम CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  प्रत्यागम CURLM_OK;
}

शून्य Curl_mntfy_सङ्कलय(संरचना Curl_easy *दत्त, अचिह्नित पूर्णाङ्क type)
{
  संरचना Curl_multi *multi = दत्त ? दत्त->multi : रिक्त;
  यदि(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_चरमः &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    संरचना mntfy_chunk *पुच्छ = mntfy_non_पूर्णम्_पुच्छ(&multi->ntfy);
    CURL_TRC_M(दत्त, "[NTFY] add %u for xfer %u", type, दत्त->mid);
    यदि(पुच्छ)
      mntfy_chunk_append(पुच्छ, दत्त, (uint32_t)type);
    अन्यथा
      multi->ntfy.failure = CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(संरचना Curl_multi *multi)
{
  संरचना Curl_mapi_guard guard;

  यदि(!multi)
    प्रत्यागम CURLM_BAD_FUNCTION_तर्कः;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  यावत्(multi->ntfy.शीर्ष && !multi->ntfy.failure) {
    संरचना mntfy_chunk *chunk = multi->ntfy.शीर्ष;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_विस्थापनम् == chunk->w_विस्थापनम्);

    यदि(chunk == multi->ntfy.पुच्छ) /* last one, keep */
      भङ्ग;
    DEBUGASSERT(chunk->अग्र);
    DEBUGASSERT(multi->ntfy.शीर्ष != multi->ntfy.पुच्छ);
    multi->ntfy.शीर्ष = chunk->अग्र;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_अन्तः(&guard);

  यदि(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    प्रत्यागम mresult;
  }
  अन्यथा
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  प्रत्यागम CURLM_OK;
}
