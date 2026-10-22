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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "ws.h"

#ifndef CURL_DISABLE_WEBSOCKETS

#شامل "url.h"
#شامل "bufq.h"
#شامل "curlx/dynbuf.h"
#شامل "rand.h"
#شامل "curlx/base64.h"
#شامل "cf-recvbuf.h"
#شامل "connect.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "multiif.h"
#شامل "easyif.h"
#شامل "transfer.h"
#شامل "select.h"
#شامل "curlx/strparse.h"
#شامل "curlx/strcopy.h"

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
#define WS_CHUNK_شمار 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
شمارش ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_HEAD,
  WS_DEC_PAYLOAD
};

ساختار ws_decoder {
  صحیح frame_age;        /* zero */
  صحیح frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t head[10];
  صحیح head_len, head_total;
  شمارش ws_dec_state state;
  صحیح cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
ساختار ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  unsigned صحیح xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#define WS_MAX_CNTRL_LEN    125

ساختار ws_cntrl_frame {
  unsigned صحیح type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
ساختار websocket {
  ساختار Curl_easy *data; /* used for write callback handling */
  ساختار ws_decoder dec;  /* decode of ws frames */
  ساختار ws_encoder enc;  /* encode of ws frames */
  ساختار bufq recvbuf;    /* raw data from the server */
  ساختار bufq sendbuf;    /* raw data to be sent to the server */
  ساختار curl_ws_frame recvframe;  /* the current WS FRAME received */
  ساختار ws_cntrl_frame pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#ifdef CURLVERBOSE
ایستا ثابت نویسه *ws_frame_name_of_op(uint8_t firstbyte)
{
  انتخاب(firstbyte & WSBIT_OPCODE_MASK) {
  حالت WSBIT_OPCODE_CONT:
    بازگشت "CONT";
  حالت WSBIT_OPCODE_TEXT:
    بازگشت "TEXT";
  حالت WSBIT_OPCODE_BIN:
    بازگشت "BIN";
  حالت WSBIT_OPCODE_CLOSE:
    بازگشت "CLOSE";
  حالت WSBIT_OPCODE_PING:
    بازگشت "PING";
  حالت WSBIT_OPCODE_PONG:
    بازگشت "PONG";
  پیشفرض:
    بازگشت "???";
  }
}
#endif

ایستا صحیح ws_frame_firstbyte2flags(ساختار Curl_easy *data,
                                    uint8_t firstbyte, صحیح cont_flags)
{
  انتخاب(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  حالت WSBIT_OPCODE_CONT:
    اگر(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      بازگشت 0;
    }
    بازگشت cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  حالت (WSBIT_OPCODE_CONT | WSBIT_FIN):
    اگر(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      بازگشت 0;
    }
    بازگشت cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  حالت WSBIT_OPCODE_TEXT:
    اگر(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      بازگشت 0;
    }
    بازگشت CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  حالت (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    اگر(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      بازگشت 0;
    }
    بازگشت CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  حالت WSBIT_OPCODE_BIN:
    اگر(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      بازگشت 0;
    }
    بازگشت CURLWS_دودویی | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  حالت (WSBIT_OPCODE_BIN | WSBIT_FIN):
    اگر(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      بازگشت 0;
    }
    بازگشت CURLWS_دودویی;
  /* 0x08 - first CLOSE fragment */
  حالت WSBIT_OPCODE_CLOSE:
    failf(data, "[WS] invalid fragmented CLOSE frame");
    بازگشت 0;
  /* 0x88 - unfragmented CLOSE */
  حالت (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    بازگشت CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  حالت WSBIT_OPCODE_PING:
    failf(data, "[WS] invalid fragmented PING frame");
    بازگشت 0;
  /* 0x89 - unfragmented PING */
  حالت (WSBIT_OPCODE_PING | WSBIT_FIN):
    بازگشت CURLWS_PING;
  /* 0x0a - first PONG fragment */
  حالت WSBIT_OPCODE_PONG:
    failf(data, "[WS] invalid fragmented PONG frame");
    بازگشت 0;
  /* 0x8a - unfragmented PONG */
  حالت (WSBIT_OPCODE_PONG | WSBIT_FIN):
    بازگشت CURLWS_PONG;
  /* invalid first byte */
  پیشفرض:
    اگر(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(data, "[WS] invalid reserved bits: %02x", firstbyte);
    وگرنه
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(data, "[WS] invalid opcode: %02x", firstbyte);
    بازگشت 0;
  }
}

ایستا CURLcode ws_frame_flags2firstbyte(ساختار Curl_easy *data,
                                         unsigned صحیح flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  انتخاب(flags & ~CURLWS_OFFSET) {
  حالت 0:
    اگر(contfragment) {
      CURL_TRC_WS(data, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      بازگشت CURLE_OK;
    }
    failf(data, "[WS] no flags given");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  حالت CURLWS_CONT:
    اگر(contfragment) {
      infof(data, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      بازگشت CURLE_OK;
    }
    failf(data, "[WS] No ongoing fragmented message to continue");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  حالت CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    بازگشت CURLE_OK;
  حالت (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    بازگشت CURLE_OK;
  حالت CURLWS_دودویی:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    بازگشت CURLE_OK;
  حالت (CURLWS_دودویی | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    بازگشت CURLE_OK;
  حالت CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    بازگشت CURLE_OK;
  حالت (CURLWS_CLOSE | CURLWS_CONT):
    failf(data, "[WS] CLOSE frame must not be fragmented");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  حالت CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    بازگشت CURLE_OK;
  حالت (CURLWS_PING | CURLWS_CONT):
    failf(data, "[WS] PING frame must not be fragmented");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  حالت CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    بازگشت CURLE_OK;
  حالت (CURLWS_PONG | CURLWS_CONT):
    failf(data, "[WS] PONG frame must not be fragmented");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  پیشفرض:
    failf(data, "[WS] unknown flags: %x", flags);
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

ایستا تهی ws_dec_info(ساختار ws_decoder *dec, ساختار Curl_easy *data,
                        ثابت نویسه *پیام)
{
  NOVERBOSE((تهی)پیام);
  انتخاب(dec->head_len) {
  حالت 0:
    بشکن;
  حالت 1:
    CURL_TRC_WS(data, "decoded %s [%s%s]", پیام,
                ws_frame_name_of_op(dec->head[0]),
                (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    بشکن;
  پیشفرض:
    اگر(dec->head_len < dec->head_total) {
      CURL_TRC_WS(data, "decoded %s [%s%s](%d/%d)", پیام,
                  ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->head_len, dec->head_total);
    }
    وگرنه {
      CURL_TRC_WS(data, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  پیام, ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    بشکن;
  }
}

ایستا CURLcode ws_send_raw_blocking(ساختار Curl_easy *data,
                                     ساختار websocket *ws,
                                     ثابت نویسه *buffer, size_t buflen);

تعریفنوع CURLcode ws_write_payload(ثابت uint8_t *buf, size_t buflen,
                                  صحیح frame_age, صحیح frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  تهی *userp,
                                  size_t *pnwritten);

ایستا تهی ws_dec_next_frame(ساختار ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ایستا تهی ws_dec_reset(ساختار ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

ایستا تهی ws_dec_init(ساختار ws_decoder *dec)
{
  ws_dec_reset(dec);
}

ایستا CURLcode ws_dec_read_head(ساختار ws_decoder *dec,
                                 ساختار Curl_easy *data,
                                 ساختار bufq *inraw)
{
  ثابت uint8_t *inbuf;
  size_t inlen;

  وقتی(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    اگر(dec->head_len == 0) {
      dec->head[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(data, dec->head[0],
                                                  dec->cont_flags);
      اگر(!dec->frame_flags) {
        ws_dec_reset(dec);
        بازگشت CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      اگر(dec->frame_flags & (CURLWS_TEXT | CURLWS_دودویی)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->head_len = 1;
#اگر 0
      ws_dec_info(dec, data, "seeing opcode");
#endif
      ادامه;
    }
    وگرنه اگر(dec->head_len == 1) {
      dec->head[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->head_len = 2;

      اگر(dec->head[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(data, "[WS] masked input frame");
        ws_dec_reset(dec);
        بازگشت CURLE_RECV_ERROR;
      }
      اگر(dec->frame_flags & CURLWS_PING && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(data, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        بازگشت CURLE_RECV_ERROR;
      }
      اگر(dec->frame_flags & CURLWS_PONG && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(data, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        بازگشت CURLE_RECV_ERROR;
      }
      اگر(dec->frame_flags & CURLWS_CLOSE && dec->head[1] > WS_MAX_CNTRL_LEN) {
        failf(data, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        بازگشت CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      اگر(dec->head[1] == 126) {
        dec->head_total = 4;
        ادامه;
      }
      وگرنه اگر(dec->head[1] == 127) {
        dec->head_total = 10;
        ادامه;
      }
      وگرنه {
        dec->head_total = 2;
      }
    }

    اگر(dec->head_len < dec->head_total) {
      dec->head[dec->head_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->head_len;
      اگر(dec->head_len < dec->head_total) {
#اگر 0
        ws_dec_info(dec, data, "decoding head");
#endif
        ادامه;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->head_len == dec->head_total);
    انتخاب(dec->head_total) {
    حالت 2:
      dec->payload_len = dec->head[1];
      بشکن;
    حالت 4:
      dec->payload_len = (dec->head[2] << 8) | dec->head[3];
      بشکن;
    حالت 10:
      اگر(dec->head[2] > 127) {
        failf(data, "[WS] frame length longer than 63 bits not supported");
        بازگشت CURLE_RECV_ERROR;
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
      بشکن;
    پیشفرض:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(data, "[WS] unexpected frame header length");
      بازگشت CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, data, "head");
    بازگشت CURLE_OK;
  }
  بازگشت CURLE_AGAIN;
}

ایستا CURLcode ws_dec_pass_payload(ساختار ws_decoder *dec,
                                    ساختار Curl_easy *data,
                                    ساختار bufq *inraw,
                                    ws_write_payload *write_cb,
                                    تهی *write_ctx)
{
  ثابت uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode result;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  وقتی(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(data)) {
    اگر(inlen > remain)
      inlen = remain;
    result = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    اگر(result)
      بازگشت result;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(data, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  بازگشت remain ? CURLE_AGAIN : CURLE_OK;
}

ایستا CURLcode ws_dec_pass(ساختار ws_decoder *dec,
                            ساختار Curl_easy *data,
                            ساختار bufq *inraw,
                            ws_write_payload *write_cb,
                            تهی *write_ctx)
{
  CURLcode result;

  اگر(Curl_bufq_is_empty(inraw))
    بازگشت CURLE_AGAIN;

  انتخاب(dec->state) {
  حالت WS_DEC_INIT:
    ws_dec_next_frame(dec);
    dec->state = WS_DEC_HEAD;
    FALLTHROUGH();
  حالت WS_DEC_HEAD:
    result = ws_dec_read_head(dec, data, inraw);
    اگر(result) {
      اگر(result != CURLE_AGAIN) {
        failf(data, "[WS] decode frame error %d", (صحیح)result);
        بشکن;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      بشکن;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    اگر(dec->payload_len == 0) {
      size_t nwritten;
      ثابت uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      result = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      اگر(result)
        بازگشت result;
      dec->state = WS_DEC_INIT;
      بشکن;
    }
    FALLTHROUGH();
  حالت WS_DEC_PAYLOAD:
    result = ws_dec_pass_payload(dec, data, inraw, write_cb, write_ctx);
    ws_dec_info(dec, data, "passing");
    اگر(result)
      بازگشت result;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    بشکن;
  پیشفرض:
    /* we covered all enums above, but some code analyzers are wimps */
    result = CURLE_FAILED_INIT;
  }
  بازگشت result;
}

ایستا تهی update_meta(ساختار websocket *ws,
                        صحیح frame_age, صحیح frame_flags,
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
ساختار ws_cw_ctx {
  ساختار Curl_cwriter super;
  ساختار bufq buf;
};

ایستا CURLcode ws_cw_init(ساختار Curl_easy *data,
                           ساختار Curl_cwriter *writer)
{
  ساختار ws_cw_ctx *ctx = writer->ctx;
  (تهی)data;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_SIZE, 1, BUFQ_OPT_SOFT_LIMIT);
  بازگشت CURLE_OK;
}

ایستا تهی ws_cw_close(ساختار Curl_easy *data, ساختار Curl_cwriter *writer)
{
  ساختار ws_cw_ctx *ctx = writer->ctx;
  (تهی)data;
  Curl_bufq_free(&ctx->buf);
}

ساختار ws_cw_dec_ctx {
  ساختار Curl_easy *data;
  ساختار websocket *ws;
  ساختار Curl_cwriter *next_writer;
  صحیح cw_type;
};

ایستا CURLcode ws_flush(ساختار Curl_easy *data, ساختار websocket *ws,
                         bool blocking);
ایستا CURLcode ws_enc_add_pending(ساختار Curl_easy *data,
                                   ساختار websocket *ws);

ایستا CURLcode ws_enc_add_cntrl(ساختار Curl_easy *data,
                                 ساختار websocket *ws,
                                 ثابت uint8_t *payload,
                                 size_t plen,
                                 unsigned صحیح frame_type)
{
  (تهی)data;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  اگر(plen > WS_MAX_CNTRL_LEN)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  memcpy(ws->pending.payload, payload, plen);
  بازگشت CURLE_OK;
}

ایستا curl_off_t ws_payload_remain(curl_off_t payload_total,
                                    curl_off_t payload_offset,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_total - payload_offset;
  اگر((payload_total < 0) || (payload_offset < 0) || (remain < 0))
    بازگشت -1;
  buffered = curlx_uztoso(payload_buffered);
  اگر(remain < buffered)
    بازگشت -1;
  بازگشت remain - buffered;
}

ایستا CURLcode ws_cw_dec_next(ثابت uint8_t *buf, size_t buflen,
                               صحیح frame_age, صحیح frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               تهی *user_data,
                               size_t *pnwritten)
{
  ساختار ws_cw_dec_ctx *ctx = user_data;
  ساختار Curl_easy *data = ctx->data;
  ساختار websocket *ws = ctx->ws;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result;

  (تهی)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  اگر(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  اگر(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(data, ws, buf, buflen, CURLWS_PONG);
    اگر(result)
      بازگشت result;
  }
  وگرنه اگر(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(data, "[WS] pass %zu decoded bytes", buflen);
    result = Curl_cwriter_write(data, ctx->next_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ثابت نویسه *)buf, buflen);
    اگر(result)
      بازگشت result;
  }
  *pnwritten = buflen;
  بازگشت CURLE_OK;
}

ایستا CURLcode ws_cw_write(ساختار Curl_easy *data,
                            ساختار Curl_cwriter *writer, صحیح type,
                            ثابت نویسه *buf, size_t nbytes)
{
  ساختار ws_cw_ctx *ctx = writer->ctx;
  ساختار websocket *ws;
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[WS] write(len=%zu, type=%d)", nbytes, type);
  اگر(!(type & CLIENTWRITE_BODY) || data->set.ws_raw_mode)
    بازگشت Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  اگر(!ws) {
    failf(data, "[WS] not a websocket transfer");
    بازگشت CURLE_FAILED_INIT;
  }

  اگر(nbytes) {
    size_t nwritten;
    result = Curl_bufq_write(&ctx->buf, (ثابت uint8_t *)buf,
                             nbytes, &nwritten);
    اگر(result) {
      infof(data, "[WS] error adding data to buffer %d", (صحیح)result);
      بازگشت result;
    }
  }

  result = Curl_cwriter_flush(data, writer->next);
  اگر(result)
    برو out;

  وقتی(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
    ساختار ws_cw_dec_ctx pass_ctx;
    pass_ctx.data = data;
    pass_ctx.ws = ws;
    pass_ctx.next_writer = writer->next;
    pass_ctx.cw_type = type;
    result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                         ws_cw_dec_next, &pass_ctx);
    اگر(result == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      result = CURLE_OK;
      برو out;
    }
    وگرنه اگر(result) {
      failf(data, "[WS] decode payload error %d", (صحیح)result);
      Curl_bufq_reset(&ctx->buf);
      برو out;
    }
  }

out:
  اگر(!result && !ws->using_reader) {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    اگر(result == CURLE_AGAIN)
      result = CURLE_OK;
  }

  اگر(!result && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(data, "[WS] decode ended with incomplete frame");
    result = CURLE_RECV_ERROR;
  }
  بازگشت result;
}

ایستا CURLcode ws_cw_flush(ساختار Curl_easy *data,
                            ساختار Curl_cwriter *writer)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[ws] flush");
  اگر(!data->set.ws_raw_mode) {
    ساختار ws_cw_ctx *ctx = writer->ctx;
    ساختار websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    result = Curl_cwriter_flush(data, writer->next);
    اگر(result)
      برو out;

    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    اگر(!ws) {
      failf(data, "[WS] not a websocket transfer");
      بازگشت CURLE_FAILED_INIT;
    }

    وقتی(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
      ساختار ws_cw_dec_ctx pass_ctx;
      pass_ctx.data = data;
      pass_ctx.ws = ws;
      pass_ctx.next_writer = writer->next;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                           ws_cw_dec_next, &pass_ctx);
      اگر(result == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        result = CURLE_OK;
        برو out;
      }
      وگرنه اگر(result) {
        failf(data, "[WS] decode payload error %d", (صحیح)result);
        Curl_bufq_reset(&ctx->buf);
        برو out;
      }
    }
  }

out:
  اگر(!result)
    result = Curl_cwriter_flush(data, writer->next);
  بازگشت result;
}

/* WebSocket payload decoding client writer. */
ایستا ثابت ساختار Curl_cwtype ws_cw_decode = {
  "ws-decode",
  پوچ,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  اندازه(ساختار ws_cw_ctx)
};

ایستا تهی ws_enc_info(ساختار ws_encoder *enc, ساختار Curl_easy *data,
                        ثابت نویسه *پیام)
{
  NOVERBOSE((تهی)enc);
  NOVERBOSE((تهی)پیام);
  CURL_TRC_WS(data, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              پیام, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

ایستا تهی ws_enc_reset(ساختار ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

ایستا تهی ws_enc_init(ساختار ws_encoder *enc)
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

ایستا CURLcode ws_enc_add_frame(ساختار Curl_easy *data,
                                 ساختار ws_encoder *enc,
                                 unsigned صحیح flags,
                                 curl_off_t payload_len,
                                 ساختار bufq *out)
{
  uint8_t firstb = 0;
  uint8_t head[14];
  CURLcode result;
  size_t hlen, nwritten;

  اگر(payload_len < 0) {
    failf(data, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    بازگشت CURLE_SEND_ERROR;
  }

  اگر(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(data, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    بازگشت CURLE_SEND_ERROR;
  }

  result = ws_frame_flags2firstbyte(data, flags, (bool)enc->contfragment,
                                    &firstb);
  اگر(result)
    بازگشت result;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  اگر(flags & (CURLWS_TEXT | CURLWS_دودویی)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  اگر(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PING frame is too big");
    بازگشت CURLE_TOO_LARGE;
  }
  اگر(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PONG frame is too big");
    بازگشت CURLE_TOO_LARGE;
  }
  اگر(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given CLOSE frame is too big");
    بازگشت CURLE_TOO_LARGE;
  }

  head[0] = enc->firstbyte = firstb;
  اگر(payload_len > 65535) {
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
  وگرنه اگر(payload_len >= 126) {
    head[1] = 126 | WSBIT_MASK;
    head[2] = (uint8_t)((payload_len >> 8) & 0xff);
    head[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  وگرنه {
    head[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, data, "sending");

  /* 4 bytes random */

  result = Curl_rand(data, (uint8_t *)&enc->mask, اندازه(enc->mask));
  اگر(result)
    بازگشت result;

#ifdef DEBUGBUILD
  اگر(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->mask, 0, اندازه(enc->mask));
#endif

  /* add 4 bytes mask */
  memcpy(&head[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  result = Curl_bufq_write(out, head, hlen, &nwritten);
  اگر(result)
    بازگشت result;
  اگر(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    بازگشت CURLE_SEND_ERROR;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ws_enc_write_head(ساختار Curl_easy *data,
                                  ساختار websocket *ws,
                                  ساختار ws_encoder *enc,
                                  unsigned صحیح flags,
                                  curl_off_t payload_len,
                                  ساختار bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  اگر(ws->pending.type) {
    CURLcode result = ws_enc_add_pending(data, ws);
    اگر(result)
      بازگشت result;
  }
  بازگشت ws_enc_add_frame(data, enc, flags, payload_len, out);
}

ایستا CURLcode ws_enc_write_payload(ساختار ws_encoder *enc,
                                     ساختار Curl_easy *data,
                                     ثابت uint8_t *buf, size_t buflen,
                                     ساختار bufq *out, size_t *pnwritten)
{
  CURLcode result;
  size_t i, len, n, remain;

  *pnwritten = 0;
  اگر(Curl_bufq_is_full(out))
    بازگشت CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  اگر(remain < len)
    len = remain;

  برای(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    result = Curl_bufq_write(out, &c, 1, &n);
    اگر(result) {
      اگر((result != CURLE_AGAIN) || !i)
        بازگشت result;
      بشکن;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, data, "buffered");
  بازگشت CURLE_OK;
}

ایستا CURLcode ws_enc_add_pending(ساختار Curl_easy *data,
                                   ساختار websocket *ws)
{
  CURLcode result;
  size_t n;

  اگر(!ws->pending.type) /* no pending frame here */
    بازگشت CURLE_OK;
  اگر(ws->enc.payload_remain) /* in the middle of another frame */
    بازگشت CURLE_AGAIN;

  result = ws_enc_add_frame(data, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  اگر(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding head: %d",
                (صحیح)result);
    برو out;
  }
  result = ws_enc_write_payload(&ws->enc, data, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  اگر(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding payload: %d",
                (صحیح)result);
    برو out;
  }
  اگر(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(data, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    result = CURLE_SEND_ERROR;
    برو out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, اندازه(ws->pending));

out:
  بازگشت result;
}

ایستا CURLcode ws_enc_send(ساختار Curl_easy *data,
                            ساختار websocket *ws,
                            ثابت uint8_t *buffer,
                            size_t buflen,
                            curl_off_t fragsize,
                            unsigned صحیح flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!data->set.ws_raw_mode);
  *pnsent = 0;

  اگر(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    اگر(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(data, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }
    اگر((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(data, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  وگرنه {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    اگر(result)
      بازگشت result;

    result = ws_enc_write_head(data, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    اگر(result) {
      CURL_TRC_WS(data, "curl_ws_send(), error writing frame head %d",
                  (صحیح)result);
      بازگشت result;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  وقتی(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    اگر(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      result = ws_enc_write_payload(&ws->enc, data,
                                    buffer + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      اگر(result && (result != CURLE_AGAIN))
        بازگشت result;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      اگر(!ws->sendbuf_payload) {
        بازگشت CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    اگر(!result && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      buffer += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    وگرنه اگر(result == CURLE_AGAIN) {
      اگر(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        بازگشت CURLE_OK;
      }
      وگرنه {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(data, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        بازگشت CURLE_AGAIN;
      }
    }
    وگرنه
      بازگشت result;  /* real error sending the data */
  }
  بازگشت CURLE_OK;
}

ساختار cr_ws_ctx {
  ساختار Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

ایستا CURLcode cr_ws_init(ساختار Curl_easy *data, ساختار Curl_creader *reader)
{
  (تهی)data;
  (تهی)reader;
  بازگشت CURLE_OK;
}

ایستا تهی cr_ws_close(ساختار Curl_easy *data, ساختار Curl_creader *reader)
{
  (تهی)data;
  (تهی)reader;
}

ایستا CURLcode cr_ws_read(ساختار Curl_easy *data,
                           ساختار Curl_creader *reader,
                           نویسه *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ساختار cr_ws_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, n;
  ساختار websocket *ws;
  bool eos;

  *pnread = 0;
  اگر(ctx->eos) {
    *peos = TRUE;
    بازگشت CURLE_OK;
  }

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  اگر(!ws) {
    failf(data, "[WS] not a websocket transfer");
    بازگشت CURLE_FAILED_INIT;
  }

  اگر(Curl_bufq_is_empty(&ws->sendbuf)) {
    اگر(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      بازگشت CURLE_OK;
    }

    اگر(ws->enc.payload_remain) {
      CURL_TRC_WS(data, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    اگر(result)
      بازگشت result;
    ctx->read_eos = eos;

    اگر(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(data, reader->next);
    }
    وگرنه اگر(!nread) {
      /* nothing to convert, return this right away */
      اگر(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      برو out;
    }

    اگر(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      result = ws_enc_write_head(data, ws, &ws->enc, CURLWS_دودویی, nread,
                                 &ws->sendbuf);
      اگر(result)
        برو out;
    }

    result = ws_enc_write_payload(&ws->enc, data, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    اگر(result)
      برو out;
    CURL_TRC_READ(data, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  اگر(!result && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحیح)result, *pnread, *peos);
  بازگشت result;
}

ایستا curl_off_t cr_ws_total_length(ساختار Curl_easy *data,
                                     ساختار Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (تهی)data;
  (تهی)reader;
  بازگشت -1;
}

ایستا ثابت ساختار Curl_crtype ws_cr_encode = {
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
  اندازه(ساختار cr_ws_ctx)
};

ساختار wsfield {
  ثابت نویسه *name;
  ثابت نویسه *val;
};

CURLcode Curl_ws_request(ساختار Curl_easy *data, ساختار dynbuf *req)
{
  unsigned صحیح i;
  CURLcode result = CURLE_OK;
  uint8_t rand[16];
  نویسه *randstr;
  size_t randlen;
  نویسه keyval[40];
  ساختار SingleRequest *k = &data->req;
  ساختار wsfield heads[] = {
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
      "Sec-WebSocket-Key", پوچ,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  result = Curl_rand(data, rand, اندازه(rand));
  اگر(result)
    بازگشت result;
  result = curlx_base64_encode(rand, اندازه(rand), &randstr, &randlen);
  اگر(result)
    بازگشت result;
  DEBUGASSERT(randlen < اندازه(keyval));
  اگر(randlen >= اندازه(keyval)) {
    curlx_free(randstr);
    بازگشت CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, اندازه(keyval), randstr, randlen);
  curlx_free(randstr);
  برای(i = 0; !result && (i < CURL_ARRAYSIZE(heads)); i++) {
    اگر(!Curl_checkheaders(data, heads[i].name, strlen(heads[i].name))) {
      result = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  data->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  data->conn->bits.upgrade_in_progress = TRUE;
  بازگشت result;
}

ایستا تهی ws_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار websocket *ws = entry;
  (تهی)کلید;
  (تهی)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(ساختار Curl_easy *data,
                        ثابت نویسه *mem, size_t nread)
{
  ساختار SingleRequest *k = &data->req;
  ساختار websocket *ws;
  ساختار Curl_cwriter *ws_dec_writer = پوچ;
  ساختار Curl_creader *ws_enc_reader = پوچ;
  CURLcode result;

  DEBUGASSERT(data->conn);
  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  اگر(!ws) {
    size_t chunk_size = WS_CHUNK_SIZE;
    ws = curlx_calloc(1, اندازه(*ws));
    اگر(!ws)
      بازگشت CURLE_OUT_OF_MEMORY;
#ifdef DEBUGBUILD
    {
      ثابت نویسه *p = getenv("CURL_WS_CHUNK_SIZE");
      اگر(p) {
        curl_off_t l;
        اگر(!curlx_str_عدد(&p, &l, 1 * 1024 * 1024))
          chunk_size = (size_t)l;
      }
    }
#endif
    CURL_TRC_WS(data, "WS, using chunk size %zu", chunk_size);
    Curl_bufq_init2(&ws->recvbuf, chunk_size, WS_CHUNK_شمار,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_size, WS_CHUNK_شمار,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    result = Curl_conn_meta_set(data->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    اگر(result)
      بازگشت result;
  }
  وگرنه {
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
  اگر(result)
    برو out;
  result = Curl_cwriter_add(data, ws_dec_writer);
  اگر(result)
    برو out;
  ws_dec_writer = پوچ; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  اگر(data->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    اگر(nread) {
      result = Curl_cf_recvbuf_add(data, data->conn, FIRSTSOCKET,
                                   (ثابت uint8_t *)mem, nread);
      اگر(result)
        برو out;
    }
    CURL_REQ_CLEAR_RECV(data); /* read no more content */
  }
  وگرنه { /* !connect_only */
    اگر(data->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(data, "UPLOAD set, add ws-encode reader");
      result = Curl_creader_set_fread(data, -1);
      اگر(result)
        برو out;

      اگر(!data->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        result = Curl_creader_create(&ws_enc_reader, data, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        اگر(result)
          برو out;
        result = Curl_creader_add(data, ws_enc_reader);
        اگر(result)
          برو out;
        ws_enc_reader = پوچ; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      data->req.eos_read = FALSE;
      data->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(data);
    }

    /* Then pass any additional data to the writers */
    اگر(nread) {
      result = Curl_client_write(data, CLIENTWRITE_BODY, mem, nread);
      اگر(result)
        برو out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  اگر(ws_dec_writer)
    Curl_cwriter_free(data, ws_dec_writer);
  اگر(ws_enc_reader)
    Curl_creader_free(data, ws_enc_reader);
  اگر(result)
    CURL_TRC_WS(data, "Curl_ws_accept() failed -> %d", (صحیح)result);
  وگرنه
    CURL_TRC_WS(data, "websocket established, %s mode",
                data->set.connect_only ? "connect-only" : "callback");
  بازگشت result;
}

ساختار ws_collect {
  ساختار Curl_easy *data;
  ساختار websocket *ws;
  uint8_t *buffer;
  size_t buflen;
  size_t bufidx;
  صحیح frame_age;
  صحیح frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

ایستا CURLcode ws_client_collect(ثابت uint8_t *buf, size_t buflen,
                                  صحیح frame_age, صحیح frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  تهی *userp,
                                  size_t *pnwritten)
{
  ساختار ws_collect *ctx = userp;
  ساختار Curl_easy *data = ctx->data;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  اگر(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  اگر(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  اگر(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(ctx->data, ctx->ws, buf, buflen, CURLWS_PONG);
    اگر(result)
      بازگشت result;
    *pnwritten = buflen;
  }
  وگرنه {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    اگر(!write_len) {
      اگر(!buflen)  /* 0 length write, we accept that */
        بازگشت CURLE_OK;
      بازگشت CURLE_AGAIN;  /* no more space */
    }
    memcpy(ctx->buffer + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  بازگشت result;
}

ایستا CURLcode nw_in_recv(تهی *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  ساختار Curl_easy *data = reader_ctx;
  بازگشت Curl_easy_recv(data, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, تهی *buffer,
                      size_t buflen, size_t *recv,
                      ثابت ساختار curl_ws_frame **metap)
{
  ساختار Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  اگر(CURL_EAPI_ENTER(&guard, curl, ws_recv, &result)) {
    ساختار Curl_easy *data = curl;
    ساختار connectdata *conn;
    ساختار websocket *ws;
    ساختار ws_collect ctx;

    اگر(!recv || !metap || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      برو out;
    }

    *recv = 0;
    *metap = پوچ;

    conn = data->conn;
    اگر(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      اگر(!data->set.connect_only) {
        failf(data, "[WS] CONNECT_ONLY is required");
        result = CURLE_UNSUPPORTED_PROTOCOL;
        برو out;
      }

      Curl_getconnectinfo(data, &conn);
      اگر(!conn) {
        failf(data, "[WS] connection not found");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        برو out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    اگر(!ws) {
      failf(data, "[WS] connection is not setup for websocket");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      برو out;
    }

    memset(&ctx, 0, اندازه(ctx));
    ctx.data = data;
    ctx.ws = ws;
    ctx.buffer = buffer;
    ctx.buflen = buflen;

    وقتی(1) {
      /* receive more when our buffer is empty */
      اگر(Curl_bufq_is_empty(&ws->recvbuf)) {
        size_t n;
        result = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, data, &n);
        اگر(result)
          برو out;
        وگرنه اگر(n == 0) {
          /* connection closed */
          infof(data, "[WS] connection expectedly closed?");
          result = CURLE_GOT_NOTHING;
          برو out;
        }
        CURL_TRC_WS(data, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      result = ws_dec_pass(&ws->dec, data, &ws->recvbuf,
                           ws_client_collect, &ctx);
      اگر(result == CURLE_AGAIN) {
        اگر(!ctx.written) {
          ws_dec_info(&ws->dec, data, "need more input");
          ادامه;  /* nothing written, try more input */
        }
        بشکن;
      }
      وگرنه اگر(result) {
        برو out;
      }
      وگرنه اگر(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        بشکن;
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
    اگر(!data->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(data, ws);
      اگر(!r2)
        (تهی)ws_flush(data, ws, Curl_api_is_in_callback(data));
    }
    result = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}

ایستا CURLcode ws_flush(ساختار Curl_easy *data, ساختار websocket *ws,
                         bool blocking)
{
  CURLcode result;

  /* If there is space, add any pending control frame */
  اگر(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_size) {
    result = ws_enc_add_pending(data, ws);
    اگر(result && (result != CURLE_AGAIN))
      بازگشت result;
  }

  اگر(!Curl_bufq_is_empty(&ws->sendbuf)) {
    ثابت uint8_t *out;
    size_t outlen, n;
#ifdef DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_next = FALSE;
    size_t chunk_egain = 0;
    ثابت نویسه *p = getenv("CURL_WS_CHUNK_EAGAIN");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#endif

    وقتی(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#ifdef DEBUGBUILD
      اگر(eagain_next)
        بازگشت CURLE_AGAIN;
      اگر(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_next = TRUE;
      }
#endif
      اگر(blocking) {
        result = ws_send_raw_blocking(data, ws, (ثابت نویسه *)out, outlen);
        n = result ? 0 : outlen;
      }
      وگرنه اگر(data->set.connect_only || Curl_api_is_in_callback(data))
        result = Curl_senddata(data, out, outlen, &n);
      وگرنه {
        result = Curl_xfer_send(data, out, outlen, FALSE, &n);
        اگر(!result && !n && outlen)
          result = CURLE_AGAIN;
      }

      اگر(result == CURLE_AGAIN) {
        CURL_TRC_WS(data, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        بازگشت result;
      }
      وگرنه اگر(result) {
        failf(data, "[WS] flush, write error %d", (صحیح)result);
        بازگشت result;
      }
      وگرنه {
        CURL_TRC_WS(data, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ws_send_raw_blocking(ساختار Curl_easy *data,
                                     ساختار websocket *ws,
                                     ثابت نویسه *buffer, size_t buflen)
{
  CURLcode result = CURLE_OK;
  size_t nwritten;

  اگر(!data)
    بازگشت result;

  (تهی)ws;
  وقتی(buflen) {
    result = Curl_xfer_send(data, buffer, buflen, FALSE, &nwritten);
    اگر(result)
      بازگشت result;
    DEBUGASSERT(nwritten <= buflen);
    buffer += nwritten;
    buflen -= nwritten;
    اگر(buflen) {
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      timediff_t چپ_ms;
      صحیح ev;

      CURL_TRC_WS(data, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      چپ_ms = Curl_timeleft_ms(data);
      اگر(چپ_ms < 0) {
        failf(data, "[WS] Timeout waiting for socket becoming writable");
        بازگشت CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      اگر(sock == CURL_SOCKET_BAD)
        بازگشت CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, چپ_ms ? چپ_ms : 500);
      اگر(ev < 0) {
        failf(data, "[WS] Error while waiting for socket becoming writable");
        بازگشت CURLE_SEND_ERROR;
      }
    }
  }
  بازگشت result;
}

ایستا CURLcode ws_send_raw(ساختار Curl_easy *data, ثابت تهی *buffer,
                            size_t buflen, size_t *pnwritten)
{
  ساختار websocket *ws;
  CURLcode result;

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  اگر(!ws) {
    failf(data, "[WS] Not a websocket transfer");
    بازگشت CURLE_SEND_ERROR;
  }
  اگر(!buflen)
    بازگشت CURLE_OK;

  اگر(Curl_api_is_in_callback(data)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    result = ws_flush(data, ws, TRUE);
    اگر(result)
      بازگشت result;
    result = ws_send_raw_blocking(data, ws, buffer, buflen);
    اگر(!result)
      *pnwritten = buflen;
  }
  وگرنه {
    /* We need any pending data to be sent or EAGAIN this call. */
    result = ws_flush(data, ws, FALSE);
    اگر(result)
      بازگشت result;
    result = Curl_senddata(data, buffer, buflen, pnwritten);
  }

  CURL_TRC_WS(data, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (صحیح)result, *pnwritten);
  بازگشت result;
}

CURLcode curl_ws_send(CURL *curl, ثابت تهی *buffer_arg,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned صحیح flags)
{
  ساختار Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  اگر(CURL_EAPI_ENTER(&guard, curl, ws_send, &result)) {
    ساختار websocket *ws;
    ثابت uint8_t *buffer = buffer_arg;
    ساختار Curl_easy *data = curl;
    size_t ndummy;
    size_t *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, data->set.ws_raw_mode);

    *pnsent = 0;

    اگر(!buffer && buflen) {
      failf(data, "[WS] buffer is NULL when buflen is not");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      برو out;
    }

    اگر(!data->conn && data->set.connect_only) {
      result = Curl_connect_only_attach(data);
      اگر(result)
        برو out;
    }
    اگر(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      برو out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    اگر(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      برو out;
    }

    اگر(data->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      result = ws_flush(data, ws, FALSE);
      اگر(result)
        برو out;

      اگر(!buffer) {
        failf(data, "[WS] buffer is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        برو out;
      }
      اگر(!sent) {
        failf(data, "[WS] sent is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        برو out;
      }
      اگر(fragsize || flags) {
        failf(data, "[WS] fragsize and flags must be zero in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        برو out;
      }
      result = ws_send_raw(data, buffer, buflen, pnsent);
      برو out;
    }

    /* Not RAW mode, we do the frame encoding */
    result = ws_enc_send(data, ws, buffer, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, data->set.ws_raw_mode, (صحیح)result,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}

ایستا CURLcode ws_setup_conn(ساختار Curl_easy *data,
                              ساختار connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  data->state.http_neg.accept_09 = FALSE;
  data->state.http_neg.only_10 = FALSE;
  data->state.http_neg.wanted = CURL_HTTP_V1x;
  data->state.http_neg.allowed = CURL_HTTP_V1x;
  بازگشت Curl_http_setup_conn(data, conn);
}

ثابت ساختار curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  ساختار Curl_easy *data = curl;
  اگر(GOOD_EASY_HANDLE(data) && Curl_api_is_in_callback(data) &&
     data->conn && !data->set.ws_raw_mode) {
    ساختار websocket *ws;
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    اگر(ws)
      بازگشت &ws->recvframe;
  }
  بازگشت پوچ;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned صحیح flags,
                                         curl_off_t frame_len)
{
  ساختار Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  اگر(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &result)) {
    ساختار Curl_easy *data = curl;
    ساختار websocket *ws;

    اگر(data->set.ws_raw_mode) {
      failf(data, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      result = CURLE_FAILED_INIT;
      برو out;
    }

    CURL_TRC_WS(data, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    اگر(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      برو out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    اگر(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      برو out;
    }

    اگر(ws->enc.payload_remain) {
      failf(data, "[WS] previous frame not finished");
      result = CURLE_SEND_ERROR;
      برو out;
    }

    result = ws_enc_write_head(data, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    اگر(result)
      CURL_TRC_WS(data, "curl_start_frame(), error adding frame head %d",
                  (صحیح)result);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}

ثابت ساختار Curl_protocol Curl_protocol_ws = {
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

#وگرنه

CURLcode curl_ws_recv(CURL *curl, تهی *buffer, size_t buflen,
                      size_t *recv,
                      ثابت ساختار curl_ws_frame **metap)
{
  (تهی)curl;
  (تهی)buffer;
  (تهی)buflen;
  (تهی)recv;
  (تهی)metap;
  بازگشت CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, ثابت تهی *buffer,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned صحیح flags)
{
  (تهی)curl;
  (تهی)buffer;
  (تهی)buflen;
  (تهی)sent;
  (تهی)fragsize;
  (تهی)flags;
  بازگشت CURLE_NOT_BUILT_IN;
}

ثابت ساختار curl_ws_frame *curl_ws_meta(CURL *data)
{
  (تهی)data;
  بازگشت پوچ;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned صحیح flags,
                                         curl_off_t frame_len)
{
  (تهی)curl;
  (تهی)flags;
  (تهی)frame_len;
  بازگشت CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
