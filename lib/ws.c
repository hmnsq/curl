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
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "ws.h"

#ifndef CURL_DISABLE_WEBSOCKETS

#ضمن "url.h"
#ضمن "bufq.h"
#ضمن "curlx/dynbuf.h"
#ضمن "rand.h"
#ضمن "curlx/base64.h"
#ضمن "cf-recvbuf.h"
#ضمن "connect.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "multiif.h"
#ضمن "easyif.h"
#ضمن "transfer.h"
#ضمن "select.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/strcopy.h"

/* RFC 6455 Section 5.2

    0 1 2 3 4 5 6 7
   +-+-+-+-+-------+
   |F|R|R|R| opcode|
   |I|S|S|S|  (4)  |
   |N|V|V|V|       |
   | |1|2|3|       |
 */
#define WSBIT_FIN          0x80
#define WSBIT_RSV1         0x40
#define WSBIT_RSV2         0x20
#define WSBIT_RSV3         0x10
#define WSBIT_RSV_MASK     (WSBIT_RSV1 | WSBIT_RSV2 | WSBIT_RSV3)
#define WSBIT_OPCODE_CONT  0x0
#define WSBIT_OPCODE_TEXT  0x1
#define WSBIT_OPCODE_BIN   0x2
#define WSBIT_OPCODE_CLOSE 0x8
#define WSBIT_OPCODE_PING  0x9
#define WSBIT_OPCODE_PONG  0xa
#ifdef CURLVERBOSE
#define WSBIT_OPCODE_MASK  0xf
#endif

#define WSBIT_MASK 0x80

/* buffer dimensioning */
#define WS_CHUNK_SIZE  65535
#define WS_CHUNK_عدد 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
تعداد ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_HEAD,
  WS_DEC_PAYLOAD
};

بنية ws_decoder {
  صحيح frame_age;        /* zero */
  صحيح frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t head[10];
  صحيح head_len, head_total;
  تعداد ws_dec_state state;
  صحيح cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
بنية ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  unsigned صحيح xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#define WS_MAX_CNTRL_LEN    125

بنية ws_cntrl_frame {
  unsigned صحيح type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
بنية websocket {
  بنية Curl_easy *data; /* used for write callback handling */
  بنية ws_decoder dec;  /* decode of ws frames */
  بنية ws_encoder enc;  /* encode of ws frames */
  بنية bufq recvbuf;    /* raw data from the server */
  بنية bufq sendbuf;    /* raw data to be sent to the server */
  بنية curl_ws_frame recvframe;  /* the current WS FRAME received */
  بنية ws_cntrl_frame pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#ifdef CURLVERBOSE
ساكن ثابت حرف *ws_frame_name_of_op(uint8_t firstbyte)
{
  بدل(firstbyte & WSBIT_OPCODE_MASK) {
  حالة WSBIT_OPCODE_CONT:
    أرجع "CONT";
  حالة WSBIT_OPCODE_TEXT:
    أرجع "TEXT";
  حالة WSBIT_OPCODE_BIN:
    أرجع "BIN";
  حالة WSBIT_OPCODE_CLOSE:
    أرجع "CLOSE";
  حالة WSBIT_OPCODE_PING:
    أرجع "PING";
  حالة WSBIT_OPCODE_PONG:
    أرجع "PONG";
  افتراضي:
    أرجع "???";
  }
}
#endif

ساكن صحيح ws_frame_firstbyte2flags(بنية Curl_easy *data,
                                    uint8_t firstbyte, صحيح cont_flags)
{
  بدل(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  حالة WSBIT_OPCODE_CONT:
    إذا(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      أرجع 0;
    }
    أرجع cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  حالة (WSBIT_OPCODE_CONT | WSBIT_FIN):
    إذا(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      أرجع 0;
    }
    أرجع cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  حالة WSBIT_OPCODE_TEXT:
    إذا(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      أرجع 0;
    }
    أرجع CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  حالة (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    إذا(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      أرجع 0;
    }
    أرجع CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  حالة WSBIT_OPCODE_BIN:
    إذا(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      أرجع 0;
    }
    أرجع CURLWS_ثنائي | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  حالة (WSBIT_OPCODE_BIN | WSBIT_FIN):
    إذا(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      أرجع 0;
    }
    أرجع CURLWS_ثنائي;
  /* 0x08 - first CLOSE fragment */
  حالة WSBIT_OPCODE_CLOSE:
    failf(data, "[WS] invalid fragmented CLOSE frame");
    أرجع 0;
  /* 0x88 - unfragmented CLOSE */
  حالة (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    أرجع CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  حالة WSBIT_OPCODE_PING:
    failf(data, "[WS] invalid fragmented PING frame");
    أرجع 0;
  /* 0x89 - unfragmented PING */
  حالة (WSBIT_OPCODE_PING | WSBIT_FIN):
    أرجع CURLWS_PING;
  /* 0x0a - first PONG fragment */
  حالة WSBIT_OPCODE_PONG:
    failf(data, "[WS] invalid fragmented PONG frame");
    أرجع 0;
  /* 0x8a - unfragmented PONG */
  حالة (WSBIT_OPCODE_PONG | WSBIT_FIN):
    أرجع CURLWS_PONG;
  /* invalid first byte */
  افتراضي:
    إذا(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(data, "[WS] invalid reserved bits: %02x", firstbyte);
    وإلا
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(data, "[WS] invalid opcode: %02x", firstbyte);
    أرجع 0;
  }
}

ساكن CURLcode ws_frame_flags2firstbyte(بنية Curl_easy *data,
                                         unsigned صحيح flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  بدل(flags & ~CURLWS_OFFSET) {
  حالة 0:
    إذا(contfragment) {
      CURL_TRC_WS(data, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      أرجع CURLE_OK;
    }
    failf(data, "[WS] no flags given");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  حالة CURLWS_CONT:
    إذا(contfragment) {
      infof(data, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      أرجع CURLE_OK;
    }
    failf(data, "[WS] No ongoing fragmented message to continue");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  حالة CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    أرجع CURLE_OK;
  حالة (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    أرجع CURLE_OK;
  حالة CURLWS_ثنائي:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    أرجع CURLE_OK;
  حالة (CURLWS_ثنائي | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    أرجع CURLE_OK;
  حالة CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_CLOSE | CURLWS_CONT):
    failf(data, "[WS] CLOSE frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  حالة CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_PING | CURLWS_CONT):
    failf(data, "[WS] PING frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  حالة CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_PONG | CURLWS_CONT):
    failf(data, "[WS] PONG frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  افتراضي:
    failf(data, "[WS] unknown flags: %x", flags);
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

ساكن فراغ ws_dec_info(بنية ws_decoder *dec, بنية Curl_easy *data,
                        ثابت حرف *رسالة)
{
  NOVERBOSE((فراغ)رسالة);
  بدل(dec->head_len) {
  حالة 0:
    اكسر;
  حالة 1:
    CURL_TRC_WS(data, "decoded %s [%s%s]", رسالة,
                ws_frame_name_of_op(dec->head[0]),
                (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    اكسر;
  افتراضي:
    إذا(dec->head_len < dec->head_total) {
      CURL_TRC_WS(data, "decoded %s [%s%s](%d/%d)", رسالة,
                  ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->head_len, dec->head_total);
    }
    وإلا {
      CURL_TRC_WS(data, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  رسالة, ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    اكسر;
  }
}

ساكن CURLcode ws_send_raw_blocking(بنية Curl_easy *data,
                                     بنية websocket *ws,
                                     ثابت حرف *buffer, size_t buflen);

تعريفنوع CURLcode ws_write_payload(ثابت uint8_t *buf, size_t buflen,
                                  صحيح frame_age, صحيح frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  فراغ *userp,
                                  size_t *pnwritten);

ساكن فراغ ws_dec_next_frame(بنية ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ساكن فراغ ws_dec_reset(بنية ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

ساكن فراغ ws_dec_init(بنية ws_decoder *dec)
{
  ws_dec_reset(dec);
}

ساكن CURLcode ws_dec_read_head(بنية ws_decoder *dec,
                                 بنية Curl_easy *data,
                                 بنية bufq *inraw)
{
  ثابت uint8_t *inbuf;
  size_t inlen;

  طالما(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    إذا(dec->head_len == 0) {
      dec->head[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(data, dec->head[0],
                                                  dec->cont_flags);
      إذا(!dec->frame_flags) {
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      إذا(dec->frame_flags & (CURLWS_TEXT | CURLWS_ثنائي)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->head_len = 1;
#إذا 0
      ws_dec_info(dec, data, "seeing opcode");
#endif
      استمر;
    }
    وإلا إذا(dec->head_len == 1) {
      dec->head[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->head_len = 2;

      إذا(dec->head[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(data, "[WS] masked input frame");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->frame_flags & CURLWS_PING && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(data, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->frame_flags & CURLWS_PONG && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(data, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->frame_flags & CURLWS_CLOSE && dec->head[1] > WS_MAX_CNTRL_LEN) {
        failf(data, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      إذا(dec->head[1] == 126) {
        dec->head_total = 4;
        استمر;
      }
      وإلا إذا(dec->head[1] == 127) {
        dec->head_total = 10;
        استمر;
      }
      وإلا {
        dec->head_total = 2;
      }
    }

    إذا(dec->head_len < dec->head_total) {
      dec->head[dec->head_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->head_len;
      إذا(dec->head_len < dec->head_total) {
#إذا 0
        ws_dec_info(dec, data, "decoding head");
#endif
        استمر;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->head_len == dec->head_total);
    بدل(dec->head_total) {
    حالة 2:
      dec->payload_len = dec->head[1];
      اكسر;
    حالة 4:
      dec->payload_len = (dec->head[2] << 8) | dec->head[3];
      اكسر;
    حالة 10:
      إذا(dec->head[2] > 127) {
        failf(data, "[WS] frame length longer than 63 bits not supported");
        أرجع CURLE_RECV_ERROR;
      }
      dec->payload_len =
        (curl_off_t)dec->head[2] << 56 |
        (curl_off_t)dec->head[3] << 48 |
        (curl_off_t)dec->head[4] << 40 |
        (curl_off_t)dec->head[5] << 32 |
        (curl_off_t)dec->head[6] << 24 |
        (curl_off_t)dec->head[7] << 16 |
        (curl_off_t)dec->head[8] <<  8 |
        dec->head[9];
      اكسر;
    افتراضي:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(data, "[WS] unexpected frame header length");
      أرجع CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, data, "head");
    أرجع CURLE_OK;
  }
  أرجع CURLE_AGAIN;
}

ساكن CURLcode ws_dec_pass_payload(بنية ws_decoder *dec,
                                    بنية Curl_easy *data,
                                    بنية bufq *inraw,
                                    ws_write_payload *write_cb,
                                    فراغ *write_ctx)
{
  ثابت uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode result;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  طالما(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(data)) {
    إذا(inlen > remain)
      inlen = remain;
    result = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    إذا(result)
      أرجع result;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(data, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  أرجع remain ? CURLE_AGAIN : CURLE_OK;
}

ساكن CURLcode ws_dec_pass(بنية ws_decoder *dec,
                            بنية Curl_easy *data,
                            بنية bufq *inraw,
                            ws_write_payload *write_cb,
                            فراغ *write_ctx)
{
  CURLcode result;

  إذا(Curl_bufq_is_empty(inraw))
    أرجع CURLE_AGAIN;

  بدل(dec->state) {
  حالة WS_DEC_INIT:
    ws_dec_next_frame(dec);
    dec->state = WS_DEC_HEAD;
    FALLTHROUGH();
  حالة WS_DEC_HEAD:
    result = ws_dec_read_head(dec, data, inraw);
    إذا(result) {
      إذا(result != CURLE_AGAIN) {
        failf(data, "[WS] decode frame error %d", (صحيح)result);
        اكسر;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      اكسر;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    إذا(dec->payload_len == 0) {
      size_t nwritten;
      ثابت uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      result = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      إذا(result)
        أرجع result;
      dec->state = WS_DEC_INIT;
      اكسر;
    }
    FALLTHROUGH();
  حالة WS_DEC_PAYLOAD:
    result = ws_dec_pass_payload(dec, data, inraw, write_cb, write_ctx);
    ws_dec_info(dec, data, "passing");
    إذا(result)
      أرجع result;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    اكسر;
  افتراضي:
    /* we covered all enums above, but some code analyzers are wimps */
    result = CURLE_FAILED_INIT;
  }
  أرجع result;
}

ساكن فراغ update_meta(بنية websocket *ws,
                        صحيح frame_age, صحيح frame_flags,
                        curl_off_t payload_offset,
                        curl_off_t payload_len,
                        size_t cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_offset - cur_len);

  ws->recvframe.age = frame_age;
  ws->recvframe.flags = frame_flags;
  ws->recvframe.offset = payload_offset;
  ws->recvframe.len = cur_len;
  ws->recvframe.bytesleft = bytesleft;
}

/* WebSocket decoding client writer */
بنية ws_cw_ctx {
  بنية Curl_cwriter super;
  بنية bufq buf;
};

ساكن CURLcode ws_cw_init(بنية Curl_easy *data,
                           بنية Curl_cwriter *writer)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  (فراغ)data;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_SIZE, 1, BUFQ_OPT_SOFT_LIMIT);
  أرجع CURLE_OK;
}

ساكن فراغ ws_cw_close(بنية Curl_easy *data, بنية Curl_cwriter *writer)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  (فراغ)data;
  Curl_bufq_free(&ctx->buf);
}

بنية ws_cw_dec_ctx {
  بنية Curl_easy *data;
  بنية websocket *ws;
  بنية Curl_cwriter *next_writer;
  صحيح cw_type;
};

ساكن CURLcode ws_flush(بنية Curl_easy *data, بنية websocket *ws,
                         bool blocking);
ساكن CURLcode ws_enc_add_pending(بنية Curl_easy *data,
                                   بنية websocket *ws);

ساكن CURLcode ws_enc_add_cntrl(بنية Curl_easy *data,
                                 بنية websocket *ws,
                                 ثابت uint8_t *payload,
                                 size_t plen,
                                 unsigned صحيح frame_type)
{
  (فراغ)data;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  إذا(plen > WS_MAX_CNTRL_LEN)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  memcpy(ws->pending.payload, payload, plen);
  أرجع CURLE_OK;
}

ساكن curl_off_t ws_payload_remain(curl_off_t payload_total,
                                    curl_off_t payload_offset,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_total - payload_offset;
  إذا((payload_total < 0) || (payload_offset < 0) || (remain < 0))
    أرجع -1;
  buffered = curlx_uztoso(payload_buffered);
  إذا(remain < buffered)
    أرجع -1;
  أرجع remain - buffered;
}

ساكن CURLcode ws_cw_dec_next(ثابت uint8_t *buf, size_t buflen,
                               صحيح frame_age, صحيح frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               فراغ *user_data,
                               size_t *pnwritten)
{
  بنية ws_cw_dec_ctx *ctx = user_data;
  بنية Curl_easy *data = ctx->data;
  بنية websocket *ws = ctx->ws;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result;

  (فراغ)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  إذا(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }

  إذا(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(data, ws, buf, buflen, CURLWS_PONG);
    إذا(result)
      أرجع result;
  }
  وإلا إذا(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(data, "[WS] pass %zu decoded bytes", buflen);
    result = Curl_cwriter_write(data, ctx->next_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ثابت حرف *)buf, buflen);
    إذا(result)
      أرجع result;
  }
  *pnwritten = buflen;
  أرجع CURLE_OK;
}

ساكن CURLcode ws_cw_write(بنية Curl_easy *data,
                            بنية Curl_cwriter *writer, صحيح type,
                            ثابت حرف *buf, size_t nbytes)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  بنية websocket *ws;
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[WS] write(len=%zu, type=%d)", nbytes, type);
  إذا(!(type & CLIENTWRITE_BODY) || data->set.ws_raw_mode)
    أرجع Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(data, "[WS] not a websocket transfer");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(nbytes) {
    size_t nwritten;
    result = Curl_bufq_write(&ctx->buf, (ثابت uint8_t *)buf,
                             nbytes, &nwritten);
    إذا(result) {
      infof(data, "[WS] error adding data to buffer %d", (صحيح)result);
      أرجع result;
    }
  }

  result = Curl_cwriter_flush(data, writer->next);
  إذا(result)
    اذهب out;

  طالما(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
    بنية ws_cw_dec_ctx pass_ctx;
    pass_ctx.data = data;
    pass_ctx.ws = ws;
    pass_ctx.next_writer = writer->next;
    pass_ctx.cw_type = type;
    result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                         ws_cw_dec_next, &pass_ctx);
    إذا(result == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      result = CURLE_OK;
      اذهب out;
    }
    وإلا إذا(result) {
      failf(data, "[WS] decode payload error %d", (صحيح)result);
      Curl_bufq_reset(&ctx->buf);
      اذهب out;
    }
  }

out:
  إذا(!result && !ws->using_reader) {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    إذا(result == CURLE_AGAIN)
      result = CURLE_OK;
  }

  إذا(!result && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(data, "[WS] decode ended with incomplete frame");
    result = CURLE_RECV_ERROR;
  }
  أرجع result;
}

ساكن CURLcode ws_cw_flush(بنية Curl_easy *data,
                            بنية Curl_cwriter *writer)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[ws] flush");
  إذا(!data->set.ws_raw_mode) {
    بنية ws_cw_ctx *ctx = writer->ctx;
    بنية websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    result = Curl_cwriter_flush(data, writer->next);
    إذا(result)
      اذهب out;

    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(data, "[WS] not a websocket transfer");
      أرجع CURLE_FAILED_INIT;
    }

    طالما(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
      بنية ws_cw_dec_ctx pass_ctx;
      pass_ctx.data = data;
      pass_ctx.ws = ws;
      pass_ctx.next_writer = writer->next;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                           ws_cw_dec_next, &pass_ctx);
      إذا(result == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        result = CURLE_OK;
        اذهب out;
      }
      وإلا إذا(result) {
        failf(data, "[WS] decode payload error %d", (صحيح)result);
        Curl_bufq_reset(&ctx->buf);
        اذهب out;
      }
    }
  }

out:
  إذا(!result)
    result = Curl_cwriter_flush(data, writer->next);
  أرجع result;
}

/* WebSocket payload decoding client writer. */
ساكن ثابت بنية Curl_cwtype ws_cw_decode = {
  "ws-decode",
  عدم,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  حجم(بنية ws_cw_ctx)
};

ساكن فراغ ws_enc_info(بنية ws_encoder *enc, بنية Curl_easy *data,
                        ثابت حرف *رسالة)
{
  NOVERBOSE((فراغ)enc);
  NOVERBOSE((فراغ)رسالة);
  CURL_TRC_WS(data, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              رسالة, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

ساكن فراغ ws_enc_reset(بنية ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

ساكن فراغ ws_enc_init(بنية ws_encoder *enc)
{
  ws_enc_reset(enc);
}

/* RFC 6455 Section 5.2

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-------+-+-------------+-------------------------------+
   |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
   |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
   |N|V|V|V|       |S|             |   (if payload len==126/127)   |
   | |1|2|3|       |K|             |                               |
   +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
   |     Extended payload length continued, if payload len == 127  |
   + - - - - - - - - - - - - - - - +-------------------------------+
   |                               |Masking-key, if MASK set to 1  |
   +-------------------------------+-------------------------------+
   | Masking-key (continued)       |          Payload Data         |
   +-------------------------------- - - - - - - - - - - - - - - - +
   :                     Payload Data continued ...                :
   + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
   |                     Payload Data continued ...                |
   +---------------------------------------------------------------+
 */

ساكن CURLcode ws_enc_add_frame(بنية Curl_easy *data,
                                 بنية ws_encoder *enc,
                                 unsigned صحيح flags,
                                 curl_off_t payload_len,
                                 بنية bufq *out)
{
  uint8_t firstb = 0;
  uint8_t head[14];
  CURLcode result;
  size_t hlen, nwritten;

  إذا(payload_len < 0) {
    failf(data, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    أرجع CURLE_SEND_ERROR;
  }

  إذا(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(data, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    أرجع CURLE_SEND_ERROR;
  }

  result = ws_frame_flags2firstbyte(data, flags, (bool)enc->contfragment,
                                    &firstb);
  إذا(result)
    أرجع result;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  إذا(flags & (CURLWS_TEXT | CURLWS_ثنائي)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  إذا(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PING frame is too big");
    أرجع CURLE_TOO_LARGE;
  }
  إذا(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PONG frame is too big");
    أرجع CURLE_TOO_LARGE;
  }
  إذا(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given CLOSE frame is too big");
    أرجع CURLE_TOO_LARGE;
  }

  head[0] = enc->firstbyte = firstb;
  إذا(payload_len > 65535) {
    head[1] = 127 | WSBIT_MASK;
    head[2] = (uint8_t)((payload_len >> 56) & 0xff);
    head[3] = (uint8_t)((payload_len >> 48) & 0xff);
    head[4] = (uint8_t)((payload_len >> 40) & 0xff);
    head[5] = (uint8_t)((payload_len >> 32) & 0xff);
    head[6] = (uint8_t)((payload_len >> 24) & 0xff);
    head[7] = (uint8_t)((payload_len >> 16) & 0xff);
    head[8] = (uint8_t)((payload_len >> 8) & 0xff);
    head[9] = (uint8_t)(payload_len & 0xff);
    hlen = 10;
  }
  وإلا إذا(payload_len >= 126) {
    head[1] = 126 | WSBIT_MASK;
    head[2] = (uint8_t)((payload_len >> 8) & 0xff);
    head[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  وإلا {
    head[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, data, "sending");

  /* 4 bytes random */

  result = Curl_rand(data, (uint8_t *)&enc->mask, حجم(enc->mask));
  إذا(result)
    أرجع result;

#ifdef DEBUGBUILD
  إذا(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->mask, 0, حجم(enc->mask));
#endif

  /* add 4 bytes mask */
  memcpy(&head[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  result = Curl_bufq_write(out, head, hlen, &nwritten);
  إذا(result)
    أرجع result;
  إذا(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ws_enc_write_head(بنية Curl_easy *data,
                                  بنية websocket *ws,
                                  بنية ws_encoder *enc,
                                  unsigned صحيح flags,
                                  curl_off_t payload_len,
                                  بنية bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  إذا(ws->pending.type) {
    CURLcode result = ws_enc_add_pending(data, ws);
    إذا(result)
      أرجع result;
  }
  أرجع ws_enc_add_frame(data, enc, flags, payload_len, out);
}

ساكن CURLcode ws_enc_write_payload(بنية ws_encoder *enc,
                                     بنية Curl_easy *data,
                                     ثابت uint8_t *buf, size_t buflen,
                                     بنية bufq *out, size_t *pnwritten)
{
  CURLcode result;
  size_t i, len, n, remain;

  *pnwritten = 0;
  إذا(Curl_bufq_is_full(out))
    أرجع CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  إذا(remain < len)
    len = remain;

  لكل(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    result = Curl_bufq_write(out, &c, 1, &n);
    إذا(result) {
      إذا((result != CURLE_AGAIN) || !i)
        أرجع result;
      اكسر;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, data, "buffered");
  أرجع CURLE_OK;
}

ساكن CURLcode ws_enc_add_pending(بنية Curl_easy *data,
                                   بنية websocket *ws)
{
  CURLcode result;
  size_t n;

  إذا(!ws->pending.type) /* no pending frame here */
    أرجع CURLE_OK;
  إذا(ws->enc.payload_remain) /* in the middle of another frame */
    أرجع CURLE_AGAIN;

  result = ws_enc_add_frame(data, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  إذا(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding head: %d",
                (صحيح)result);
    اذهب out;
  }
  result = ws_enc_write_payload(&ws->enc, data, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  إذا(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding payload: %d",
                (صحيح)result);
    اذهب out;
  }
  إذا(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(data, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    result = CURLE_SEND_ERROR;
    اذهب out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, حجم(ws->pending));

out:
  أرجع result;
}

ساكن CURLcode ws_enc_send(بنية Curl_easy *data,
                            بنية websocket *ws,
                            ثابت uint8_t *buffer,
                            size_t buflen,
                            curl_off_t fragsize,
                            unsigned صحيح flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!data->set.ws_raw_mode);
  *pnsent = 0;

  إذا(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    إذا(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(data, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    }
    إذا((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(data, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  وإلا {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    إذا(result)
      أرجع result;

    result = ws_enc_write_head(data, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    إذا(result) {
      CURL_TRC_WS(data, "curl_ws_send(), error writing frame head %d",
                  (صحيح)result);
      أرجع result;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  طالما(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    إذا(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      result = ws_enc_write_payload(&ws->enc, data,
                                    buffer + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      إذا(result && (result != CURLE_AGAIN))
        أرجع result;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      إذا(!ws->sendbuf_payload) {
        أرجع CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    إذا(!result && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      buffer += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    وإلا إذا(result == CURLE_AGAIN) {
      إذا(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        أرجع CURLE_OK;
      }
      وإلا {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(data, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        أرجع CURLE_AGAIN;
      }
    }
    وإلا
      أرجع result;  /* real error sending the data */
  }
  أرجع CURLE_OK;
}

بنية cr_ws_ctx {
  بنية Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

ساكن CURLcode cr_ws_init(بنية Curl_easy *data, بنية Curl_creader *reader)
{
  (فراغ)data;
  (فراغ)reader;
  أرجع CURLE_OK;
}

ساكن فراغ cr_ws_close(بنية Curl_easy *data, بنية Curl_creader *reader)
{
  (فراغ)data;
  (فراغ)reader;
}

ساكن CURLcode cr_ws_read(بنية Curl_easy *data,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_ws_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, n;
  بنية websocket *ws;
  bool eos;

  *pnread = 0;
  إذا(ctx->eos) {
    *peos = TRUE;
    أرجع CURLE_OK;
  }

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(data, "[WS] not a websocket transfer");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(Curl_bufq_is_empty(&ws->sendbuf)) {
    إذا(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      أرجع CURLE_OK;
    }

    إذا(ws->enc.payload_remain) {
      CURL_TRC_WS(data, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    إذا(result)
      أرجع result;
    ctx->read_eos = eos;

    إذا(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(data, reader->next);
    }
    وإلا إذا(!nread) {
      /* nothing to convert, return this right away */
      إذا(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      اذهب out;
    }

    إذا(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      result = ws_enc_write_head(data, ws, &ws->enc, CURLWS_ثنائي, nread,
                                 &ws->sendbuf);
      إذا(result)
        اذهب out;
    }

    result = ws_enc_write_payload(&ws->enc, data, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    إذا(result)
      اذهب out;
    CURL_TRC_READ(data, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  إذا(!result && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)result, *pnread, *peos);
  أرجع result;
}

ساكن curl_off_t cr_ws_total_length(بنية Curl_easy *data,
                                     بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)data;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_total_length,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  حجم(بنية cr_ws_ctx)
};

بنية wsfield {
  ثابت حرف *name;
  ثابت حرف *val;
};

CURLcode Curl_ws_request(بنية Curl_easy *data, بنية dynbuf *req)
{
  unsigned صحيح i;
  CURLcode result = CURLE_OK;
  uint8_t rand[16];
  حرف *randstr;
  size_t randlen;
  حرف keyval[40];
  بنية SingleRequest *k = &data->req;
  بنية wsfield heads[] = {
    {
      /* The request MUST contain an |Upgrade| header field whose value
         MUST include the "websocket" keyword. */
      "Upgrade", "websocket"
    },
    {
      /* The request MUST include a header field with the name
         |Sec-WebSocket-Version|. The value of this header field MUST be
         13. */
      "Sec-WebSocket-Version", "13",
    },
    {
      /* The request MUST include a header field with the name
         |Sec-WebSocket-Key|. The value of this header field MUST be a nonce
         consisting of a randomly selected 16-byte value that has been
         base64-encoded (see Section 4 of [RFC4648]). The nonce MUST be
         selected randomly for each connection. */
      "Sec-WebSocket-Key", عدم,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  result = Curl_rand(data, rand, حجم(rand));
  إذا(result)
    أرجع result;
  result = curlx_base64_encode(rand, حجم(rand), &randstr, &randlen);
  إذا(result)
    أرجع result;
  DEBUGASSERT(randlen < حجم(keyval));
  إذا(randlen >= حجم(keyval)) {
    curlx_free(randstr);
    أرجع CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, حجم(keyval), randstr, randlen);
  curlx_free(randstr);
  لكل(i = 0; !result && (i < CURL_ARRAYSIZE(heads)); i++) {
    إذا(!Curl_checkheaders(data, heads[i].name, strlen(heads[i].name))) {
      result = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  data->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  data->conn->bits.upgrade_in_progress = TRUE;
  أرجع result;
}

ساكن فراغ ws_conn_dtor(ثابت فراغ *مفتاح, size_t klen, فراغ *entry)
{
  بنية websocket *ws = entry;
  (فراغ)مفتاح;
  (فراغ)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(بنية Curl_easy *data,
                        ثابت حرف *mem, size_t nread)
{
  بنية SingleRequest *k = &data->req;
  بنية websocket *ws;
  بنية Curl_cwriter *ws_dec_writer = عدم;
  بنية Curl_creader *ws_enc_reader = عدم;
  CURLcode result;

  DEBUGASSERT(data->conn);
  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    size_t chunk_size = WS_CHUNK_SIZE;
    ws = curlx_calloc(1, حجم(*ws));
    إذا(!ws)
      أرجع CURLE_OUT_OF_MEMORY;
#ifdef DEBUGBUILD
    {
      ثابت حرف *p = getenv("CURL_WS_CHUNK_SIZE");
      إذا(p) {
        curl_off_t l;
        إذا(!curlx_str_رقم(&p, &l, 1 * 1024 * 1024))
          chunk_size = (size_t)l;
      }
    }
#endif
    CURL_TRC_WS(data, "WS, using chunk size %zu", chunk_size);
    Curl_bufq_init2(&ws->recvbuf, chunk_size, WS_CHUNK_عدد,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_size, WS_CHUNK_عدد,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    result = Curl_conn_meta_set(data->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    إذا(result)
      أرجع result;
  }
  وإلا {
    Curl_bufq_reset(&ws->recvbuf);
    ws_dec_reset(&ws->dec);
    ws_enc_reset(&ws->enc);
  }
  /* Verify the Sec-WebSocket-Accept response.

     The sent value is the base64 encoded version of a SHA-1 hash done on the
     |Sec-WebSocket-Key| header field concatenated with
     the string "258EAFA5-E914-47DA-95CA-C5AB0DC85B11". */

  /* If the response includes a |Sec-WebSocket-Extensions| header field and
     this header field indicates the use of an extension that was not present
     in the client's handshake (the server has indicated an extension not
     requested by the client), the client MUST Fail the WebSocket Connection.
   */

  /* If the response includes a |Sec-WebSocket-Protocol| header field
     and this header field indicates the use of a subprotocol that was
     not present in the client's handshake (the server has indicated a
     subprotocol not requested by the client), the client MUST Fail
     the WebSocket Connection. */

  infof(data, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  result = Curl_cwriter_create(&ws_dec_writer, data, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  إذا(result)
    اذهب out;
  result = Curl_cwriter_add(data, ws_dec_writer);
  إذا(result)
    اذهب out;
  ws_dec_writer = عدم; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  إذا(data->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    إذا(nread) {
      result = Curl_cf_recvbuf_add(data, data->conn, FIRSTSOCKET,
                                   (ثابت uint8_t *)mem, nread);
      إذا(result)
        اذهب out;
    }
    CURL_REQ_CLEAR_RECV(data); /* read no more content */
  }
  وإلا { /* !connect_only */
    إذا(data->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(data, "UPLOAD set, add ws-encode reader");
      result = Curl_creader_set_fread(data, -1);
      إذا(result)
        اذهب out;

      إذا(!data->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        result = Curl_creader_create(&ws_enc_reader, data, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        إذا(result)
          اذهب out;
        result = Curl_creader_add(data, ws_enc_reader);
        إذا(result)
          اذهب out;
        ws_enc_reader = عدم; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      data->req.eos_read = FALSE;
      data->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(data);
    }

    /* Then pass any additional data to the writers */
    إذا(nread) {
      result = Curl_client_write(data, CLIENTWRITE_BODY, mem, nread);
      إذا(result)
        اذهب out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  إذا(ws_dec_writer)
    Curl_cwriter_free(data, ws_dec_writer);
  إذا(ws_enc_reader)
    Curl_creader_free(data, ws_enc_reader);
  إذا(result)
    CURL_TRC_WS(data, "Curl_ws_accept() failed -> %d", (صحيح)result);
  وإلا
    CURL_TRC_WS(data, "websocket established, %s mode",
                data->set.connect_only ? "connect-only" : "callback");
  أرجع result;
}

بنية ws_collect {
  بنية Curl_easy *data;
  بنية websocket *ws;
  uint8_t *buffer;
  size_t buflen;
  size_t bufidx;
  صحيح frame_age;
  صحيح frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

ساكن CURLcode ws_client_collect(ثابت uint8_t *buf, size_t buflen,
                                  صحيح frame_age, صحيح frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  فراغ *userp,
                                  size_t *pnwritten)
{
  بنية ws_collect *ctx = userp;
  بنية Curl_easy *data = ctx->data;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  إذا(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }

  إذا(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  إذا(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(ctx->data, ctx->ws, buf, buflen, CURLWS_PONG);
    إذا(result)
      أرجع result;
    *pnwritten = buflen;
  }
  وإلا {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    إذا(!write_len) {
      إذا(!buflen)  /* 0 length write, we accept that */
        أرجع CURLE_OK;
      أرجع CURLE_AGAIN;  /* no more space */
    }
    memcpy(ctx->buffer + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  أرجع result;
}

ساكن CURLcode nw_in_recv(فراغ *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  بنية Curl_easy *data = reader_ctx;
  أرجع Curl_easy_recv(data, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, فراغ *buffer,
                      size_t buflen, size_t *recv,
                      ثابت بنية curl_ws_frame **metap)
{
  بنية Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_recv, &result)) {
    بنية Curl_easy *data = curl;
    بنية connectdata *conn;
    بنية websocket *ws;
    بنية ws_collect ctx;

    إذا(!recv || !metap || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      اذهب out;
    }

    *recv = 0;
    *metap = عدم;

    conn = data->conn;
    إذا(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      إذا(!data->set.connect_only) {
        failf(data, "[WS] CONNECT_ONLY is required");
        result = CURLE_UNSUPPORTED_PROTOCOL;
        اذهب out;
      }

      Curl_getconnectinfo(data, &conn);
      إذا(!conn) {
        failf(data, "[WS] connection not found");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(data, "[WS] connection is not setup for websocket");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      اذهب out;
    }

    memset(&ctx, 0, حجم(ctx));
    ctx.data = data;
    ctx.ws = ws;
    ctx.buffer = buffer;
    ctx.buflen = buflen;

    طالما(1) {
      /* receive more when our buffer is empty */
      إذا(Curl_bufq_is_empty(&ws->recvbuf)) {
        size_t n;
        result = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, data, &n);
        إذا(result)
          اذهب out;
        وإلا إذا(n == 0) {
          /* connection closed */
          infof(data, "[WS] connection expectedly closed?");
          result = CURLE_GOT_NOTHING;
          اذهب out;
        }
        CURL_TRC_WS(data, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      result = ws_dec_pass(&ws->dec, data, &ws->recvbuf,
                           ws_client_collect, &ctx);
      إذا(result == CURLE_AGAIN) {
        إذا(!ctx.written) {
          ws_dec_info(&ws->dec, data, "need more input");
          استمر;  /* nothing written, try more input */
        }
        اكسر;
      }
      وإلا إذا(result) {
        اذهب out;
      }
      وإلا إذا(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        اكسر;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.frame_age, ctx.frame_flags, ctx.payload_offset,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(data, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.offset,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    إذا(!data->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(data, ws);
      إذا(!r2)
        (فراغ)ws_flush(data, ws, Curl_api_is_in_callback(data));
    }
    result = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
}

ساكن CURLcode ws_flush(بنية Curl_easy *data, بنية websocket *ws,
                         bool blocking)
{
  CURLcode result;

  /* If there is space, add any pending control frame */
  إذا(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_size) {
    result = ws_enc_add_pending(data, ws);
    إذا(result && (result != CURLE_AGAIN))
      أرجع result;
  }

  إذا(!Curl_bufq_is_empty(&ws->sendbuf)) {
    ثابت uint8_t *out;
    size_t outlen, n;
#ifdef DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_next = FALSE;
    size_t chunk_egain = 0;
    ثابت حرف *p = getenv("CURL_WS_CHUNK_EAGAIN");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_رقم(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#endif

    طالما(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#ifdef DEBUGBUILD
      إذا(eagain_next)
        أرجع CURLE_AGAIN;
      إذا(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_next = TRUE;
      }
#endif
      إذا(blocking) {
        result = ws_send_raw_blocking(data, ws, (ثابت حرف *)out, outlen);
        n = result ? 0 : outlen;
      }
      وإلا إذا(data->set.connect_only || Curl_api_is_in_callback(data))
        result = Curl_senddata(data, out, outlen, &n);
      وإلا {
        result = Curl_xfer_send(data, out, outlen, FALSE, &n);
        إذا(!result && !n && outlen)
          result = CURLE_AGAIN;
      }

      إذا(result == CURLE_AGAIN) {
        CURL_TRC_WS(data, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        أرجع result;
      }
      وإلا إذا(result) {
        failf(data, "[WS] flush, write error %d", (صحيح)result);
        أرجع result;
      }
      وإلا {
        CURL_TRC_WS(data, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ws_send_raw_blocking(بنية Curl_easy *data,
                                     بنية websocket *ws,
                                     ثابت حرف *buffer, size_t buflen)
{
  CURLcode result = CURLE_OK;
  size_t nwritten;

  إذا(!data)
    أرجع result;

  (فراغ)ws;
  طالما(buflen) {
    result = Curl_xfer_send(data, buffer, buflen, FALSE, &nwritten);
    إذا(result)
      أرجع result;
    DEBUGASSERT(nwritten <= buflen);
    buffer += nwritten;
    buflen -= nwritten;
    إذا(buflen) {
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      timediff_t يسار_ms;
      صحيح ev;

      CURL_TRC_WS(data, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      يسار_ms = Curl_timeleft_ms(data);
      إذا(يسار_ms < 0) {
        failf(data, "[WS] Timeout waiting for socket becoming writable");
        أرجع CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      إذا(sock == CURL_SOCKET_BAD)
        أرجع CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, يسار_ms ? يسار_ms : 500);
      إذا(ev < 0) {
        failf(data, "[WS] Error while waiting for socket becoming writable");
        أرجع CURLE_SEND_ERROR;
      }
    }
  }
  أرجع result;
}

ساكن CURLcode ws_send_raw(بنية Curl_easy *data, ثابت فراغ *buffer,
                            size_t buflen, size_t *pnwritten)
{
  بنية websocket *ws;
  CURLcode result;

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(data, "[WS] Not a websocket transfer");
    أرجع CURLE_SEND_ERROR;
  }
  إذا(!buflen)
    أرجع CURLE_OK;

  إذا(Curl_api_is_in_callback(data)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    result = ws_flush(data, ws, TRUE);
    إذا(result)
      أرجع result;
    result = ws_send_raw_blocking(data, ws, buffer, buflen);
    إذا(!result)
      *pnwritten = buflen;
  }
  وإلا {
    /* We need any pending data to be sent or EAGAIN this call. */
    result = ws_flush(data, ws, FALSE);
    إذا(result)
      أرجع result;
    result = Curl_senddata(data, buffer, buflen, pnwritten);
  }

  CURL_TRC_WS(data, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (صحيح)result, *pnwritten);
  أرجع result;
}

CURLcode curl_ws_send(CURL *curl, ثابت فراغ *buffer_arg,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned صحيح flags)
{
  بنية Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_send, &result)) {
    بنية websocket *ws;
    ثابت uint8_t *buffer = buffer_arg;
    بنية Curl_easy *data = curl;
    size_t ndummy;
    size_t *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, data->set.ws_raw_mode);

    *pnsent = 0;

    إذا(!buffer && buflen) {
      failf(data, "[WS] buffer is NULL when buflen is not");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      اذهب out;
    }

    إذا(!data->conn && data->set.connect_only) {
      result = Curl_connect_only_attach(data);
      إذا(result)
        اذهب out;
    }
    إذا(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      اذهب out;
    }

    إذا(data->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      result = ws_flush(data, ws, FALSE);
      إذا(result)
        اذهب out;

      إذا(!buffer) {
        failf(data, "[WS] buffer is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب out;
      }
      إذا(!sent) {
        failf(data, "[WS] sent is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب out;
      }
      إذا(fragsize || flags) {
        failf(data, "[WS] fragsize and flags must be zero in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب out;
      }
      result = ws_send_raw(data, buffer, buflen, pnsent);
      اذهب out;
    }

    /* Not RAW mode, we do the frame encoding */
    result = ws_enc_send(data, ws, buffer, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, data->set.ws_raw_mode, (صحيح)result,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
}

ساكن CURLcode ws_setup_conn(بنية Curl_easy *data,
                              بنية connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  data->state.http_neg.accept_09 = FALSE;
  data->state.http_neg.only_10 = FALSE;
  data->state.http_neg.wanted = CURL_HTTP_V1x;
  data->state.http_neg.allowed = CURL_HTTP_V1x;
  أرجع Curl_http_setup_conn(data, conn);
}

ثابت بنية curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  بنية Curl_easy *data = curl;
  إذا(GOOD_EASY_HANDLE(data) && Curl_api_is_in_callback(data) &&
     data->conn && !data->set.ws_raw_mode) {
    بنية websocket *ws;
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    إذا(ws)
      أرجع &ws->recvframe;
  }
  أرجع عدم;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned صحيح flags,
                                         curl_off_t frame_len)
{
  بنية Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &result)) {
    بنية Curl_easy *data = curl;
    بنية websocket *ws;

    إذا(data->set.ws_raw_mode) {
      failf(data, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      result = CURLE_FAILED_INIT;
      اذهب out;
    }

    CURL_TRC_WS(data, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    إذا(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      اذهب out;
    }

    إذا(ws->enc.payload_remain) {
      failf(data, "[WS] previous frame not finished");
      result = CURLE_SEND_ERROR;
      اذهب out;
    }

    result = ws_enc_write_head(data, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    إذا(result)
      CURL_TRC_WS(data, "curl_start_frame(), error adding frame head %d",
                  (صحيح)result);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
}

ثابت بنية Curl_protocol Curl_protocol_ws = {
  ws_setup_conn,                        /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_done,                       /* done */
  ZERO_NULL,                            /* do_more */
  ZERO_NULL,                            /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  Curl_http_doing_pollset,              /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_http_perform_pollset,            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  Curl_http_write_resp,                 /* write_resp */
  Curl_http_write_resp_hd,              /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  Curl_http_follow,                     /* follow */
};

#وإلا

CURLcode curl_ws_recv(CURL *curl, فراغ *buffer, size_t buflen,
                      size_t *recv,
                      ثابت بنية curl_ws_frame **metap)
{
  (فراغ)curl;
  (فراغ)buffer;
  (فراغ)buflen;
  (فراغ)recv;
  (فراغ)metap;
  أرجع CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, ثابت فراغ *buffer,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned صحيح flags)
{
  (فراغ)curl;
  (فراغ)buffer;
  (فراغ)buflen;
  (فراغ)sent;
  (فراغ)fragsize;
  (فراغ)flags;
  أرجع CURLE_NOT_BUILT_IN;
}

ثابت بنية curl_ws_frame *curl_ws_meta(CURL *data)
{
  (فراغ)data;
  أرجع عدم;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned صحيح flags,
                                         curl_off_t frame_len)
{
  (فراغ)curl;
  (فراغ)flags;
  (فراغ)frame_len;
  أرجع CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
