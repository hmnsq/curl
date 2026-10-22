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

#که_نه_تعریف_شوی CURL_DISABLE_WEBSOCKETS

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
#تعریف_کړه WSBIT_FIN          0x80
#تعریف_کړه WSBIT_RSV1         0x40
#تعریف_کړه WSBIT_RSV2         0x20
#تعریف_کړه WSBIT_RSV3         0x10
#تعریف_کړه WSBIT_RSV_ماسک     (WSBIT_RSV1 | WSBIT_RSV2 | WSBIT_RSV3)
#تعریف_کړه WSBIT_OPCODE_CONT  0x0
#تعریف_کړه WSBIT_OPCODE_متن  0x1
#تعریف_کړه WSBIT_OPCODE_BIN   0x2
#تعریف_کړه WSBIT_OPCODE_CLOSE 0x8
#تعریف_کړه WSBIT_OPCODE_PING  0x9
#تعریف_کړه WSBIT_OPCODE_PONG  0xa
#که_تعریف_شوی CURLVERBOSE
#تعریف_کړه WSBIT_OPCODE_ماسک  0xf
#د_که_پای

#تعریف_کړه WSBIT_ماسک 0x80

/* buffer dimensioning */
#تعریف_کړه WS_CHUNK_اندازه  65535
#تعریف_کړه WS_CHUNK_شمېر 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
تعداد ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_سر,
  WS_DEC_PAYLOAD
};

بنية ws_decoder {
  صحيح چوکاټ_age;        /* zero */
  صحيح چوکاټ_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_دننه_واټن;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t سر[10];
  صحيح سر_len, سر_ټول;
  تعداد ws_dec_state state;
  صحيح cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
بنية ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  بینښه صحيح xori; /* xor index */
  uint8_t ماسک[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  بټ(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#تعریف_کړه WS_MAX_CNTRL_LEN    125

بنية ws_cntrl_چوکاټ {
  بینښه صحيح type;
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
  بنية curl_ws_چوکاټ recvframe;  /* the current WS FRAME received */
  بنية ws_cntrl_چوکاټ pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  بټ(using_reader);      /* sending WS data is handled via a client reader */
};

#که_تعریف_شوی CURLVERBOSE
ساكن ثابت حرف *ws_چوکاټ_name_د_op(uint8_t firstbyte)
{
  بدل(firstbyte & WSBIT_OPCODE_ماسک) {
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
#د_که_پای

ساكن صحيح ws_چوکاټ_firstbyte2flags(بنية Curl_easy *معلومات,
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
    أرجع CURLWS_دوګونی | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  حالة (WSBIT_OPCODE_BIN | WSBIT_FIN):
    إذا(cont_flags & CURLWS_CONT) {
      failf(معلومات, "[WS] fragmented message interrupted by new BINARY msg");
      أرجع 0;
    }
    أرجع CURLWS_دوګونی;
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
    إذا(firstbyte & WSBIT_RSV_ماسک)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(معلومات, "[WS] invalid reserved bits: %02x", firstbyte);
    وإلا
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(معلومات, "[WS] invalid opcode: %02x", firstbyte);
    أرجع 0;
  }
}

ساكن CURLcode ws_چوکاټ_flags2firstbyte(بنية Curl_easy *معلومات,
                                         بینښه صحيح flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  بدل(flags & ~CURLWS_دننه_واټن) {
  حالة 0:
    إذا(contfragment) {
      CURL_TRC_WS(معلومات, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      أرجع CURLE_OK;
    }
    failf(معلومات, "[WS] no flags given");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  حالة CURLWS_CONT:
    إذا(contfragment) {
      infof(معلومات, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      أرجع CURLE_OK;
    }
    failf(معلومات, "[WS] No ongoing fragmented message to continue");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  حالة CURLWS_متن:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_متن | WSBIT_FIN);
    أرجع CURLE_OK;
  حالة (CURLWS_متن | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_متن;
    أرجع CURLE_OK;
  حالة CURLWS_دوګونی:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    أرجع CURLE_OK;
  حالة (CURLWS_دوګونی | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    أرجع CURLE_OK;
  حالة CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_CLOSE | CURLWS_CONT):
    failf(معلومات, "[WS] CLOSE frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  حالة CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_PING | CURLWS_CONT):
    failf(معلومات, "[WS] PING frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  حالة CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_PONG | CURLWS_CONT):
    failf(معلومات, "[WS] PONG frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  افتراضي:
    failf(معلومات, "[WS] unknown flags: %x", flags);
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
}

ساكن فراغ ws_dec_info(بنية ws_decoder *dec, بنية Curl_easy *معلومات,
                        ثابت حرف *پیغام)
{
  NOVERBOSE((فراغ)پیغام);
  بدل(dec->سر_len) {
  حالة 0:
    اكسر;
  حالة 1:
    CURL_TRC_WS(معلومات, "decoded %s [%s%s]", پیغام,
                ws_چوکاټ_name_د_op(dec->سر[0]),
                (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    اكسر;
  افتراضي:
    إذا(dec->سر_len < dec->سر_ټول) {
      CURL_TRC_WS(معلومات, "decoded %s [%s%s](%d/%d)", پیغام,
                  ws_چوکاټ_name_د_op(dec->سر[0]),
                  (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->سر_len, dec->سر_ټول);
    }
    وإلا {
      CURL_TRC_WS(معلومات, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  پیغام, ws_چوکاټ_name_د_op(dec->سر[0]),
                  (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_دننه_واټن, dec->payload_len);
    }
    اكسر;
  }
}

ساكن CURLcode ws_send_raw_blocking(بنية Curl_easy *معلومات,
                                     بنية websocket *ws,
                                     ثابت حرف *بفر, size_t buflen);

تعريفنوع CURLcode ws_write_payload(ثابت uint8_t *buf, size_t buflen,
                                  صحيح چوکاټ_age, صحيح چوکاټ_flags,
                                  curl_off_t payload_دننه_واټن,
                                  curl_off_t payload_len,
                                  فراغ *userp,
                                  size_t *pnwritten);

ساكن فراغ ws_dec_بل_چوکاټ(بنية ws_decoder *dec)
{
  dec->چوکاټ_age = 0;
  dec->چوکاټ_flags = 0;
  dec->payload_دننه_واټن = 0;
  dec->payload_len = 0;
  dec->سر_len = dec->سر_ټول = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ساكن فراغ ws_dec_reset(بنية ws_decoder *dec)
{
  dec->چوکاټ_age = 0;
  dec->چوکاټ_flags = 0;
  dec->payload_دننه_واټن = 0;
  dec->payload_len = 0;
  dec->سر_len = dec->سر_ټول = 0;
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

      dec->چوکاټ_flags = ws_چوکاټ_firstbyte2flags(معلومات, dec->سر[0],
                                                  dec->cont_flags);
      إذا(!dec->چوکاټ_flags) {
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      إذا(dec->چوکاټ_flags & (CURLWS_متن | CURLWS_دوګونی)) {
        dec->cont_flags = dec->چوکاټ_flags;
      }

      dec->سر_len = 1;
#إذا 0
      ws_dec_info(dec, معلومات, "seeing opcode");
#د_که_پای
      استمر;
    }
    وإلا إذا(dec->سر_len == 1) {
      dec->سر[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->سر_len = 2;

      إذا(dec->سر[1] & WSBIT_ماسک) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(معلومات, "[WS] masked input frame");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->چوکاټ_flags & CURLWS_PING && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(معلومات, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->چوکاټ_flags & CURLWS_PONG && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(معلومات, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->چوکاټ_flags & CURLWS_CLOSE && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        failf(معلومات, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      إذا(dec->سر[1] == 126) {
        dec->سر_ټول = 4;
        استمر;
      }
      وإلا إذا(dec->سر[1] == 127) {
        dec->سر_ټول = 10;
        استمر;
      }
      وإلا {
        dec->سر_ټول = 2;
      }
    }

    إذا(dec->سر_len < dec->سر_ټول) {
      dec->سر[dec->سر_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->سر_len;
      إذا(dec->سر_len < dec->سر_ټول) {
#إذا 0
        ws_dec_info(dec, معلومات, "decoding head");
#د_که_پای
        استمر;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->سر_len == dec->سر_ټول);
    بدل(dec->سر_ټول) {
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

    dec->چوکاټ_age = 0;
    dec->payload_دننه_واټن = 0;
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
  CURLcode پایله;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_دننه_واټن,
                                     0, SIZE_MAX);

  طالما(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(معلومات)) {
    إذا(inlen > remain)
      inlen = remain;
    پایله = write_cb(inbuf, inlen, dec->چوکاټ_age, dec->چوکاټ_flags,
                      dec->payload_دننه_واټن, dec->payload_len,
                      write_ctx, &nwritten);
    إذا(پایله)
      أرجع پایله;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_دننه_واټن += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_دننه_واټن,
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
  CURLcode پایله;

  إذا(Curl_bufq_is_تشه(inraw))
    أرجع CURLE_AGAIN;

  بدل(dec->state) {
  حالة WS_DEC_INIT:
    ws_dec_بل_چوکاټ(dec);
    dec->state = WS_DEC_سر;
    FALLTHROUGH();
  حالة WS_DEC_سر:
    پایله = ws_dec_read_سر(dec, معلومات, inraw);
    إذا(پایله) {
      إذا(پایله != CURLE_AGAIN) {
        failf(معلومات, "[WS] decode frame error %d", (صحيح)پایله);
        اكسر;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_تشه(inraw));
      اكسر;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    إذا(dec->payload_len == 0) {
      size_t nwritten;
      ثابت uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      پایله = write_cb(&tmp, 0, dec->چوکاټ_age, dec->چوکاټ_flags,
                        0, 0, write_ctx, &nwritten);
      إذا(پایله)
        أرجع پایله;
      dec->state = WS_DEC_INIT;
      اكسر;
    }
    FALLTHROUGH();
  حالة WS_DEC_PAYLOAD:
    پایله = ws_dec_pass_payload(dec, معلومات, inraw, write_cb, write_ctx);
    ws_dec_info(dec, معلومات, "passing");
    إذا(پایله)
      أرجع پایله;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    اكسر;
  افتراضي:
    /* we covered all enums above, but some code analyzers are wimps */
    پایله = CURLE_FAILED_INIT;
  }
  أرجع پایله;
}

ساكن فراغ update_meta(بنية websocket *ws,
                        صحيح چوکاټ_age, صحيح چوکاټ_flags,
                        curl_off_t payload_دننه_واټن,
                        curl_off_t payload_len,
                        size_t cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_دننه_واټن - cur_len);

  ws->recvframe.age = چوکاټ_age;
  ws->recvframe.flags = چوکاټ_flags;
  ws->recvframe.دننه_واټن = payload_دننه_واټن;
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
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_اندازه, 1, BUFQ_OPT_SOFT_حد);
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
  بنية Curl_cwriter *بل_writer;
  صحيح cw_type;
};

ساكن CURLcode ws_flush(بنية Curl_easy *معلومات, بنية websocket *ws,
                         bool blocking);
ساكن CURLcode ws_enc_جمع_کړه_pending(بنية Curl_easy *معلومات,
                                   بنية websocket *ws);

ساكن CURLcode ws_enc_جمع_کړه_cntrl(بنية Curl_easy *معلومات,
                                 بنية websocket *ws,
                                 ثابت uint8_t *payload,
                                 size_t plen,
                                 بینښه صحيح چوکاټ_type)
{
  (فراغ)معلومات;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  إذا(plen > WS_MAX_CNTRL_LEN)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = چوکاټ_type;
  ws->pending.payload_len = plen;
  حافظه_نقل(ws->pending.payload, payload, plen);
  أرجع CURLE_OK;
}

ساكن curl_off_t ws_payload_remain(curl_off_t payload_ټول,
                                    curl_off_t payload_دننه_واټن,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_ټول - payload_دننه_واټن;
  إذا((payload_ټول < 0) || (payload_دننه_واټن < 0) || (remain < 0))
    أرجع -1;
  buffered = curlx_uztoso(payload_buffered);
  إذا(remain < buffered)
    أرجع -1;
  أرجع remain - buffered;
}

ساكن CURLcode ws_cw_dec_بل(ثابت uint8_t *buf, size_t buflen,
                               صحيح چوکاټ_age, صحيح چوکاټ_flags,
                               curl_off_t payload_دننه_واټن,
                               curl_off_t payload_len,
                               فراغ *user_معلومات,
                               size_t *pnwritten)
{
  بنية ws_cw_dec_ctx *ctx = user_معلومات;
  بنية Curl_easy *معلومات = ctx->معلومات;
  بنية websocket *ws = ctx->ws;
  bool auto_pong = !معلومات->سېټ.ws_raw_ډېرپېښ && !معلومات->سېټ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode پایله;

  (فراغ)چوکاټ_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_دننه_واټن, buflen);
  إذا(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  إذا(auto_pong && (چوکاټ_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(معلومات, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_دننه_واټن, payload_len);
    /* send back the exact same content as a PONG */
    پایله = ws_enc_جمع_کړه_cntrl(معلومات, ws, buf, buflen, CURLWS_PONG);
    إذا(پایله)
      أرجع پایله;
  }
  وإلا إذا(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, چوکاټ_age, چوکاټ_flags, payload_دننه_واټن,
                payload_len, buflen);

    CURL_TRC_WRITE(معلومات, "[WS] pass %zu decoded bytes", buflen);
    پایله = Curl_cwriter_write(معلومات, ctx->بل_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ثابت حرف *)buf, buflen);
    إذا(پایله)
      أرجع پایله;
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
  CURLcode پایله = CURLE_OK;

  CURL_TRC_WRITE(معلومات, "[WS] write(len=%zu, type=%d)", nbytes, type);
  إذا(!(type & CLIENTWRITE_BODY) || معلومات->سېټ.ws_raw_ډېرپېښ)
    أرجع Curl_cwriter_write(معلومات, writer->بل, type, buf, nbytes);

  ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(معلومات, "[WS] not a websocket transfer");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(nbytes) {
    size_t nwritten;
    پایله = Curl_bufq_write(&ctx->buf, (ثابت uint8_t *)buf,
                             nbytes, &nwritten);
    إذا(پایله) {
      infof(معلومات, "[WS] error adding data to buffer %d", (صحيح)پایله);
      أرجع پایله;
    }
  }

  پایله = Curl_cwriter_flush(معلومات, writer->بل);
  إذا(پایله)
    اذهب بهر;

  طالما(!Curl_bufq_is_تشه(&ctx->buf) && !Curl_cwriter_is_paused(معلومات)) {
    بنية ws_cw_dec_ctx pass_ctx;
    pass_ctx.معلومات = معلومات;
    pass_ctx.ws = ws;
    pass_ctx.بل_writer = writer->بل;
    pass_ctx.cw_type = type;
    پایله = ws_dec_pass(&ws->dec, معلومات, &ctx->buf,
                         ws_cw_dec_بل, &pass_ctx);
    إذا(پایله == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      پایله = CURLE_OK;
      اذهب بهر;
    }
    وإلا إذا(پایله) {
      failf(معلومات, "[WS] decode payload error %d", (صحيح)پایله);
      Curl_bufq_reset(&ctx->buf);
      اذهب بهر;
    }
  }

بهر:
  إذا(!پایله && !ws->using_reader) {
    پایله = ws_flush(معلومات, ws, Curl_api_is_دننه_callback(معلومات));
    إذا(پایله == CURLE_AGAIN)
      پایله = CURLE_OK;
  }

  إذا(!پایله && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_تشه(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(معلومات, "[WS] decode ended with incomplete frame");
    پایله = CURLE_RECV_ERROR;
  }
  أرجع پایله;
}

ساكن CURLcode ws_cw_flush(بنية Curl_easy *معلومات,
                            بنية Curl_cwriter *writer)
{
  CURLcode پایله = CURLE_OK;

  CURL_TRC_WRITE(معلومات, "[ws] flush");
  إذا(!معلومات->سېټ.ws_raw_ډېرپېښ) {
    بنية ws_cw_ctx *ctx = writer->ctx;
    بنية websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    پایله = Curl_cwriter_flush(معلومات, writer->بل);
    إذا(پایله)
      اذهب بهر;

    ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(معلومات, "[WS] not a websocket transfer");
      أرجع CURLE_FAILED_INIT;
    }

    طالما(!Curl_bufq_is_تشه(&ctx->buf) && !Curl_cwriter_is_paused(معلومات)) {
      بنية ws_cw_dec_ctx pass_ctx;
      pass_ctx.معلومات = معلومات;
      pass_ctx.ws = ws;
      pass_ctx.بل_writer = writer->بل;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      پایله = ws_dec_pass(&ws->dec, معلومات, &ctx->buf,
                           ws_cw_dec_بل, &pass_ctx);
      إذا(پایله == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        پایله = CURLE_OK;
        اذهب بهر;
      }
      وإلا إذا(پایله) {
        failf(معلومات, "[WS] decode payload error %d", (صحيح)پایله);
        Curl_bufq_reset(&ctx->buf);
        اذهب بهر;
      }
    }
  }

بهر:
  إذا(!پایله)
    پایله = Curl_cwriter_flush(معلومات, writer->بل);
  أرجع پایله;
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
                        ثابت حرف *پیغام)
{
  NOVERBOSE((فراغ)enc);
  NOVERBOSE((فراغ)پیغام);
  CURL_TRC_WS(معلومات, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              پیغام, ws_چوکاټ_name_د_op(enc->firstbyte),
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

ساكن CURLcode ws_enc_جمع_کړه_چوکاټ(بنية Curl_easy *معلومات,
                                 بنية ws_encoder *enc,
                                 بینښه صحيح flags,
                                 curl_off_t payload_len,
                                 بنية bufq *بهر)
{
  uint8_t firstb = 0;
  uint8_t سر[14];
  CURLcode پایله;
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

  پایله = ws_چوکاټ_flags2firstbyte(معلومات, flags, (bool)enc->contfragment,
                                    &firstb);
  إذا(پایله)
    أرجع پایله;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  إذا(flags & (CURLWS_متن | CURLWS_دوګونی)) {
    enc->contfragment = (curl_بټ)((flags & CURLWS_CONT) ? TRUE : FALSE);
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
    سر[1] = 127 | WSBIT_ماسک;
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
    سر[1] = 126 | WSBIT_ماسک;
    سر[2] = (uint8_t)((payload_len >> 8) & 0xff);
    سر[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  وإلا {
    سر[1] = (uint8_t)payload_len | WSBIT_ماسک;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, معلومات, "sending");

  /* 4 bytes random */

  پایله = Curl_rand(معلومات, (uint8_t *)&enc->ماسک, حجم(enc->ماسک));
  إذا(پایله)
    أرجع پایله;

#که_تعریف_شوی DEBUGBUILD
  إذا(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->ماسک, 0, حجم(enc->ماسک));
#د_که_پای

  /* add 4 bytes mask */
  حافظه_نقل(&سر[hlen], &enc->ماسک, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  پایله = Curl_bufq_write(بهر, سر, hlen, &nwritten);
  إذا(پایله)
    أرجع پایله;
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
                                  بینښه صحيح flags,
                                  curl_off_t payload_len,
                                  بنية bufq *بهر)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  إذا(ws->pending.type) {
    CURLcode پایله = ws_enc_جمع_کړه_pending(معلومات, ws);
    إذا(پایله)
      أرجع پایله;
  }
  أرجع ws_enc_جمع_کړه_چوکاټ(معلومات, enc, flags, payload_len, بهر);
}

ساكن CURLcode ws_enc_write_payload(بنية ws_encoder *enc,
                                     بنية Curl_easy *معلومات,
                                     ثابت uint8_t *buf, size_t buflen,
                                     بنية bufq *بهر, size_t *pnwritten)
{
  CURLcode پایله;
  size_t i, len, n, remain;

  *pnwritten = 0;
  إذا(Curl_bufq_is_ډک(بهر))
    أرجع CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  إذا(remain < len)
    len = remain;

  لكل(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->ماسک[enc->xori];
    پایله = Curl_bufq_write(بهر, &c, 1, &n);
    إذا(پایله) {
      إذا((پایله != CURLE_AGAIN) || !i)
        أرجع پایله;
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

ساكن CURLcode ws_enc_جمع_کړه_pending(بنية Curl_easy *معلومات,
                                   بنية websocket *ws)
{
  CURLcode پایله;
  size_t n;

  إذا(!ws->pending.type) /* no pending frame here */
    أرجع CURLE_OK;
  إذا(ws->enc.payload_remain) /* in the middle of another frame */
    أرجع CURLE_AGAIN;

  پایله = ws_enc_جمع_کړه_چوکاټ(معلومات, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  إذا(پایله) {
    CURL_TRC_WS(معلومات, "ws_enc_cntrl(), error adding head: %d",
                (صحيح)پایله);
    اذهب بهر;
  }
  پایله = ws_enc_write_payload(&ws->enc, معلومات, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  إذا(پایله) {
    CURL_TRC_WS(معلومات, "ws_enc_cntrl(), error adding payload: %d",
                (صحيح)پایله);
    اذهب بهر;
  }
  إذا(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(معلومات, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, حجم(ws->pending));

بهر:
  أرجع پایله;
}

ساكن CURLcode ws_enc_send(بنية Curl_easy *معلومات,
                            بنية websocket *ws,
                            ثابت uint8_t *بفر,
                            size_t buflen,
                            curl_off_t fragsize,
                            بینښه صحيح flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(!معلومات->سېټ.ws_raw_ډېرپېښ);
  *pnsent = 0;

  إذا(ws->enc.payload_remain || !Curl_bufq_is_تشه(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    إذا(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(معلومات, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    إذا((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(معلومات, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
  }
  وإلا {
    پایله = ws_flush(معلومات, ws, Curl_api_is_دننه_callback(معلومات));
    إذا(پایله)
      أرجع پایله;

    پایله = ws_enc_write_سر(معلومات, ws, &ws->enc, flags,
                               (flags & CURLWS_دننه_واټن) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    إذا(پایله) {
      CURL_TRC_WS(معلومات, "curl_ws_send(), error writing frame head %d",
                  (صحيح)پایله);
      أرجع پایله;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  طالما(!Curl_bufq_is_تشه(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    إذا(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      پایله = ws_enc_write_payload(&ws->enc, معلومات,
                                    بفر + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      إذا(پایله && (پایله != CURLE_AGAIN))
        أرجع پایله;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      إذا(!ws->sendbuf_payload) {
        أرجع CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    پایله = ws_flush(معلومات, ws, Curl_api_is_دننه_callback(معلومات));
    إذا(!پایله && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      بفر += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    وإلا إذا(پایله == CURLE_AGAIN) {
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
      أرجع پایله;  /* real error sending the data */
  }
  أرجع CURLE_OK;
}

بنية cr_ws_ctx {
  بنية Curl_creader super;
  بټ(read_eos);  /* we read an EOS from the next reader */
  بټ(eos);       /* we have returned an EOS */
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
  CURLcode پایله = CURLE_OK;
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

  إذا(Curl_bufq_is_تشه(&ws->sendbuf)) {
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

    پایله = Curl_creader_read(معلومات, reader->بل, buf, blen, &nread, &eos);
    إذا(پایله)
      أرجع پایله;
    ctx->read_eos = eos;

    إذا(!Curl_bufq_is_تشه(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(معلومات, reader->بل);
    }
    وإلا إذا(!nread) {
      /* nothing to convert, return this right away */
      إذا(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      اذهب بهر;
    }

    إذا(!ws->enc.payload_remain && Curl_bufq_is_تشه(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      پایله = ws_enc_write_سر(معلومات, ws, &ws->enc, CURLWS_دوګونی, nread,
                                 &ws->sendbuf);
      إذا(پایله)
        اذهب بهر;
    }

    پایله = ws_enc_write_payload(&ws->enc, معلومات, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    إذا(پایله)
      اذهب بهر;
    CURL_TRC_READ(معلومات, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_تشه(&ws->sendbuf));
  *peos = FALSE;
  پایله = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  إذا(!پایله && ctx->read_eos && Curl_bufq_is_تشه(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

بهر:
  CURL_TRC_READ(معلومات, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)پایله, *pnread, *peos);
  أرجع پایله;
}

ساكن curl_off_t cr_ws_ټول_اوږدتیا(بنية Curl_easy *معلومات,
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
  cr_ws_ټول_اوږدتیا,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_بشپړ,
  حجم(بنية cr_ws_ctx)
};

بنية wsfield {
  ثابت حرف *name;
  ثابت حرف *val;
};

CURLcode Curl_ws_request(بنية Curl_easy *معلومات, بنية dynbuf *req)
{
  بینښه صحيح i;
  CURLcode پایله = CURLE_OK;
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
  پایله = Curl_rand(معلومات, rand, حجم(rand));
  إذا(پایله)
    أرجع پایله;
  پایله = curlx_base64_encode(rand, حجم(rand), &randstr, &randlen);
  إذا(پایله)
    أرجع پایله;
  DEBUGASSERT(randlen < حجم(keyval));
  إذا(randlen >= حجم(keyval)) {
    curlx_free(randstr);
    أرجع CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, حجم(keyval), randstr, randlen);
  curlx_free(randstr);
  لكل(i = 0; !پایله && (i < CURL_ARRAYSIZE(heads)); i++) {
    إذا(!Curl_checkheaders(معلومات, heads[i].name, مزي_اوږدوالی(heads[i].name))) {
      پایله = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  معلومات->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  معلومات->conn->بټونه.upgrade_دننه_پرمختګ = TRUE;
  أرجع پایله;
}

ساكن فراغ ws_conn_dtor(ثابت فراغ *کلی, size_t klen, فراغ *ثبت)
{
  بنية websocket *ws = ثبت;
  (فراغ)کلی;
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
  CURLcode پایله;

  DEBUGASSERT(معلومات->conn);
  ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    size_t chunk_اندازه = WS_CHUNK_اندازه;
    ws = curlx_calloc(1, حجم(*ws));
    إذا(!ws)
      أرجع CURLE_بهر_د_حافظه;
#که_تعریف_شوی DEBUGBUILD
    {
      ثابت حرف *p = getenv("CURL_WS_CHUNK_SIZE");
      إذا(p) {
        curl_off_t l;
        إذا(!curlx_str_شمېره(&p, &l, 1 * 1024 * 1024))
          chunk_اندازه = (size_t)l;
      }
    }
#د_که_پای
    CURL_TRC_WS(معلومات, "WS, using chunk size %zu", chunk_اندازه);
    Curl_bufq_init2(&ws->recvbuf, chunk_اندازه, WS_CHUNK_شمېر,
                    BUFQ_OPT_SOFT_حد);
    Curl_bufq_init2(&ws->sendbuf, chunk_اندازه, WS_CHUNK_شمېر,
                    BUFQ_OPT_SOFT_حد);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    پایله = Curl_conn_meta_سېټ(معلومات->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    إذا(پایله)
      أرجع پایله;
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
  پایله = Curl_cwriter_create(&ws_dec_writer, معلومات, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  إذا(پایله)
    اذهب بهر;
  پایله = Curl_cwriter_جمع_کړه(معلومات, ws_dec_writer);
  إذا(پایله)
    اذهب بهر;
  ws_dec_writer = عدم; /* owned by transfer now */

  k->سرلیک = FALSE; /* we will not get more response headers */

  إذا(معلومات->سېټ.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    إذا(nread) {
      پایله = Curl_cf_recvbuf_جمع_کړه(معلومات, معلومات->conn, FIRSTSOCKET,
                                   (ثابت uint8_t *)mem, nread);
      إذا(پایله)
        اذهب بهر;
    }
    CURL_REQ_CLEAR_RECV(معلومات); /* read no more content */
  }
  وإلا { /* !connect_only */
    إذا(معلومات->سېټ.لاره == HTTPREQ_PUT) {
      CURL_TRC_WS(معلومات, "UPLOAD set, add ws-encode reader");
      پایله = Curl_creader_سېټ_fread(معلومات, -1);
      إذا(پایله)
        اذهب بهر;

      إذا(!معلومات->سېټ.ws_raw_ډېرپېښ) {
        /* Add our client reader encoding WS BINARY frames */
        پایله = Curl_creader_create(&ws_enc_reader, معلومات, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        إذا(پایله)
          اذهب بهر;
        پایله = Curl_creader_جمع_کړه(معلومات, ws_enc_reader);
        إذا(پایله)
          اذهب بهر;
        ws_enc_reader = عدم; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      معلومات->req.eos_read = FALSE;
      معلومات->req.upload_بشپړ = FALSE;
      CURL_REQ_سېټ_SEND(معلومات);
    }

    /* Then pass any additional data to the writers */
    إذا(nread) {
      پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, mem, nread);
      إذا(پایله)
        اذهب بهر;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->سرلیک = FALSE; /* we will not get more responses */

بهر:
  إذا(ws_dec_writer)
    Curl_cwriter_free(معلومات, ws_dec_writer);
  إذا(ws_enc_reader)
    Curl_creader_free(معلومات, ws_enc_reader);
  إذا(پایله)
    CURL_TRC_WS(معلومات, "Curl_ws_accept() failed -> %d", (صحيح)پایله);
  وإلا
    CURL_TRC_WS(معلومات, "websocket established, %s mode",
                معلومات->سېټ.connect_only ? "connect-only" : "callback");
  أرجع پایله;
}

بنية ws_collect {
  بنية Curl_easy *معلومات;
  بنية websocket *ws;
  uint8_t *بفر;
  size_t buflen;
  size_t bufidx;
  صحيح چوکاټ_age;
  صحيح چوکاټ_flags;
  curl_off_t payload_دننه_واټن;
  curl_off_t payload_len;
  bool written;
};

ساكن CURLcode ws_client_collect(ثابت uint8_t *buf, size_t buflen,
                                  صحيح چوکاټ_age, صحيح چوکاټ_flags,
                                  curl_off_t payload_دننه_واټن,
                                  curl_off_t payload_len,
                                  فراغ *userp,
                                  size_t *pnwritten)
{
  بنية ws_collect *ctx = userp;
  بنية Curl_easy *معلومات = ctx->معلومات;
  bool auto_pong = !معلومات->سېټ.ws_raw_ډېرپېښ && !معلومات->سېټ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode پایله = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_دننه_واټن, buflen);
  إذا(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  إذا(!ctx->bufidx) {
    /* first write */
    ctx->چوکاټ_age = چوکاټ_age;
    ctx->چوکاټ_flags = چوکاټ_flags;
    ctx->payload_دننه_واټن = payload_دننه_واټن;
    ctx->payload_len = payload_len;
  }

  إذا(auto_pong && (چوکاټ_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(معلومات, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_دننه_واټن, payload_len);
    /* send back the exact same content as a PONG */
    پایله = ws_enc_جمع_کړه_cntrl(ctx->معلومات, ctx->ws, buf, buflen, CURLWS_PONG);
    إذا(پایله)
      أرجع پایله;
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
    حافظه_نقل(ctx->بفر + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  أرجع پایله;
}

ساكن CURLcode nw_دننه_recv(فراغ *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  بنية Curl_easy *معلومات = reader_ctx;
  أرجع Curl_easy_recv(معلومات, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, فراغ *بفر,
                      size_t buflen, size_t *recv,
                      ثابت بنية curl_ws_چوکاټ **metap)
{
  بنية Curl_eapi_guard guard;
  CURLcode پایله = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_recv, &پایله)) {
    بنية Curl_easy *معلومات = curl;
    بنية connectdata *conn;
    بنية websocket *ws;
    بنية ws_collect ctx;

    إذا(!recv || !metap || (buflen && !بفر)) {
      پایله = CURLE_BAD_FUNCTION_دلیل;
      اذهب بهر;
    }

    *recv = 0;
    *metap = عدم;

    conn = معلومات->conn;
    إذا(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      إذا(!معلومات->سېټ.connect_only) {
        failf(معلومات, "[WS] CONNECT_ONLY is required");
        پایله = CURLE_UNSUPPORTED_PROTOCOL;
        اذهب بهر;
      }

      Curl_getconnectinfo(معلومات, &conn);
      إذا(!conn) {
        failf(معلومات, "[WS] connection not found");
        پایله = CURLE_BAD_FUNCTION_دلیل;
        اذهب بهر;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(معلومات, "[WS] connection is not setup for websocket");
      پایله = CURLE_BAD_FUNCTION_دلیل;
      اذهب بهر;
    }

    memset(&ctx, 0, حجم(ctx));
    ctx.معلومات = معلومات;
    ctx.ws = ws;
    ctx.بفر = بفر;
    ctx.buflen = buflen;

    طالما(1) {
      /* receive more when our buffer is empty */
      إذا(Curl_bufq_is_تشه(&ws->recvbuf)) {
        size_t n;
        پایله = Curl_bufq_slurp(&ws->recvbuf, nw_دننه_recv, معلومات, &n);
        إذا(پایله)
          اذهب بهر;
        وإلا إذا(n == 0) {
          /* connection closed */
          infof(معلومات, "[WS] connection expectedly closed?");
          پایله = CURLE_GOT_NOTHING;
          اذهب بهر;
        }
        CURL_TRC_WS(معلومات, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      پایله = ws_dec_pass(&ws->dec, معلومات, &ws->recvbuf,
                           ws_client_collect, &ctx);
      إذا(پایله == CURLE_AGAIN) {
        إذا(!ctx.written) {
          ws_dec_info(&ws->dec, معلومات, "need more input");
          استمر;  /* nothing written, try more input */
        }
        اكسر;
      }
      وإلا إذا(پایله) {
        اذهب بهر;
      }
      وإلا إذا(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        اكسر;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.چوکاټ_age, ctx.چوکاټ_flags, ctx.payload_دننه_واټن,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(معلومات, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.دننه_واټن,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    إذا(!معلومات->سېټ.ws_raw_ډېرپېښ && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_جمع_کړه_pending(معلومات, ws);
      إذا(!r2)
        (فراغ)ws_flush(معلومات, ws, Curl_api_is_دننه_callback(معلومات));
    }
    پایله = CURLE_OK;
  }
بهر:
  CURL_EAPI_LEAVE(&guard);
  أرجع پایله;
}

ساكن CURLcode ws_flush(بنية Curl_easy *معلومات, بنية websocket *ws,
                         bool blocking)
{
  CURLcode پایله;

  /* If there is space, add any pending control frame */
  إذا(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_اندازه) {
    پایله = ws_enc_جمع_کړه_pending(معلومات, ws);
    إذا(پایله && (پایله != CURLE_AGAIN))
      أرجع پایله;
  }

  إذا(!Curl_bufq_is_تشه(&ws->sendbuf)) {
    ثابت uint8_t *بهر;
    size_t outlen, n;
#که_تعریف_شوی DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_بل = FALSE;
    size_t chunk_egain = 0;
    ثابت حرف *p = getenv("CURL_WS_CHUNK_EAGAIN");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_شمېره(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#د_که_پای

    طالما(Curl_bufq_peek(&ws->sendbuf, &بهر, &outlen)) {
#که_تعریف_شوی DEBUGBUILD
      إذا(eagain_بل)
        أرجع CURLE_AGAIN;
      إذا(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_بل = TRUE;
      }
#د_که_پای
      إذا(blocking) {
        پایله = ws_send_raw_blocking(معلومات, ws, (ثابت حرف *)بهر, outlen);
        n = پایله ? 0 : outlen;
      }
      وإلا إذا(معلومات->سېټ.connect_only || Curl_api_is_دننه_callback(معلومات))
        پایله = Curl_senddata(معلومات, بهر, outlen, &n);
      وإلا {
        پایله = Curl_xfer_send(معلومات, بهر, outlen, FALSE, &n);
        إذا(!پایله && !n && outlen)
          پایله = CURLE_AGAIN;
      }

      إذا(پایله == CURLE_AGAIN) {
        CURL_TRC_WS(معلومات, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        أرجع پایله;
      }
      وإلا إذا(پایله) {
        failf(معلومات, "[WS] flush, write error %d", (صحيح)پایله);
        أرجع پایله;
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
  CURLcode پایله = CURLE_OK;
  size_t nwritten;

  إذا(!معلومات)
    أرجع پایله;

  (فراغ)ws;
  طالما(buflen) {
    پایله = Curl_xfer_send(معلومات, بفر, buflen, FALSE, &nwritten);
    إذا(پایله)
      أرجع پایله;
    DEBUGASSERT(nwritten <= buflen);
    بفر += nwritten;
    buflen -= nwritten;
    إذا(buflen) {
      curl_socket_t sock = معلومات->conn->sock[FIRSTSOCKET];
      timediff_t کیڼ_ms;
      صحيح ev;

      CURL_TRC_WS(معلومات, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      کیڼ_ms = Curl_timeleft_ms(معلومات);
      إذا(کیڼ_ms < 0) {
        failf(معلومات, "[WS] Timeout waiting for socket becoming writable");
        أرجع CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      إذا(sock == CURL_SOCKET_BAD)
        أرجع CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, کیڼ_ms ? کیڼ_ms : 500);
      إذا(ev < 0) {
        failf(معلومات, "[WS] Error while waiting for socket becoming writable");
        أرجع CURLE_SEND_ERROR;
      }
    }
  }
  أرجع پایله;
}

ساكن CURLcode ws_send_raw(بنية Curl_easy *معلومات, ثابت فراغ *بفر,
                            size_t buflen, size_t *pnwritten)
{
  بنية websocket *ws;
  CURLcode پایله;

  ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(معلومات, "[WS] Not a websocket transfer");
    أرجع CURLE_SEND_ERROR;
  }
  إذا(!buflen)
    أرجع CURLE_OK;

  إذا(Curl_api_is_دننه_callback(معلومات)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    پایله = ws_flush(معلومات, ws, TRUE);
    إذا(پایله)
      أرجع پایله;
    پایله = ws_send_raw_blocking(معلومات, ws, بفر, buflen);
    إذا(!پایله)
      *pnwritten = buflen;
  }
  وإلا {
    /* We need any pending data to be sent or EAGAIN this call. */
    پایله = ws_flush(معلومات, ws, FALSE);
    إذا(پایله)
      أرجع پایله;
    پایله = Curl_senddata(معلومات, بفر, buflen, pnwritten);
  }

  CURL_TRC_WS(معلومات, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (صحيح)پایله, *pnwritten);
  أرجع پایله;
}

CURLcode curl_ws_send(CURL *curl, ثابت فراغ *بفر_arg,
                      size_t buflen, size_t *لېږل_شوی,
                      curl_off_t fragsize,
                      بینښه صحيح flags)
{
  بنية Curl_eapi_guard guard;
  CURLcode پایله = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_send, &پایله)) {
    بنية websocket *ws;
    ثابت uint8_t *بفر = بفر_arg;
    بنية Curl_easy *معلومات = curl;
    size_t ndummy;
    size_t *pnsent = لېږل_شوی ? لېږل_شوی : &ndummy;

    CURL_TRC_WS(معلومات, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, معلومات->سېټ.ws_raw_ډېرپېښ);

    *pnsent = 0;

    إذا(!بفر && buflen) {
      failf(معلومات, "[WS] buffer is NULL when buflen is not");
      پایله = CURLE_BAD_FUNCTION_دلیل;
      اذهب بهر;
    }

    إذا(!معلومات->conn && معلومات->سېټ.connect_only) {
      پایله = Curl_connect_only_attach(معلومات);
      إذا(پایله)
        اذهب بهر;
    }
    إذا(!معلومات->conn) {
      failf(معلومات, "[WS] No associated connection");
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
    ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(معلومات, "[WS] Not a websocket transfer");
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }

    إذا(معلومات->سېټ.ws_raw_ډېرپېښ) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      پایله = ws_flush(معلومات, ws, FALSE);
      إذا(پایله)
        اذهب بهر;

      إذا(!بفر) {
        failf(معلومات, "[WS] buffer is NULL in raw mode");
        پایله = CURLE_BAD_FUNCTION_دلیل;
        اذهب بهر;
      }
      إذا(!لېږل_شوی) {
        failf(معلومات, "[WS] sent is NULL in raw mode");
        پایله = CURLE_BAD_FUNCTION_دلیل;
        اذهب بهر;
      }
      إذا(fragsize || flags) {
        failf(معلومات, "[WS] fragsize and flags must be zero in raw mode");
        پایله = CURLE_BAD_FUNCTION_دلیل;
        اذهب بهر;
      }
      پایله = ws_send_raw(معلومات, بفر, buflen, pnsent);
      اذهب بهر;
    }

    /* Not RAW mode, we do the frame encoding */
    پایله = ws_enc_send(معلومات, ws, بفر, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(معلومات, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, معلومات->سېټ.ws_raw_ډېرپېښ, (صحيح)پایله,
                *pnsent);
  }
بهر:
  CURL_EAPI_LEAVE(&guard);
  أرجع پایله;
}

ساكن CURLcode ws_تیاری_conn(بنية Curl_easy *معلومات,
                              بنية connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  معلومات->state.http_neg.accept_09 = FALSE;
  معلومات->state.http_neg.only_10 = FALSE;
  معلومات->state.http_neg.wanted = CURL_HTTP_V1x;
  معلومات->state.http_neg.allowed = CURL_HTTP_V1x;
  أرجع Curl_http_تیاری_conn(معلومات, conn);
}

ثابت بنية curl_ws_چوکاټ *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  بنية Curl_easy *معلومات = curl;
  إذا(GOOD_EASY_HANDLE(معلومات) && Curl_api_is_دننه_callback(معلومات) &&
     معلومات->conn && !معلومات->سېټ.ws_raw_ډېرپېښ) {
    بنية websocket *ws;
    ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
    إذا(ws)
      أرجع &ws->recvframe;
  }
  أرجع عدم;
}

CURL_EXTERN CURLcode curl_ws_پیل_چوکاټ(CURL *curl,
                                         بینښه صحيح flags,
                                         curl_off_t چوکاټ_len)
{
  بنية Curl_eapi_guard guard;
  CURLcode پایله = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_پیل_چوکاټ, &پایله)) {
    بنية Curl_easy *معلومات = curl;
    بنية websocket *ws;

    إذا(معلومات->سېټ.ws_raw_ډېرپېښ) {
      failf(معلومات, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      پایله = CURLE_FAILED_INIT;
      اذهب بهر;
    }

    CURL_TRC_WS(معلومات, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, چوکاټ_len);

    إذا(!معلومات->conn) {
      failf(معلومات, "[WS] No associated connection");
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
    ws = Curl_conn_meta_get(معلومات->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(معلومات, "[WS] Not a websocket transfer");
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }

    إذا(ws->enc.payload_remain) {
      failf(معلومات, "[WS] previous frame not finished");
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }

    پایله = ws_enc_write_سر(معلومات, ws, &ws->enc, flags, چوکاټ_len,
                               &ws->sendbuf);
    إذا(پایله)
      CURL_TRC_WS(معلومات, "curl_start_frame(), error adding frame head %d",
                  (صحيح)پایله);
  }
بهر:
  CURL_EAPI_LEAVE(&guard);
  أرجع پایله;
}

ثابت بنية Curl_protocol Curl_protocol_ws = {
  ws_تیاری_conn,                        /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_بشپړ,                       /* done */
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
                      ثابت بنية curl_ws_چوکاټ **metap)
{
  (فراغ)curl;
  (فراغ)بفر;
  (فراغ)buflen;
  (فراغ)recv;
  (فراغ)metap;
  أرجع CURLE_NOT_BUILT_دننه;
}

CURLcode curl_ws_send(CURL *curl, ثابت فراغ *بفر,
                      size_t buflen, size_t *لېږل_شوی,
                      curl_off_t fragsize,
                      بینښه صحيح flags)
{
  (فراغ)curl;
  (فراغ)بفر;
  (فراغ)buflen;
  (فراغ)لېږل_شوی;
  (فراغ)fragsize;
  (فراغ)flags;
  أرجع CURLE_NOT_BUILT_دننه;
}

ثابت بنية curl_ws_چوکاټ *curl_ws_meta(CURL *معلومات)
{
  (فراغ)معلومات;
  أرجع عدم;
}

CURL_EXTERN CURLcode curl_ws_پیل_چوکاټ(CURL *curl,
                                         بینښه صحيح flags,
                                         curl_off_t چوکاټ_len)
{
  (فراغ)curl;
  (فراغ)flags;
  (فراغ)چوکاټ_len;
  أرجع CURLE_NOT_BUILT_دننه;
}

#د_که_پای /* !CURL_DISABLE_WEBSOCKETS */
