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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS

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
#تعریف_کرو WSBIT_FIN          0x80
#تعریف_کرو WSBIT_RSV1         0x40
#تعریف_کرو WSBIT_RSV2         0x20
#تعریف_کرو WSBIT_RSV3         0x10
#تعریف_کرو WSBIT_RSV_نقاب     (WSBIT_RSV1 | WSBIT_RSV2 | WSBIT_RSV3)
#تعریف_کرو WSBIT_OPCODE_CONT  0x0
#تعریف_کرو WSBIT_OPCODE_متن  0x1
#تعریف_کرو WSBIT_OPCODE_BIN   0x2
#تعریف_کرو WSBIT_OPCODE_CLOSE 0x8
#تعریف_کرو WSBIT_OPCODE_PING  0x9
#تعریف_کرو WSBIT_OPCODE_PONG  0xa
#اگر_تعریف_شدہ CURLVERBOSE
#تعریف_کرو WSBIT_OPCODE_نقاب  0xf
#اگر_ختم

#تعریف_کرو WSBIT_نقاب 0x80

/* buffer dimensioning */
#تعریف_کرو WS_CHUNK_سائز  65535
#تعریف_کرو WS_CHUNK_گنتی 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
شمار ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_سر,
  WS_DEC_PAYLOAD
};

ساخت ws_decoder {
  صحیح چوکھٹا_age;        /* zero */
  صحیح چوکھٹا_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_انحراف;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t سر[10];
  صحیح سر_len, سر_کل;
  شمار ws_dec_state state;
  صحیح cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
ساخت ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  غیرعلامتی صحیح xori; /* xor index */
  uint8_t نقاب[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  ثنائی_ہندسہ(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#تعریف_کرو WS_MAX_CNTRL_LEN    125

ساخت ws_cntrl_چوکھٹا {
  غیرعلامتی صحیح type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
ساخت websocket {
  ساخت Curl_easy *ڈیٹا; /* used for write callback handling */
  ساخت ws_decoder dec;  /* decode of ws frames */
  ساخت ws_encoder enc;  /* encode of ws frames */
  ساخت bufq recvbuf;    /* raw data from the server */
  ساخت bufq sendbuf;    /* raw data to be sent to the server */
  ساخت curl_ws_چوکھٹا recvframe;  /* the current WS FRAME received */
  ساخت ws_cntrl_چوکھٹا pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  ثنائی_ہندسہ(using_reader);      /* sending WS data is handled via a client reader */
};

#اگر_تعریف_شدہ CURLVERBOSE
ساکن ثابت حرف *ws_چوکھٹا_name_کا_op(uint8_t firstbyte)
{
  انتخاب(firstbyte & WSBIT_OPCODE_نقاب) {
  حالت WSBIT_OPCODE_CONT:
    واپس "CONT";
  حالت WSBIT_OPCODE_متن:
    واپس "TEXT";
  حالت WSBIT_OPCODE_BIN:
    واپس "BIN";
  حالت WSBIT_OPCODE_CLOSE:
    واپس "CLOSE";
  حالت WSBIT_OPCODE_PING:
    واپس "PING";
  حالت WSBIT_OPCODE_PONG:
    واپس "PONG";
  طےشدہ:
    واپس "???";
  }
}
#اگر_ختم

ساکن صحیح ws_چوکھٹا_firstbyte2flags(ساخت Curl_easy *ڈیٹا,
                                    uint8_t firstbyte, صحیح cont_flags)
{
  انتخاب(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  حالت WSBIT_OPCODE_CONT:
    اگر(!(cont_flags & CURLWS_CONT)) {
      failf(ڈیٹا, "[WS] no ongoing fragmented message to resume");
      واپس 0;
    }
    واپس cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  حالت (WSBIT_OPCODE_CONT | WSBIT_FIN):
    اگر(!(cont_flags & CURLWS_CONT)) {
      failf(ڈیٹا, "[WS] no ongoing fragmented message to resume");
      واپس 0;
    }
    واپس cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  حالت WSBIT_OPCODE_متن:
    اگر(cont_flags & CURLWS_CONT) {
      failf(ڈیٹا, "[WS] fragmented message interrupted by new TEXT msg");
      واپس 0;
    }
    واپس CURLWS_متن | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  حالت (WSBIT_OPCODE_متن | WSBIT_FIN):
    اگر(cont_flags & CURLWS_CONT) {
      failf(ڈیٹا, "[WS] fragmented message interrupted by new TEXT msg");
      واپس 0;
    }
    واپس CURLWS_متن;
  /* 0x02 - first BINARY fragment */
  حالت WSBIT_OPCODE_BIN:
    اگر(cont_flags & CURLWS_CONT) {
      failf(ڈیٹا, "[WS] fragmented message interrupted by new BINARY msg");
      واپس 0;
    }
    واپس CURLWS_ثنائی | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  حالت (WSBIT_OPCODE_BIN | WSBIT_FIN):
    اگر(cont_flags & CURLWS_CONT) {
      failf(ڈیٹا, "[WS] fragmented message interrupted by new BINARY msg");
      واپس 0;
    }
    واپس CURLWS_ثنائی;
  /* 0x08 - first CLOSE fragment */
  حالت WSBIT_OPCODE_CLOSE:
    failf(ڈیٹا, "[WS] invalid fragmented CLOSE frame");
    واپس 0;
  /* 0x88 - unfragmented CLOSE */
  حالت (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    واپس CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  حالت WSBIT_OPCODE_PING:
    failf(ڈیٹا, "[WS] invalid fragmented PING frame");
    واپس 0;
  /* 0x89 - unfragmented PING */
  حالت (WSBIT_OPCODE_PING | WSBIT_FIN):
    واپس CURLWS_PING;
  /* 0x0a - first PONG fragment */
  حالت WSBIT_OPCODE_PONG:
    failf(ڈیٹا, "[WS] invalid fragmented PONG frame");
    واپس 0;
  /* 0x8a - unfragmented PONG */
  حالت (WSBIT_OPCODE_PONG | WSBIT_FIN):
    واپس CURLWS_PONG;
  /* invalid first byte */
  طےشدہ:
    اگر(firstbyte & WSBIT_RSV_نقاب)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(ڈیٹا, "[WS] invalid reserved bits: %02x", firstbyte);
    ورنہ
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(ڈیٹا, "[WS] invalid opcode: %02x", firstbyte);
    واپس 0;
  }
}

ساکن CURLcode ws_چوکھٹا_flags2firstbyte(ساخت Curl_easy *ڈیٹا,
                                         غیرعلامتی صحیح flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  انتخاب(flags & ~CURLWS_انحراف) {
  حالت 0:
    اگر(contfragment) {
      CURL_TRC_WS(ڈیٹا, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      واپس CURLE_OK;
    }
    failf(ڈیٹا, "[WS] no flags given");
    واپس CURLE_BAD_FUNCTION_دلیل;
  حالت CURLWS_CONT:
    اگر(contfragment) {
      infof(ڈیٹا, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      واپس CURLE_OK;
    }
    failf(ڈیٹا, "[WS] No ongoing fragmented message to continue");
    واپس CURLE_BAD_FUNCTION_دلیل;
  حالت CURLWS_متن:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_متن | WSBIT_FIN);
    واپس CURLE_OK;
  حالت (CURLWS_متن | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_متن;
    واپس CURLE_OK;
  حالت CURLWS_ثنائی:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    واپس CURLE_OK;
  حالت (CURLWS_ثنائی | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    واپس CURLE_OK;
  حالت CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    واپس CURLE_OK;
  حالت (CURLWS_CLOSE | CURLWS_CONT):
    failf(ڈیٹا, "[WS] CLOSE frame must not be fragmented");
    واپس CURLE_BAD_FUNCTION_دلیل;
  حالت CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    واپس CURLE_OK;
  حالت (CURLWS_PING | CURLWS_CONT):
    failf(ڈیٹا, "[WS] PING frame must not be fragmented");
    واپس CURLE_BAD_FUNCTION_دلیل;
  حالت CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    واپس CURLE_OK;
  حالت (CURLWS_PONG | CURLWS_CONT):
    failf(ڈیٹا, "[WS] PONG frame must not be fragmented");
    واپس CURLE_BAD_FUNCTION_دلیل;
  طےشدہ:
    failf(ڈیٹا, "[WS] unknown flags: %x", flags);
    واپس CURLE_BAD_FUNCTION_دلیل;
  }
}

ساکن خالی ws_dec_info(ساخت ws_decoder *dec, ساخت Curl_easy *ڈیٹا,
                        ثابت حرف *پیغام)
{
  NOVERBOSE((خالی)پیغام);
  انتخاب(dec->سر_len) {
  حالت 0:
    توڑو;
  حالت 1:
    CURL_TRC_WS(ڈیٹا, "decoded %s [%s%s]", پیغام,
                ws_چوکھٹا_name_کا_op(dec->سر[0]),
                (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    توڑو;
  طےشدہ:
    اگر(dec->سر_len < dec->سر_کل) {
      CURL_TRC_WS(ڈیٹا, "decoded %s [%s%s](%d/%d)", پیغام,
                  ws_چوکھٹا_name_کا_op(dec->سر[0]),
                  (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->سر_len, dec->سر_کل);
    }
    ورنہ {
      CURL_TRC_WS(ڈیٹا, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  پیغام, ws_چوکھٹا_name_کا_op(dec->سر[0]),
                  (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_انحراف, dec->payload_len);
    }
    توڑو;
  }
}

ساکن CURLcode ws_send_raw_blocking(ساخت Curl_easy *ڈیٹا,
                                     ساخت websocket *ws,
                                     ثابت حرف *بفر, size_t buflen);

قسمتعریف CURLcode ws_write_payload(ثابت uint8_t *buf, size_t buflen,
                                  صحیح چوکھٹا_age, صحیح چوکھٹا_flags,
                                  curl_off_t payload_انحراف,
                                  curl_off_t payload_len,
                                  خالی *userp,
                                  size_t *pnwritten);

ساکن خالی ws_dec_اگلا_چوکھٹا(ساخت ws_decoder *dec)
{
  dec->چوکھٹا_age = 0;
  dec->چوکھٹا_flags = 0;
  dec->payload_انحراف = 0;
  dec->payload_len = 0;
  dec->سر_len = dec->سر_کل = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ساکن خالی ws_dec_reset(ساخت ws_decoder *dec)
{
  dec->چوکھٹا_age = 0;
  dec->چوکھٹا_flags = 0;
  dec->payload_انحراف = 0;
  dec->payload_len = 0;
  dec->سر_len = dec->سر_کل = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

ساکن خالی ws_dec_init(ساخت ws_decoder *dec)
{
  ws_dec_reset(dec);
}

ساکن CURLcode ws_dec_read_سر(ساخت ws_decoder *dec,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت bufq *inraw)
{
  ثابت uint8_t *inbuf;
  size_t inlen;

  جب(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    اگر(dec->سر_len == 0) {
      dec->سر[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->چوکھٹا_flags = ws_چوکھٹا_firstbyte2flags(ڈیٹا, dec->سر[0],
                                                  dec->cont_flags);
      اگر(!dec->چوکھٹا_flags) {
        ws_dec_reset(dec);
        واپس CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      اگر(dec->چوکھٹا_flags & (CURLWS_متن | CURLWS_ثنائی)) {
        dec->cont_flags = dec->چوکھٹا_flags;
      }

      dec->سر_len = 1;
#اگر 0
      ws_dec_info(dec, ڈیٹا, "seeing opcode");
#اگر_ختم
      جاری;
    }
    ورنہ اگر(dec->سر_len == 1) {
      dec->سر[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->سر_len = 2;

      اگر(dec->سر[1] & WSBIT_نقاب) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(ڈیٹا, "[WS] masked input frame");
        ws_dec_reset(dec);
        واپس CURLE_RECV_ERROR;
      }
      اگر(dec->چوکھٹا_flags & CURLWS_PING && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(ڈیٹا, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        واپس CURLE_RECV_ERROR;
      }
      اگر(dec->چوکھٹا_flags & CURLWS_PONG && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(ڈیٹا, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        واپس CURLE_RECV_ERROR;
      }
      اگر(dec->چوکھٹا_flags & CURLWS_CLOSE && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        failf(ڈیٹا, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        واپس CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      اگر(dec->سر[1] == 126) {
        dec->سر_کل = 4;
        جاری;
      }
      ورنہ اگر(dec->سر[1] == 127) {
        dec->سر_کل = 10;
        جاری;
      }
      ورنہ {
        dec->سر_کل = 2;
      }
    }

    اگر(dec->سر_len < dec->سر_کل) {
      dec->سر[dec->سر_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->سر_len;
      اگر(dec->سر_len < dec->سر_کل) {
#اگر 0
        ws_dec_info(dec, ڈیٹا, "decoding head");
#اگر_ختم
        جاری;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->سر_len == dec->سر_کل);
    انتخاب(dec->سر_کل) {
    حالت 2:
      dec->payload_len = dec->سر[1];
      توڑو;
    حالت 4:
      dec->payload_len = (dec->سر[2] << 8) | dec->سر[3];
      توڑو;
    حالت 10:
      اگر(dec->سر[2] > 127) {
        failf(ڈیٹا, "[WS] frame length longer than 63 bits not supported");
        واپس CURLE_RECV_ERROR;
      }
      dec->payload_len =
        (curl_off_t)dec->سر[2] << 56 |
        (curl_off_t)dec->سر[3] << 48 |
        (curl_off_t)dec->سر[4] << 40 |
        (curl_off_t)dec->سر[5] << 32 |
        (curl_off_t)dec->سر[6] << 24 |
        (curl_off_t)dec->سر[7] << 16 |
        (curl_off_t)dec->سر[8] <<  8 |
        dec->سر[9];
      توڑو;
    طےشدہ:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(ڈیٹا, "[WS] unexpected frame header length");
      واپس CURLE_RECV_ERROR;
    }

    dec->چوکھٹا_age = 0;
    dec->payload_انحراف = 0;
    ws_dec_info(dec, ڈیٹا, "head");
    واپس CURLE_OK;
  }
  واپس CURLE_AGAIN;
}

ساکن CURLcode ws_dec_pass_payload(ساخت ws_decoder *dec,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت bufq *inraw,
                                    ws_write_payload *write_cb,
                                    خالی *write_ctx)
{
  ثابت uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode نتیجہ;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_انحراف,
                                     0, SIZE_MAX);

  جب(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(ڈیٹا)) {
    اگر(inlen > remain)
      inlen = remain;
    نتیجہ = write_cb(inbuf, inlen, dec->چوکھٹا_age, dec->چوکھٹا_flags,
                      dec->payload_انحراف, dec->payload_len,
                      write_ctx, &nwritten);
    اگر(نتیجہ)
      واپس نتیجہ;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_انحراف += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_انحراف,
                                0, SIZE_MAX);
    CURL_TRC_WS(ڈیٹا, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  واپس remain ? CURLE_AGAIN : CURLE_OK;
}

ساکن CURLcode ws_dec_pass(ساخت ws_decoder *dec,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت bufq *inraw,
                            ws_write_payload *write_cb,
                            خالی *write_ctx)
{
  CURLcode نتیجہ;

  اگر(Curl_bufq_is_خالی_خانے(inraw))
    واپس CURLE_AGAIN;

  انتخاب(dec->state) {
  حالت WS_DEC_INIT:
    ws_dec_اگلا_چوکھٹا(dec);
    dec->state = WS_DEC_سر;
    FALLTHROUGH();
  حالت WS_DEC_سر:
    نتیجہ = ws_dec_read_سر(dec, ڈیٹا, inraw);
    اگر(نتیجہ) {
      اگر(نتیجہ != CURLE_AGAIN) {
        failf(ڈیٹا, "[WS] decode frame error %d", (صحیح)نتیجہ);
        توڑو;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_خالی_خانے(inraw));
      توڑو;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    اگر(dec->payload_len == 0) {
      size_t nwritten;
      ثابت uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      نتیجہ = write_cb(&tmp, 0, dec->چوکھٹا_age, dec->چوکھٹا_flags,
                        0, 0, write_ctx, &nwritten);
      اگر(نتیجہ)
        واپس نتیجہ;
      dec->state = WS_DEC_INIT;
      توڑو;
    }
    FALLTHROUGH();
  حالت WS_DEC_PAYLOAD:
    نتیجہ = ws_dec_pass_payload(dec, ڈیٹا, inraw, write_cb, write_ctx);
    ws_dec_info(dec, ڈیٹا, "passing");
    اگر(نتیجہ)
      واپس نتیجہ;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    توڑو;
  طےشدہ:
    /* we covered all enums above, but some code analyzers are wimps */
    نتیجہ = CURLE_FAILED_INIT;
  }
  واپس نتیجہ;
}

ساکن خالی update_meta(ساخت websocket *ws,
                        صحیح چوکھٹا_age, صحیح چوکھٹا_flags,
                        curl_off_t payload_انحراف,
                        curl_off_t payload_len,
                        size_t cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_انحراف - cur_len);

  ws->recvframe.age = چوکھٹا_age;
  ws->recvframe.flags = چوکھٹا_flags;
  ws->recvframe.انحراف = payload_انحراف;
  ws->recvframe.len = cur_len;
  ws->recvframe.bytesleft = bytesleft;
}

/* WebSocket decoding client writer */
ساخت ws_cw_ctx {
  ساخت Curl_cwriter super;
  ساخت bufq buf;
};

ساکن CURLcode ws_cw_init(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_cwriter *writer)
{
  ساخت ws_cw_ctx *ctx = writer->ctx;
  (خالی)ڈیٹا;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_سائز, 1, BUFQ_OPT_SOFT_حد_بندی);
  واپس CURLE_OK;
}

ساکن خالی ws_cw_close(ساخت Curl_easy *ڈیٹا, ساخت Curl_cwriter *writer)
{
  ساخت ws_cw_ctx *ctx = writer->ctx;
  (خالی)ڈیٹا;
  Curl_bufq_free(&ctx->buf);
}

ساخت ws_cw_dec_ctx {
  ساخت Curl_easy *ڈیٹا;
  ساخت websocket *ws;
  ساخت Curl_cwriter *اگلا_writer;
  صحیح cw_type;
};

ساکن CURLcode ws_flush(ساخت Curl_easy *ڈیٹا, ساخت websocket *ws,
                         bool blocking);
ساکن CURLcode ws_enc_جوڑو_pending(ساخت Curl_easy *ڈیٹا,
                                   ساخت websocket *ws);

ساکن CURLcode ws_enc_جوڑو_cntrl(ساخت Curl_easy *ڈیٹا,
                                 ساخت websocket *ws,
                                 ثابت uint8_t *payload,
                                 size_t plen,
                                 غیرعلامتی صحیح چوکھٹا_type)
{
  (خالی)ڈیٹا;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  اگر(plen > WS_MAX_CNTRL_LEN)
    واپس CURLE_BAD_FUNCTION_دلیل;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = چوکھٹا_type;
  ws->pending.payload_len = plen;
  حافظہ_نقل(ws->pending.payload, payload, plen);
  واپس CURLE_OK;
}

ساکن curl_off_t ws_payload_remain(curl_off_t payload_کل,
                                    curl_off_t payload_انحراف,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_کل - payload_انحراف;
  اگر((payload_کل < 0) || (payload_انحراف < 0) || (remain < 0))
    واپس -1;
  buffered = curlx_uztoso(payload_buffered);
  اگر(remain < buffered)
    واپس -1;
  واپس remain - buffered;
}

ساکن CURLcode ws_cw_dec_اگلا(ثابت uint8_t *buf, size_t buflen,
                               صحیح چوکھٹا_age, صحیح چوکھٹا_flags,
                               curl_off_t payload_انحراف,
                               curl_off_t payload_len,
                               خالی *user_ڈیٹا,
                               size_t *pnwritten)
{
  ساخت ws_cw_dec_ctx *ctx = user_ڈیٹا;
  ساخت Curl_easy *ڈیٹا = ctx->ڈیٹا;
  ساخت websocket *ws = ctx->ws;
  bool auto_pong = !ڈیٹا->سیٹ.ws_raw_منوال && !ڈیٹا->سیٹ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode نتیجہ;

  (خالی)چوکھٹا_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_انحراف, buflen);
  اگر(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  اگر(auto_pong && (چوکھٹا_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(ڈیٹا, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_انحراف, payload_len);
    /* send back the exact same content as a PONG */
    نتیجہ = ws_enc_جوڑو_cntrl(ڈیٹا, ws, buf, buflen, CURLWS_PONG);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  ورنہ اگر(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, چوکھٹا_age, چوکھٹا_flags, payload_انحراف,
                payload_len, buflen);

    CURL_TRC_WRITE(ڈیٹا, "[WS] pass %zu decoded bytes", buflen);
    نتیجہ = Curl_cwriter_write(ڈیٹا, ctx->اگلا_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ثابت حرف *)buf, buflen);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  *pnwritten = buflen;
  واپس CURLE_OK;
}

ساکن CURLcode ws_cw_write(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_cwriter *writer, صحیح type,
                            ثابت حرف *buf, size_t nbytes)
{
  ساخت ws_cw_ctx *ctx = writer->ctx;
  ساخت websocket *ws;
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_WRITE(ڈیٹا, "[WS] write(len=%zu, type=%d)", nbytes, type);
  اگر(!(type & CLIENTWRITE_BODY) || ڈیٹا->سیٹ.ws_raw_منوال)
    واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, nbytes);

  ws = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_PROTO_WS_CONN);
  اگر(!ws) {
    failf(ڈیٹا, "[WS] not a websocket transfer");
    واپس CURLE_FAILED_INIT;
  }

  اگر(nbytes) {
    size_t nwritten;
    نتیجہ = Curl_bufq_write(&ctx->buf, (ثابت uint8_t *)buf,
                             nbytes, &nwritten);
    اگر(نتیجہ) {
      infof(ڈیٹا, "[WS] error adding data to buffer %d", (صحیح)نتیجہ);
      واپس نتیجہ;
    }
  }

  نتیجہ = Curl_cwriter_flush(ڈیٹا, writer->اگلا);
  اگر(نتیجہ)
    جاؤ برآمد;

  جب(!Curl_bufq_is_خالی_خانے(&ctx->buf) && !Curl_cwriter_is_paused(ڈیٹا)) {
    ساخت ws_cw_dec_ctx pass_ctx;
    pass_ctx.ڈیٹا = ڈیٹا;
    pass_ctx.ws = ws;
    pass_ctx.اگلا_writer = writer->اگلا;
    pass_ctx.cw_type = type;
    نتیجہ = ws_dec_pass(&ws->dec, ڈیٹا, &ctx->buf,
                         ws_cw_dec_اگلا, &pass_ctx);
    اگر(نتیجہ == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      نتیجہ = CURLE_OK;
      جاؤ برآمد;
    }
    ورنہ اگر(نتیجہ) {
      failf(ڈیٹا, "[WS] decode payload error %d", (صحیح)نتیجہ);
      Curl_bufq_reset(&ctx->buf);
      جاؤ برآمد;
    }
  }

برآمد:
  اگر(!نتیجہ && !ws->using_reader) {
    نتیجہ = ws_flush(ڈیٹا, ws, Curl_api_is_اندر_callback(ڈیٹا));
    اگر(نتیجہ == CURLE_AGAIN)
      نتیجہ = CURLE_OK;
  }

  اگر(!نتیجہ && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_خالی_خانے(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(ڈیٹا, "[WS] decode ended with incomplete frame");
    نتیجہ = CURLE_RECV_ERROR;
  }
  واپس نتیجہ;
}

ساکن CURLcode ws_cw_flush(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_cwriter *writer)
{
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_WRITE(ڈیٹا, "[ws] flush");
  اگر(!ڈیٹا->سیٹ.ws_raw_منوال) {
    ساخت ws_cw_ctx *ctx = writer->ctx;
    ساخت websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    نتیجہ = Curl_cwriter_flush(ڈیٹا, writer->اگلا);
    اگر(نتیجہ)
      جاؤ برآمد;

    ws = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_PROTO_WS_CONN);
    اگر(!ws) {
      failf(ڈیٹا, "[WS] not a websocket transfer");
      واپس CURLE_FAILED_INIT;
    }

    جب(!Curl_bufq_is_خالی_خانے(&ctx->buf) && !Curl_cwriter_is_paused(ڈیٹا)) {
      ساخت ws_cw_dec_ctx pass_ctx;
      pass_ctx.ڈیٹا = ڈیٹا;
      pass_ctx.ws = ws;
      pass_ctx.اگلا_writer = writer->اگلا;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      نتیجہ = ws_dec_pass(&ws->dec, ڈیٹا, &ctx->buf,
                           ws_cw_dec_اگلا, &pass_ctx);
      اگر(نتیجہ == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        نتیجہ = CURLE_OK;
        جاؤ برآمد;
      }
      ورنہ اگر(نتیجہ) {
        failf(ڈیٹا, "[WS] decode payload error %d", (صحیح)نتیجہ);
        Curl_bufq_reset(&ctx->buf);
        جاؤ برآمد;
      }
    }
  }

برآمد:
  اگر(!نتیجہ)
    نتیجہ = Curl_cwriter_flush(ڈیٹا, writer->اگلا);
  واپس نتیجہ;
}

/* WebSocket payload decoding client writer. */
ساکن ثابت ساخت Curl_cwtype ws_cw_decode = {
  "ws-decode",
  تہی,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  حجم(ساخت ws_cw_ctx)
};

ساکن خالی ws_enc_info(ساخت ws_encoder *enc, ساخت Curl_easy *ڈیٹا,
                        ثابت حرف *پیغام)
{
  NOVERBOSE((خالی)enc);
  NOVERBOSE((خالی)پیغام);
  CURL_TRC_WS(ڈیٹا, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              پیغام, ws_چوکھٹا_name_کا_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

ساکن خالی ws_enc_reset(ساخت ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

ساکن خالی ws_enc_init(ساخت ws_encoder *enc)
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

ساکن CURLcode ws_enc_جوڑو_چوکھٹا(ساخت Curl_easy *ڈیٹا,
                                 ساخت ws_encoder *enc,
                                 غیرعلامتی صحیح flags,
                                 curl_off_t payload_len,
                                 ساخت bufq *برآمد)
{
  uint8_t firstb = 0;
  uint8_t سر[14];
  CURLcode نتیجہ;
  size_t hlen, nwritten;

  اگر(payload_len < 0) {
    failf(ڈیٹا, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    واپس CURLE_SEND_ERROR;
  }

  اگر(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(ڈیٹا, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    واپس CURLE_SEND_ERROR;
  }

  نتیجہ = ws_چوکھٹا_flags2firstbyte(ڈیٹا, flags, (bool)enc->contfragment,
                                    &firstb);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  اگر(flags & (CURLWS_متن | CURLWS_ثنائی)) {
    enc->contfragment = (curl_ثنائی_ہندسہ)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  اگر(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(ڈیٹا, "[WS] given PING frame is too big");
    واپس CURLE_TOO_LARGE;
  }
  اگر(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(ڈیٹا, "[WS] given PONG frame is too big");
    واپس CURLE_TOO_LARGE;
  }
  اگر(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(ڈیٹا, "[WS] given CLOSE frame is too big");
    واپس CURLE_TOO_LARGE;
  }

  سر[0] = enc->firstbyte = firstb;
  اگر(payload_len > 65535) {
    سر[1] = 127 | WSBIT_نقاب;
    سر[2] = (uint8_t)((payload_len >> 56) & 0xff);
    سر[3] = (uint8_t)((payload_len >> 48) & 0xff);
    سر[4] = (uint8_t)((payload_len >> 40) & 0xff);
    سر[5] = (uint8_t)((payload_len >> 32) & 0xff);
    سر[6] = (uint8_t)((payload_len >> 24) & 0xff);
    سر[7] = (uint8_t)((payload_len >> 16) & 0xff);
    سر[8] = (uint8_t)((payload_len >> 8) & 0xff);
    سر[9] = (uint8_t)(payload_len & 0xff);
    hlen = 10;
  }
  ورنہ اگر(payload_len >= 126) {
    سر[1] = 126 | WSBIT_نقاب;
    سر[2] = (uint8_t)((payload_len >> 8) & 0xff);
    سر[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  ورنہ {
    سر[1] = (uint8_t)payload_len | WSBIT_نقاب;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, ڈیٹا, "sending");

  /* 4 bytes random */

  نتیجہ = Curl_rand(ڈیٹا, (uint8_t *)&enc->نقاب, حجم(enc->نقاب));
  اگر(نتیجہ)
    واپس نتیجہ;

#اگر_تعریف_شدہ DEBUGBUILD
  اگر(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->نقاب, 0, حجم(enc->نقاب));
#اگر_ختم

  /* add 4 bytes mask */
  حافظہ_نقل(&سر[hlen], &enc->نقاب, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  نتیجہ = Curl_bufq_write(برآمد, سر, hlen, &nwritten);
  اگر(نتیجہ)
    واپس نتیجہ;
  اگر(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    واپس CURLE_SEND_ERROR;
  }
  واپس CURLE_OK;
}

ساکن CURLcode ws_enc_write_سر(ساخت Curl_easy *ڈیٹا,
                                  ساخت websocket *ws,
                                  ساخت ws_encoder *enc,
                                  غیرعلامتی صحیح flags,
                                  curl_off_t payload_len,
                                  ساخت bufq *برآمد)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  اگر(ws->pending.type) {
    CURLcode نتیجہ = ws_enc_جوڑو_pending(ڈیٹا, ws);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  واپس ws_enc_جوڑو_چوکھٹا(ڈیٹا, enc, flags, payload_len, برآمد);
}

ساکن CURLcode ws_enc_write_payload(ساخت ws_encoder *enc,
                                     ساخت Curl_easy *ڈیٹا,
                                     ثابت uint8_t *buf, size_t buflen,
                                     ساخت bufq *برآمد, size_t *pnwritten)
{
  CURLcode نتیجہ;
  size_t i, len, n, remain;

  *pnwritten = 0;
  اگر(Curl_bufq_is_بھرا(برآمد))
    واپس CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  اگر(remain < len)
    len = remain;

  برائے(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->نقاب[enc->xori];
    نتیجہ = Curl_bufq_write(برآمد, &c, 1, &n);
    اگر(نتیجہ) {
      اگر((نتیجہ != CURLE_AGAIN) || !i)
        واپس نتیجہ;
      توڑو;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, ڈیٹا, "buffered");
  واپس CURLE_OK;
}

ساکن CURLcode ws_enc_جوڑو_pending(ساخت Curl_easy *ڈیٹا,
                                   ساخت websocket *ws)
{
  CURLcode نتیجہ;
  size_t n;

  اگر(!ws->pending.type) /* no pending frame here */
    واپس CURLE_OK;
  اگر(ws->enc.payload_remain) /* in the middle of another frame */
    واپس CURLE_AGAIN;

  نتیجہ = ws_enc_جوڑو_چوکھٹا(ڈیٹا, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  اگر(نتیجہ) {
    CURL_TRC_WS(ڈیٹا, "ws_enc_cntrl(), error adding head: %d",
                (صحیح)نتیجہ);
    جاؤ برآمد;
  }
  نتیجہ = ws_enc_write_payload(&ws->enc, ڈیٹا, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  اگر(نتیجہ) {
    CURL_TRC_WS(ڈیٹا, "ws_enc_cntrl(), error adding payload: %d",
                (صحیح)نتیجہ);
    جاؤ برآمد;
  }
  اگر(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(ڈیٹا, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, حجم(ws->pending));

برآمد:
  واپس نتیجہ;
}

ساکن CURLcode ws_enc_send(ساخت Curl_easy *ڈیٹا,
                            ساخت websocket *ws,
                            ثابت uint8_t *بفر,
                            size_t buflen,
                            curl_off_t fragsize,
                            غیرعلامتی صحیح flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(!ڈیٹا->سیٹ.ws_raw_منوال);
  *pnsent = 0;

  اگر(ws->enc.payload_remain || !Curl_bufq_is_خالی_خانے(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    اگر(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(ڈیٹا, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
    اگر((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(ڈیٹا, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
  }
  ورنہ {
    نتیجہ = ws_flush(ڈیٹا, ws, Curl_api_is_اندر_callback(ڈیٹا));
    اگر(نتیجہ)
      واپس نتیجہ;

    نتیجہ = ws_enc_write_سر(ڈیٹا, ws, &ws->enc, flags,
                               (flags & CURLWS_انحراف) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    اگر(نتیجہ) {
      CURL_TRC_WS(ڈیٹا, "curl_ws_send(), error writing frame head %d",
                  (صحیح)نتیجہ);
      واپس نتیجہ;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  جب(!Curl_bufq_is_خالی_خانے(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    اگر(buflen > ws->sendbuf_payload) {
      size_t پچھلا_len = Curl_bufq_len(&ws->sendbuf);
      نتیجہ = ws_enc_write_payload(&ws->enc, ڈیٹا,
                                    بفر + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      اگر(نتیجہ && (نتیجہ != CURLE_AGAIN))
        واپس نتیجہ;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - پچھلا_len;
      اگر(!ws->sendbuf_payload) {
        واپس CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    نتیجہ = ws_flush(ڈیٹا, ws, Curl_api_is_اندر_callback(ڈیٹا));
    اگر(!نتیجہ && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      بفر += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    ورنہ اگر(نتیجہ == CURLE_AGAIN) {
      اگر(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        واپس CURLE_OK;
      }
      ورنہ {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(ڈیٹا, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        واپس CURLE_AGAIN;
      }
    }
    ورنہ
      واپس نتیجہ;  /* real error sending the data */
  }
  واپس CURLE_OK;
}

ساخت cr_ws_ctx {
  ساخت Curl_creader super;
  ثنائی_ہندسہ(read_eos);  /* we read an EOS from the next reader */
  ثنائی_ہندسہ(eos);       /* we have returned an EOS */
};

ساکن CURLcode cr_ws_init(ساخت Curl_easy *ڈیٹا, ساخت Curl_creader *reader)
{
  (خالی)ڈیٹا;
  (خالی)reader;
  واپس CURLE_OK;
}

ساکن خالی cr_ws_close(ساخت Curl_easy *ڈیٹا, ساخت Curl_creader *reader)
{
  (خالی)ڈیٹا;
  (خالی)reader;
}

ساکن CURLcode cr_ws_read(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ساخت cr_ws_ctx *ctx = reader->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread, n;
  ساخت websocket *ws;
  bool eos;

  *pnread = 0;
  اگر(ctx->eos) {
    *peos = TRUE;
    واپس CURLE_OK;
  }

  ws = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_PROTO_WS_CONN);
  اگر(!ws) {
    failf(ڈیٹا, "[WS] not a websocket transfer");
    واپس CURLE_FAILED_INIT;
  }

  اگر(Curl_bufq_is_خالی_خانے(&ws->sendbuf)) {
    اگر(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      واپس CURLE_OK;
    }

    اگر(ws->enc.payload_remain) {
      CURL_TRC_WS(ڈیٹا, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    نتیجہ = Curl_creader_read(ڈیٹا, reader->اگلا, buf, blen, &nread, &eos);
    اگر(نتیجہ)
      واپس نتیجہ;
    ctx->read_eos = eos;

    اگر(!Curl_bufq_is_خالی_خانے(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(ڈیٹا, reader->اگلا);
    }
    ورنہ اگر(!nread) {
      /* nothing to convert, return this right away */
      اگر(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      جاؤ برآمد;
    }

    اگر(!ws->enc.payload_remain && Curl_bufq_is_خالی_خانے(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      نتیجہ = ws_enc_write_سر(ڈیٹا, ws, &ws->enc, CURLWS_ثنائی, nread,
                                 &ws->sendbuf);
      اگر(نتیجہ)
        جاؤ برآمد;
    }

    نتیجہ = ws_enc_write_payload(&ws->enc, ڈیٹا, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    اگر(نتیجہ)
      جاؤ برآمد;
    CURL_TRC_READ(ڈیٹا, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_خالی_خانے(&ws->sendbuf));
  *peos = FALSE;
  نتیجہ = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  اگر(!نتیجہ && ctx->read_eos && Curl_bufq_is_خالی_خانے(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

برآمد:
  CURL_TRC_READ(ڈیٹا, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحیح)نتیجہ, *pnread, *peos);
  واپس نتیجہ;
}

ساکن curl_off_t cr_ws_کل_لمبائی(ساخت Curl_easy *ڈیٹا,
                                     ساخت Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (خالی)ڈیٹا;
  (خالی)reader;
  واپس -1;
}

ساکن ثابت ساخت Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_کل_لمبائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_مکمل,
  حجم(ساخت cr_ws_ctx)
};

ساخت wsfield {
  ثابت حرف *name;
  ثابت حرف *val;
};

CURLcode Curl_ws_request(ساخت Curl_easy *ڈیٹا, ساخت dynbuf *req)
{
  غیرعلامتی صحیح i;
  CURLcode نتیجہ = CURLE_OK;
  uint8_t rand[16];
  حرف *randstr;
  size_t randlen;
  حرف keyval[40];
  ساخت SingleRequest *k = &ڈیٹا->req;
  ساخت wsfield heads[] = {
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
      "Sec-WebSocket-Key", تہی,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  نتیجہ = Curl_rand(ڈیٹا, rand, حجم(rand));
  اگر(نتیجہ)
    واپس نتیجہ;
  نتیجہ = curlx_base64_encode(rand, حجم(rand), &randstr, &randlen);
  اگر(نتیجہ)
    واپس نتیجہ;
  DEBUGASSERT(randlen < حجم(keyval));
  اگر(randlen >= حجم(keyval)) {
    curlx_free(randstr);
    واپس CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, حجم(keyval), randstr, randlen);
  curlx_free(randstr);
  برائے(i = 0; !نتیجہ && (i < CURL_ARRAYSIZE(heads)); i++) {
    اگر(!Curl_checkheaders(ڈیٹا, heads[i].name, لڑی_لمبائی(heads[i].name))) {
      نتیجہ = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  ڈیٹا->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  ڈیٹا->conn->بٹ.upgrade_اندر_پیش_رفت = TRUE;
  واپس نتیجہ;
}

ساکن خالی ws_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت websocket *ws = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(ساخت Curl_easy *ڈیٹا,
                        ثابت حرف *mem, size_t nread)
{
  ساخت SingleRequest *k = &ڈیٹا->req;
  ساخت websocket *ws;
  ساخت Curl_cwriter *ws_dec_writer = تہی;
  ساخت Curl_creader *ws_enc_reader = تہی;
  CURLcode نتیجہ;

  DEBUGASSERT(ڈیٹا->conn);
  ws = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_PROTO_WS_CONN);
  اگر(!ws) {
    size_t chunk_سائز = WS_CHUNK_سائز;
    ws = curlx_calloc(1, حجم(*ws));
    اگر(!ws)
      واپس CURLE_برآمد_کا_حافظہ;
#اگر_تعریف_شدہ DEBUGBUILD
    {
      ثابت حرف *p = getenv("CURL_WS_CHUNK_SIZE");
      اگر(p) {
        curl_off_t l;
        اگر(!curlx_str_نمبر(&p, &l, 1 * 1024 * 1024))
          chunk_سائز = (size_t)l;
      }
    }
#اگر_ختم
    CURL_TRC_WS(ڈیٹا, "WS, using chunk size %zu", chunk_سائز);
    Curl_bufq_init2(&ws->recvbuf, chunk_سائز, WS_CHUNK_گنتی,
                    BUFQ_OPT_SOFT_حد_بندی);
    Curl_bufq_init2(&ws->sendbuf, chunk_سائز, WS_CHUNK_گنتی,
                    BUFQ_OPT_SOFT_حد_بندی);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    نتیجہ = Curl_conn_meta_سیٹ(ڈیٹا->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  ورنہ {
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

  infof(ڈیٹا, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  نتیجہ = Curl_cwriter_create(&ws_dec_writer, ڈیٹا, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  اگر(نتیجہ)
    جاؤ برآمد;
  نتیجہ = Curl_cwriter_جوڑو(ڈیٹا, ws_dec_writer);
  اگر(نتیجہ)
    جاؤ برآمد;
  ws_dec_writer = تہی; /* owned by transfer now */

  k->سرخی = FALSE; /* we will not get more response headers */

  اگر(ڈیٹا->سیٹ.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    اگر(nread) {
      نتیجہ = Curl_cf_recvbuf_جوڑو(ڈیٹا, ڈیٹا->conn, FIRSTSOCKET,
                                   (ثابت uint8_t *)mem, nread);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
    CURL_REQ_CLEAR_RECV(ڈیٹا); /* read no more content */
  }
  ورنہ { /* !connect_only */
    اگر(ڈیٹا->سیٹ.طریقہ == HTTPREQ_PUT) {
      CURL_TRC_WS(ڈیٹا, "UPLOAD set, add ws-encode reader");
      نتیجہ = Curl_creader_سیٹ_fread(ڈیٹا, -1);
      اگر(نتیجہ)
        جاؤ برآمد;

      اگر(!ڈیٹا->سیٹ.ws_raw_منوال) {
        /* Add our client reader encoding WS BINARY frames */
        نتیجہ = Curl_creader_create(&ws_enc_reader, ڈیٹا, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        اگر(نتیجہ)
          جاؤ برآمد;
        نتیجہ = Curl_creader_جوڑو(ڈیٹا, ws_enc_reader);
        اگر(نتیجہ)
          جاؤ برآمد;
        ws_enc_reader = تہی; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      ڈیٹا->req.eos_read = FALSE;
      ڈیٹا->req.upload_مکمل = FALSE;
      CURL_REQ_سیٹ_SEND(ڈیٹا);
    }

    /* Then pass any additional data to the writers */
    اگر(nread) {
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, mem, nread);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->سرخی = FALSE; /* we will not get more responses */

برآمد:
  اگر(ws_dec_writer)
    Curl_cwriter_free(ڈیٹا, ws_dec_writer);
  اگر(ws_enc_reader)
    Curl_creader_free(ڈیٹا, ws_enc_reader);
  اگر(نتیجہ)
    CURL_TRC_WS(ڈیٹا, "Curl_ws_accept() failed -> %d", (صحیح)نتیجہ);
  ورنہ
    CURL_TRC_WS(ڈیٹا, "websocket established, %s mode",
                ڈیٹا->سیٹ.connect_only ? "connect-only" : "callback");
  واپس نتیجہ;
}

ساخت ws_collect {
  ساخت Curl_easy *ڈیٹا;
  ساخت websocket *ws;
  uint8_t *بفر;
  size_t buflen;
  size_t bufidx;
  صحیح چوکھٹا_age;
  صحیح چوکھٹا_flags;
  curl_off_t payload_انحراف;
  curl_off_t payload_len;
  bool written;
};

ساکن CURLcode ws_client_collect(ثابت uint8_t *buf, size_t buflen,
                                  صحیح چوکھٹا_age, صحیح چوکھٹا_flags,
                                  curl_off_t payload_انحراف,
                                  curl_off_t payload_len,
                                  خالی *userp,
                                  size_t *pnwritten)
{
  ساخت ws_collect *ctx = userp;
  ساخت Curl_easy *ڈیٹا = ctx->ڈیٹا;
  bool auto_pong = !ڈیٹا->سیٹ.ws_raw_منوال && !ڈیٹا->سیٹ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode نتیجہ = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_انحراف, buflen);
  اگر(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  اگر(!ctx->bufidx) {
    /* first write */
    ctx->چوکھٹا_age = چوکھٹا_age;
    ctx->چوکھٹا_flags = چوکھٹا_flags;
    ctx->payload_انحراف = payload_انحراف;
    ctx->payload_len = payload_len;
  }

  اگر(auto_pong && (چوکھٹا_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(ڈیٹا, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_انحراف, payload_len);
    /* send back the exact same content as a PONG */
    نتیجہ = ws_enc_جوڑو_cntrl(ctx->ڈیٹا, ctx->ws, buf, buflen, CURLWS_PONG);
    اگر(نتیجہ)
      واپس نتیجہ;
    *pnwritten = buflen;
  }
  ورنہ {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    اگر(!write_len) {
      اگر(!buflen)  /* 0 length write, we accept that */
        واپس CURLE_OK;
      واپس CURLE_AGAIN;  /* no more space */
    }
    حافظہ_نقل(ctx->بفر + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  واپس نتیجہ;
}

ساکن CURLcode nw_اندر_recv(خالی *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  ساخت Curl_easy *ڈیٹا = reader_ctx;
  واپس Curl_easy_recv(ڈیٹا, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, خالی *بفر,
                      size_t buflen, size_t *recv,
                      ثابت ساخت curl_ws_چوکھٹا **metap)
{
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ = CURLE_OK;

  اگر(CURL_EAPI_ENTER(&guard, curl, ws_recv, &نتیجہ)) {
    ساخت Curl_easy *ڈیٹا = curl;
    ساخت connectdata *conn;
    ساخت websocket *ws;
    ساخت ws_collect ctx;

    اگر(!recv || !metap || (buflen && !بفر)) {
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }

    *recv = 0;
    *metap = تہی;

    conn = ڈیٹا->conn;
    اگر(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      اگر(!ڈیٹا->سیٹ.connect_only) {
        failf(ڈیٹا, "[WS] CONNECT_ONLY is required");
        نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
        جاؤ برآمد;
      }

      Curl_getconnectinfo(ڈیٹا, &conn);
      اگر(!conn) {
        failf(ڈیٹا, "[WS] connection not found");
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        جاؤ برآمد;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    اگر(!ws) {
      failf(ڈیٹا, "[WS] connection is not setup for websocket");
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }

    memset(&ctx, 0, حجم(ctx));
    ctx.ڈیٹا = ڈیٹا;
    ctx.ws = ws;
    ctx.بفر = بفر;
    ctx.buflen = buflen;

    جب(1) {
      /* receive more when our buffer is empty */
      اگر(Curl_bufq_is_خالی_خانے(&ws->recvbuf)) {
        size_t n;
        نتیجہ = Curl_bufq_slurp(&ws->recvbuf, nw_اندر_recv, ڈیٹا, &n);
        اگر(نتیجہ)
          جاؤ برآمد;
        ورنہ اگر(n == 0) {
          /* connection closed */
          infof(ڈیٹا, "[WS] connection expectedly closed?");
          نتیجہ = CURLE_GOT_NOTHING;
          جاؤ برآمد;
        }
        CURL_TRC_WS(ڈیٹا, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      نتیجہ = ws_dec_pass(&ws->dec, ڈیٹا, &ws->recvbuf,
                           ws_client_collect, &ctx);
      اگر(نتیجہ == CURLE_AGAIN) {
        اگر(!ctx.written) {
          ws_dec_info(&ws->dec, ڈیٹا, "need more input");
          جاری;  /* nothing written, try more input */
        }
        توڑو;
      }
      ورنہ اگر(نتیجہ) {
        جاؤ برآمد;
      }
      ورنہ اگر(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        توڑو;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.چوکھٹا_age, ctx.چوکھٹا_flags, ctx.payload_انحراف,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(ڈیٹا, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.انحراف,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    اگر(!ڈیٹا->سیٹ.ws_raw_منوال && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_جوڑو_pending(ڈیٹا, ws);
      اگر(!r2)
        (خالی)ws_flush(ڈیٹا, ws, Curl_api_is_اندر_callback(ڈیٹا));
    }
    نتیجہ = CURLE_OK;
  }
برآمد:
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}

ساکن CURLcode ws_flush(ساخت Curl_easy *ڈیٹا, ساخت websocket *ws,
                         bool blocking)
{
  CURLcode نتیجہ;

  /* If there is space, add any pending control frame */
  اگر(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_سائز) {
    نتیجہ = ws_enc_جوڑو_pending(ڈیٹا, ws);
    اگر(نتیجہ && (نتیجہ != CURLE_AGAIN))
      واپس نتیجہ;
  }

  اگر(!Curl_bufq_is_خالی_خانے(&ws->sendbuf)) {
    ثابت uint8_t *برآمد;
    size_t outlen, n;
#اگر_تعریف_شدہ DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_اگلا = FALSE;
    size_t chunk_egain = 0;
    ثابت حرف *p = getenv("CURL_WS_CHUNK_EAGAIN");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#اگر_ختم

    جب(Curl_bufq_peek(&ws->sendbuf, &برآمد, &outlen)) {
#اگر_تعریف_شدہ DEBUGBUILD
      اگر(eagain_اگلا)
        واپس CURLE_AGAIN;
      اگر(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_اگلا = TRUE;
      }
#اگر_ختم
      اگر(blocking) {
        نتیجہ = ws_send_raw_blocking(ڈیٹا, ws, (ثابت حرف *)برآمد, outlen);
        n = نتیجہ ? 0 : outlen;
      }
      ورنہ اگر(ڈیٹا->سیٹ.connect_only || Curl_api_is_اندر_callback(ڈیٹا))
        نتیجہ = Curl_senddata(ڈیٹا, برآمد, outlen, &n);
      ورنہ {
        نتیجہ = Curl_xfer_send(ڈیٹا, برآمد, outlen, FALSE, &n);
        اگر(!نتیجہ && !n && outlen)
          نتیجہ = CURLE_AGAIN;
      }

      اگر(نتیجہ == CURLE_AGAIN) {
        CURL_TRC_WS(ڈیٹا, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        واپس نتیجہ;
      }
      ورنہ اگر(نتیجہ) {
        failf(ڈیٹا, "[WS] flush, write error %d", (صحیح)نتیجہ);
        واپس نتیجہ;
      }
      ورنہ {
        CURL_TRC_WS(ڈیٹا, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  واپس CURLE_OK;
}

ساکن CURLcode ws_send_raw_blocking(ساخت Curl_easy *ڈیٹا,
                                     ساخت websocket *ws,
                                     ثابت حرف *بفر, size_t buflen)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t nwritten;

  اگر(!ڈیٹا)
    واپس نتیجہ;

  (خالی)ws;
  جب(buflen) {
    نتیجہ = Curl_xfer_send(ڈیٹا, بفر, buflen, FALSE, &nwritten);
    اگر(نتیجہ)
      واپس نتیجہ;
    DEBUGASSERT(nwritten <= buflen);
    بفر += nwritten;
    buflen -= nwritten;
    اگر(buflen) {
      curl_socket_t sock = ڈیٹا->conn->sock[FIRSTSOCKET];
      timediff_t بایاں_ms;
      صحیح ev;

      CURL_TRC_WS(ڈیٹا, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      بایاں_ms = Curl_timeleft_ms(ڈیٹا);
      اگر(بایاں_ms < 0) {
        failf(ڈیٹا, "[WS] Timeout waiting for socket becoming writable");
        واپس CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      اگر(sock == CURL_SOCKET_BAD)
        واپس CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, بایاں_ms ? بایاں_ms : 500);
      اگر(ev < 0) {
        failf(ڈیٹا, "[WS] Error while waiting for socket becoming writable");
        واپس CURLE_SEND_ERROR;
      }
    }
  }
  واپس نتیجہ;
}

ساکن CURLcode ws_send_raw(ساخت Curl_easy *ڈیٹا, ثابت خالی *بفر,
                            size_t buflen, size_t *pnwritten)
{
  ساخت websocket *ws;
  CURLcode نتیجہ;

  ws = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_PROTO_WS_CONN);
  اگر(!ws) {
    failf(ڈیٹا, "[WS] Not a websocket transfer");
    واپس CURLE_SEND_ERROR;
  }
  اگر(!buflen)
    واپس CURLE_OK;

  اگر(Curl_api_is_اندر_callback(ڈیٹا)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    نتیجہ = ws_flush(ڈیٹا, ws, TRUE);
    اگر(نتیجہ)
      واپس نتیجہ;
    نتیجہ = ws_send_raw_blocking(ڈیٹا, ws, بفر, buflen);
    اگر(!نتیجہ)
      *pnwritten = buflen;
  }
  ورنہ {
    /* We need any pending data to be sent or EAGAIN this call. */
    نتیجہ = ws_flush(ڈیٹا, ws, FALSE);
    اگر(نتیجہ)
      واپس نتیجہ;
    نتیجہ = Curl_senddata(ڈیٹا, بفر, buflen, pnwritten);
  }

  CURL_TRC_WS(ڈیٹا, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (صحیح)نتیجہ, *pnwritten);
  واپس نتیجہ;
}

CURLcode curl_ws_send(CURL *curl, ثابت خالی *بفر_arg,
                      size_t buflen, size_t *بھیجا,
                      curl_off_t fragsize,
                      غیرعلامتی صحیح flags)
{
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ = CURLE_OK;

  اگر(CURL_EAPI_ENTER(&guard, curl, ws_send, &نتیجہ)) {
    ساخت websocket *ws;
    ثابت uint8_t *بفر = بفر_arg;
    ساخت Curl_easy *ڈیٹا = curl;
    size_t ndummy;
    size_t *pnsent = بھیجا ? بھیجا : &ndummy;

    CURL_TRC_WS(ڈیٹا, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, ڈیٹا->سیٹ.ws_raw_منوال);

    *pnsent = 0;

    اگر(!بفر && buflen) {
      failf(ڈیٹا, "[WS] buffer is NULL when buflen is not");
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }

    اگر(!ڈیٹا->conn && ڈیٹا->سیٹ.connect_only) {
      نتیجہ = Curl_connect_only_attach(ڈیٹا);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
    اگر(!ڈیٹا->conn) {
      failf(ڈیٹا, "[WS] No associated connection");
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
    ws = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_PROTO_WS_CONN);
    اگر(!ws) {
      failf(ڈیٹا, "[WS] Not a websocket transfer");
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }

    اگر(ڈیٹا->سیٹ.ws_raw_منوال) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      نتیجہ = ws_flush(ڈیٹا, ws, FALSE);
      اگر(نتیجہ)
        جاؤ برآمد;

      اگر(!بفر) {
        failf(ڈیٹا, "[WS] buffer is NULL in raw mode");
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        جاؤ برآمد;
      }
      اگر(!بھیجا) {
        failf(ڈیٹا, "[WS] sent is NULL in raw mode");
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        جاؤ برآمد;
      }
      اگر(fragsize || flags) {
        failf(ڈیٹا, "[WS] fragsize and flags must be zero in raw mode");
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        جاؤ برآمد;
      }
      نتیجہ = ws_send_raw(ڈیٹا, بفر, buflen, pnsent);
      جاؤ برآمد;
    }

    /* Not RAW mode, we do the frame encoding */
    نتیجہ = ws_enc_send(ڈیٹا, ws, بفر, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(ڈیٹا, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, ڈیٹا->سیٹ.ws_raw_منوال, (صحیح)نتیجہ,
                *pnsent);
  }
برآمد:
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}

ساکن CURLcode ws_تیاری_conn(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  ڈیٹا->state.http_neg.accept_09 = FALSE;
  ڈیٹا->state.http_neg.only_10 = FALSE;
  ڈیٹا->state.http_neg.wanted = CURL_HTTP_V1x;
  ڈیٹا->state.http_neg.allowed = CURL_HTTP_V1x;
  واپس Curl_http_تیاری_conn(ڈیٹا, conn);
}

ثابت ساخت curl_ws_چوکھٹا *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  ساخت Curl_easy *ڈیٹا = curl;
  اگر(GOOD_EASY_HANDLE(ڈیٹا) && Curl_api_is_اندر_callback(ڈیٹا) &&
     ڈیٹا->conn && !ڈیٹا->سیٹ.ws_raw_منوال) {
    ساخت websocket *ws;
    ws = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_PROTO_WS_CONN);
    اگر(ws)
      واپس &ws->recvframe;
  }
  واپس تہی;
}

CURL_EXTERN CURLcode curl_ws_آغاز_چوکھٹا(CURL *curl,
                                         غیرعلامتی صحیح flags,
                                         curl_off_t چوکھٹا_len)
{
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ = CURLE_OK;

  اگر(CURL_EAPI_ENTER(&guard, curl, ws_آغاز_چوکھٹا, &نتیجہ)) {
    ساخت Curl_easy *ڈیٹا = curl;
    ساخت websocket *ws;

    اگر(ڈیٹا->سیٹ.ws_raw_منوال) {
      failf(ڈیٹا, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      نتیجہ = CURLE_FAILED_INIT;
      جاؤ برآمد;
    }

    CURL_TRC_WS(ڈیٹا, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, چوکھٹا_len);

    اگر(!ڈیٹا->conn) {
      failf(ڈیٹا, "[WS] No associated connection");
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
    ws = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_PROTO_WS_CONN);
    اگر(!ws) {
      failf(ڈیٹا, "[WS] Not a websocket transfer");
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }

    اگر(ws->enc.payload_remain) {
      failf(ڈیٹا, "[WS] previous frame not finished");
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }

    نتیجہ = ws_enc_write_سر(ڈیٹا, ws, &ws->enc, flags, چوکھٹا_len,
                               &ws->sendbuf);
    اگر(نتیجہ)
      CURL_TRC_WS(ڈیٹا, "curl_start_frame(), error adding frame head %d",
                  (صحیح)نتیجہ);
  }
برآمد:
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}

ثابت ساخت Curl_protocol Curl_protocol_ws = {
  ws_تیاری_conn,                        /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_مکمل,                       /* done */
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

#ورنہ

CURLcode curl_ws_recv(CURL *curl, خالی *بفر, size_t buflen,
                      size_t *recv,
                      ثابت ساخت curl_ws_چوکھٹا **metap)
{
  (خالی)curl;
  (خالی)بفر;
  (خالی)buflen;
  (خالی)recv;
  (خالی)metap;
  واپس CURLE_NOT_BUILT_اندر;
}

CURLcode curl_ws_send(CURL *curl, ثابت خالی *بفر,
                      size_t buflen, size_t *بھیجا,
                      curl_off_t fragsize,
                      غیرعلامتی صحیح flags)
{
  (خالی)curl;
  (خالی)بفر;
  (خالی)buflen;
  (خالی)بھیجا;
  (خالی)fragsize;
  (خالی)flags;
  واپس CURLE_NOT_BUILT_اندر;
}

ثابت ساخت curl_ws_چوکھٹا *curl_ws_meta(CURL *ڈیٹا)
{
  (خالی)ڈیٹا;
  واپس تہی;
}

CURL_EXTERN CURLcode curl_ws_آغاز_چوکھٹا(CURL *curl,
                                         غیرعلامتی صحیح flags,
                                         curl_off_t چوکھٹا_len)
{
  (خالی)curl;
  (خالی)flags;
  (خالی)چوکھٹا_len;
  واپس CURLE_NOT_BUILT_اندر;
}

#اگر_ختم /* !CURL_DISABLE_WEBSOCKETS */
