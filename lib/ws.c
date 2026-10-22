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
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "ws.h"

#ifndef CURL_DISABLE_WEBSOCKETS

#অন্তর্ভাব "url.h"
#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "cf-recvbuf.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "easyif.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curlx/strcopy.h"

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
#define WS_CHUNK_গুনতি 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
গণনা ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_HEAD,
  WS_DEC_PAYLOAD
};

সংরচনা ws_decoder {
  পূর্ণাঙ্ক frame_age;        /* zero */
  পূর্ণাঙ্ক frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t head[10];
  পূর্ণাঙ্ক head_len, head_total;
  গণনা ws_dec_state state;
  পূর্ণাঙ্ক cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
সংরচনা ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  unsigned পূর্ণাঙ্ক xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#define WS_MAX_CNTRL_LEN    125

সংরচনা ws_cntrl_frame {
  unsigned পূর্ণাঙ্ক type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
সংরচনা websocket {
  সংরচনা Curl_easy *data; /* used for write callback handling */
  সংরচনা ws_decoder dec;  /* decode of ws frames */
  সংরচনা ws_encoder enc;  /* encode of ws frames */
  সংরচনা bufq recvbuf;    /* raw data from the server */
  সংরচনা bufq sendbuf;    /* raw data to be sent to the server */
  সংরচনা curl_ws_frame recvframe;  /* the current WS FRAME received */
  সংরচনা ws_cntrl_frame pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#ifdef CURLVERBOSE
স্থির ধ্রুব বর্ণ *ws_frame_name_of_op(uint8_t firstbyte)
{
  বিকল্প(firstbyte & WSBIT_OPCODE_MASK) {
  পক্ষ WSBIT_OPCODE_CONT:
    প্রত্যাগম "CONT";
  পক্ষ WSBIT_OPCODE_TEXT:
    প্রত্যাগম "TEXT";
  পক্ষ WSBIT_OPCODE_BIN:
    প্রত্যাগম "BIN";
  পক্ষ WSBIT_OPCODE_CLOSE:
    প্রত্যাগম "CLOSE";
  পক্ষ WSBIT_OPCODE_PING:
    প্রত্যাগম "PING";
  পক্ষ WSBIT_OPCODE_PONG:
    প্রত্যাগম "PONG";
  স্বভাব:
    প্রত্যাগম "???";
  }
}
#endif

স্থির পূর্ণাঙ্ক ws_frame_firstbyte2flags(সংরচনা Curl_easy *data,
                                    uint8_t firstbyte, পূর্ণাঙ্ক cont_flags)
{
  বিকল্প(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  পক্ষ WSBIT_OPCODE_CONT:
    যদি(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      প্রত্যাগম 0;
    }
    প্রত্যাগম cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  পক্ষ (WSBIT_OPCODE_CONT | WSBIT_FIN):
    যদি(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      প্রত্যাগম 0;
    }
    প্রত্যাগম cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  পক্ষ WSBIT_OPCODE_TEXT:
    যদি(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      প্রত্যাগম 0;
    }
    প্রত্যাগম CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  পক্ষ (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    যদি(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      প্রত্যাগম 0;
    }
    প্রত্যাগম CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  পক্ষ WSBIT_OPCODE_BIN:
    যদি(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      প্রত্যাগম 0;
    }
    প্রত্যাগম CURLWS_দ্বিমিক | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  পক্ষ (WSBIT_OPCODE_BIN | WSBIT_FIN):
    যদি(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      প্রত্যাগম 0;
    }
    প্রত্যাগম CURLWS_দ্বিমিক;
  /* 0x08 - first CLOSE fragment */
  পক্ষ WSBIT_OPCODE_CLOSE:
    failf(data, "[WS] invalid fragmented CLOSE frame");
    প্রত্যাগম 0;
  /* 0x88 - unfragmented CLOSE */
  পক্ষ (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    প্রত্যাগম CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  পক্ষ WSBIT_OPCODE_PING:
    failf(data, "[WS] invalid fragmented PING frame");
    প্রত্যাগম 0;
  /* 0x89 - unfragmented PING */
  পক্ষ (WSBIT_OPCODE_PING | WSBIT_FIN):
    প্রত্যাগম CURLWS_PING;
  /* 0x0a - first PONG fragment */
  পক্ষ WSBIT_OPCODE_PONG:
    failf(data, "[WS] invalid fragmented PONG frame");
    প্রত্যাগম 0;
  /* 0x8a - unfragmented PONG */
  পক্ষ (WSBIT_OPCODE_PONG | WSBIT_FIN):
    প্রত্যাগম CURLWS_PONG;
  /* invalid first byte */
  স্বভাব:
    যদি(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(data, "[WS] invalid reserved bits: %02x", firstbyte);
    অন্যথা
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(data, "[WS] invalid opcode: %02x", firstbyte);
    প্রত্যাগম 0;
  }
}

স্থির CURLcode ws_frame_flags2firstbyte(সংরচনা Curl_easy *data,
                                         unsigned পূর্ণাঙ্ক flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  বিকল্প(flags & ~CURLWS_OFFSET) {
  পক্ষ 0:
    যদি(contfragment) {
      CURL_TRC_WS(data, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      প্রত্যাগম CURLE_OK;
    }
    failf(data, "[WS] no flags given");
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  পক্ষ CURLWS_CONT:
    যদি(contfragment) {
      infof(data, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      প্রত্যাগম CURLE_OK;
    }
    failf(data, "[WS] No ongoing fragmented message to continue");
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  পক্ষ CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    প্রত্যাগম CURLE_OK;
  পক্ষ (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    প্রত্যাগম CURLE_OK;
  পক্ষ CURLWS_দ্বিমিক:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    প্রত্যাগম CURLE_OK;
  পক্ষ (CURLWS_দ্বিমিক | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    প্রত্যাগম CURLE_OK;
  পক্ষ CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    প্রত্যাগম CURLE_OK;
  পক্ষ (CURLWS_CLOSE | CURLWS_CONT):
    failf(data, "[WS] CLOSE frame must not be fragmented");
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  পক্ষ CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    প্রত্যাগম CURLE_OK;
  পক্ষ (CURLWS_PING | CURLWS_CONT):
    failf(data, "[WS] PING frame must not be fragmented");
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  পক্ষ CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    প্রত্যাগম CURLE_OK;
  পক্ষ (CURLWS_PONG | CURLWS_CONT):
    failf(data, "[WS] PONG frame must not be fragmented");
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  স্বভাব:
    failf(data, "[WS] unknown flags: %x", flags);
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

স্থির শূন্য ws_dec_info(সংরচনা ws_decoder *dec, সংরচনা Curl_easy *data,
                        ধ্রুব বর্ণ *বার্তা)
{
  NOVERBOSE((শূন্য)বার্তা);
  বিকল্প(dec->head_len) {
  পক্ষ 0:
    ভঙ্গ;
  পক্ষ 1:
    CURL_TRC_WS(data, "decoded %s [%s%s]", বার্তা,
                ws_frame_name_of_op(dec->head[0]),
                (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    ভঙ্গ;
  স্বভাব:
    যদি(dec->head_len < dec->head_total) {
      CURL_TRC_WS(data, "decoded %s [%s%s](%d/%d)", বার্তা,
                  ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->head_len, dec->head_total);
    }
    অন্যথা {
      CURL_TRC_WS(data, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  বার্তা, ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    ভঙ্গ;
  }
}

স্থির CURLcode ws_send_raw_blocking(সংরচনা Curl_easy *data,
                                     সংরচনা websocket *ws,
                                     ধ্রুব বর্ণ *buffer, size_t buflen);

প্রকারনির্দেশ CURLcode ws_write_payload(ধ্রুব uint8_t *buf, size_t buflen,
                                  পূর্ণাঙ্ক frame_age, পূর্ণাঙ্ক frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  শূন্য *userp,
                                  size_t *pnwritten);

স্থির শূন্য ws_dec_next_frame(সংরচনা ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

স্থির শূন্য ws_dec_reset(সংরচনা ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

স্থির শূন্য ws_dec_init(সংরচনা ws_decoder *dec)
{
  ws_dec_reset(dec);
}

স্থির CURLcode ws_dec_read_head(সংরচনা ws_decoder *dec,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা bufq *inraw)
{
  ধ্রুব uint8_t *inbuf;
  size_t inlen;

  যাবত্(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    যদি(dec->head_len == 0) {
      dec->head[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(data, dec->head[0],
                                                  dec->cont_flags);
      যদি(!dec->frame_flags) {
        ws_dec_reset(dec);
        প্রত্যাগম CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      যদি(dec->frame_flags & (CURLWS_TEXT | CURLWS_দ্বিমিক)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->head_len = 1;
#যদি 0
      ws_dec_info(dec, data, "seeing opcode");
#endif
      অনুবর্ত;
    }
    অন্যথা যদি(dec->head_len == 1) {
      dec->head[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->head_len = 2;

      যদি(dec->head[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(data, "[WS] masked input frame");
        ws_dec_reset(dec);
        প্রত্যাগম CURLE_RECV_ERROR;
      }
      যদি(dec->frame_flags & CURLWS_PING && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(data, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        প্রত্যাগম CURLE_RECV_ERROR;
      }
      যদি(dec->frame_flags & CURLWS_PONG && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(data, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        প্রত্যাগম CURLE_RECV_ERROR;
      }
      যদি(dec->frame_flags & CURLWS_CLOSE && dec->head[1] > WS_MAX_CNTRL_LEN) {
        failf(data, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        প্রত্যাগম CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      যদি(dec->head[1] == 126) {
        dec->head_total = 4;
        অনুবর্ত;
      }
      অন্যথা যদি(dec->head[1] == 127) {
        dec->head_total = 10;
        অনুবর্ত;
      }
      অন্যথা {
        dec->head_total = 2;
      }
    }

    যদি(dec->head_len < dec->head_total) {
      dec->head[dec->head_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->head_len;
      যদি(dec->head_len < dec->head_total) {
#যদি 0
        ws_dec_info(dec, data, "decoding head");
#endif
        অনুবর্ত;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->head_len == dec->head_total);
    বিকল্প(dec->head_total) {
    পক্ষ 2:
      dec->payload_len = dec->head[1];
      ভঙ্গ;
    পক্ষ 4:
      dec->payload_len = (dec->head[2] << 8) | dec->head[3];
      ভঙ্গ;
    পক্ষ 10:
      যদি(dec->head[2] > 127) {
        failf(data, "[WS] frame length longer than 63 bits not supported");
        প্রত্যাগম CURLE_RECV_ERROR;
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
      ভঙ্গ;
    স্বভাব:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(data, "[WS] unexpected frame header length");
      প্রত্যাগম CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, data, "head");
    প্রত্যাগম CURLE_OK;
  }
  প্রত্যাগম CURLE_AGAIN;
}

স্থির CURLcode ws_dec_pass_payload(সংরচনা ws_decoder *dec,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা bufq *inraw,
                                    ws_write_payload *write_cb,
                                    শূন্য *write_ctx)
{
  ধ্রুব uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode result;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  যাবত্(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(data)) {
    যদি(inlen > remain)
      inlen = remain;
    result = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    যদি(result)
      প্রত্যাগম result;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(data, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  প্রত্যাগম remain ? CURLE_AGAIN : CURLE_OK;
}

স্থির CURLcode ws_dec_pass(সংরচনা ws_decoder *dec,
                            সংরচনা Curl_easy *data,
                            সংরচনা bufq *inraw,
                            ws_write_payload *write_cb,
                            শূন্য *write_ctx)
{
  CURLcode result;

  যদি(Curl_bufq_is_empty(inraw))
    প্রত্যাগম CURLE_AGAIN;

  বিকল্প(dec->state) {
  পক্ষ WS_DEC_INIT:
    ws_dec_next_frame(dec);
    dec->state = WS_DEC_HEAD;
    FALLTHROUGH();
  পক্ষ WS_DEC_HEAD:
    result = ws_dec_read_head(dec, data, inraw);
    যদি(result) {
      যদি(result != CURLE_AGAIN) {
        failf(data, "[WS] decode frame error %d", (পূর্ণাঙ্ক)result);
        ভঙ্গ;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      ভঙ্গ;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    যদি(dec->payload_len == 0) {
      size_t nwritten;
      ধ্রুব uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      result = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      যদি(result)
        প্রত্যাগম result;
      dec->state = WS_DEC_INIT;
      ভঙ্গ;
    }
    FALLTHROUGH();
  পক্ষ WS_DEC_PAYLOAD:
    result = ws_dec_pass_payload(dec, data, inraw, write_cb, write_ctx);
    ws_dec_info(dec, data, "passing");
    যদি(result)
      প্রত্যাগম result;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    ভঙ্গ;
  স্বভাব:
    /* we covered all enums above, but some code analyzers are wimps */
    result = CURLE_FAILED_INIT;
  }
  প্রত্যাগম result;
}

স্থির শূন্য update_meta(সংরচনা websocket *ws,
                        পূর্ণাঙ্ক frame_age, পূর্ণাঙ্ক frame_flags,
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
সংরচনা ws_cw_ctx {
  সংরচনা Curl_cwriter super;
  সংরচনা bufq buf;
};

স্থির CURLcode ws_cw_init(সংরচনা Curl_easy *data,
                           সংরচনা Curl_cwriter *writer)
{
  সংরচনা ws_cw_ctx *ctx = writer->ctx;
  (শূন্য)data;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_SIZE, 1, BUFQ_OPT_SOFT_LIMIT);
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য ws_cw_close(সংরচনা Curl_easy *data, সংরচনা Curl_cwriter *writer)
{
  সংরচনা ws_cw_ctx *ctx = writer->ctx;
  (শূন্য)data;
  Curl_bufq_free(&ctx->buf);
}

সংরচনা ws_cw_dec_ctx {
  সংরচনা Curl_easy *data;
  সংরচনা websocket *ws;
  সংরচনা Curl_cwriter *next_writer;
  পূর্ণাঙ্ক cw_type;
};

স্থির CURLcode ws_flush(সংরচনা Curl_easy *data, সংরচনা websocket *ws,
                         bool blocking);
স্থির CURLcode ws_enc_add_pending(সংরচনা Curl_easy *data,
                                   সংরচনা websocket *ws);

স্থির CURLcode ws_enc_add_cntrl(সংরচনা Curl_easy *data,
                                 সংরচনা websocket *ws,
                                 ধ্রুব uint8_t *payload,
                                 size_t plen,
                                 unsigned পূর্ণাঙ্ক frame_type)
{
  (শূন্য)data;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  যদি(plen > WS_MAX_CNTRL_LEN)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  memcpy(ws->pending.payload, payload, plen);
  প্রত্যাগম CURLE_OK;
}

স্থির curl_off_t ws_payload_remain(curl_off_t payload_total,
                                    curl_off_t payload_offset,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_total - payload_offset;
  যদি((payload_total < 0) || (payload_offset < 0) || (remain < 0))
    প্রত্যাগম -1;
  buffered = curlx_uztoso(payload_buffered);
  যদি(remain < buffered)
    প্রত্যাগম -1;
  প্রত্যাগম remain - buffered;
}

স্থির CURLcode ws_cw_dec_next(ধ্রুব uint8_t *buf, size_t buflen,
                               পূর্ণাঙ্ক frame_age, পূর্ণাঙ্ক frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               শূন্য *user_data,
                               size_t *pnwritten)
{
  সংরচনা ws_cw_dec_ctx *ctx = user_data;
  সংরচনা Curl_easy *data = ctx->data;
  সংরচনা websocket *ws = ctx->ws;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result;

  (শূন্য)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  যদি(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }

  যদি(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(data, ws, buf, buflen, CURLWS_PONG);
    যদি(result)
      প্রত্যাগম result;
  }
  অন্যথা যদি(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(data, "[WS] pass %zu decoded bytes", buflen);
    result = Curl_cwriter_write(data, ctx->next_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ধ্রুব বর্ণ *)buf, buflen);
    যদি(result)
      প্রত্যাগম result;
  }
  *pnwritten = buflen;
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode ws_cw_write(সংরচনা Curl_easy *data,
                            সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                            ধ্রুব বর্ণ *buf, size_t nbytes)
{
  সংরচনা ws_cw_ctx *ctx = writer->ctx;
  সংরচনা websocket *ws;
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[WS] write(len=%zu, type=%d)", nbytes, type);
  যদি(!(type & CLIENTWRITE_BODY) || data->set.ws_raw_mode)
    প্রত্যাগম Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  যদি(!ws) {
    failf(data, "[WS] not a websocket transfer");
    প্রত্যাগম CURLE_FAILED_INIT;
  }

  যদি(nbytes) {
    size_t nwritten;
    result = Curl_bufq_write(&ctx->buf, (ধ্রুব uint8_t *)buf,
                             nbytes, &nwritten);
    যদি(result) {
      infof(data, "[WS] error adding data to buffer %d", (পূর্ণাঙ্ক)result);
      প্রত্যাগম result;
    }
  }

  result = Curl_cwriter_flush(data, writer->next);
  যদি(result)
    গচ্ছ out;

  যাবত্(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
    সংরচনা ws_cw_dec_ctx pass_ctx;
    pass_ctx.data = data;
    pass_ctx.ws = ws;
    pass_ctx.next_writer = writer->next;
    pass_ctx.cw_type = type;
    result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                         ws_cw_dec_next, &pass_ctx);
    যদি(result == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      result = CURLE_OK;
      গচ্ছ out;
    }
    অন্যথা যদি(result) {
      failf(data, "[WS] decode payload error %d", (পূর্ণাঙ্ক)result);
      Curl_bufq_reset(&ctx->buf);
      গচ্ছ out;
    }
  }

out:
  যদি(!result && !ws->using_reader) {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    যদি(result == CURLE_AGAIN)
      result = CURLE_OK;
  }

  যদি(!result && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(data, "[WS] decode ended with incomplete frame");
    result = CURLE_RECV_ERROR;
  }
  প্রত্যাগম result;
}

স্থির CURLcode ws_cw_flush(সংরচনা Curl_easy *data,
                            সংরচনা Curl_cwriter *writer)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[ws] flush");
  যদি(!data->set.ws_raw_mode) {
    সংরচনা ws_cw_ctx *ctx = writer->ctx;
    সংরচনা websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    result = Curl_cwriter_flush(data, writer->next);
    যদি(result)
      গচ্ছ out;

    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    যদি(!ws) {
      failf(data, "[WS] not a websocket transfer");
      প্রত্যাগম CURLE_FAILED_INIT;
    }

    যাবত্(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
      সংরচনা ws_cw_dec_ctx pass_ctx;
      pass_ctx.data = data;
      pass_ctx.ws = ws;
      pass_ctx.next_writer = writer->next;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                           ws_cw_dec_next, &pass_ctx);
      যদি(result == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        result = CURLE_OK;
        গচ্ছ out;
      }
      অন্যথা যদি(result) {
        failf(data, "[WS] decode payload error %d", (পূর্ণাঙ্ক)result);
        Curl_bufq_reset(&ctx->buf);
        গচ্ছ out;
      }
    }
  }

out:
  যদি(!result)
    result = Curl_cwriter_flush(data, writer->next);
  প্রত্যাগম result;
}

/* WebSocket payload decoding client writer. */
স্থির ধ্রুব সংরচনা Curl_cwtype ws_cw_decode = {
  "ws-decode",
  রিক্ত,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  পরিমাণ(সংরচনা ws_cw_ctx)
};

স্থির শূন্য ws_enc_info(সংরচনা ws_encoder *enc, সংরচনা Curl_easy *data,
                        ধ্রুব বর্ণ *বার্তা)
{
  NOVERBOSE((শূন্য)enc);
  NOVERBOSE((শূন্য)বার্তা);
  CURL_TRC_WS(data, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              বার্তা, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

স্থির শূন্য ws_enc_reset(সংরচনা ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

স্থির শূন্য ws_enc_init(সংরচনা ws_encoder *enc)
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

স্থির CURLcode ws_enc_add_frame(সংরচনা Curl_easy *data,
                                 সংরচনা ws_encoder *enc,
                                 unsigned পূর্ণাঙ্ক flags,
                                 curl_off_t payload_len,
                                 সংরচনা bufq *out)
{
  uint8_t firstb = 0;
  uint8_t head[14];
  CURLcode result;
  size_t hlen, nwritten;

  যদি(payload_len < 0) {
    failf(data, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    প্রত্যাগম CURLE_SEND_ERROR;
  }

  যদি(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(data, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    প্রত্যাগম CURLE_SEND_ERROR;
  }

  result = ws_frame_flags2firstbyte(data, flags, (bool)enc->contfragment,
                                    &firstb);
  যদি(result)
    প্রত্যাগম result;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  যদি(flags & (CURLWS_TEXT | CURLWS_দ্বিমিক)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  যদি(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PING frame is too big");
    প্রত্যাগম CURLE_TOO_LARGE;
  }
  যদি(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PONG frame is too big");
    প্রত্যাগম CURLE_TOO_LARGE;
  }
  যদি(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given CLOSE frame is too big");
    প্রত্যাগম CURLE_TOO_LARGE;
  }

  head[0] = enc->firstbyte = firstb;
  যদি(payload_len > 65535) {
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
  অন্যথা যদি(payload_len >= 126) {
    head[1] = 126 | WSBIT_MASK;
    head[2] = (uint8_t)((payload_len >> 8) & 0xff);
    head[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  অন্যথা {
    head[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, data, "sending");

  /* 4 bytes random */

  result = Curl_rand(data, (uint8_t *)&enc->mask, পরিমাণ(enc->mask));
  যদি(result)
    প্রত্যাগম result;

#ifdef DEBUGBUILD
  যদি(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->mask, 0, পরিমাণ(enc->mask));
#endif

  /* add 4 bytes mask */
  memcpy(&head[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  result = Curl_bufq_write(out, head, hlen, &nwritten);
  যদি(result)
    প্রত্যাগম result;
  যদি(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    প্রত্যাগম CURLE_SEND_ERROR;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode ws_enc_write_head(সংরচনা Curl_easy *data,
                                  সংরচনা websocket *ws,
                                  সংরচনা ws_encoder *enc,
                                  unsigned পূর্ণাঙ্ক flags,
                                  curl_off_t payload_len,
                                  সংরচনা bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  যদি(ws->pending.type) {
    CURLcode result = ws_enc_add_pending(data, ws);
    যদি(result)
      প্রত্যাগম result;
  }
  প্রত্যাগম ws_enc_add_frame(data, enc, flags, payload_len, out);
}

স্থির CURLcode ws_enc_write_payload(সংরচনা ws_encoder *enc,
                                     সংরচনা Curl_easy *data,
                                     ধ্রুব uint8_t *buf, size_t buflen,
                                     সংরচনা bufq *out, size_t *pnwritten)
{
  CURLcode result;
  size_t i, len, n, remain;

  *pnwritten = 0;
  যদি(Curl_bufq_is_full(out))
    প্রত্যাগম CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  যদি(remain < len)
    len = remain;

  প্রতি(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    result = Curl_bufq_write(out, &c, 1, &n);
    যদি(result) {
      যদি((result != CURLE_AGAIN) || !i)
        প্রত্যাগম result;
      ভঙ্গ;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, data, "buffered");
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode ws_enc_add_pending(সংরচনা Curl_easy *data,
                                   সংরচনা websocket *ws)
{
  CURLcode result;
  size_t n;

  যদি(!ws->pending.type) /* no pending frame here */
    প্রত্যাগম CURLE_OK;
  যদি(ws->enc.payload_remain) /* in the middle of another frame */
    প্রত্যাগম CURLE_AGAIN;

  result = ws_enc_add_frame(data, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  যদি(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding head: %d",
                (পূর্ণাঙ্ক)result);
    গচ্ছ out;
  }
  result = ws_enc_write_payload(&ws->enc, data, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  যদি(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding payload: %d",
                (পূর্ণাঙ্ক)result);
    গচ্ছ out;
  }
  যদি(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(data, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, পরিমাণ(ws->pending));

out:
  প্রত্যাগম result;
}

স্থির CURLcode ws_enc_send(সংরচনা Curl_easy *data,
                            সংরচনা websocket *ws,
                            ধ্রুব uint8_t *buffer,
                            size_t buflen,
                            curl_off_t fragsize,
                            unsigned পূর্ণাঙ্ক flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!data->set.ws_raw_mode);
  *pnsent = 0;

  যদি(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    যদি(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(data, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    }
    যদি((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(data, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  অন্যথা {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    যদি(result)
      প্রত্যাগম result;

    result = ws_enc_write_head(data, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    যদি(result) {
      CURL_TRC_WS(data, "curl_ws_send(), error writing frame head %d",
                  (পূর্ণাঙ্ক)result);
      প্রত্যাগম result;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  যাবত্(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    যদি(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      result = ws_enc_write_payload(&ws->enc, data,
                                    buffer + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      যদি(result && (result != CURLE_AGAIN))
        প্রত্যাগম result;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      যদি(!ws->sendbuf_payload) {
        প্রত্যাগম CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    যদি(!result && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      buffer += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    অন্যথা যদি(result == CURLE_AGAIN) {
      যদি(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        প্রত্যাগম CURLE_OK;
      }
      অন্যথা {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(data, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        প্রত্যাগম CURLE_AGAIN;
      }
    }
    অন্যথা
      প্রত্যাগম result;  /* real error sending the data */
  }
  প্রত্যাগম CURLE_OK;
}

সংরচনা cr_ws_ctx {
  সংরচনা Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

স্থির CURLcode cr_ws_init(সংরচনা Curl_easy *data, সংরচনা Curl_creader *reader)
{
  (শূন্য)data;
  (শূন্য)reader;
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য cr_ws_close(সংরচনা Curl_easy *data, সংরচনা Curl_creader *reader)
{
  (শূন্য)data;
  (শূন্য)reader;
}

স্থির CURLcode cr_ws_read(সংরচনা Curl_easy *data,
                           সংরচনা Curl_creader *reader,
                           বর্ণ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  সংরচনা cr_ws_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, n;
  সংরচনা websocket *ws;
  bool eos;

  *pnread = 0;
  যদি(ctx->eos) {
    *peos = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  যদি(!ws) {
    failf(data, "[WS] not a websocket transfer");
    প্রত্যাগম CURLE_FAILED_INIT;
  }

  যদি(Curl_bufq_is_empty(&ws->sendbuf)) {
    যদি(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      প্রত্যাগম CURLE_OK;
    }

    যদি(ws->enc.payload_remain) {
      CURL_TRC_WS(data, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    যদি(result)
      প্রত্যাগম result;
    ctx->read_eos = eos;

    যদি(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(data, reader->next);
    }
    অন্যথা যদি(!nread) {
      /* nothing to convert, return this right away */
      যদি(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      গচ্ছ out;
    }

    যদি(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      result = ws_enc_write_head(data, ws, &ws->enc, CURLWS_দ্বিমিক, nread,
                                 &ws->sendbuf);
      যদি(result)
        গচ্ছ out;
    }

    result = ws_enc_write_payload(&ws->enc, data, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    যদি(result)
      গচ্ছ out;
    CURL_TRC_READ(data, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  যদি(!result && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (পূর্ণাঙ্ক)result, *pnread, *peos);
  প্রত্যাগম result;
}

স্থির curl_off_t cr_ws_total_length(সংরচনা Curl_easy *data,
                                     সংরচনা Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (শূন্য)data;
  (শূন্য)reader;
  প্রত্যাগম -1;
}

স্থির ধ্রুব সংরচনা Curl_crtype ws_cr_encode = {
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
  পরিমাণ(সংরচনা cr_ws_ctx)
};

সংরচনা wsfield {
  ধ্রুব বর্ণ *name;
  ধ্রুব বর্ণ *val;
};

CURLcode Curl_ws_request(সংরচনা Curl_easy *data, সংরচনা dynbuf *req)
{
  unsigned পূর্ণাঙ্ক i;
  CURLcode result = CURLE_OK;
  uint8_t rand[16];
  বর্ণ *randstr;
  size_t randlen;
  বর্ণ keyval[40];
  সংরচনা SingleRequest *k = &data->req;
  সংরচনা wsfield heads[] = {
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
      "Sec-WebSocket-Key", রিক্ত,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  result = Curl_rand(data, rand, পরিমাণ(rand));
  যদি(result)
    প্রত্যাগম result;
  result = curlx_base64_encode(rand, পরিমাণ(rand), &randstr, &randlen);
  যদি(result)
    প্রত্যাগম result;
  DEBUGASSERT(randlen < পরিমাণ(keyval));
  যদি(randlen >= পরিমাণ(keyval)) {
    curlx_free(randstr);
    প্রত্যাগম CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, পরিমাণ(keyval), randstr, randlen);
  curlx_free(randstr);
  প্রতি(i = 0; !result && (i < CURL_ARRAYSIZE(heads)); i++) {
    যদি(!Curl_checkheaders(data, heads[i].name, strlen(heads[i].name))) {
      result = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  data->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  data->conn->bits.upgrade_in_progress = TRUE;
  প্রত্যাগম result;
}

স্থির শূন্য ws_conn_dtor(ধ্রুব শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা websocket *ws = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(সংরচনা Curl_easy *data,
                        ধ্রুব বর্ণ *mem, size_t nread)
{
  সংরচনা SingleRequest *k = &data->req;
  সংরচনা websocket *ws;
  সংরচনা Curl_cwriter *ws_dec_writer = রিক্ত;
  সংরচনা Curl_creader *ws_enc_reader = রিক্ত;
  CURLcode result;

  DEBUGASSERT(data->conn);
  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  যদি(!ws) {
    size_t chunk_size = WS_CHUNK_SIZE;
    ws = curlx_calloc(1, পরিমাণ(*ws));
    যদি(!ws)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
#ifdef DEBUGBUILD
    {
      ধ্রুব বর্ণ *p = getenv("CURL_WS_CHUNK_SIZE");
      যদি(p) {
        curl_off_t l;
        যদি(!curlx_str_সংখ্যা(&p, &l, 1 * 1024 * 1024))
          chunk_size = (size_t)l;
      }
    }
#endif
    CURL_TRC_WS(data, "WS, using chunk size %zu", chunk_size);
    Curl_bufq_init2(&ws->recvbuf, chunk_size, WS_CHUNK_গুনতি,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_size, WS_CHUNK_গুনতি,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    result = Curl_conn_meta_set(data->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    যদি(result)
      প্রত্যাগম result;
  }
  অন্যথা {
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
  যদি(result)
    গচ্ছ out;
  result = Curl_cwriter_add(data, ws_dec_writer);
  যদি(result)
    গচ্ছ out;
  ws_dec_writer = রিক্ত; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  যদি(data->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    যদি(nread) {
      result = Curl_cf_recvbuf_add(data, data->conn, FIRSTSOCKET,
                                   (ধ্রুব uint8_t *)mem, nread);
      যদি(result)
        গচ্ছ out;
    }
    CURL_REQ_CLEAR_RECV(data); /* read no more content */
  }
  অন্যথা { /* !connect_only */
    যদি(data->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(data, "UPLOAD set, add ws-encode reader");
      result = Curl_creader_set_fread(data, -1);
      যদি(result)
        গচ্ছ out;

      যদি(!data->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        result = Curl_creader_create(&ws_enc_reader, data, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        যদি(result)
          গচ্ছ out;
        result = Curl_creader_add(data, ws_enc_reader);
        যদি(result)
          গচ্ছ out;
        ws_enc_reader = রিক্ত; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      data->req.eos_read = FALSE;
      data->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(data);
    }

    /* Then pass any additional data to the writers */
    যদি(nread) {
      result = Curl_client_write(data, CLIENTWRITE_BODY, mem, nread);
      যদি(result)
        গচ্ছ out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  যদি(ws_dec_writer)
    Curl_cwriter_free(data, ws_dec_writer);
  যদি(ws_enc_reader)
    Curl_creader_free(data, ws_enc_reader);
  যদি(result)
    CURL_TRC_WS(data, "Curl_ws_accept() failed -> %d", (পূর্ণাঙ্ক)result);
  অন্যথা
    CURL_TRC_WS(data, "websocket established, %s mode",
                data->set.connect_only ? "connect-only" : "callback");
  প্রত্যাগম result;
}

সংরচনা ws_collect {
  সংরচনা Curl_easy *data;
  সংরচনা websocket *ws;
  uint8_t *buffer;
  size_t buflen;
  size_t bufidx;
  পূর্ণাঙ্ক frame_age;
  পূর্ণাঙ্ক frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

স্থির CURLcode ws_client_collect(ধ্রুব uint8_t *buf, size_t buflen,
                                  পূর্ণাঙ্ক frame_age, পূর্ণাঙ্ক frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  শূন্য *userp,
                                  size_t *pnwritten)
{
  সংরচনা ws_collect *ctx = userp;
  সংরচনা Curl_easy *data = ctx->data;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  যদি(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }

  যদি(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  যদি(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(ctx->data, ctx->ws, buf, buflen, CURLWS_PONG);
    যদি(result)
      প্রত্যাগম result;
    *pnwritten = buflen;
  }
  অন্যথা {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    যদি(!write_len) {
      যদি(!buflen)  /* 0 length write, we accept that */
        প্রত্যাগম CURLE_OK;
      প্রত্যাগম CURLE_AGAIN;  /* no more space */
    }
    memcpy(ctx->buffer + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  প্রত্যাগম result;
}

স্থির CURLcode nw_in_recv(শূন্য *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  সংরচনা Curl_easy *data = reader_ctx;
  প্রত্যাগম Curl_easy_recv(data, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, শূন্য *buffer,
                      size_t buflen, size_t *recv,
                      ধ্রুব সংরচনা curl_ws_frame **metap)
{
  সংরচনা Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  যদি(CURL_EAPI_ENTER(&guard, curl, ws_recv, &result)) {
    সংরচনা Curl_easy *data = curl;
    সংরচনা connectdata *conn;
    সংরচনা websocket *ws;
    সংরচনা ws_collect ctx;

    যদি(!recv || !metap || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      গচ্ছ out;
    }

    *recv = 0;
    *metap = রিক্ত;

    conn = data->conn;
    যদি(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      যদি(!data->set.connect_only) {
        failf(data, "[WS] CONNECT_ONLY is required");
        result = CURLE_UNSUPPORTED_PROTOCOL;
        গচ্ছ out;
      }

      Curl_getconnectinfo(data, &conn);
      যদি(!conn) {
        failf(data, "[WS] connection not found");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        গচ্ছ out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    যদি(!ws) {
      failf(data, "[WS] connection is not setup for websocket");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      গচ্ছ out;
    }

    memset(&ctx, 0, পরিমাণ(ctx));
    ctx.data = data;
    ctx.ws = ws;
    ctx.buffer = buffer;
    ctx.buflen = buflen;

    যাবত্(1) {
      /* receive more when our buffer is empty */
      যদি(Curl_bufq_is_empty(&ws->recvbuf)) {
        size_t n;
        result = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, data, &n);
        যদি(result)
          গচ্ছ out;
        অন্যথা যদি(n == 0) {
          /* connection closed */
          infof(data, "[WS] connection expectedly closed?");
          result = CURLE_GOT_NOTHING;
          গচ্ছ out;
        }
        CURL_TRC_WS(data, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      result = ws_dec_pass(&ws->dec, data, &ws->recvbuf,
                           ws_client_collect, &ctx);
      যদি(result == CURLE_AGAIN) {
        যদি(!ctx.written) {
          ws_dec_info(&ws->dec, data, "need more input");
          অনুবর্ত;  /* nothing written, try more input */
        }
        ভঙ্গ;
      }
      অন্যথা যদি(result) {
        গচ্ছ out;
      }
      অন্যথা যদি(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        ভঙ্গ;
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
    যদি(!data->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(data, ws);
      যদি(!r2)
        (শূন্য)ws_flush(data, ws, Curl_api_is_in_callback(data));
    }
    result = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  প্রত্যাগম result;
}

স্থির CURLcode ws_flush(সংরচনা Curl_easy *data, সংরচনা websocket *ws,
                         bool blocking)
{
  CURLcode result;

  /* If there is space, add any pending control frame */
  যদি(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_size) {
    result = ws_enc_add_pending(data, ws);
    যদি(result && (result != CURLE_AGAIN))
      প্রত্যাগম result;
  }

  যদি(!Curl_bufq_is_empty(&ws->sendbuf)) {
    ধ্রুব uint8_t *out;
    size_t outlen, n;
#ifdef DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_next = FALSE;
    size_t chunk_egain = 0;
    ধ্রুব বর্ণ *p = getenv("CURL_WS_CHUNK_EAGAIN");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#endif

    যাবত্(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#ifdef DEBUGBUILD
      যদি(eagain_next)
        প্রত্যাগম CURLE_AGAIN;
      যদি(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_next = TRUE;
      }
#endif
      যদি(blocking) {
        result = ws_send_raw_blocking(data, ws, (ধ্রুব বর্ণ *)out, outlen);
        n = result ? 0 : outlen;
      }
      অন্যথা যদি(data->set.connect_only || Curl_api_is_in_callback(data))
        result = Curl_senddata(data, out, outlen, &n);
      অন্যথা {
        result = Curl_xfer_send(data, out, outlen, FALSE, &n);
        যদি(!result && !n && outlen)
          result = CURLE_AGAIN;
      }

      যদি(result == CURLE_AGAIN) {
        CURL_TRC_WS(data, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        প্রত্যাগম result;
      }
      অন্যথা যদি(result) {
        failf(data, "[WS] flush, write error %d", (পূর্ণাঙ্ক)result);
        প্রত্যাগম result;
      }
      অন্যথা {
        CURL_TRC_WS(data, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode ws_send_raw_blocking(সংরচনা Curl_easy *data,
                                     সংরচনা websocket *ws,
                                     ধ্রুব বর্ণ *buffer, size_t buflen)
{
  CURLcode result = CURLE_OK;
  size_t nwritten;

  যদি(!data)
    প্রত্যাগম result;

  (শূন্য)ws;
  যাবত্(buflen) {
    result = Curl_xfer_send(data, buffer, buflen, FALSE, &nwritten);
    যদি(result)
      প্রত্যাগম result;
    DEBUGASSERT(nwritten <= buflen);
    buffer += nwritten;
    buflen -= nwritten;
    যদি(buflen) {
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      timediff_t বাম_ms;
      পূর্ণাঙ্ক ev;

      CURL_TRC_WS(data, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      বাম_ms = Curl_timeleft_ms(data);
      যদি(বাম_ms < 0) {
        failf(data, "[WS] Timeout waiting for socket becoming writable");
        প্রত্যাগম CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      যদি(sock == CURL_SOCKET_BAD)
        প্রত্যাগম CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, বাম_ms ? বাম_ms : 500);
      যদি(ev < 0) {
        failf(data, "[WS] Error while waiting for socket becoming writable");
        প্রত্যাগম CURLE_SEND_ERROR;
      }
    }
  }
  প্রত্যাগম result;
}

স্থির CURLcode ws_send_raw(সংরচনা Curl_easy *data, ধ্রুব শূন্য *buffer,
                            size_t buflen, size_t *pnwritten)
{
  সংরচনা websocket *ws;
  CURLcode result;

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  যদি(!ws) {
    failf(data, "[WS] Not a websocket transfer");
    প্রত্যাগম CURLE_SEND_ERROR;
  }
  যদি(!buflen)
    প্রত্যাগম CURLE_OK;

  যদি(Curl_api_is_in_callback(data)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    result = ws_flush(data, ws, TRUE);
    যদি(result)
      প্রত্যাগম result;
    result = ws_send_raw_blocking(data, ws, buffer, buflen);
    যদি(!result)
      *pnwritten = buflen;
  }
  অন্যথা {
    /* We need any pending data to be sent or EAGAIN this call. */
    result = ws_flush(data, ws, FALSE);
    যদি(result)
      প্রত্যাগম result;
    result = Curl_senddata(data, buffer, buflen, pnwritten);
  }

  CURL_TRC_WS(data, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (পূর্ণাঙ্ক)result, *pnwritten);
  প্রত্যাগম result;
}

CURLcode curl_ws_send(CURL *curl, ধ্রুব শূন্য *buffer_arg,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned পূর্ণাঙ্ক flags)
{
  সংরচনা Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  যদি(CURL_EAPI_ENTER(&guard, curl, ws_send, &result)) {
    সংরচনা websocket *ws;
    ধ্রুব uint8_t *buffer = buffer_arg;
    সংরচনা Curl_easy *data = curl;
    size_t ndummy;
    size_t *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, data->set.ws_raw_mode);

    *pnsent = 0;

    যদি(!buffer && buflen) {
      failf(data, "[WS] buffer is NULL when buflen is not");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      গচ্ছ out;
    }

    যদি(!data->conn && data->set.connect_only) {
      result = Curl_connect_only_attach(data);
      যদি(result)
        গচ্ছ out;
    }
    যদি(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    যদি(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }

    যদি(data->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      result = ws_flush(data, ws, FALSE);
      যদি(result)
        গচ্ছ out;

      যদি(!buffer) {
        failf(data, "[WS] buffer is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        গচ্ছ out;
      }
      যদি(!sent) {
        failf(data, "[WS] sent is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        গচ্ছ out;
      }
      যদি(fragsize || flags) {
        failf(data, "[WS] fragsize and flags must be zero in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        গচ্ছ out;
      }
      result = ws_send_raw(data, buffer, buflen, pnsent);
      গচ্ছ out;
    }

    /* Not RAW mode, we do the frame encoding */
    result = ws_enc_send(data, ws, buffer, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, data->set.ws_raw_mode, (পূর্ণাঙ্ক)result,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  প্রত্যাগম result;
}

স্থির CURLcode ws_setup_conn(সংরচনা Curl_easy *data,
                              সংরচনা connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  data->state.http_neg.accept_09 = FALSE;
  data->state.http_neg.only_10 = FALSE;
  data->state.http_neg.wanted = CURL_HTTP_V1x;
  data->state.http_neg.allowed = CURL_HTTP_V1x;
  প্রত্যাগম Curl_http_setup_conn(data, conn);
}

ধ্রুব সংরচনা curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  সংরচনা Curl_easy *data = curl;
  যদি(GOOD_EASY_HANDLE(data) && Curl_api_is_in_callback(data) &&
     data->conn && !data->set.ws_raw_mode) {
    সংরচনা websocket *ws;
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    যদি(ws)
      প্রত্যাগম &ws->recvframe;
  }
  প্রত্যাগম রিক্ত;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned পূর্ণাঙ্ক flags,
                                         curl_off_t frame_len)
{
  সংরচনা Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  যদি(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &result)) {
    সংরচনা Curl_easy *data = curl;
    সংরচনা websocket *ws;

    যদি(data->set.ws_raw_mode) {
      failf(data, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      result = CURLE_FAILED_INIT;
      গচ্ছ out;
    }

    CURL_TRC_WS(data, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    যদি(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    যদি(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }

    যদি(ws->enc.payload_remain) {
      failf(data, "[WS] previous frame not finished");
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }

    result = ws_enc_write_head(data, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    যদি(result)
      CURL_TRC_WS(data, "curl_start_frame(), error adding frame head %d",
                  (পূর্ণাঙ্ক)result);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  প্রত্যাগম result;
}

ধ্রুব সংরচনা Curl_protocol Curl_protocol_ws = {
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

#অন্যথা

CURLcode curl_ws_recv(CURL *curl, শূন্য *buffer, size_t buflen,
                      size_t *recv,
                      ধ্রুব সংরচনা curl_ws_frame **metap)
{
  (শূন্য)curl;
  (শূন্য)buffer;
  (শূন্য)buflen;
  (শূন্য)recv;
  (শূন্য)metap;
  প্রত্যাগম CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, ধ্রুব শূন্য *buffer,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned পূর্ণাঙ্ক flags)
{
  (শূন্য)curl;
  (শূন্য)buffer;
  (শূন্য)buflen;
  (শূন্য)sent;
  (শূন্য)fragsize;
  (শূন্য)flags;
  প্রত্যাগম CURLE_NOT_BUILT_IN;
}

ধ্রুব সংরচনা curl_ws_frame *curl_ws_meta(CURL *data)
{
  (শূন্য)data;
  প্রত্যাগম রিক্ত;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned পূর্ণাঙ্ক flags,
                                         curl_off_t frame_len)
{
  (শূন্য)curl;
  (শূন্য)flags;
  (শূন্য)frame_len;
  প্রত্যাগম CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
