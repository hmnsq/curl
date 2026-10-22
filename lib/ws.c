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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_WEBSOCKETS

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
#وصف_ڪريو WSBIT_FIN          0x80
#وصف_ڪريو WSBIT_RSV1         0x40
#وصف_ڪريو WSBIT_RSV2         0x20
#وصف_ڪريو WSBIT_RSV3         0x10
#وصف_ڪريو WSBIT_RSV_نقاب     (WSBIT_RSV1 | WSBIT_RSV2 | WSBIT_RSV3)
#وصف_ڪريو WSBIT_OPCODE_CONT  0x0
#وصف_ڪريو WSBIT_OPCODE_متن  0x1
#وصف_ڪريو WSBIT_OPCODE_BIN   0x2
#وصف_ڪريو WSBIT_OPCODE_CLOSE 0x8
#وصف_ڪريو WSBIT_OPCODE_PING  0x9
#وصف_ڪريو WSBIT_OPCODE_PONG  0xa
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
#وصف_ڪريو WSBIT_OPCODE_نقاب  0xf
#جيڪڏهن_پورو

#وصف_ڪريو WSBIT_نقاب 0x80

/* buffer dimensioning */
#وصف_ڪريو WS_CHUNK_سائيز  65535
#وصف_ڪريو WS_CHUNK_ڳڻپ 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
تعداد ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_سر,
  WS_DEC_PAYLOAD
};

بنية ws_decoder {
  صحيح چوکٺو_age;        /* zero */
  صحيح چوکٺو_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_وٿي;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t سر[10];
  صحيح سر_len, سر_ڪل;
  تعداد ws_dec_state state;
  صحيح cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
بنية ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  بنانشان صحيح xori; /* xor index */
  uint8_t نقاب[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  ٻٽو_انگ(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#وصف_ڪريو WS_MAX_CNTRL_LEN    125

بنية ws_cntrl_چوکٺو {
  بنانشان صحيح type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
بنية websocket {
  بنية Curl_easy *معلومات; /* used for write callback handling */
  بنية ws_decoder dec;  /* decode of ws frames */
  بنية ws_encoder enc;  /* encode of ws frames */
  بنية bufq recvbuf;    /* raw data from the server */
  بنية bufq sendbuf;    /* raw data to be sent to the server */
  بنية curl_ws_چوکٺو recvframe;  /* the current WS FRAME received */
  بنية ws_cntrl_چوکٺو pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  ٻٽو_انگ(using_reader);      /* sending WS data is handled via a client reader */
};

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ساكن ثابت حرف *ws_چوکٺو_name_جو_op(uint8_t firstbyte)
{
  بدل(firstbyte & WSBIT_OPCODE_نقاب) {
  حالة WSBIT_OPCODE_CONT:
    أرجع "CONT";
  حالة WSBIT_OPCODE_متن:
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
#جيڪڏهن_پورو

ساكن صحيح ws_چوکٺو_firstbyte2flags(بنية Curl_easy *معلومات,
                                    uint8_t firstbyte, صحيح cont_flags)
{
  بدل(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  حالة WSBIT_OPCODE_CONT:
    إذا(!(cont_flags & CURLWS_CONT)) {
      failf(معلومات, "[WS] no ongoing fragmented message to resume");
      أرجع 0;
    }
    أرجع cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  حالة (WSBIT_OPCODE_CONT | WSBIT_FIN):
    إذا(!(cont_flags & CURLWS_CONT)) {
      failf(معلومات, "[WS] no ongoing fragmented message to resume");
      أرجع 0;
    }
    أرجع cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  حالة WSBIT_OPCODE_متن:
    إذا(cont_flags & CURLWS_CONT) {
      failf(معلومات, "[WS] fragmented message interrupted by new TEXT msg");
      أرجع 0;
    }
    أرجع CURLWS_متن | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  حالة (WSBIT_OPCODE_متن | WSBIT_FIN):
    إذا(cont_flags & CURLWS_CONT) {
      failf(معلومات, "[WS] fragmented message interrupted by new TEXT msg");
      أرجع 0;
    }
    أرجع CURLWS_متن;
  /* 0x02 - first BINARY fragment */
  حالة WSBIT_OPCODE_BIN:
    إذا(cont_flags & CURLWS_CONT) {
      failf(معلومات, "[WS] fragmented message interrupted by new BINARY msg");
      أرجع 0;
    }
    أرجع CURLWS_ٻٽو | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  حالة (WSBIT_OPCODE_BIN | WSBIT_FIN):
    إذا(cont_flags & CURLWS_CONT) {
      failf(معلومات, "[WS] fragmented message interrupted by new BINARY msg");
      أرجع 0;
    }
    أرجع CURLWS_ٻٽو;
  /* 0x08 - first CLOSE fragment */
  حالة WSBIT_OPCODE_CLOSE:
    failf(معلومات, "[WS] invalid fragmented CLOSE frame");
    أرجع 0;
  /* 0x88 - unfragmented CLOSE */
  حالة (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    أرجع CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  حالة WSBIT_OPCODE_PING:
    failf(معلومات, "[WS] invalid fragmented PING frame");
    أرجع 0;
  /* 0x89 - unfragmented PING */
  حالة (WSBIT_OPCODE_PING | WSBIT_FIN):
    أرجع CURLWS_PING;
  /* 0x0a - first PONG fragment */
  حالة WSBIT_OPCODE_PONG:
    failf(معلومات, "[WS] invalid fragmented PONG frame");
    أرجع 0;
  /* 0x8a - unfragmented PONG */
  حالة (WSBIT_OPCODE_PONG | WSBIT_FIN):
    أرجع CURLWS_PONG;
  /* invalid first byte */
  افتراضي:
    إذا(firstbyte & WSBIT_RSV_نقاب)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(معلومات, "[WS] invalid reserved bits: %02x", firstbyte);
    وإلا
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(معلومات, "[WS] invalid opcode: %02x", firstbyte);
    أرجع 0;
  }
}

ساكن CURLcode ws_چوکٺو_flags2firstbyte(بنية Curl_easy *معلومات,
                                         بنانشان صحيح flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  بدل(flags & ~CURLWS_وٿي) {
  حالة 0:
    إذا(contfragment) {
      CURL_TRC_WS(معلومات, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      أرجع CURLE_OK;
    }
    failf(معلومات, "[WS] no flags given");
    أرجع CURLE_BAD_FUNCTION_دليل;
  حالة CURLWS_CONT:
    إذا(contfragment) {
      infof(معلومات, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      أرجع CURLE_OK;
    }
    failf(معلومات, "[WS] No ongoing fragmented message to continue");
    أرجع CURLE_BAD_FUNCTION_دليل;
  حالة CURLWS_متن:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_متن | WSBIT_FIN);
    أرجع CURLE_OK;
  حالة (CURLWS_متن | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_متن;
    أرجع CURLE_OK;
  حالة CURLWS_ٻٽو:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    أرجع CURLE_OK;
  حالة (CURLWS_ٻٽو | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    أرجع CURLE_OK;
  حالة CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_CLOSE | CURLWS_CONT):
    failf(معلومات, "[WS] CLOSE frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_دليل;
  حالة CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_PING | CURLWS_CONT):
    failf(معلومات, "[WS] PING frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_دليل;
  حالة CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_PONG | CURLWS_CONT):
    failf(معلومات, "[WS] PONG frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_دليل;
  افتراضي:
    failf(معلومات, "[WS] unknown flags: %x", flags);
    أرجع CURLE_BAD_FUNCTION_دليل;
  }
}

ساكن فراغ ws_dec_info(بنية ws_decoder *dec, بنية Curl_easy *معلومات,
                        ثابت حرف *نياپو)
{
  NOVERBOSE((فراغ)نياپو);
  بدل(dec->سر_len) {
  حالة 0:
    اكسر;
  حالة 1:
    CURL_TRC_WS(معلومات, "decoded %s [%s%s]", نياپو,
                ws_چوکٺو_name_جو_op(dec->سر[0]),
                (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    اكسر;
  افتراضي:
    إذا(dec->سر_len < dec->سر_ڪل) {
      CURL_TRC_WS(معلومات, "decoded %s [%s%s](%d/%d)", نياپو,
                  ws_چوکٺو_name_جو_op(dec->سر[0]),
                  (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->سر_len, dec->سر_ڪل);
    }
    وإلا {
      CURL_TRC_WS(معلومات, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  نياپو, ws_چوکٺو_name_جو_op(dec->سر[0]),
                  (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_وٿي, dec->payload_len);
    }
    اكسر;
  }
}

ساكن CURLcode ws_send_raw_blocking(بنية Curl_easy *معلومات,
                                     بنية websocket *ws,
                                     ثابت حرف *بفر, size_t buflen);

تعريفنوع CURLcode ws_write_payload(ثابت uint8_t *buf, size_t buflen,
                                  صحيح چوکٺو_age, صحيح چوکٺو_flags,
                                  curl_off_t payload_وٿي,
                                  curl_off_t payload_len,
                                  فراغ *userp,
                                  size_t *pnwritten);

ساكن فراغ ws_dec_ايندڙ_چوکٺو(بنية ws_decoder *dec)
{
  dec->چوکٺو_age = 0;
  dec->چوکٺو_flags = 0;
  dec->payload_وٿي = 0;
  dec->payload_len = 0;
  dec->سر_len = dec->سر_ڪل = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ساكن فراغ ws_dec_reset(بنية ws_decoder *dec)
{
  dec->چوکٺو_age = 0;
  dec->چوکٺو_flags = 0;
  dec->payload_وٿي = 0;
  dec->payload_len = 0;
  dec->سر_len = dec->سر_ڪل = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

ساكن فراغ ws_dec_init(بنية ws_decoder *dec)
{
  ws_dec_reset(dec);
}

ساكن CURLcode ws_dec_read_سر(بنية ws_decoder *dec,
                                 بنية Curl_easy *معلومات,
                                 بنية bufq *inraw)
{
  ثابت uint8_t *inbuf;
  size_t inlen;

  طالما(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    إذا(dec->سر_len == 0) {
      dec->سر[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->چوکٺو_flags = ws_چوکٺو_firstbyte2flags(معلومات, dec->سر[0],
                                                  dec->cont_flags);
      إذا(!dec->چوکٺو_flags) {
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      إذا(dec->چوکٺو_flags & (CURLWS_متن | CURLWS_ٻٽو)) {
        dec->cont_flags = dec->چوکٺو_flags;
      }

      dec->سر_len = 1;
#إذا 0
      ws_dec_info(dec, معلومات, "seeing opcode");
#جيڪڏهن_پورو
      استمر;
    }
    وإلا إذا(dec->سر_len == 1) {
      dec->سر[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->سر_len = 2;

      إذا(dec->سر[1] & WSBIT_نقاب) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(معلومات, "[WS] masked input frame");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->چوکٺو_flags & CURLWS_PING && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(معلومات, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->چوکٺو_flags & CURLWS_PONG && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(معلومات, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->چوکٺو_flags & CURLWS_CLOSE && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        failf(معلومات, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      إذا(dec->سر[1] == 126) {
        dec->سر_ڪل = 4;
        استمر;
      }
      وإلا إذا(dec->سر[1] == 127) {
        dec->سر_ڪل = 10;
        استمر;
      }
      وإلا {
        dec->سر_ڪل = 2;
      }
    }

    إذا(dec->سر_len < dec->سر_ڪل) {
      dec->سر[dec->سر_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->سر_len;
      إذا(dec->سر_len < dec->سر_ڪل) {
#إذا 0
        ws_dec_info(dec, معلومات, "decoding head");
#جيڪڏهن_پورو
        استمر;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->سر_len == dec->سر_ڪل);
    بدل(dec->سر_ڪل) {
    حالة 2:
      dec->payload_len = dec->سر[1];
      اكسر;
    حالة 4:
      dec->payload_len = (dec->سر[2] << 8) | dec->سر[3];
      اكسر;
    حالة 10:
      إذا(dec->سر[2] > 127) {
        failf(معلومات, "[WS] frame length longer than 63 bits not supported");
        أرجع CURLE_RECV_ERROR;
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
      اكسر;
    افتراضي:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(معلومات, "[WS] unexpected frame header length");
      أرجع CURLE_RECV_ERROR;
    }

    dec->چوکٺو_age = 0;
    dec->payload_وٿي = 0;
    ws_dec_info(dec, معلومات, "head");
    أرجع CURLE_OK;
  }
  أرجع CURLE_AGAIN;
}

ساكن CURLcode ws_dec_pass_payload(بنية ws_decoder *dec,
                                    بنية Curl_easy *معلومات,
                                    بنية bufq *inraw,
                                    ws_write_payload *write_cb,
                                    فراغ *write_ctx)
{
  ثابت uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode نتيجو;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_وٿي,
                                     0, SIZE_MAX);

  طالما(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(معلومات)) {
    إذا(inlen > remain)
      inlen = remain;
    نتيجو = write_cb(inbuf, inlen, dec->چوکٺو_age, dec->چوکٺو_flags,
                      dec->payload_وٿي, dec->payload_len,
                      write_ctx, &nwritten);
    إذا(نتيجو)
      أرجع نتيجو;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_وٿي += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_وٿي,
                                0, SIZE_MAX);
    CURL_TRC_WS(معلومات, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  أرجع remain ? CURLE_AGAIN : CURLE_OK;
}

ساكن CURLcode ws_dec_pass(بنية ws_decoder *dec,
                            بنية Curl_easy *معلومات,
                            بنية bufq *inraw,
                            ws_write_payload *write_cb,
                            فراغ *write_ctx)
{
  CURLcode نتيجو;

  إذا(Curl_bufq_is_خالي_جايون(inraw))
    أرجع CURLE_AGAIN;

  بدل(dec->state) {
  حالة WS_DEC_INIT:
    ws_dec_ايندڙ_چوکٺو(dec);
    dec->state = WS_DEC_سر;
    FALLTHROUGH();
  حالة WS_DEC_سر:
    نتيجو = ws_dec_read_سر(dec, معلومات, inraw);
    إذا(نتيجو) {
      إذا(نتيجو != CURLE_AGAIN) {
        failf(معلومات, "[WS] decode frame error %d", (صحيح)نتيجو);
        اكسر;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_خالي_جايون(inraw));
      اكسر;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    إذا(dec->payload_len == 0) {
      size_t nwritten;
      ثابت uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      نتيجو = write_cb(&tmp, 0, dec->چوکٺو_age, dec->چوکٺو_flags,
                        0, 0, write_ctx, &nwritten);
      إذا(نتيجو)
        أرجع نتيجو;
      dec->state = WS_DEC_INIT;
      اكسر;
    }
    FALLTHROUGH();
  حالة WS_DEC_PAYLOAD:
    نتيجو = ws_dec_pass_payload(dec, معلومات, inraw, write_cb, write_ctx);
    ws_dec_info(dec, معلومات, "passing");
    إذا(نتيجو)
      أرجع نتيجو;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    اكسر;
  افتراضي:
    /* we covered all enums above, but some code analyzers are wimps */
    نتيجو = CURLE_FAILED_INIT;
  }
  أرجع نتيجو;
}

ساكن فراغ update_meta(بنية websocket *ws,
                        صحيح چوکٺو_age, صحيح چوکٺو_flags,
                        curl_off_t payload_وٿي,
                        curl_off_t payload_len,
                        size_t cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_وٿي - cur_len);

  ws->recvframe.age = چوکٺو_age;
  ws->recvframe.flags = چوکٺو_flags;
  ws->recvframe.وٿي = payload_وٿي;
  ws->recvframe.len = cur_len;
  ws->recvframe.bytesleft = bytesleft;
}

/* WebSocket decoding client writer */
بنية ws_cw_ctx {
  بنية Curl_cwriter super;
  بنية bufq buf;
};

ساكن CURLcode ws_cw_init(بنية Curl_easy *معلومات,
                           بنية Curl_cwriter *writer)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  (فراغ)معلومات;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_سائيز, 1, BUFQ_OPT_SOFT_حد_بندي);
  أرجع CURLE_OK;
}

ساكن فراغ ws_cw_close(بنية Curl_easy *معلومات, بنية Curl_cwriter *writer)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  (فراغ)معلومات;
  Curl_bufq_free(&ctx->buf);
}

بنية ws_cw_dec_ctx {
  بنية Curl_easy *معلومات;
  بنية websocket *ws;
  بنية Curl_cwriter *ايندڙ_writer;
  صحيح cw_type;
};

ساكن CURLcode ws_flush(بنية Curl_easy *معلومات, بنية websocket *ws,
                         bool blocking);
ساكن CURLcode ws_enc_جوڙيو_pending(بنية Curl_easy *معلومات,
                                   بنية websocket *ws);

ساكن CURLcode ws_enc_جوڙيو_cntrl(بنية Curl_easy *معلومات,
                                 بنية websocket *ws,
                                 ثابت uint8_t *payload,
                                 size_t plen,
                                 بنانشان صحيح چوکٺو_type)
{
  (فراغ)معلومات;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  إذا(plen > WS_MAX_CNTRL_LEN)
    أرجع CURLE_BAD_FUNCTION_دليل;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = چوکٺو_type;
  ws->pending.payload_len = plen;
  يادگيري_نقل(ws->pending.payload, payload, plen);
  أرجع CURLE_OK;
}

ساكن curl_off_t ws_payload_remain(curl_off_t payload_ڪل,
                                    curl_off_t payload_وٿي,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_ڪل - payload_وٿي;
  إذا((payload_ڪل < 0) || (payload_وٿي < 0) || (remain < 0))
    أرجع -1;
  buffered = curlx_uztoso(payload_buffered);
  إذا(remain < buffered)
    أرجع -1;
  أرجع remain - buffered;
}

ساكن CURLcode ws_cw_dec_ايندڙ(ثابت uint8_t *buf, size_t buflen,
                               صحيح چوکٺو_age, صحيح چوکٺو_flags,
                               curl_off_t payload_وٿي,
                               curl_off_t payload_len,
                               فراغ *user_معلومات,
                               size_t *pnwritten)
{
  بنية ws_cw_dec_ctx *ctx = user_معلومات;
  بنية Curl_easy *معلومات = ctx->معلومات;
  بنية websocket *ws = ctx->ws;
  bool auto_pong = !معلومات->سيٽ.ws_raw_گهڻائي && !معلومات->سيٽ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode نتيجو;

  (فراغ)چوکٺو_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_وٿي, buflen);
  إذا(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  إذا(auto_pong && (چوکٺو_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(معلومات, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_وٿي, payload_len);
    /* send back the exact same content as a PONG */
    نتيجو = ws_enc_جوڙيو_cntrl(معلومات, ws, buf, buflen, CURLWS_PONG);
    إذا(نتيجو)
      أرجع نتيجو;
  }
  وإلا إذا(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, چوکٺو_age, چوکٺو_flags, payload_وٿي,
                payload_len, buflen);

    CURL_TRC_WRITE(معلومات, "[WS] pass %zu decoded bytes", buflen);
    نتيجو = Curl_cwriter_write(معلومات, ctx->ايندڙ_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ثابت حرف *)buf, buflen);
    إذا(نتيجو)
      أرجع نتيجو;
  }
  *pnwritten = buflen;
  أرجع CURLE_OK;
}

ساكن CURLcode ws_cw_write(بنية Curl_easy *معلومات,
                            بنية Curl_cwriter *writer, صحيح type,
                            ثابت حرف *buf, size_t nbytes)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  بنية websocket *ws;
  CURLcode نتيجو = CURLE_OK;

  CURL_TRC_WRITE(معلومات, "[WS] write(len=%zu, type=%d)", nbytes, type);
  إذا(!(type & CLIENTWRITE_BODY) || معلومات->سيٽ.ws_raw_گهڻائي)
    أرجع Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, nbytes);

  ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(معلومات, "[WS] not a websocket transfer");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(nbytes) {
    size_t nwritten;
    نتيجو = Curl_bufq_write(&ctx->buf, (ثابت uint8_t *)buf,
                             nbytes, &nwritten);
    إذا(نتيجو) {
      infof(معلومات, "[WS] error adding data to buffer %d", (صحيح)نتيجو);
      أرجع نتيجو;
    }
  }

  نتيجو = Curl_cwriter_flush(معلومات, writer->ايندڙ);
  إذا(نتيجو)
    اذهب ٻاهريون;

  طالما(!Curl_bufq_is_خالي_جايون(&ctx->buf) && !Curl_cwriter_is_paused(معلومات)) {
    بنية ws_cw_dec_ctx pass_ctx;
    pass_ctx.معلومات = معلومات;
    pass_ctx.ws = ws;
    pass_ctx.ايندڙ_writer = writer->ايندڙ;
    pass_ctx.cw_type = type;
    نتيجو = ws_dec_pass(&ws->dec, معلومات, &ctx->buf,
                         ws_cw_dec_ايندڙ, &pass_ctx);
    إذا(نتيجو == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      نتيجو = CURLE_OK;
      اذهب ٻاهريون;
    }
    وإلا إذا(نتيجو) {
      failf(معلومات, "[WS] decode payload error %d", (صحيح)نتيجو);
      Curl_bufq_reset(&ctx->buf);
      اذهب ٻاهريون;
    }
  }

ٻاهريون:
  إذا(!نتيجو && !ws->using_reader) {
    نتيجو = ws_flush(معلومات, ws, Curl_api_is_اندريون_callback(معلومات));
    إذا(نتيجو == CURLE_AGAIN)
      نتيجو = CURLE_OK;
  }

  إذا(!نتيجو && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_خالي_جايون(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(معلومات, "[WS] decode ended with incomplete frame");
    نتيجو = CURLE_RECV_ERROR;
  }
  أرجع نتيجو;
}

ساكن CURLcode ws_cw_flush(بنية Curl_easy *معلومات,
                            بنية Curl_cwriter *writer)
{
  CURLcode نتيجو = CURLE_OK;

  CURL_TRC_WRITE(معلومات, "[ws] flush");
  إذا(!معلومات->سيٽ.ws_raw_گهڻائي) {
    بنية ws_cw_ctx *ctx = writer->ctx;
    بنية websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    نتيجو = Curl_cwriter_flush(معلومات, writer->ايندڙ);
    إذا(نتيجو)
      اذهب ٻاهريون;

    ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(معلومات, "[WS] not a websocket transfer");
      أرجع CURLE_FAILED_INIT;
    }

    طالما(!Curl_bufq_is_خالي_جايون(&ctx->buf) && !Curl_cwriter_is_paused(معلومات)) {
      بنية ws_cw_dec_ctx pass_ctx;
      pass_ctx.معلومات = معلومات;
      pass_ctx.ws = ws;
      pass_ctx.ايندڙ_writer = writer->ايندڙ;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      نتيجو = ws_dec_pass(&ws->dec, معلومات, &ctx->buf,
                           ws_cw_dec_ايندڙ, &pass_ctx);
      إذا(نتيجو == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        نتيجو = CURLE_OK;
        اذهب ٻاهريون;
      }
      وإلا إذا(نتيجو) {
        failf(معلومات, "[WS] decode payload error %d", (صحيح)نتيجو);
        Curl_bufq_reset(&ctx->buf);
        اذهب ٻاهريون;
      }
    }
  }

ٻاهريون:
  إذا(!نتيجو)
    نتيجو = Curl_cwriter_flush(معلومات, writer->ايندڙ);
  أرجع نتيجو;
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

ساكن فراغ ws_enc_info(بنية ws_encoder *enc, بنية Curl_easy *معلومات,
                        ثابت حرف *نياپو)
{
  NOVERBOSE((فراغ)enc);
  NOVERBOSE((فراغ)نياپو);
  CURL_TRC_WS(معلومات, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              نياپو, ws_چوکٺو_name_جو_op(enc->firstbyte),
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

ساكن CURLcode ws_enc_جوڙيو_چوکٺو(بنية Curl_easy *معلومات,
                                 بنية ws_encoder *enc,
                                 بنانشان صحيح flags,
                                 curl_off_t payload_len,
                                 بنية bufq *ٻاهريون)
{
  uint8_t firstb = 0;
  uint8_t سر[14];
  CURLcode نتيجو;
  size_t hlen, nwritten;

  إذا(payload_len < 0) {
    failf(معلومات, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    أرجع CURLE_SEND_ERROR;
  }

  إذا(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(معلومات, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    أرجع CURLE_SEND_ERROR;
  }

  نتيجو = ws_چوکٺو_flags2firstbyte(معلومات, flags, (bool)enc->contfragment,
                                    &firstb);
  إذا(نتيجو)
    أرجع نتيجو;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  إذا(flags & (CURLWS_متن | CURLWS_ٻٽو)) {
    enc->contfragment = (curl_ٻٽو_انگ)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  إذا(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(معلومات, "[WS] given PING frame is too big");
    أرجع CURLE_TOO_LARGE;
  }
  إذا(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(معلومات, "[WS] given PONG frame is too big");
    أرجع CURLE_TOO_LARGE;
  }
  إذا(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(معلومات, "[WS] given CLOSE frame is too big");
    أرجع CURLE_TOO_LARGE;
  }

  سر[0] = enc->firstbyte = firstb;
  إذا(payload_len > 65535) {
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
  وإلا إذا(payload_len >= 126) {
    سر[1] = 126 | WSBIT_نقاب;
    سر[2] = (uint8_t)((payload_len >> 8) & 0xff);
    سر[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  وإلا {
    سر[1] = (uint8_t)payload_len | WSBIT_نقاب;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, معلومات, "sending");

  /* 4 bytes random */

  نتيجو = Curl_rand(معلومات, (uint8_t *)&enc->نقاب, حجم(enc->نقاب));
  إذا(نتيجو)
    أرجع نتيجو;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  إذا(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->نقاب, 0, حجم(enc->نقاب));
#جيڪڏهن_پورو

  /* add 4 bytes mask */
  يادگيري_نقل(&سر[hlen], &enc->نقاب, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  نتيجو = Curl_bufq_write(ٻاهريون, سر, hlen, &nwritten);
  إذا(نتيجو)
    أرجع نتيجو;
  إذا(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ws_enc_write_سر(بنية Curl_easy *معلومات,
                                  بنية websocket *ws,
                                  بنية ws_encoder *enc,
                                  بنانشان صحيح flags,
                                  curl_off_t payload_len,
                                  بنية bufq *ٻاهريون)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  إذا(ws->pending.type) {
    CURLcode نتيجو = ws_enc_جوڙيو_pending(معلومات, ws);
    إذا(نتيجو)
      أرجع نتيجو;
  }
  أرجع ws_enc_جوڙيو_چوکٺو(معلومات, enc, flags, payload_len, ٻاهريون);
}

ساكن CURLcode ws_enc_write_payload(بنية ws_encoder *enc,
                                     بنية Curl_easy *معلومات,
                                     ثابت uint8_t *buf, size_t buflen,
                                     بنية bufq *ٻاهريون, size_t *pnwritten)
{
  CURLcode نتيجو;
  size_t i, len, n, remain;

  *pnwritten = 0;
  إذا(Curl_bufq_is_ڀريل(ٻاهريون))
    أرجع CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  إذا(remain < len)
    len = remain;

  لكل(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->نقاب[enc->xori];
    نتيجو = Curl_bufq_write(ٻاهريون, &c, 1, &n);
    إذا(نتيجو) {
      إذا((نتيجو != CURLE_AGAIN) || !i)
        أرجع نتيجو;
      اكسر;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, معلومات, "buffered");
  أرجع CURLE_OK;
}

ساكن CURLcode ws_enc_جوڙيو_pending(بنية Curl_easy *معلومات,
                                   بنية websocket *ws)
{
  CURLcode نتيجو;
  size_t n;

  إذا(!ws->pending.type) /* no pending frame here */
    أرجع CURLE_OK;
  إذا(ws->enc.payload_remain) /* in the middle of another frame */
    أرجع CURLE_AGAIN;

  نتيجو = ws_enc_جوڙيو_چوکٺو(معلومات, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  إذا(نتيجو) {
    CURL_TRC_WS(معلومات, "ws_enc_cntrl(), error adding head: %d",
                (صحيح)نتيجو);
    اذهب ٻاهريون;
  }
  نتيجو = ws_enc_write_payload(&ws->enc, معلومات, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  إذا(نتيجو) {
    CURL_TRC_WS(معلومات, "ws_enc_cntrl(), error adding payload: %d",
                (صحيح)نتيجو);
    اذهب ٻاهريون;
  }
  إذا(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(معلومات, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    نتيجو = CURLE_SEND_ERROR;
    اذهب ٻاهريون;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, حجم(ws->pending));

ٻاهريون:
  أرجع نتيجو;
}

ساكن CURLcode ws_enc_send(بنية Curl_easy *معلومات,
                            بنية websocket *ws,
                            ثابت uint8_t *بفر,
                            size_t buflen,
                            curl_off_t fragsize,
                            بنانشان صحيح flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(!معلومات->سيٽ.ws_raw_گهڻائي);
  *pnsent = 0;

  إذا(ws->enc.payload_remain || !Curl_bufq_is_خالي_جايون(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    إذا(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(معلومات, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      أرجع CURLE_BAD_FUNCTION_دليل;
    }
    إذا((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(معلومات, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      أرجع CURLE_BAD_FUNCTION_دليل;
    }
  }
  وإلا {
    نتيجو = ws_flush(معلومات, ws, Curl_api_is_اندريون_callback(معلومات));
    إذا(نتيجو)
      أرجع نتيجو;

    نتيجو = ws_enc_write_سر(معلومات, ws, &ws->enc, flags,
                               (flags & CURLWS_وٿي) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    إذا(نتيجو) {
      CURL_TRC_WS(معلومات, "curl_ws_send(), error writing frame head %d",
                  (صحيح)نتيجو);
      أرجع نتيجو;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  طالما(!Curl_bufq_is_خالي_جايون(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    إذا(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      نتيجو = ws_enc_write_payload(&ws->enc, معلومات,
                                    بفر + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      إذا(نتيجو && (نتيجو != CURLE_AGAIN))
        أرجع نتيجو;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      إذا(!ws->sendbuf_payload) {
        أرجع CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    نتيجو = ws_flush(معلومات, ws, Curl_api_is_اندريون_callback(معلومات));
    إذا(!نتيجو && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      بفر += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    وإلا إذا(نتيجو == CURLE_AGAIN) {
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
        CURL_TRC_WS(معلومات, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        أرجع CURLE_AGAIN;
      }
    }
    وإلا
      أرجع نتيجو;  /* real error sending the data */
  }
  أرجع CURLE_OK;
}

بنية cr_ws_ctx {
  بنية Curl_creader super;
  ٻٽو_انگ(read_eos);  /* we read an EOS from the next reader */
  ٻٽو_انگ(eos);       /* we have returned an EOS */
};

ساكن CURLcode cr_ws_init(بنية Curl_easy *معلومات, بنية Curl_creader *reader)
{
  (فراغ)معلومات;
  (فراغ)reader;
  أرجع CURLE_OK;
}

ساكن فراغ cr_ws_close(بنية Curl_easy *معلومات, بنية Curl_creader *reader)
{
  (فراغ)معلومات;
  (فراغ)reader;
}

ساكن CURLcode cr_ws_read(بنية Curl_easy *معلومات,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_ws_ctx *ctx = reader->ctx;
  CURLcode نتيجو = CURLE_OK;
  size_t nread, n;
  بنية websocket *ws;
  bool eos;

  *pnread = 0;
  إذا(ctx->eos) {
    *peos = TRUE;
    أرجع CURLE_OK;
  }

  ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(معلومات, "[WS] not a websocket transfer");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(Curl_bufq_is_خالي_جايون(&ws->sendbuf)) {
    إذا(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      أرجع CURLE_OK;
    }

    إذا(ws->enc.payload_remain) {
      CURL_TRC_WS(معلومات, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    نتيجو = Curl_creader_read(معلومات, reader->ايندڙ, buf, blen, &nread, &eos);
    إذا(نتيجو)
      أرجع نتيجو;
    ctx->read_eos = eos;

    إذا(!Curl_bufq_is_خالي_جايون(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(معلومات, reader->ايندڙ);
    }
    وإلا إذا(!nread) {
      /* nothing to convert, return this right away */
      إذا(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      اذهب ٻاهريون;
    }

    إذا(!ws->enc.payload_remain && Curl_bufq_is_خالي_جايون(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      نتيجو = ws_enc_write_سر(معلومات, ws, &ws->enc, CURLWS_ٻٽو, nread,
                                 &ws->sendbuf);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }

    نتيجو = ws_enc_write_payload(&ws->enc, معلومات, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    إذا(نتيجو)
      اذهب ٻاهريون;
    CURL_TRC_READ(معلومات, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_خالي_جايون(&ws->sendbuf));
  *peos = FALSE;
  نتيجو = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  إذا(!نتيجو && ctx->read_eos && Curl_bufq_is_خالي_جايون(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

ٻاهريون:
  CURL_TRC_READ(معلومات, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)نتيجو, *pnread, *peos);
  أرجع نتيجو;
}

ساكن curl_off_t cr_ws_ڪل_ڊيگهائي(بنية Curl_easy *معلومات,
                                     بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)معلومات;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_ڪل_ڊيگهائي,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورو,
  حجم(بنية cr_ws_ctx)
};

بنية wsfield {
  ثابت حرف *name;
  ثابت حرف *val;
};

CURLcode Curl_ws_request(بنية Curl_easy *معلومات, بنية dynbuf *req)
{
  بنانشان صحيح i;
  CURLcode نتيجو = CURLE_OK;
  uint8_t rand[16];
  حرف *randstr;
  size_t randlen;
  حرف keyval[40];
  بنية SingleRequest *k = &معلومات->req;
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
  نتيجو = Curl_rand(معلومات, rand, حجم(rand));
  إذا(نتيجو)
    أرجع نتيجو;
  نتيجو = curlx_base64_encode(rand, حجم(rand), &randstr, &randlen);
  إذا(نتيجو)
    أرجع نتيجو;
  DEBUGASSERT(randlen < حجم(keyval));
  إذا(randlen >= حجم(keyval)) {
    curlx_free(randstr);
    أرجع CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, حجم(keyval), randstr, randlen);
  curlx_free(randstr);
  لكل(i = 0; !نتيجو && (i < CURL_ARRAYSIZE(heads)); i++) {
    إذا(!Curl_checkheaders(معلومات, heads[i].name, لڙي_ڊيگهه(heads[i].name))) {
      نتيجو = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  معلومات->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  معلومات->conn->بٽ.upgrade_اندريون_ترقي = TRUE;
  أرجع نتيجو;
}

ساكن فراغ ws_conn_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  بنية websocket *ws = داخلا_2;
  (فراغ)ڪنجي;
  (فراغ)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(بنية Curl_easy *معلومات,
                        ثابت حرف *mem, size_t nread)
{
  بنية SingleRequest *k = &معلومات->req;
  بنية websocket *ws;
  بنية Curl_cwriter *ws_dec_writer = عدم;
  بنية Curl_creader *ws_enc_reader = عدم;
  CURLcode نتيجو;

  DEBUGASSERT(معلومات->conn);
  ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    size_t chunk_سائيز = WS_CHUNK_سائيز;
    ws = curlx_calloc(1, حجم(*ws));
    إذا(!ws)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
    {
      ثابت حرف *p = getenv("CURL_WS_CHUNK_SIZE");
      إذا(p) {
        curl_off_t l;
        إذا(!curlx_str_عدد(&p, &l, 1 * 1024 * 1024))
          chunk_سائيز = (size_t)l;
      }
    }
#جيڪڏهن_پورو
    CURL_TRC_WS(معلومات, "WS, using chunk size %zu", chunk_سائيز);
    Curl_bufq_init2(&ws->recvbuf, chunk_سائيز, WS_CHUNK_ڳڻپ,
                    BUFQ_OPT_SOFT_حد_بندي);
    Curl_bufq_init2(&ws->sendbuf, chunk_سائيز, WS_CHUNK_ڳڻپ,
                    BUFQ_OPT_SOFT_حد_بندي);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    نتيجو = Curl_conn_meta_سيٽ(معلومات->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    إذا(نتيجو)
      أرجع نتيجو;
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

  infof(معلومات, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  نتيجو = Curl_cwriter_create(&ws_dec_writer, معلومات, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  إذا(نتيجو)
    اذهب ٻاهريون;
  نتيجو = Curl_cwriter_جوڙيو(معلومات, ws_dec_writer);
  إذا(نتيجو)
    اذهب ٻاهريون;
  ws_dec_writer = عدم; /* owned by transfer now */

  k->سرو = FALSE; /* we will not get more response headers */

  إذا(معلومات->سيٽ.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    إذا(nread) {
      نتيجو = Curl_cf_recvbuf_جوڙيو(معلومات, معلومات->conn, FIRSTSOCKET,
                                   (ثابت uint8_t *)mem, nread);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
    CURL_REQ_CLEAR_RECV(معلومات); /* read no more content */
  }
  وإلا { /* !connect_only */
    إذا(معلومات->سيٽ.طريقو == HTTPREQ_PUT) {
      CURL_TRC_WS(معلومات, "UPLOAD set, add ws-encode reader");
      نتيجو = Curl_creader_سيٽ_fread(معلومات, -1);
      إذا(نتيجو)
        اذهب ٻاهريون;

      إذا(!معلومات->سيٽ.ws_raw_گهڻائي) {
        /* Add our client reader encoding WS BINARY frames */
        نتيجو = Curl_creader_create(&ws_enc_reader, معلومات, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        إذا(نتيجو)
          اذهب ٻاهريون;
        نتيجو = Curl_creader_جوڙيو(معلومات, ws_enc_reader);
        إذا(نتيجو)
          اذهب ٻاهريون;
        ws_enc_reader = عدم; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      معلومات->req.eos_read = FALSE;
      معلومات->req.upload_پورو = FALSE;
      CURL_REQ_سيٽ_SEND(معلومات);
    }

    /* Then pass any additional data to the writers */
    إذا(nread) {
      نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, mem, nread);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->سرو = FALSE; /* we will not get more responses */

ٻاهريون:
  إذا(ws_dec_writer)
    Curl_cwriter_free(معلومات, ws_dec_writer);
  إذا(ws_enc_reader)
    Curl_creader_free(معلومات, ws_enc_reader);
  إذا(نتيجو)
    CURL_TRC_WS(معلومات, "Curl_ws_accept() failed -> %d", (صحيح)نتيجو);
  وإلا
    CURL_TRC_WS(معلومات, "websocket established, %s mode",
                معلومات->سيٽ.connect_only ? "connect-only" : "callback");
  أرجع نتيجو;
}

بنية ws_collect {
  بنية Curl_easy *معلومات;
  بنية websocket *ws;
  uint8_t *بفر;
  size_t buflen;
  size_t bufidx;
  صحيح چوکٺو_age;
  صحيح چوکٺو_flags;
  curl_off_t payload_وٿي;
  curl_off_t payload_len;
  bool written;
};

ساكن CURLcode ws_client_collect(ثابت uint8_t *buf, size_t buflen,
                                  صحيح چوکٺو_age, صحيح چوکٺو_flags,
                                  curl_off_t payload_وٿي,
                                  curl_off_t payload_len,
                                  فراغ *userp,
                                  size_t *pnwritten)
{
  بنية ws_collect *ctx = userp;
  بنية Curl_easy *معلومات = ctx->معلومات;
  bool auto_pong = !معلومات->سيٽ.ws_raw_گهڻائي && !معلومات->سيٽ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode نتيجو = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_وٿي, buflen);
  إذا(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  إذا(!ctx->bufidx) {
    /* first write */
    ctx->چوکٺو_age = چوکٺو_age;
    ctx->چوکٺو_flags = چوکٺو_flags;
    ctx->payload_وٿي = payload_وٿي;
    ctx->payload_len = payload_len;
  }

  إذا(auto_pong && (چوکٺو_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(معلومات, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_وٿي, payload_len);
    /* send back the exact same content as a PONG */
    نتيجو = ws_enc_جوڙيو_cntrl(ctx->معلومات, ctx->ws, buf, buflen, CURLWS_PONG);
    إذا(نتيجو)
      أرجع نتيجو;
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
    يادگيري_نقل(ctx->بفر + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  أرجع نتيجو;
}

ساكن CURLcode nw_اندريون_recv(فراغ *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  بنية Curl_easy *معلومات = reader_ctx;
  أرجع Curl_easy_recv(معلومات, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, فراغ *بفر,
                      size_t buflen, size_t *recv,
                      ثابت بنية curl_ws_چوکٺو **metap)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_recv, &نتيجو)) {
    بنية Curl_easy *معلومات = curl;
    بنية connectdata *conn;
    بنية websocket *ws;
    بنية ws_collect ctx;

    إذا(!recv || !metap || (buflen && !بفر)) {
      نتيجو = CURLE_BAD_FUNCTION_دليل;
      اذهب ٻاهريون;
    }

    *recv = 0;
    *metap = عدم;

    conn = معلومات->conn;
    إذا(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      إذا(!معلومات->سيٽ.connect_only) {
        failf(معلومات, "[WS] CONNECT_ONLY is required");
        نتيجو = CURLE_UNSUPPORTED_PROTOCOL;
        اذهب ٻاهريون;
      }

      Curl_getconnectinfo(معلومات, &conn);
      إذا(!conn) {
        failf(معلومات, "[WS] connection not found");
        نتيجو = CURLE_BAD_FUNCTION_دليل;
        اذهب ٻاهريون;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(معلومات, "[WS] connection is not setup for websocket");
      نتيجو = CURLE_BAD_FUNCTION_دليل;
      اذهب ٻاهريون;
    }

    memset(&ctx, 0, حجم(ctx));
    ctx.معلومات = معلومات;
    ctx.ws = ws;
    ctx.بفر = بفر;
    ctx.buflen = buflen;

    طالما(1) {
      /* receive more when our buffer is empty */
      إذا(Curl_bufq_is_خالي_جايون(&ws->recvbuf)) {
        size_t n;
        نتيجو = Curl_bufq_slurp(&ws->recvbuf, nw_اندريون_recv, معلومات, &n);
        إذا(نتيجو)
          اذهب ٻاهريون;
        وإلا إذا(n == 0) {
          /* connection closed */
          infof(معلومات, "[WS] connection expectedly closed?");
          نتيجو = CURLE_GOT_NOTHING;
          اذهب ٻاهريون;
        }
        CURL_TRC_WS(معلومات, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      نتيجو = ws_dec_pass(&ws->dec, معلومات, &ws->recvbuf,
                           ws_client_collect, &ctx);
      إذا(نتيجو == CURLE_AGAIN) {
        إذا(!ctx.written) {
          ws_dec_info(&ws->dec, معلومات, "need more input");
          استمر;  /* nothing written, try more input */
        }
        اكسر;
      }
      وإلا إذا(نتيجو) {
        اذهب ٻاهريون;
      }
      وإلا إذا(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        اكسر;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.چوکٺو_age, ctx.چوکٺو_flags, ctx.payload_وٿي,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(معلومات, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.وٿي,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    إذا(!معلومات->سيٽ.ws_raw_گهڻائي && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_جوڙيو_pending(معلومات, ws);
      إذا(!r2)
        (فراغ)ws_flush(معلومات, ws, Curl_api_is_اندريون_callback(معلومات));
    }
    نتيجو = CURLE_OK;
  }
ٻاهريون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}

ساكن CURLcode ws_flush(بنية Curl_easy *معلومات, بنية websocket *ws,
                         bool blocking)
{
  CURLcode نتيجو;

  /* If there is space, add any pending control frame */
  إذا(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_سائيز) {
    نتيجو = ws_enc_جوڙيو_pending(معلومات, ws);
    إذا(نتيجو && (نتيجو != CURLE_AGAIN))
      أرجع نتيجو;
  }

  إذا(!Curl_bufq_is_خالي_جايون(&ws->sendbuf)) {
    ثابت uint8_t *ٻاهريون;
    size_t outlen, n;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_ايندڙ = FALSE;
    size_t chunk_egain = 0;
    ثابت حرف *p = getenv("CURL_WS_CHUNK_EAGAIN");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#جيڪڏهن_پورو

    طالما(Curl_bufq_peek(&ws->sendbuf, &ٻاهريون, &outlen)) {
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
      إذا(eagain_ايندڙ)
        أرجع CURLE_AGAIN;
      إذا(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_ايندڙ = TRUE;
      }
#جيڪڏهن_پورو
      إذا(blocking) {
        نتيجو = ws_send_raw_blocking(معلومات, ws, (ثابت حرف *)ٻاهريون, outlen);
        n = نتيجو ? 0 : outlen;
      }
      وإلا إذا(معلومات->سيٽ.connect_only || Curl_api_is_اندريون_callback(معلومات))
        نتيجو = Curl_senddata(معلومات, ٻاهريون, outlen, &n);
      وإلا {
        نتيجو = Curl_xfer_send(معلومات, ٻاهريون, outlen, FALSE, &n);
        إذا(!نتيجو && !n && outlen)
          نتيجو = CURLE_AGAIN;
      }

      إذا(نتيجو == CURLE_AGAIN) {
        CURL_TRC_WS(معلومات, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        أرجع نتيجو;
      }
      وإلا إذا(نتيجو) {
        failf(معلومات, "[WS] flush, write error %d", (صحيح)نتيجو);
        أرجع نتيجو;
      }
      وإلا {
        CURL_TRC_WS(معلومات, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ws_send_raw_blocking(بنية Curl_easy *معلومات,
                                     بنية websocket *ws,
                                     ثابت حرف *بفر, size_t buflen)
{
  CURLcode نتيجو = CURLE_OK;
  size_t nwritten;

  إذا(!معلومات)
    أرجع نتيجو;

  (فراغ)ws;
  طالما(buflen) {
    نتيجو = Curl_xfer_send(معلومات, بفر, buflen, FALSE, &nwritten);
    إذا(نتيجو)
      أرجع نتيجو;
    DEBUGASSERT(nwritten <= buflen);
    بفر += nwritten;
    buflen -= nwritten;
    إذا(buflen) {
      curl_socket_t sock = معلومات->conn->sock[FIRSTSOCKET];
      timediff_t کاٻو_ms;
      صحيح ev;

      CURL_TRC_WS(معلومات, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      کاٻو_ms = Curl_timeleft_ms(معلومات);
      إذا(کاٻو_ms < 0) {
        failf(معلومات, "[WS] Timeout waiting for socket becoming writable");
        أرجع CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      إذا(sock == CURL_SOCKET_BAD)
        أرجع CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, کاٻو_ms ? کاٻو_ms : 500);
      إذا(ev < 0) {
        failf(معلومات, "[WS] Error while waiting for socket becoming writable");
        أرجع CURLE_SEND_ERROR;
      }
    }
  }
  أرجع نتيجو;
}

ساكن CURLcode ws_send_raw(بنية Curl_easy *معلومات, ثابت فراغ *بفر,
                            size_t buflen, size_t *pnwritten)
{
  بنية websocket *ws;
  CURLcode نتيجو;

  ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(معلومات, "[WS] Not a websocket transfer");
    أرجع CURLE_SEND_ERROR;
  }
  إذا(!buflen)
    أرجع CURLE_OK;

  إذا(Curl_api_is_اندريون_callback(معلومات)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    نتيجو = ws_flush(معلومات, ws, TRUE);
    إذا(نتيجو)
      أرجع نتيجو;
    نتيجو = ws_send_raw_blocking(معلومات, ws, بفر, buflen);
    إذا(!نتيجو)
      *pnwritten = buflen;
  }
  وإلا {
    /* We need any pending data to be sent or EAGAIN this call. */
    نتيجو = ws_flush(معلومات, ws, FALSE);
    إذا(نتيجو)
      أرجع نتيجو;
    نتيجو = Curl_senddata(معلومات, بفر, buflen, pnwritten);
  }

  CURL_TRC_WS(معلومات, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (صحيح)نتيجو, *pnwritten);
  أرجع نتيجو;
}

CURLcode curl_ws_send(CURL *curl, ثابت فراغ *بفر_arg,
                      size_t buflen, size_t *موڪليل,
                      curl_off_t fragsize,
                      بنانشان صحيح flags)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_send, &نتيجو)) {
    بنية websocket *ws;
    ثابت uint8_t *بفر = بفر_arg;
    بنية Curl_easy *معلومات = curl;
    size_t ndummy;
    size_t *pnsent = موڪليل ? موڪليل : &ndummy;

    CURL_TRC_WS(معلومات, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, معلومات->سيٽ.ws_raw_گهڻائي);

    *pnsent = 0;

    إذا(!بفر && buflen) {
      failf(معلومات, "[WS] buffer is NULL when buflen is not");
      نتيجو = CURLE_BAD_FUNCTION_دليل;
      اذهب ٻاهريون;
    }

    إذا(!معلومات->conn && معلومات->سيٽ.connect_only) {
      نتيجو = Curl_connect_only_attach(معلومات);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
    إذا(!معلومات->conn) {
      failf(معلومات, "[WS] No associated connection");
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
    ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(معلومات, "[WS] Not a websocket transfer");
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }

    إذا(معلومات->سيٽ.ws_raw_گهڻائي) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      نتيجو = ws_flush(معلومات, ws, FALSE);
      إذا(نتيجو)
        اذهب ٻاهريون;

      إذا(!بفر) {
        failf(معلومات, "[WS] buffer is NULL in raw mode");
        نتيجو = CURLE_BAD_FUNCTION_دليل;
        اذهب ٻاهريون;
      }
      إذا(!موڪليل) {
        failf(معلومات, "[WS] sent is NULL in raw mode");
        نتيجو = CURLE_BAD_FUNCTION_دليل;
        اذهب ٻاهريون;
      }
      إذا(fragsize || flags) {
        failf(معلومات, "[WS] fragsize and flags must be zero in raw mode");
        نتيجو = CURLE_BAD_FUNCTION_دليل;
        اذهب ٻاهريون;
      }
      نتيجو = ws_send_raw(معلومات, بفر, buflen, pnsent);
      اذهب ٻاهريون;
    }

    /* Not RAW mode, we do the frame encoding */
    نتيجو = ws_enc_send(معلومات, ws, بفر, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(معلومات, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, معلومات->سيٽ.ws_raw_گهڻائي, (صحيح)نتيجو,
                *pnsent);
  }
ٻاهريون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}

ساكن CURLcode ws_تياري_conn(بنية Curl_easy *معلومات,
                              بنية connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  معلومات->state.http_neg.accept_09 = FALSE;
  معلومات->state.http_neg.only_10 = FALSE;
  معلومات->state.http_neg.wanted = CURL_HTTP_V1x;
  معلومات->state.http_neg.allowed = CURL_HTTP_V1x;
  أرجع Curl_http_تياري_conn(معلومات, conn);
}

ثابت بنية curl_ws_چوکٺو *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  بنية Curl_easy *معلومات = curl;
  إذا(GOOD_EASY_HANDLE(معلومات) && Curl_api_is_اندريون_callback(معلومات) &&
     معلومات->conn && !معلومات->سيٽ.ws_raw_گهڻائي) {
    بنية websocket *ws;
    ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
    إذا(ws)
      أرجع &ws->recvframe;
  }
  أرجع عدم;
}

CURL_EXTERN CURLcode curl_ws_شروعات_چوکٺو(CURL *curl,
                                         بنانشان صحيح flags,
                                         curl_off_t چوکٺو_len)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_شروعات_چوکٺو, &نتيجو)) {
    بنية Curl_easy *معلومات = curl;
    بنية websocket *ws;

    إذا(معلومات->سيٽ.ws_raw_گهڻائي) {
      failf(معلومات, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      نتيجو = CURLE_FAILED_INIT;
      اذهب ٻاهريون;
    }

    CURL_TRC_WS(معلومات, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, چوکٺو_len);

    إذا(!معلومات->conn) {
      failf(معلومات, "[WS] No associated connection");
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
    ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(معلومات, "[WS] Not a websocket transfer");
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }

    إذا(ws->enc.payload_remain) {
      failf(معلومات, "[WS] previous frame not finished");
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }

    نتيجو = ws_enc_write_سر(معلومات, ws, &ws->enc, flags, چوکٺو_len,
                               &ws->sendbuf);
    إذا(نتيجو)
      CURL_TRC_WS(معلومات, "curl_start_frame(), error adding frame head %d",
                  (صحيح)نتيجو);
  }
ٻاهريون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}

ثابت بنية Curl_protocol Curl_protocol_ws = {
  ws_تياري_conn,                        /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_پورو,                       /* done */
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

CURLcode curl_ws_recv(CURL *curl, فراغ *بفر, size_t buflen,
                      size_t *recv,
                      ثابت بنية curl_ws_چوکٺو **metap)
{
  (فراغ)curl;
  (فراغ)بفر;
  (فراغ)buflen;
  (فراغ)recv;
  (فراغ)metap;
  أرجع CURLE_NOT_BUILT_اندريون;
}

CURLcode curl_ws_send(CURL *curl, ثابت فراغ *بفر,
                      size_t buflen, size_t *موڪليل,
                      curl_off_t fragsize,
                      بنانشان صحيح flags)
{
  (فراغ)curl;
  (فراغ)بفر;
  (فراغ)buflen;
  (فراغ)موڪليل;
  (فراغ)fragsize;
  (فراغ)flags;
  أرجع CURLE_NOT_BUILT_اندريون;
}

ثابت بنية curl_ws_چوکٺو *curl_ws_meta(CURL *معلومات)
{
  (فراغ)معلومات;
  أرجع عدم;
}

CURL_EXTERN CURLcode curl_ws_شروعات_چوکٺو(CURL *curl,
                                         بنانشان صحيح flags,
                                         curl_off_t چوکٺو_len)
{
  (فراغ)curl;
  (فراغ)flags;
  (فراغ)چوکٺو_len;
  أرجع CURLE_NOT_BUILT_اندريون;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_WEBSOCKETS */
