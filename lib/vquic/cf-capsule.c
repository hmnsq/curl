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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "vquic/capsule.h"
#അന്തര്ഭാവ "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_SIZE     (16 * 1024)

ഘടന cf_capsule_ctx {
  ഘടന bufq recvbuf;
  ഘടന bufq sendbuf;
};

സ്ഥിതം ശൂന്യം cf_capsule_destroy(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data)
{
  ഘടന cf_capsule_ctx *ctx = cf->ctx;
  (ശൂന്യം)data;
  എങ്കിൽ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

സ്ഥിതം CURLcode cf_capsule_connect(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   bool *done)
{
  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }
  എങ്കിൽ(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    എങ്കിൽ(!result && *done)
      cf->connected = TRUE;
    തിരികെ result;
  }
  *done = FALSE;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_capsule_flush(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data)
{
  ഘടന cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  എങ്കിൽ(Curl_bufq_is_empty(&ctx->sendbuf))
    തിരികെ CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, ശൂന്യ, 0,
                             &nwritten);
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    തിരികെ result;
  }
  തിരികെ Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

സ്ഥിതം CURLcode cf_capsule_send(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                സ്ഥിരം uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  ഘടന cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (ശൂന്യം)eos;
  *pnwritten = 0;

  എങ്കിൽ(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  എങ്കിൽ(result)
    തിരികെ result;

  result = cf_capsule_flush(cf, data);
  എങ്കിൽ(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  എങ്കിൽ(!result)
    *pnwritten = len;
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_capsule_recv(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                അക്ഷരം *buf, size_t len,
                                size_t *pnread)
{
  ഘടന cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  ആകുമ്പോൾ(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    എങ്കിൽ(result == CURLE_AGAIN)
      നിർത്തുക;
    എങ്കിൽ(result)
      തിരികെ result;
    എങ്കിൽ(!nread)
      നിർത്തുക;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned അക്ഷരം *)buf, len,
                                         &result);
  തിരികെ result;
}

സ്ഥിതം bool cf_capsule_data_pending(ഘടന Curl_cfilter *cf,
                                    സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന cf_capsule_ctx *ctx = cf->ctx;

  എങ്കിൽ(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    തിരികെ TRUE;
  തിരികെ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

സ്ഥിതം CURLcode cf_capsule_cntrl(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  CURLcode result = CURLE_OK;

  (ശൂന്യം)arg1;
  (ശൂന്യം)arg2;
  തിരഞ്ഞെടുപ്പ്(event) {
  അവസ്ഥ CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_capsule_query(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_capsule_ctx *ctx = cf->ctx;

  (ശൂന്യം)pres2;
  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_NEED_FLUSH: {
    എങ്കിൽ(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  }
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

സ്ഥിതം CURLcode cf_capsule_adjust_pollset(ഘടന Curl_cfilter *cf,
                                          ഘടന Curl_easy *data,
                                          ഘടന easy_pollset *ps)
{
  ഘടന cf_capsule_ctx *ctx = cf->ctx;

  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    എങ്കിൽ(sock != CURL_SOCKET_BAD)
      തിരികെ Curl_pollset_add_out(data, ps, sock);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_capsule_shutdown(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  അല്ലെങ്കിൽ {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    എങ്കിൽ(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  തിരികെ result;
}

ഘടന Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_data_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

സ്ഥിതം CURLcode cf_capsule_create(ഘടന Curl_cfilter **pcf,
                                  ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന cf_capsule_ctx *ctx;
  CURLcode result;

  (ശൂന്യം)data;
  (ശൂന്യം)conn;
  *pcf = ശൂന്യ;
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : ശൂന്യ;
  എങ്കിൽ(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  തിരികെ result;
}

CURLcode Curl_cf_capsule_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                      ഘടന Curl_easy *data)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  എങ്കിൽ(!result)
    Curl_conn_cf_തിരുകുക_after(cf_at, cf);
  തിരികെ result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
