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

#যদি !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "vquic/capsule.h"
#অন্তর্ভাব "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_আকার     (16 * 1024)

কাঠামো cf_capsule_ctx {
  কাঠামো bufq recvbuf;
  কাঠামো bufq sendbuf;
};

স্থির শূন্য cf_capsule_destroy(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_capsule_ctx *ctx = cf->ctx;
  (শূন্য)তথ্য;
  যদি(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

স্থির CURLcode cf_capsule_connect(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   bool *done)
{
  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }
  যদি(cf->পরবর্তী) {
    CURLcode ফলাফল = cf->পরবর্তী->cft->do_connect(cf->পরবর্তী, তথ্য, done);
    যদি(!ফলাফল && *done)
      cf->connected = TRUE;
    ফেরত ফলাফল;
  }
  *done = FALSE;
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_capsule_flush(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  size_t nwritten;

  যদি(Curl_bufq_is_empty(&ctx->sendbuf))
    ফেরত CURLE_OK;

  ফলাফল = Curl_cf_send_bufq(cf->পরবর্তী, তথ্য, &ctx->sendbuf, ফাঁকা, 0,
                             &nwritten);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN) {
      CURL_TRC_CF(তথ্য, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    ফেরত ফলাফল;
  }
  ফেরত Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

স্থির CURLcode cf_capsule_send(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                ধ্রুবক uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  কাঠামো cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;

  (শূন্য)eos;
  *pnwritten = 0;

  যদি(Curl_bufq_is_full(&ctx->sendbuf)) {
    ফলাফল = cf_capsule_flush(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* encapsulate new payload into a capsule */
  ফলাফল = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = cf_capsule_flush(cf, তথ্য);
  যদি(ফলাফল == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    ফলাফল = CURLE_OK;
  }

  যদি(!ফলাফল)
    *pnwritten = len;
  ফেরত ফলাফল;
}

স্থির CURLcode cf_capsule_recv(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                অক্ষর *buf, size_t len,
                                size_t *pnread)
{
  কাঠামো cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  যখন(!Curl_bufq_is_full(&ctx->recvbuf)) {
    ফলাফল = Curl_cf_recv_bufq(cf->পরবর্তী, তথ্য, &ctx->recvbuf, 0, &nread);
    যদি(ফলাফল == CURLE_AGAIN)
      বিরতি;
    যদি(ফলাফল)
      ফেরত ফলাফল;
    যদি(!nread)
      বিরতি;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, তথ্য, &ctx->recvbuf,
                                         (unsigned অক্ষর *)buf, len,
                                         &ফলাফল);
  ফেরত ফলাফল;
}

স্থির bool cf_capsule_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                                    ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_capsule_ctx *ctx = cf->ctx;

  যদি(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    ফেরত TRUE;
  ফেরত cf->পরবর্তী ? cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য) : FALSE;
}

স্থির CURLcode cf_capsule_cntrl(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)arg1;
  (শূন্য)arg2;
  নির্বাচন(event) {
  ক্ষেত্র CF_CTRL_FLUSH:
    ফলাফল = cf_capsule_flush(cf, তথ্য);
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_capsule_query(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_capsule_ctx *ctx = cf->ctx;

  (শূন্য)pres2;
  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_NEED_FLUSH: {
    যদি(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      ফেরত CURLE_OK;
    }
    বিরতি;
  }
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode cf_capsule_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                          কাঠামো Curl_easy *তথ্য,
                                          কাঠামো easy_pollset *ps)
{
  কাঠামো cf_capsule_ctx *ctx = cf->ctx;

  যদি(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, তথ্য);
    যদি(sock != CURL_SOCKET_BAD)
      ফেরত Curl_pollset_add_out(তথ্য, ps, sock);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_capsule_shutdown(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  অন্যথা {
    ফলাফল = cf_capsule_flush(cf, তথ্য);
    *done = !ফলাফল;
    যদি(ফলাফল == CURLE_AGAIN)
      ফলাফল = CURLE_OK;
  }
  ফেরত ফলাফল;
}

কাঠামো Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_তথ্য_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

স্থির CURLcode cf_capsule_create(কাঠামো Curl_cfilter **pcf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো cf_capsule_ctx *ctx;
  CURLcode ফলাফল;

  (শূন্য)তথ্য;
  (শূন্য)conn;
  *pcf = ফাঁকা;
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_আকার, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_আকার, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!ফলাফল) ? cf : ফাঁকা;
  যদি(ফলাফল && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  ফেরত ফলাফল;
}

CURLcode Curl_cf_capsule_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                      কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;

  ফলাফল = cf_capsule_create(&cf, তথ্য, cf_at->conn);
  যদি(!ফলাফল)
    Curl_conn_cf_ঢোকাও_after(cf_at, cf);
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
