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

#جے_بنا_تعریف CURL_DISABLE_WEBSOCKETS

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
#جے_تعریف_شدہ CURLVERBOSE
#تعریف_کرو WSBIT_OPCODE_نقاب  0xf
#جے_انت

#تعریف_کرو WSBIT_نقاب 0x80

/* buffer dimensioning */
#تعریف_کرو WS_CHUNK_ناپ  65535
#تعریف_کرو WS_CHUNK_گنتی 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
تعداد ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_سر,
  WS_DEC_PAYLOAD
};

بنية ws_decoder {
  صحيح چوکھٹا_age;        /* zero */
  صحيح چوکھٹا_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_وتھ;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t سر[10];
  صحيح سر_len, سر_کل;
  تعداد ws_dec_state state;
  صحيح cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
بنية ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  بنانشانی صحيح xori; /* xor index */
  uint8_t نقاب[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  ثنائی_ہندسہ(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#تعریف_کرو WS_MAX_CNTRL_LEN    125

بنية ws_cntrl_چوکھٹا {
  بنانشانی صحيح type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
بنية websocket {
  بنية Curl_easy *انکڑے; /* used for write callback handling */
  بنية ws_decoder dec;  /* decode of ws frames */
  بنية ws_encoder enc;  /* encode of ws frames */
  بنية bufq recvbuf;    /* raw data from the server */
  بنية bufq sendbuf;    /* raw data to be sent to the server */
  بنية curl_ws_چوکھٹا recvframe;  /* the current WS FRAME received */
  بنية ws_cntrl_چوکھٹا pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  ثنائی_ہندسہ(using_reader);      /* sending WS data is handled via a client reader */
};

#جے_تعریف_شدہ CURLVERBOSE
ساكن ثابت حرف *ws_چوکھٹا_name_دا_op(uint8_t firstbyte)
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
#جے_انت

ساكن صحيح ws_چوکھٹا_firstbyte2flags(بنية Curl_easy *انکڑے,
                                    uint8_t firstbyte, صحيح cont_flags)
{
  بدل(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  حالة WSBIT_OPCODE_CONT:
    إذا(!(cont_flags & CURLWS_CONT)) {
      failf(انکڑے, "[WS] no ongoing fragmented message to resume");
      أرجع 0;
    }
    أرجع cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  حالة (WSBIT_OPCODE_CONT | WSBIT_FIN):
    إذا(!(cont_flags & CURLWS_CONT)) {
      failf(انکڑے, "[WS] no ongoing fragmented message to resume");
      أرجع 0;
    }
    أرجع cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  حالة WSBIT_OPCODE_متن:
    إذا(cont_flags & CURLWS_CONT) {
      failf(انکڑے, "[WS] fragmented message interrupted by new TEXT msg");
      أرجع 0;
    }
    أرجع CURLWS_متن | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  حالة (WSBIT_OPCODE_متن | WSBIT_FIN):
    إذا(cont_flags & CURLWS_CONT) {
      failf(انکڑے, "[WS] fragmented message interrupted by new TEXT msg");
      أرجع 0;
    }
    أرجع CURLWS_متن;
  /* 0x02 - first BINARY fragment */
  حالة WSBIT_OPCODE_BIN:
    إذا(cont_flags & CURLWS_CONT) {
      failf(انکڑے, "[WS] fragmented message interrupted by new BINARY msg");
      أرجع 0;
    }
    أرجع CURLWS_ثنائی | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  حالة (WSBIT_OPCODE_BIN | WSBIT_FIN):
    إذا(cont_flags & CURLWS_CONT) {
      failf(انکڑے, "[WS] fragmented message interrupted by new BINARY msg");
      أرجع 0;
    }
    أرجع CURLWS_ثنائی;
  /* 0x08 - first CLOSE fragment */
  حالة WSBIT_OPCODE_CLOSE:
    failf(انکڑے, "[WS] invalid fragmented CLOSE frame");
    أرجع 0;
  /* 0x88 - unfragmented CLOSE */
  حالة (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    أرجع CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  حالة WSBIT_OPCODE_PING:
    failf(انکڑے, "[WS] invalid fragmented PING frame");
    أرجع 0;
  /* 0x89 - unfragmented PING */
  حالة (WSBIT_OPCODE_PING | WSBIT_FIN):
    أرجع CURLWS_PING;
  /* 0x0a - first PONG fragment */
  حالة WSBIT_OPCODE_PONG:
    failf(انکڑے, "[WS] invalid fragmented PONG frame");
    أرجع 0;
  /* 0x8a - unfragmented PONG */
  حالة (WSBIT_OPCODE_PONG | WSBIT_FIN):
    أرجع CURLWS_PONG;
  /* invalid first byte */
  افتراضي:
    إذا(firstbyte & WSBIT_RSV_نقاب)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(انکڑے, "[WS] invalid reserved bits: %02x", firstbyte);
    وإلا
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(انکڑے, "[WS] invalid opcode: %02x", firstbyte);
    أرجع 0;
  }
}

ساكن CURLcode ws_چوکھٹا_flags2firstbyte(بنية Curl_easy *انکڑے,
                                         بنانشانی صحيح flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  بدل(flags & ~CURLWS_وتھ) {
  حالة 0:
    إذا(contfragment) {
      CURL_TRC_WS(انکڑے, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      أرجع CURLE_OK;
    }
    failf(انکڑے, "[WS] no flags given");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  حالة CURLWS_CONT:
    إذا(contfragment) {
      infof(انکڑے, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      أرجع CURLE_OK;
    }
    failf(انکڑے, "[WS] No ongoing fragmented message to continue");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  حالة CURLWS_متن:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_متن | WSBIT_FIN);
    أرجع CURLE_OK;
  حالة (CURLWS_متن | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_متن;
    أرجع CURLE_OK;
  حالة CURLWS_ثنائی:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    أرجع CURLE_OK;
  حالة (CURLWS_ثنائی | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    أرجع CURLE_OK;
  حالة CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_CLOSE | CURLWS_CONT):
    failf(انکڑے, "[WS] CLOSE frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  حالة CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_PING | CURLWS_CONT):
    failf(انکڑے, "[WS] PING frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  حالة CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_PONG | CURLWS_CONT):
    failf(انکڑے, "[WS] PONG frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  افتراضي:
    failf(انکڑے, "[WS] unknown flags: %x", flags);
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
}

ساكن فراغ ws_dec_info(بنية ws_decoder *dec, بنية Curl_easy *انکڑے,
                        ثابت حرف *پیغام)
{
  NOVERBOSE((فراغ)پیغام);
  بدل(dec->سر_len) {
  حالة 0:
    اكسر;
  حالة 1:
    CURL_TRC_WS(انکڑے, "decoded %s [%s%s]", پیغام,
                ws_چوکھٹا_name_دا_op(dec->سر[0]),
                (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    اكسر;
  افتراضي:
    إذا(dec->سر_len < dec->سر_کل) {
      CURL_TRC_WS(انکڑے, "decoded %s [%s%s](%d/%d)", پیغام,
                  ws_چوکھٹا_name_دا_op(dec->سر[0]),
                  (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->سر_len, dec->سر_کل);
    }
    وإلا {
      CURL_TRC_WS(انکڑے, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  پیغام, ws_چوکھٹا_name_دا_op(dec->سر[0]),
                  (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_وتھ, dec->payload_len);
    }
    اكسر;
  }
}

ساكن CURLcode ws_send_raw_blocking(بنية Curl_easy *انکڑے,
                                     بنية websocket *ws,
                                     ثابت حرف *بفر, size_t buflen);

تعريفنوع CURLcode ws_write_payload(ثابت uint8_t *buf, size_t buflen,
                                  صحيح چوکھٹا_age, صحيح چوکھٹا_flags,
                                  curl_off_t payload_وتھ,
                                  curl_off_t payload_len,
                                  فراغ *userp,
                                  size_t *pnwritten);

ساكن فراغ ws_dec_اگلا_چوکھٹا(بنية ws_decoder *dec)
{
  dec->چوکھٹا_age = 0;
  dec->چوکھٹا_flags = 0;
  dec->payload_وتھ = 0;
  dec->payload_len = 0;
  dec->سر_len = dec->سر_کل = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ساكن فراغ ws_dec_reset(بنية ws_decoder *dec)
{
  dec->چوکھٹا_age = 0;
  dec->چوکھٹا_flags = 0;
  dec->payload_وتھ = 0;
  dec->payload_len = 0;
  dec->سر_len = dec->سر_کل = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

ساكن فراغ ws_dec_init(بنية ws_decoder *dec)
{
  ws_dec_reset(dec);
}

ساكن CURLcode ws_dec_read_سر(بنية ws_decoder *dec,
                                 بنية Curl_easy *انکڑے,
                                 بنية bufq *inraw)
{
  ثابت uint8_t *inbuf;
  size_t inlen;

  طالما(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    إذا(dec->سر_len == 0) {
      dec->سر[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->چوکھٹا_flags = ws_چوکھٹا_firstbyte2flags(انکڑے, dec->سر[0],
                                                  dec->cont_flags);
      إذا(!dec->چوکھٹا_flags) {
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      إذا(dec->چوکھٹا_flags & (CURLWS_متن | CURLWS_ثنائی)) {
        dec->cont_flags = dec->چوکھٹا_flags;
      }

      dec->سر_len = 1;
#إذا 0
      ws_dec_info(dec, انکڑے, "seeing opcode");
#جے_انت
      استمر;
    }
    وإلا إذا(dec->سر_len == 1) {
      dec->سر[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->سر_len = 2;

      إذا(dec->سر[1] & WSBIT_نقاب) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(انکڑے, "[WS] masked input frame");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->چوکھٹا_flags & CURLWS_PING && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(انکڑے, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->چوکھٹا_flags & CURLWS_PONG && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(انکڑے, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->چوکھٹا_flags & CURLWS_CLOSE && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        failf(انکڑے, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      إذا(dec->سر[1] == 126) {
        dec->سر_کل = 4;
        استمر;
      }
      وإلا إذا(dec->سر[1] == 127) {
        dec->سر_کل = 10;
        استمر;
      }
      وإلا {
        dec->سر_کل = 2;
      }
    }

    إذا(dec->سر_len < dec->سر_کل) {
      dec->سر[dec->سر_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->سر_len;
      إذا(dec->سر_len < dec->سر_کل) {
#إذا 0
        ws_dec_info(dec, انکڑے, "decoding head");
#جے_انت
        استمر;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->سر_len == dec->سر_کل);
    بدل(dec->سر_کل) {
    حالة 2:
      dec->payload_len = dec->سر[1];
      اكسر;
    حالة 4:
      dec->payload_len = (dec->سر[2] << 8) | dec->سر[3];
      اكسر;
    حالة 10:
      إذا(dec->سر[2] > 127) {
        failf(انکڑے, "[WS] frame length longer than 63 bits not supported");
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
      failf(انکڑے, "[WS] unexpected frame header length");
      أرجع CURLE_RECV_ERROR;
    }

    dec->چوکھٹا_age = 0;
    dec->payload_وتھ = 0;
    ws_dec_info(dec, انکڑے, "head");
    أرجع CURLE_OK;
  }
  أرجع CURLE_AGAIN;
}

ساكن CURLcode ws_dec_pass_payload(بنية ws_decoder *dec,
                                    بنية Curl_easy *انکڑے,
                                    بنية bufq *inraw,
                                    ws_write_payload *write_cb,
                                    فراغ *write_ctx)
{
  ثابت uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode نتیجہ;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_وتھ,
                                     0, SIZE_MAX);

  طالما(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(انکڑے)) {
    إذا(inlen > remain)
      inlen = remain;
    نتیجہ = write_cb(inbuf, inlen, dec->چوکھٹا_age, dec->چوکھٹا_flags,
                      dec->payload_وتھ, dec->payload_len,
                      write_ctx, &nwritten);
    إذا(نتیجہ)
      أرجع نتیجہ;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_وتھ += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_وتھ,
                                0, SIZE_MAX);
    CURL_TRC_WS(انکڑے, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  أرجع remain ? CURLE_AGAIN : CURLE_OK;
}

ساكن CURLcode ws_dec_pass(بنية ws_decoder *dec,
                            بنية Curl_easy *انکڑے,
                            بنية bufq *inraw,
                            ws_write_payload *write_cb,
                            فراغ *write_ctx)
{
  CURLcode نتیجہ;

  إذا(Curl_bufq_is_خالی_تھاواں(inraw))
    أرجع CURLE_AGAIN;

  بدل(dec->state) {
  حالة WS_DEC_INIT:
    ws_dec_اگلا_چوکھٹا(dec);
    dec->state = WS_DEC_سر;
    FALLTHROUGH();
  حالة WS_DEC_سر:
    نتیجہ = ws_dec_read_سر(dec, انکڑے, inraw);
    إذا(نتیجہ) {
      إذا(نتیجہ != CURLE_AGAIN) {
        failf(انکڑے, "[WS] decode frame error %d", (صحيح)نتیجہ);
        اكسر;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_خالی_تھاواں(inraw));
      اكسر;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    إذا(dec->payload_len == 0) {
      size_t nwritten;
      ثابت uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      نتیجہ = write_cb(&tmp, 0, dec->چوکھٹا_age, dec->چوکھٹا_flags,
                        0, 0, write_ctx, &nwritten);
      إذا(نتیجہ)
        أرجع نتیجہ;
      dec->state = WS_DEC_INIT;
      اكسر;
    }
    FALLTHROUGH();
  حالة WS_DEC_PAYLOAD:
    نتیجہ = ws_dec_pass_payload(dec, انکڑے, inraw, write_cb, write_ctx);
    ws_dec_info(dec, انکڑے, "passing");
    إذا(نتیجہ)
      أرجع نتیجہ;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    اكسر;
  افتراضي:
    /* we covered all enums above, but some code analyzers are wimps */
    نتیجہ = CURLE_FAILED_INIT;
  }
  أرجع نتیجہ;
}

ساكن فراغ update_meta(بنية websocket *ws,
                        صحيح چوکھٹا_age, صحيح چوکھٹا_flags,
                        curl_off_t payload_وتھ,
                        curl_off_t payload_len,
                        size_t cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_وتھ - cur_len);

  ws->recvframe.age = چوکھٹا_age;
  ws->recvframe.flags = چوکھٹا_flags;
  ws->recvframe.وتھ = payload_وتھ;
  ws->recvframe.len = cur_len;
  ws->recvframe.bytesleft = bytesleft;
}

/* WebSocket decoding client writer */
بنية ws_cw_ctx {
  بنية Curl_cwriter super;
  بنية bufq buf;
};

ساكن CURLcode ws_cw_init(بنية Curl_easy *انکڑے,
                           بنية Curl_cwriter *writer)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  (فراغ)انکڑے;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_ناپ, 1, BUFQ_OPT_SOFT_حد_بندی);
  أرجع CURLE_OK;
}

ساكن فراغ ws_cw_close(بنية Curl_easy *انکڑے, بنية Curl_cwriter *writer)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  (فراغ)انکڑے;
  Curl_bufq_free(&ctx->buf);
}

بنية ws_cw_dec_ctx {
  بنية Curl_easy *انکڑے;
  بنية websocket *ws;
  بنية Curl_cwriter *اگلا_writer;
  صحيح cw_type;
};

ساكن CURLcode ws_flush(بنية Curl_easy *انکڑے, بنية websocket *ws,
                         bool blocking);
ساكن CURLcode ws_enc_جوڑو_pending(بنية Curl_easy *انکڑے,
                                   بنية websocket *ws);

ساكن CURLcode ws_enc_جوڑو_cntrl(بنية Curl_easy *انکڑے,
                                 بنية websocket *ws,
                                 ثابت uint8_t *payload,
                                 size_t plen,
                                 بنانشانی صحيح چوکھٹا_type)
{
  (فراغ)انکڑے;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  إذا(plen > WS_MAX_CNTRL_LEN)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = چوکھٹا_type;
  ws->pending.payload_len = plen;
  حافظہ_نقل(ws->pending.payload, payload, plen);
  أرجع CURLE_OK;
}

ساكن curl_off_t ws_payload_remain(curl_off_t payload_کل,
                                    curl_off_t payload_وتھ,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_کل - payload_وتھ;
  إذا((payload_کل < 0) || (payload_وتھ < 0) || (remain < 0))
    أرجع -1;
  buffered = curlx_uztoso(payload_buffered);
  إذا(remain < buffered)
    أرجع -1;
  أرجع remain - buffered;
}

ساكن CURLcode ws_cw_dec_اگلا(ثابت uint8_t *buf, size_t buflen,
                               صحيح چوکھٹا_age, صحيح چوکھٹا_flags,
                               curl_off_t payload_وتھ,
                               curl_off_t payload_len,
                               فراغ *user_انکڑے,
                               size_t *pnwritten)
{
  بنية ws_cw_dec_ctx *ctx = user_انکڑے;
  بنية Curl_easy *انکڑے = ctx->انکڑے;
  بنية websocket *ws = ctx->ws;
  bool auto_pong = !انکڑے->سیٹ.ws_raw_منوال && !انکڑے->سیٹ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode نتیجہ;

  (فراغ)چوکھٹا_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_وتھ, buflen);
  إذا(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  إذا(auto_pong && (چوکھٹا_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(انکڑے, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_وتھ, payload_len);
    /* send back the exact same content as a PONG */
    نتیجہ = ws_enc_جوڑو_cntrl(انکڑے, ws, buf, buflen, CURLWS_PONG);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  وإلا إذا(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, چوکھٹا_age, چوکھٹا_flags, payload_وتھ,
                payload_len, buflen);

    CURL_TRC_WRITE(انکڑے, "[WS] pass %zu decoded bytes", buflen);
    نتیجہ = Curl_cwriter_write(انکڑے, ctx->اگلا_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ثابت حرف *)buf, buflen);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  *pnwritten = buflen;
  أرجع CURLE_OK;
}

ساكن CURLcode ws_cw_write(بنية Curl_easy *انکڑے,
                            بنية Curl_cwriter *writer, صحيح type,
                            ثابت حرف *buf, size_t nbytes)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  بنية websocket *ws;
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_WRITE(انکڑے, "[WS] write(len=%zu, type=%d)", nbytes, type);
  إذا(!(type & CLIENTWRITE_BODY) || انکڑے->سیٹ.ws_raw_منوال)
    أرجع Curl_cwriter_write(انکڑے, writer->اگلا, type, buf, nbytes);

  ws = Curl_conn_meta_get(انکڑے->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(انکڑے, "[WS] not a websocket transfer");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(nbytes) {
    size_t nwritten;
    نتیجہ = Curl_bufq_write(&ctx->buf, (ثابت uint8_t *)buf,
                             nbytes, &nwritten);
    إذا(نتیجہ) {
      infof(انکڑے, "[WS] error adding data to buffer %d", (صحيح)نتیجہ);
      أرجع نتیجہ;
    }
  }

  نتیجہ = Curl_cwriter_flush(انکڑے, writer->اگلا);
  إذا(نتیجہ)
    اذهب باہرلا;

  طالما(!Curl_bufq_is_خالی_تھاواں(&ctx->buf) && !Curl_cwriter_is_paused(انکڑے)) {
    بنية ws_cw_dec_ctx pass_ctx;
    pass_ctx.انکڑے = انکڑے;
    pass_ctx.ws = ws;
    pass_ctx.اگلا_writer = writer->اگلا;
    pass_ctx.cw_type = type;
    نتیجہ = ws_dec_pass(&ws->dec, انکڑے, &ctx->buf,
                         ws_cw_dec_اگلا, &pass_ctx);
    إذا(نتیجہ == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      نتیجہ = CURLE_OK;
      اذهب باہرلا;
    }
    وإلا إذا(نتیجہ) {
      failf(انکڑے, "[WS] decode payload error %d", (صحيح)نتیجہ);
      Curl_bufq_reset(&ctx->buf);
      اذهب باہرلا;
    }
  }

باہرلا:
  إذا(!نتیجہ && !ws->using_reader) {
    نتیجہ = ws_flush(انکڑے, ws, Curl_api_is_وچ_callback(انکڑے));
    إذا(نتیجہ == CURLE_AGAIN)
      نتیجہ = CURLE_OK;
  }

  إذا(!نتیجہ && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_خالی_تھاواں(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(انکڑے, "[WS] decode ended with incomplete frame");
    نتیجہ = CURLE_RECV_ERROR;
  }
  أرجع نتیجہ;
}

ساكن CURLcode ws_cw_flush(بنية Curl_easy *انکڑے,
                            بنية Curl_cwriter *writer)
{
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_WRITE(انکڑے, "[ws] flush");
  إذا(!انکڑے->سیٹ.ws_raw_منوال) {
    بنية ws_cw_ctx *ctx = writer->ctx;
    بنية websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    نتیجہ = Curl_cwriter_flush(انکڑے, writer->اگلا);
    إذا(نتیجہ)
      اذهب باہرلا;

    ws = Curl_conn_meta_get(انکڑے->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(انکڑے, "[WS] not a websocket transfer");
      أرجع CURLE_FAILED_INIT;
    }

    طالما(!Curl_bufq_is_خالی_تھاواں(&ctx->buf) && !Curl_cwriter_is_paused(انکڑے)) {
      بنية ws_cw_dec_ctx pass_ctx;
      pass_ctx.انکڑے = انکڑے;
      pass_ctx.ws = ws;
      pass_ctx.اگلا_writer = writer->اگلا;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      نتیجہ = ws_dec_pass(&ws->dec, انکڑے, &ctx->buf,
                           ws_cw_dec_اگلا, &pass_ctx);
      إذا(نتیجہ == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        نتیجہ = CURLE_OK;
        اذهب باہرلا;
      }
      وإلا إذا(نتیجہ) {
        failf(انکڑے, "[WS] decode payload error %d", (صحيح)نتیجہ);
        Curl_bufq_reset(&ctx->buf);
        اذهب باہرلا;
      }
    }
  }

باہرلا:
  إذا(!نتیجہ)
    نتیجہ = Curl_cwriter_flush(انکڑے, writer->اگلا);
  أرجع نتیجہ;
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

ساكن فراغ ws_enc_info(بنية ws_encoder *enc, بنية Curl_easy *انکڑے,
                        ثابت حرف *پیغام)
{
  NOVERBOSE((فراغ)enc);
  NOVERBOSE((فراغ)پیغام);
  CURL_TRC_WS(انکڑے, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              پیغام, ws_چوکھٹا_name_دا_op(enc->firstbyte),
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

ساكن CURLcode ws_enc_جوڑو_چوکھٹا(بنية Curl_easy *انکڑے,
                                 بنية ws_encoder *enc,
                                 بنانشانی صحيح flags,
                                 curl_off_t payload_len,
                                 بنية bufq *باہرلا)
{
  uint8_t firstb = 0;
  uint8_t سر[14];
  CURLcode نتیجہ;
  size_t hlen, nwritten;

  إذا(payload_len < 0) {
    failf(انکڑے, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    أرجع CURLE_SEND_ERROR;
  }

  إذا(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(انکڑے, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    أرجع CURLE_SEND_ERROR;
  }

  نتیجہ = ws_چوکھٹا_flags2firstbyte(انکڑے, flags, (bool)enc->contfragment,
                                    &firstb);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  إذا(flags & (CURLWS_متن | CURLWS_ثنائی)) {
    enc->contfragment = (curl_ثنائی_ہندسہ)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  إذا(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(انکڑے, "[WS] given PING frame is too big");
    أرجع CURLE_TOO_LARGE;
  }
  إذا(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(انکڑے, "[WS] given PONG frame is too big");
    أرجع CURLE_TOO_LARGE;
  }
  إذا(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(انکڑے, "[WS] given CLOSE frame is too big");
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
  ws_enc_info(enc, انکڑے, "sending");

  /* 4 bytes random */

  نتیجہ = Curl_rand(انکڑے, (uint8_t *)&enc->نقاب, حجم(enc->نقاب));
  إذا(نتیجہ)
    أرجع نتیجہ;

#جے_تعریف_شدہ DEBUGBUILD
  إذا(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->نقاب, 0, حجم(enc->نقاب));
#جے_انت

  /* add 4 bytes mask */
  حافظہ_نقل(&سر[hlen], &enc->نقاب, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  نتیجہ = Curl_bufq_write(باہرلا, سر, hlen, &nwritten);
  إذا(نتیجہ)
    أرجع نتیجہ;
  إذا(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ws_enc_write_سر(بنية Curl_easy *انکڑے,
                                  بنية websocket *ws,
                                  بنية ws_encoder *enc,
                                  بنانشانی صحيح flags,
                                  curl_off_t payload_len,
                                  بنية bufq *باہرلا)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  إذا(ws->pending.type) {
    CURLcode نتیجہ = ws_enc_جوڑو_pending(انکڑے, ws);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  أرجع ws_enc_جوڑو_چوکھٹا(انکڑے, enc, flags, payload_len, باہرلا);
}

ساكن CURLcode ws_enc_write_payload(بنية ws_encoder *enc,
                                     بنية Curl_easy *انکڑے,
                                     ثابت uint8_t *buf, size_t buflen,
                                     بنية bufq *باہرلا, size_t *pnwritten)
{
  CURLcode نتیجہ;
  size_t i, len, n, remain;

  *pnwritten = 0;
  إذا(Curl_bufq_is_بھریا(باہرلا))
    أرجع CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  إذا(remain < len)
    len = remain;

  لكل(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->نقاب[enc->xori];
    نتیجہ = Curl_bufq_write(باہرلا, &c, 1, &n);
    إذا(نتیجہ) {
      إذا((نتیجہ != CURLE_AGAIN) || !i)
        أرجع نتیجہ;
      اكسر;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, انکڑے, "buffered");
  أرجع CURLE_OK;
}

ساكن CURLcode ws_enc_جوڑو_pending(بنية Curl_easy *انکڑے,
                                   بنية websocket *ws)
{
  CURLcode نتیجہ;
  size_t n;

  إذا(!ws->pending.type) /* no pending frame here */
    أرجع CURLE_OK;
  إذا(ws->enc.payload_remain) /* in the middle of another frame */
    أرجع CURLE_AGAIN;

  نتیجہ = ws_enc_جوڑو_چوکھٹا(انکڑے, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  إذا(نتیجہ) {
    CURL_TRC_WS(انکڑے, "ws_enc_cntrl(), error adding head: %d",
                (صحيح)نتیجہ);
    اذهب باہرلا;
  }
  نتیجہ = ws_enc_write_payload(&ws->enc, انکڑے, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  إذا(نتیجہ) {
    CURL_TRC_WS(انکڑے, "ws_enc_cntrl(), error adding payload: %d",
                (صحيح)نتیجہ);
    اذهب باہرلا;
  }
  إذا(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(انکڑے, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, حجم(ws->pending));

باہرلا:
  أرجع نتیجہ;
}

ساكن CURLcode ws_enc_send(بنية Curl_easy *انکڑے,
                            بنية websocket *ws,
                            ثابت uint8_t *بفر,
                            size_t buflen,
                            curl_off_t fragsize,
                            بنانشانی صحيح flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(!انکڑے->سیٹ.ws_raw_منوال);
  *pnsent = 0;

  إذا(ws->enc.payload_remain || !Curl_bufq_is_خالی_تھاواں(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    إذا(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(انکڑے, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    إذا((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(انکڑے, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
  }
  وإلا {
    نتیجہ = ws_flush(انکڑے, ws, Curl_api_is_وچ_callback(انکڑے));
    إذا(نتیجہ)
      أرجع نتیجہ;

    نتیجہ = ws_enc_write_سر(انکڑے, ws, &ws->enc, flags,
                               (flags & CURLWS_وتھ) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    إذا(نتیجہ) {
      CURL_TRC_WS(انکڑے, "curl_ws_send(), error writing frame head %d",
                  (صحيح)نتیجہ);
      أرجع نتیجہ;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  طالما(!Curl_bufq_is_خالی_تھاواں(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    إذا(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      نتیجہ = ws_enc_write_payload(&ws->enc, انکڑے,
                                    بفر + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      إذا(نتیجہ && (نتیجہ != CURLE_AGAIN))
        أرجع نتیجہ;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      إذا(!ws->sendbuf_payload) {
        أرجع CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    نتیجہ = ws_flush(انکڑے, ws, Curl_api_is_وچ_callback(انکڑے));
    إذا(!نتیجہ && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      بفر += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    وإلا إذا(نتیجہ == CURLE_AGAIN) {
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
        CURL_TRC_WS(انکڑے, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        أرجع CURLE_AGAIN;
      }
    }
    وإلا
      أرجع نتیجہ;  /* real error sending the data */
  }
  أرجع CURLE_OK;
}

بنية cr_ws_ctx {
  بنية Curl_creader super;
  ثنائی_ہندسہ(read_eos);  /* we read an EOS from the next reader */
  ثنائی_ہندسہ(eos);       /* we have returned an EOS */
};

ساكن CURLcode cr_ws_init(بنية Curl_easy *انکڑے, بنية Curl_creader *reader)
{
  (فراغ)انکڑے;
  (فراغ)reader;
  أرجع CURLE_OK;
}

ساكن فراغ cr_ws_close(بنية Curl_easy *انکڑے, بنية Curl_creader *reader)
{
  (فراغ)انکڑے;
  (فراغ)reader;
}

ساكن CURLcode cr_ws_read(بنية Curl_easy *انکڑے,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_ws_ctx *ctx = reader->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread, n;
  بنية websocket *ws;
  bool eos;

  *pnread = 0;
  إذا(ctx->eos) {
    *peos = TRUE;
    أرجع CURLE_OK;
  }

  ws = Curl_conn_meta_get(انکڑے->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(انکڑے, "[WS] not a websocket transfer");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(Curl_bufq_is_خالی_تھاواں(&ws->sendbuf)) {
    إذا(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      أرجع CURLE_OK;
    }

    إذا(ws->enc.payload_remain) {
      CURL_TRC_WS(انکڑے, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    نتیجہ = Curl_creader_read(انکڑے, reader->اگلا, buf, blen, &nread, &eos);
    إذا(نتیجہ)
      أرجع نتیجہ;
    ctx->read_eos = eos;

    إذا(!Curl_bufq_is_خالی_تھاواں(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(انکڑے, reader->اگلا);
    }
    وإلا إذا(!nread) {
      /* nothing to convert, return this right away */
      إذا(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      اذهب باہرلا;
    }

    إذا(!ws->enc.payload_remain && Curl_bufq_is_خالی_تھاواں(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      نتیجہ = ws_enc_write_سر(انکڑے, ws, &ws->enc, CURLWS_ثنائی, nread,
                                 &ws->sendbuf);
      إذا(نتیجہ)
        اذهب باہرلا;
    }

    نتیجہ = ws_enc_write_payload(&ws->enc, انکڑے, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    إذا(نتیجہ)
      اذهب باہرلا;
    CURL_TRC_READ(انکڑے, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_خالی_تھاواں(&ws->sendbuf));
  *peos = FALSE;
  نتیجہ = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  إذا(!نتیجہ && ctx->read_eos && Curl_bufq_is_خالی_تھاواں(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

باہرلا:
  CURL_TRC_READ(انکڑے, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)نتیجہ, *pnread, *peos);
  أرجع نتیجہ;
}

ساكن curl_off_t cr_ws_کل_لمائی(بنية Curl_easy *انکڑے,
                                     بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)انکڑے;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_کل_لمائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورا,
  حجم(بنية cr_ws_ctx)
};

بنية wsfield {
  ثابت حرف *name;
  ثابت حرف *val;
};

CURLcode Curl_ws_request(بنية Curl_easy *انکڑے, بنية dynbuf *req)
{
  بنانشانی صحيح i;
  CURLcode نتیجہ = CURLE_OK;
  uint8_t rand[16];
  حرف *randstr;
  size_t randlen;
  حرف keyval[40];
  بنية SingleRequest *k = &انکڑے->req;
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
  نتیجہ = Curl_rand(انکڑے, rand, حجم(rand));
  إذا(نتیجہ)
    أرجع نتیجہ;
  نتیجہ = curlx_base64_encode(rand, حجم(rand), &randstr, &randlen);
  إذا(نتیجہ)
    أرجع نتیجہ;
  DEBUGASSERT(randlen < حجم(keyval));
  إذا(randlen >= حجم(keyval)) {
    curlx_free(randstr);
    أرجع CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, حجم(keyval), randstr, randlen);
  curlx_free(randstr);
  لكل(i = 0; !نتیجہ && (i < CURL_ARRAYSIZE(heads)); i++) {
    إذا(!Curl_checkheaders(انکڑے, heads[i].name, لڑی_لمبائی(heads[i].name))) {
      نتیجہ = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  انکڑے->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  انکڑے->conn->بٹ.upgrade_وچ_ترقی = TRUE;
  أرجع نتیجہ;
}

ساكن فراغ ws_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية websocket *ws = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(بنية Curl_easy *انکڑے,
                        ثابت حرف *mem, size_t nread)
{
  بنية SingleRequest *k = &انکڑے->req;
  بنية websocket *ws;
  بنية Curl_cwriter *ws_dec_writer = عدم;
  بنية Curl_creader *ws_enc_reader = عدم;
  CURLcode نتیجہ;

  DEBUGASSERT(انکڑے->conn);
  ws = Curl_conn_meta_get(انکڑے->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    size_t chunk_ناپ = WS_CHUNK_ناپ;
    ws = curlx_calloc(1, حجم(*ws));
    إذا(!ws)
      أرجع CURLE_باہرلا_دا_حافظہ;
#جے_تعریف_شدہ DEBUGBUILD
    {
      ثابت حرف *p = getenv("CURL_WS_CHUNK_SIZE");
      إذا(p) {
        curl_off_t l;
        إذا(!curlx_str_نمبر(&p, &l, 1 * 1024 * 1024))
          chunk_ناپ = (size_t)l;
      }
    }
#جے_انت
    CURL_TRC_WS(انکڑے, "WS, using chunk size %zu", chunk_ناپ);
    Curl_bufq_init2(&ws->recvbuf, chunk_ناپ, WS_CHUNK_گنتی,
                    BUFQ_OPT_SOFT_حد_بندی);
    Curl_bufq_init2(&ws->sendbuf, chunk_ناپ, WS_CHUNK_گنتی,
                    BUFQ_OPT_SOFT_حد_بندی);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    نتیجہ = Curl_conn_meta_سیٹ(انکڑے->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    إذا(نتیجہ)
      أرجع نتیجہ;
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

  infof(انکڑے, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  نتیجہ = Curl_cwriter_create(&ws_dec_writer, انکڑے, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  إذا(نتیجہ)
    اذهب باہرلا;
  نتیجہ = Curl_cwriter_جوڑو(انکڑے, ws_dec_writer);
  إذا(نتیجہ)
    اذهب باہرلا;
  ws_dec_writer = عدم; /* owned by transfer now */

  k->سرخی = FALSE; /* we will not get more response headers */

  إذا(انکڑے->سیٹ.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    إذا(nread) {
      نتیجہ = Curl_cf_recvbuf_جوڑو(انکڑے, انکڑے->conn, FIRSTSOCKET,
                                   (ثابت uint8_t *)mem, nread);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
    CURL_REQ_CLEAR_RECV(انکڑے); /* read no more content */
  }
  وإلا { /* !connect_only */
    إذا(انکڑے->سیٹ.طریقہ == HTTPREQ_PUT) {
      CURL_TRC_WS(انکڑے, "UPLOAD set, add ws-encode reader");
      نتیجہ = Curl_creader_سیٹ_fread(انکڑے, -1);
      إذا(نتیجہ)
        اذهب باہرلا;

      إذا(!انکڑے->سیٹ.ws_raw_منوال) {
        /* Add our client reader encoding WS BINARY frames */
        نتیجہ = Curl_creader_create(&ws_enc_reader, انکڑے, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        إذا(نتیجہ)
          اذهب باہرلا;
        نتیجہ = Curl_creader_جوڑو(انکڑے, ws_enc_reader);
        إذا(نتیجہ)
          اذهب باہرلا;
        ws_enc_reader = عدم; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      انکڑے->req.eos_read = FALSE;
      انکڑے->req.upload_پورا = FALSE;
      CURL_REQ_سیٹ_SEND(انکڑے);
    }

    /* Then pass any additional data to the writers */
    إذا(nread) {
      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, mem, nread);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->سرخی = FALSE; /* we will not get more responses */

باہرلا:
  إذا(ws_dec_writer)
    Curl_cwriter_free(انکڑے, ws_dec_writer);
  إذا(ws_enc_reader)
    Curl_creader_free(انکڑے, ws_enc_reader);
  إذا(نتیجہ)
    CURL_TRC_WS(انکڑے, "Curl_ws_accept() failed -> %d", (صحيح)نتیجہ);
  وإلا
    CURL_TRC_WS(انکڑے, "websocket established, %s mode",
                انکڑے->سیٹ.connect_only ? "connect-only" : "callback");
  أرجع نتیجہ;
}

بنية ws_collect {
  بنية Curl_easy *انکڑے;
  بنية websocket *ws;
  uint8_t *بفر;
  size_t buflen;
  size_t bufidx;
  صحيح چوکھٹا_age;
  صحيح چوکھٹا_flags;
  curl_off_t payload_وتھ;
  curl_off_t payload_len;
  bool written;
};

ساكن CURLcode ws_client_collect(ثابت uint8_t *buf, size_t buflen,
                                  صحيح چوکھٹا_age, صحيح چوکھٹا_flags,
                                  curl_off_t payload_وتھ,
                                  curl_off_t payload_len,
                                  فراغ *userp,
                                  size_t *pnwritten)
{
  بنية ws_collect *ctx = userp;
  بنية Curl_easy *انکڑے = ctx->انکڑے;
  bool auto_pong = !انکڑے->سیٹ.ws_raw_منوال && !انکڑے->سیٹ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode نتیجہ = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_وتھ, buflen);
  إذا(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  إذا(!ctx->bufidx) {
    /* first write */
    ctx->چوکھٹا_age = چوکھٹا_age;
    ctx->چوکھٹا_flags = چوکھٹا_flags;
    ctx->payload_وتھ = payload_وتھ;
    ctx->payload_len = payload_len;
  }

  إذا(auto_pong && (چوکھٹا_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(انکڑے, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_وتھ, payload_len);
    /* send back the exact same content as a PONG */
    نتیجہ = ws_enc_جوڑو_cntrl(ctx->انکڑے, ctx->ws, buf, buflen, CURLWS_PONG);
    إذا(نتیجہ)
      أرجع نتیجہ;
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
    حافظہ_نقل(ctx->بفر + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  أرجع نتیجہ;
}

ساكن CURLcode nw_وچ_recv(فراغ *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  بنية Curl_easy *انکڑے = reader_ctx;
  أرجع Curl_easy_recv(انکڑے, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, فراغ *بفر,
                      size_t buflen, size_t *recv,
                      ثابت بنية curl_ws_چوکھٹا **metap)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجہ = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_recv, &نتیجہ)) {
    بنية Curl_easy *انکڑے = curl;
    بنية connectdata *conn;
    بنية websocket *ws;
    بنية ws_collect ctx;

    إذا(!recv || !metap || (buflen && !بفر)) {
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      اذهب باہرلا;
    }

    *recv = 0;
    *metap = عدم;

    conn = انکڑے->conn;
    إذا(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      إذا(!انکڑے->سیٹ.connect_only) {
        failf(انکڑے, "[WS] CONNECT_ONLY is required");
        نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
        اذهب باہرلا;
      }

      Curl_getconnectinfo(انکڑے, &conn);
      إذا(!conn) {
        failf(انکڑے, "[WS] connection not found");
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        اذهب باہرلا;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(انکڑے, "[WS] connection is not setup for websocket");
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      اذهب باہرلا;
    }

    memset(&ctx, 0, حجم(ctx));
    ctx.انکڑے = انکڑے;
    ctx.ws = ws;
    ctx.بفر = بفر;
    ctx.buflen = buflen;

    طالما(1) {
      /* receive more when our buffer is empty */
      إذا(Curl_bufq_is_خالی_تھاواں(&ws->recvbuf)) {
        size_t n;
        نتیجہ = Curl_bufq_slurp(&ws->recvbuf, nw_وچ_recv, انکڑے, &n);
        إذا(نتیجہ)
          اذهب باہرلا;
        وإلا إذا(n == 0) {
          /* connection closed */
          infof(انکڑے, "[WS] connection expectedly closed?");
          نتیجہ = CURLE_GOT_NOTHING;
          اذهب باہرلا;
        }
        CURL_TRC_WS(انکڑے, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      نتیجہ = ws_dec_pass(&ws->dec, انکڑے, &ws->recvbuf,
                           ws_client_collect, &ctx);
      إذا(نتیجہ == CURLE_AGAIN) {
        إذا(!ctx.written) {
          ws_dec_info(&ws->dec, انکڑے, "need more input");
          استمر;  /* nothing written, try more input */
        }
        اكسر;
      }
      وإلا إذا(نتیجہ) {
        اذهب باہرلا;
      }
      وإلا إذا(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        اكسر;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.چوکھٹا_age, ctx.چوکھٹا_flags, ctx.payload_وتھ,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(انکڑے, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.وتھ,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    إذا(!انکڑے->سیٹ.ws_raw_منوال && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_جوڑو_pending(انکڑے, ws);
      إذا(!r2)
        (فراغ)ws_flush(انکڑے, ws, Curl_api_is_وچ_callback(انکڑے));
    }
    نتیجہ = CURLE_OK;
  }
باہرلا:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجہ;
}

ساكن CURLcode ws_flush(بنية Curl_easy *انکڑے, بنية websocket *ws,
                         bool blocking)
{
  CURLcode نتیجہ;

  /* If there is space, add any pending control frame */
  إذا(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_ناپ) {
    نتیجہ = ws_enc_جوڑو_pending(انکڑے, ws);
    إذا(نتیجہ && (نتیجہ != CURLE_AGAIN))
      أرجع نتیجہ;
  }

  إذا(!Curl_bufq_is_خالی_تھاواں(&ws->sendbuf)) {
    ثابت uint8_t *باہرلا;
    size_t outlen, n;
#جے_تعریف_شدہ DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_اگلا = FALSE;
    size_t chunk_egain = 0;
    ثابت حرف *p = getenv("CURL_WS_CHUNK_EAGAIN");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نمبر(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#جے_انت

    طالما(Curl_bufq_peek(&ws->sendbuf, &باہرلا, &outlen)) {
#جے_تعریف_شدہ DEBUGBUILD
      إذا(eagain_اگلا)
        أرجع CURLE_AGAIN;
      إذا(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_اگلا = TRUE;
      }
#جے_انت
      إذا(blocking) {
        نتیجہ = ws_send_raw_blocking(انکڑے, ws, (ثابت حرف *)باہرلا, outlen);
        n = نتیجہ ? 0 : outlen;
      }
      وإلا إذا(انکڑے->سیٹ.connect_only || Curl_api_is_وچ_callback(انکڑے))
        نتیجہ = Curl_senddata(انکڑے, باہرلا, outlen, &n);
      وإلا {
        نتیجہ = Curl_xfer_send(انکڑے, باہرلا, outlen, FALSE, &n);
        إذا(!نتیجہ && !n && outlen)
          نتیجہ = CURLE_AGAIN;
      }

      إذا(نتیجہ == CURLE_AGAIN) {
        CURL_TRC_WS(انکڑے, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        أرجع نتیجہ;
      }
      وإلا إذا(نتیجہ) {
        failf(انکڑے, "[WS] flush, write error %d", (صحيح)نتیجہ);
        أرجع نتیجہ;
      }
      وإلا {
        CURL_TRC_WS(انکڑے, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ws_send_raw_blocking(بنية Curl_easy *انکڑے,
                                     بنية websocket *ws,
                                     ثابت حرف *بفر, size_t buflen)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t nwritten;

  إذا(!انکڑے)
    أرجع نتیجہ;

  (فراغ)ws;
  طالما(buflen) {
    نتیجہ = Curl_xfer_send(انکڑے, بفر, buflen, FALSE, &nwritten);
    إذا(نتیجہ)
      أرجع نتیجہ;
    DEBUGASSERT(nwritten <= buflen);
    بفر += nwritten;
    buflen -= nwritten;
    إذا(buflen) {
      curl_socket_t sock = انکڑے->conn->sock[FIRSTSOCKET];
      timediff_t بایاں_ms;
      صحيح ev;

      CURL_TRC_WS(انکڑے, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      بایاں_ms = Curl_timeleft_ms(انکڑے);
      إذا(بایاں_ms < 0) {
        failf(انکڑے, "[WS] Timeout waiting for socket becoming writable");
        أرجع CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      إذا(sock == CURL_SOCKET_BAD)
        أرجع CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, بایاں_ms ? بایاں_ms : 500);
      إذا(ev < 0) {
        failf(انکڑے, "[WS] Error while waiting for socket becoming writable");
        أرجع CURLE_SEND_ERROR;
      }
    }
  }
  أرجع نتیجہ;
}

ساكن CURLcode ws_send_raw(بنية Curl_easy *انکڑے, ثابت فراغ *بفر,
                            size_t buflen, size_t *pnwritten)
{
  بنية websocket *ws;
  CURLcode نتیجہ;

  ws = Curl_conn_meta_get(انکڑے->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(انکڑے, "[WS] Not a websocket transfer");
    أرجع CURLE_SEND_ERROR;
  }
  إذا(!buflen)
    أرجع CURLE_OK;

  إذا(Curl_api_is_وچ_callback(انکڑے)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    نتیجہ = ws_flush(انکڑے, ws, TRUE);
    إذا(نتیجہ)
      أرجع نتیجہ;
    نتیجہ = ws_send_raw_blocking(انکڑے, ws, بفر, buflen);
    إذا(!نتیجہ)
      *pnwritten = buflen;
  }
  وإلا {
    /* We need any pending data to be sent or EAGAIN this call. */
    نتیجہ = ws_flush(انکڑے, ws, FALSE);
    إذا(نتیجہ)
      أرجع نتیجہ;
    نتیجہ = Curl_senddata(انکڑے, بفر, buflen, pnwritten);
  }

  CURL_TRC_WS(انکڑے, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (صحيح)نتیجہ, *pnwritten);
  أرجع نتیجہ;
}

CURLcode curl_ws_send(CURL *curl, ثابت فراغ *بفر_arg,
                      size_t buflen, size_t *گھلیا,
                      curl_off_t fragsize,
                      بنانشانی صحيح flags)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجہ = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_send, &نتیجہ)) {
    بنية websocket *ws;
    ثابت uint8_t *بفر = بفر_arg;
    بنية Curl_easy *انکڑے = curl;
    size_t ndummy;
    size_t *pnsent = گھلیا ? گھلیا : &ndummy;

    CURL_TRC_WS(انکڑے, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, انکڑے->سیٹ.ws_raw_منوال);

    *pnsent = 0;

    إذا(!بفر && buflen) {
      failf(انکڑے, "[WS] buffer is NULL when buflen is not");
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      اذهب باہرلا;
    }

    إذا(!انکڑے->conn && انکڑے->سیٹ.connect_only) {
      نتیجہ = Curl_connect_only_attach(انکڑے);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
    إذا(!انکڑے->conn) {
      failf(انکڑے, "[WS] No associated connection");
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
    ws = Curl_conn_meta_get(انکڑے->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(انکڑے, "[WS] Not a websocket transfer");
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }

    إذا(انکڑے->سیٹ.ws_raw_منوال) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      نتیجہ = ws_flush(انکڑے, ws, FALSE);
      إذا(نتیجہ)
        اذهب باہرلا;

      إذا(!بفر) {
        failf(انکڑے, "[WS] buffer is NULL in raw mode");
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        اذهب باہرلا;
      }
      إذا(!گھلیا) {
        failf(انکڑے, "[WS] sent is NULL in raw mode");
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        اذهب باہرلا;
      }
      إذا(fragsize || flags) {
        failf(انکڑے, "[WS] fragsize and flags must be zero in raw mode");
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        اذهب باہرلا;
      }
      نتیجہ = ws_send_raw(انکڑے, بفر, buflen, pnsent);
      اذهب باہرلا;
    }

    /* Not RAW mode, we do the frame encoding */
    نتیجہ = ws_enc_send(انکڑے, ws, بفر, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(انکڑے, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, انکڑے->سیٹ.ws_raw_منوال, (صحيح)نتیجہ,
                *pnsent);
  }
باہرلا:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجہ;
}

ساكن CURLcode ws_تیاری_conn(بنية Curl_easy *انکڑے,
                              بنية connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  انکڑے->state.http_neg.accept_09 = FALSE;
  انکڑے->state.http_neg.only_10 = FALSE;
  انکڑے->state.http_neg.wanted = CURL_HTTP_V1x;
  انکڑے->state.http_neg.allowed = CURL_HTTP_V1x;
  أرجع Curl_http_تیاری_conn(انکڑے, conn);
}

ثابت بنية curl_ws_چوکھٹا *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  بنية Curl_easy *انکڑے = curl;
  إذا(GOOD_EASY_HANDLE(انکڑے) && Curl_api_is_وچ_callback(انکڑے) &&
     انکڑے->conn && !انکڑے->سیٹ.ws_raw_منوال) {
    بنية websocket *ws;
    ws = Curl_conn_meta_get(انکڑے->conn, CURL_META_PROTO_WS_CONN);
    إذا(ws)
      أرجع &ws->recvframe;
  }
  أرجع عدم;
}

CURL_EXTERN CURLcode curl_ws_شروع_چوکھٹا(CURL *curl,
                                         بنانشانی صحيح flags,
                                         curl_off_t چوکھٹا_len)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجہ = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_شروع_چوکھٹا, &نتیجہ)) {
    بنية Curl_easy *انکڑے = curl;
    بنية websocket *ws;

    إذا(انکڑے->سیٹ.ws_raw_منوال) {
      failf(انکڑے, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      نتیجہ = CURLE_FAILED_INIT;
      اذهب باہرلا;
    }

    CURL_TRC_WS(انکڑے, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, چوکھٹا_len);

    إذا(!انکڑے->conn) {
      failf(انکڑے, "[WS] No associated connection");
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
    ws = Curl_conn_meta_get(انکڑے->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(انکڑے, "[WS] Not a websocket transfer");
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }

    إذا(ws->enc.payload_remain) {
      failf(انکڑے, "[WS] previous frame not finished");
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }

    نتیجہ = ws_enc_write_سر(انکڑے, ws, &ws->enc, flags, چوکھٹا_len,
                               &ws->sendbuf);
    إذا(نتیجہ)
      CURL_TRC_WS(انکڑے, "curl_start_frame(), error adding frame head %d",
                  (صحيح)نتیجہ);
  }
باہرلا:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجہ;
}

ثابت بنية Curl_protocol Curl_protocol_ws = {
  ws_تیاری_conn,                        /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_پورا,                       /* done */
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
                      ثابت بنية curl_ws_چوکھٹا **metap)
{
  (فراغ)curl;
  (فراغ)بفر;
  (فراغ)buflen;
  (فراغ)recv;
  (فراغ)metap;
  أرجع CURLE_NOT_BUILT_وچ;
}

CURLcode curl_ws_send(CURL *curl, ثابت فراغ *بفر,
                      size_t buflen, size_t *گھلیا,
                      curl_off_t fragsize,
                      بنانشانی صحيح flags)
{
  (فراغ)curl;
  (فراغ)بفر;
  (فراغ)buflen;
  (فراغ)گھلیا;
  (فراغ)fragsize;
  (فراغ)flags;
  أرجع CURLE_NOT_BUILT_وچ;
}

ثابت بنية curl_ws_چوکھٹا *curl_ws_meta(CURL *انکڑے)
{
  (فراغ)انکڑے;
  أرجع عدم;
}

CURL_EXTERN CURLcode curl_ws_شروع_چوکھٹا(CURL *curl,
                                         بنانشانی صحيح flags,
                                         curl_off_t چوکھٹا_len)
{
  (فراغ)curl;
  (فراغ)flags;
  (فراغ)چوکھٹا_len;
  أرجع CURLE_NOT_BUILT_وچ;
}

#جے_انت /* !CURL_DISABLE_WEBSOCKETS */
