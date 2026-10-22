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
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "multihandle.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "multi_ntfy.h"

#ఒకవేళ CURLMNOTIFY_చివరి > 31
#దోషం "notification types must fit below the curl_multi_ntfy high flag bit"
#అయితే_ముగింపు

#నిర్వచించు CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

నిర్మాణం mntfy_నమోదు {
  uint32_t mid;
  uint32_t type;
};

#నిర్వచించు CURL_MNTFY_CHUNK_కొలత 128

నిర్మాణం mntfy_chunk {
  నిర్మాణం mntfy_chunk *తదుపరి;
  size_t r_స్థానభ్రంశం;
  size_t w_స్థానభ్రంశం;
  నిర్మాణం mntfy_నమోదు entries[CURL_MNTFY_CHUNK_కొలత];
};

స్థిర నిర్మాణం mntfy_chunk *mnfty_chunk_create(శూన్య)
{
  తిరిగి curlx_calloc(1, పరిమాణం(నిర్మాణం mntfy_chunk));
}

స్థిర శూన్య mnfty_chunk_destroy(నిర్మాణం mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

స్థిర శూన్య mnfty_chunk_reset(నిర్మాణం mntfy_chunk *chunk)
{
  chunk->r_స్థానభ్రంశం = chunk->w_స్థానభ్రంశం = 0;
}

స్థిర bool mntfy_chunk_append(నిర్మాణం mntfy_chunk *chunk,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               uint32_t type)
{
  నిర్మాణం mntfy_నమోదు *e;

  ఒకవేళ(chunk->w_స్థానభ్రంశం >= CURL_MNTFY_CHUNK_కొలత)
    తిరిగి FALSE;
  e = &chunk->entries[chunk->w_స్థానభ్రంశం++];
  e->mid = దత్తాంశం->mid;
  e->type = type;
  తిరిగి TRUE;
}

స్థిర నిర్మాణం mntfy_chunk *mntfy_non_నిండిన_తోక(నిర్మాణం curl_multi_ntfy *mntfy)
{
  నిర్మాణం mntfy_chunk *chunk;
  ఒకవేళ(!mntfy->తోక) {
    chunk = mnfty_chunk_create();
    ఒకవేళ(!chunk)
      తిరిగి ఖాళీ;
    DEBUGASSERT(!mntfy->తల);
    mntfy->తల = mntfy->తోక = chunk;
    తిరిగి chunk;
  }
  లేకపోతే ఒకవేళ(mntfy->తోక->w_స్థానభ్రంశం < CURL_MNTFY_CHUNK_కొలత)
    తిరిగి mntfy->తోక;
  లేకపోతే { /* tail is full. */
    chunk = mnfty_chunk_create();
    ఒకవేళ(!chunk)
      తిరిగి ఖాళీ;
    DEBUGASSERT(mntfy->తల);
    mntfy->తోక->తదుపరి = chunk;
    mntfy->తోక = chunk;
    తిరిగి chunk;
  }
}

స్థిర శూన్య mntfy_chunk_dispatch_all(నిర్మాణం Curl_multi *multi,
                                     నిర్మాణం mntfy_chunk *chunk)
{
  నిర్మాణం mntfy_నమోదు *e;
  నిర్మాణం Curl_easy *దత్తాంశం;

  అయితే(multi->ntfy.ntfy_cb &&
        (chunk->r_స్థానభ్రంశం < chunk->w_స్థానభ్రంశం) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_స్థానభ్రంశం];
    దత్తాంశం = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    ఒకవేళ(దత్తాంశం && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, దత్తాంశం, multi->ntfy.ntfy_cb_దత్తాంశం);
    }
    /* once dispatched, safe to increment */
    chunk->r_స్థానభ్రంశం++;
  }
  mnfty_chunk_reset(chunk);
}

శూన్య Curl_mntfy_init(నిర్మాణం Curl_multi *multi)
{
  memset(&multi->ntfy, 0, పరిమాణం(multi->ntfy));
}

శూన్య Curl_mntfy_cleanup(నిర్మాణం Curl_multi *multi)
{
  అయితే(multi->ntfy.తల) {
    నిర్మాణం mntfy_chunk *chunk = multi->ntfy.తల;
    multi->ntfy.తల = chunk->తదుపరి;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.తోక = ఖాళీ;
}

CURLMcode Curl_mntfy_enable(నిర్మాణం Curl_multi *multi, గుర్తులేని పూర్ణాంకం type)
{
  ఒకవేళ(type >= CURLMNOTIFY_చివరి)
    తిరిగి CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  తిరిగి CURLM_OK;
}

CURLMcode Curl_mntfy_disable(నిర్మాణం Curl_multi *multi, గుర్తులేని పూర్ణాంకం type)
{
  ఒకవేళ(type >= CURLMNOTIFY_చివరి)
    తిరిగి CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  తిరిగి CURLM_OK;
}

శూన్య Curl_mntfy_కూడు(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని పూర్ణాంకం type)
{
  నిర్మాణం Curl_multi *multi = దత్తాంశం ? దత్తాంశం->multi : ఖాళీ;
  ఒకవేళ(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_చివరి &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    నిర్మాణం mntfy_chunk *తోక = mntfy_non_నిండిన_తోక(&multi->ntfy);
    CURL_TRC_M(దత్తాంశం, "[NTFY] add %u for xfer %u", type, దత్తాంశం->mid);
    ఒకవేళ(తోక)
      mntfy_chunk_append(తోక, దత్తాంశం, (uint32_t)type);
    లేకపోతే
      multi->ntfy.failure = CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(నిర్మాణం Curl_multi *multi)
{
  నిర్మాణం Curl_mapi_guard guard;

  ఒకవేళ(!multi)
    తిరిగి CURLM_BAD_FUNCTION_వాదం;

  CURL_CBAPI_MULTI_ప్రారంభం(&guard, multi, multi_ntfy_cb);

  అయితే(multi->ntfy.తల && !multi->ntfy.failure) {
    నిర్మాణం mntfy_chunk *chunk = multi->ntfy.తల;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_స్థానభ్రంశం == chunk->w_స్థానభ్రంశం);

    ఒకవేళ(chunk == multi->ntfy.తోక) /* last one, keep */
      ఆపు;
    DEBUGASSERT(chunk->తదుపరి);
    DEBUGASSERT(multi->ntfy.తల != multi->ntfy.తోక);
    multi->ntfy.తల = chunk->తదుపరి;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_చివర(&guard);

  ఒకవేళ(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    తిరిగి mresult;
  }
  లేకపోతే
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  తిరిగి CURLM_OK;
}
