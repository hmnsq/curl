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

#اگر_تعریف_نشده CURL_DISABLE_WEBSOCKETS

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
#تعریف_کن WSBIT_FIN          0x80
#تعریف_کن WSBIT_RSV1         0x40
#تعریف_کن WSBIT_RSV2         0x20
#تعریف_کن WSBIT_RSV3         0x10
#تعریف_کن WSBIT_RSV_ماسک     (WSBIT_RSV1 | WSBIT_RSV2 | WSBIT_RSV3)
#تعریف_کن WSBIT_OPCODE_CONT  0x0
#تعریف_کن WSBIT_OPCODE_متن  0x1
#تعریف_کن WSBIT_OPCODE_BIN   0x2
#تعریف_کن WSBIT_OPCODE_CLOSE 0x8
#تعریف_کن WSBIT_OPCODE_PING  0x9
#تعریف_کن WSBIT_OPCODE_PONG  0xa
#اگر_تعریف_شده CURLVERBOSE
#تعریف_کن WSBIT_OPCODE_ماسک  0xf
#پایان_اگر

#تعریف_کن WSBIT_ماسک 0x80

/* buffer dimensioning */
#تعریف_کن WS_CHUNK_اندازه  65535
#تعریف_کن WS_CHUNK_شمار 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
تعداد ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_سر,
  WS_DEC_PAYLOAD
};

بنية ws_decoder {
  صحيح قاب_age;        /* zero */
  صحيح قاب_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_جابه_جایی;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t سر[10];
  صحيح سر_len, سر_مجموع;
  تعداد ws_dec_state state;
  صحيح cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
بنية ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  بیعلامت صحيح xori; /* xor index */
  uint8_t ماسک[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  بیت(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#تعریف_کن WS_MAX_CNTRL_LEN    125

بنية ws_cntrl_قاب {
  بیعلامت صحيح type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
بنية websocket {
  بنية Curl_easy *داده; /* used for write callback handling */
  بنية ws_decoder dec;  /* decode of ws frames */
  بنية ws_encoder enc;  /* encode of ws frames */
  بنية bufq recvbuf;    /* raw data from the server */
  بنية bufq sendbuf;    /* raw data to be sent to the server */
  بنية curl_ws_قاب recvframe;  /* the current WS FRAME received */
  بنية ws_cntrl_قاب pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  بیت(using_reader);      /* sending WS data is handled via a client reader */
};

#اگر_تعریف_شده CURLVERBOSE
ساكن ثابت حرف *ws_قاب_name_از_op(uint8_t firstbyte)
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
#پایان_اگر

ساكن صحيح ws_قاب_firstbyte2flags(بنية Curl_easy *داده,
                                    uint8_t firstbyte, صحيح cont_flags)
{
  بدل(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  حالة WSBIT_OPCODE_CONT:
    إذا(!(cont_flags & CURLWS_CONT)) {
      failf(داده, "[WS] no ongoing fragmented message to resume");
      أرجع 0;
    }
    أرجع cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  حالة (WSBIT_OPCODE_CONT | WSBIT_FIN):
    إذا(!(cont_flags & CURLWS_CONT)) {
      failf(داده, "[WS] no ongoing fragmented message to resume");
      أرجع 0;
    }
    أرجع cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  حالة WSBIT_OPCODE_متن:
    إذا(cont_flags & CURLWS_CONT) {
      failf(داده, "[WS] fragmented message interrupted by new TEXT msg");
      أرجع 0;
    }
    أرجع CURLWS_متن | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  حالة (WSBIT_OPCODE_متن | WSBIT_FIN):
    إذا(cont_flags & CURLWS_CONT) {
      failf(داده, "[WS] fragmented message interrupted by new TEXT msg");
      أرجع 0;
    }
    أرجع CURLWS_متن;
  /* 0x02 - first BINARY fragment */
  حالة WSBIT_OPCODE_BIN:
    إذا(cont_flags & CURLWS_CONT) {
      failf(داده, "[WS] fragmented message interrupted by new BINARY msg");
      أرجع 0;
    }
    أرجع CURLWS_دودویی | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  حالة (WSBIT_OPCODE_BIN | WSBIT_FIN):
    إذا(cont_flags & CURLWS_CONT) {
      failf(داده, "[WS] fragmented message interrupted by new BINARY msg");
      أرجع 0;
    }
    أرجع CURLWS_دودویی;
  /* 0x08 - first CLOSE fragment */
  حالة WSBIT_OPCODE_CLOSE:
    failf(داده, "[WS] invalid fragmented CLOSE frame");
    أرجع 0;
  /* 0x88 - unfragmented CLOSE */
  حالة (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    أرجع CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  حالة WSBIT_OPCODE_PING:
    failf(داده, "[WS] invalid fragmented PING frame");
    أرجع 0;
  /* 0x89 - unfragmented PING */
  حالة (WSBIT_OPCODE_PING | WSBIT_FIN):
    أرجع CURLWS_PING;
  /* 0x0a - first PONG fragment */
  حالة WSBIT_OPCODE_PONG:
    failf(داده, "[WS] invalid fragmented PONG frame");
    أرجع 0;
  /* 0x8a - unfragmented PONG */
  حالة (WSBIT_OPCODE_PONG | WSBIT_FIN):
    أرجع CURLWS_PONG;
  /* invalid first byte */
  افتراضي:
    إذا(firstbyte & WSBIT_RSV_ماسک)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(داده, "[WS] invalid reserved bits: %02x", firstbyte);
    وإلا
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(داده, "[WS] invalid opcode: %02x", firstbyte);
    أرجع 0;
  }
}

ساكن CURLcode ws_قاب_flags2firstbyte(بنية Curl_easy *داده,
                                         بیعلامت صحيح flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  بدل(flags & ~CURLWS_جابه_جایی) {
  حالة 0:
    إذا(contfragment) {
      CURL_TRC_WS(داده, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      أرجع CURLE_OK;
    }
    failf(داده, "[WS] no flags given");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  حالة CURLWS_CONT:
    إذا(contfragment) {
      infof(داده, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      أرجع CURLE_OK;
    }
    failf(داده, "[WS] No ongoing fragmented message to continue");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  حالة CURLWS_متن:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_متن | WSBIT_FIN);
    أرجع CURLE_OK;
  حالة (CURLWS_متن | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_متن;
    أرجع CURLE_OK;
  حالة CURLWS_دودویی:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    أرجع CURLE_OK;
  حالة (CURLWS_دودویی | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    أرجع CURLE_OK;
  حالة CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_CLOSE | CURLWS_CONT):
    failf(داده, "[WS] CLOSE frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  حالة CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_PING | CURLWS_CONT):
    failf(داده, "[WS] PING frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  حالة CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    أرجع CURLE_OK;
  حالة (CURLWS_PONG | CURLWS_CONT):
    failf(داده, "[WS] PONG frame must not be fragmented");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  افتراضي:
    failf(داده, "[WS] unknown flags: %x", flags);
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }
}

ساكن فراغ ws_dec_info(بنية ws_decoder *dec, بنية Curl_easy *داده,
                        ثابت حرف *پیام)
{
  NOVERBOSE((فراغ)پیام);
  بدل(dec->سر_len) {
  حالة 0:
    اكسر;
  حالة 1:
    CURL_TRC_WS(داده, "decoded %s [%s%s]", پیام,
                ws_قاب_name_از_op(dec->سر[0]),
                (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    اكسر;
  افتراضي:
    إذا(dec->سر_len < dec->سر_مجموع) {
      CURL_TRC_WS(داده, "decoded %s [%s%s](%d/%d)", پیام,
                  ws_قاب_name_از_op(dec->سر[0]),
                  (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->سر_len, dec->سر_مجموع);
    }
    وإلا {
      CURL_TRC_WS(داده, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  پیام, ws_قاب_name_از_op(dec->سر[0]),
                  (dec->سر[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_جابه_جایی, dec->payload_len);
    }
    اكسر;
  }
}

ساكن CURLcode ws_send_raw_blocking(بنية Curl_easy *داده,
                                     بنية websocket *ws,
                                     ثابت حرف *میانگیر, size_t buflen);

تعريفنوع CURLcode ws_write_payload(ثابت uint8_t *buf, size_t buflen,
                                  صحيح قاب_age, صحيح قاب_flags,
                                  curl_off_t payload_جابه_جایی,
                                  curl_off_t payload_len,
                                  فراغ *userp,
                                  size_t *pnwritten);

ساكن فراغ ws_dec_بعدی_قاب(بنية ws_decoder *dec)
{
  dec->قاب_age = 0;
  dec->قاب_flags = 0;
  dec->payload_جابه_جایی = 0;
  dec->payload_len = 0;
  dec->سر_len = dec->سر_مجموع = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ساكن فراغ ws_dec_reset(بنية ws_decoder *dec)
{
  dec->قاب_age = 0;
  dec->قاب_flags = 0;
  dec->payload_جابه_جایی = 0;
  dec->payload_len = 0;
  dec->سر_len = dec->سر_مجموع = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

ساكن فراغ ws_dec_init(بنية ws_decoder *dec)
{
  ws_dec_reset(dec);
}

ساكن CURLcode ws_dec_read_سر(بنية ws_decoder *dec,
                                 بنية Curl_easy *داده,
                                 بنية bufq *inraw)
{
  ثابت uint8_t *inbuf;
  size_t inlen;

  طالما(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    إذا(dec->سر_len == 0) {
      dec->سر[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->قاب_flags = ws_قاب_firstbyte2flags(داده, dec->سر[0],
                                                  dec->cont_flags);
      إذا(!dec->قاب_flags) {
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      إذا(dec->قاب_flags & (CURLWS_متن | CURLWS_دودویی)) {
        dec->cont_flags = dec->قاب_flags;
      }

      dec->سر_len = 1;
#إذا 0
      ws_dec_info(dec, داده, "seeing opcode");
#پایان_اگر
      استمر;
    }
    وإلا إذا(dec->سر_len == 1) {
      dec->سر[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->سر_len = 2;

      إذا(dec->سر[1] & WSBIT_ماسک) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(داده, "[WS] masked input frame");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->قاب_flags & CURLWS_PING && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(داده, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->قاب_flags & CURLWS_PONG && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(داده, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }
      إذا(dec->قاب_flags & CURLWS_CLOSE && dec->سر[1] > WS_MAX_CNTRL_LEN) {
        failf(داده, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        أرجع CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      إذا(dec->سر[1] == 126) {
        dec->سر_مجموع = 4;
        استمر;
      }
      وإلا إذا(dec->سر[1] == 127) {
        dec->سر_مجموع = 10;
        استمر;
      }
      وإلا {
        dec->سر_مجموع = 2;
      }
    }

    إذا(dec->سر_len < dec->سر_مجموع) {
      dec->سر[dec->سر_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->سر_len;
      إذا(dec->سر_len < dec->سر_مجموع) {
#إذا 0
        ws_dec_info(dec, داده, "decoding head");
#پایان_اگر
        استمر;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->سر_len == dec->سر_مجموع);
    بدل(dec->سر_مجموع) {
    حالة 2:
      dec->payload_len = dec->سر[1];
      اكسر;
    حالة 4:
      dec->payload_len = (dec->سر[2] << 8) | dec->سر[3];
      اكسر;
    حالة 10:
      إذا(dec->سر[2] > 127) {
        failf(داده, "[WS] frame length longer than 63 bits not supported");
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
      failf(داده, "[WS] unexpected frame header length");
      أرجع CURLE_RECV_ERROR;
    }

    dec->قاب_age = 0;
    dec->payload_جابه_جایی = 0;
    ws_dec_info(dec, داده, "head");
    أرجع CURLE_OK;
  }
  أرجع CURLE_AGAIN;
}

ساكن CURLcode ws_dec_pass_payload(بنية ws_decoder *dec,
                                    بنية Curl_easy *داده,
                                    بنية bufq *inraw,
                                    ws_write_payload *write_cb,
                                    فراغ *write_ctx)
{
  ثابت uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode نتیجه;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_جابه_جایی,
                                     0, SIZE_MAX);

  طالما(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(داده)) {
    إذا(inlen > remain)
      inlen = remain;
    نتیجه = write_cb(inbuf, inlen, dec->قاب_age, dec->قاب_flags,
                      dec->payload_جابه_جایی, dec->payload_len,
                      write_ctx, &nwritten);
    إذا(نتیجه)
      أرجع نتیجه;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_جابه_جایی += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_جابه_جایی,
                                0, SIZE_MAX);
    CURL_TRC_WS(داده, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  أرجع remain ? CURLE_AGAIN : CURLE_OK;
}

ساكن CURLcode ws_dec_pass(بنية ws_decoder *dec,
                            بنية Curl_easy *داده,
                            بنية bufq *inraw,
                            ws_write_payload *write_cb,
                            فراغ *write_ctx)
{
  CURLcode نتیجه;

  إذا(Curl_bufq_is_خالی(inraw))
    أرجع CURLE_AGAIN;

  بدل(dec->state) {
  حالة WS_DEC_INIT:
    ws_dec_بعدی_قاب(dec);
    dec->state = WS_DEC_سر;
    FALLTHROUGH();
  حالة WS_DEC_سر:
    نتیجه = ws_dec_read_سر(dec, داده, inraw);
    إذا(نتیجه) {
      إذا(نتیجه != CURLE_AGAIN) {
        failf(داده, "[WS] decode frame error %d", (صحيح)نتیجه);
        اكسر;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_خالی(inraw));
      اكسر;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    إذا(dec->payload_len == 0) {
      size_t nwritten;
      ثابت uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      نتیجه = write_cb(&tmp, 0, dec->قاب_age, dec->قاب_flags,
                        0, 0, write_ctx, &nwritten);
      إذا(نتیجه)
        أرجع نتیجه;
      dec->state = WS_DEC_INIT;
      اكسر;
    }
    FALLTHROUGH();
  حالة WS_DEC_PAYLOAD:
    نتیجه = ws_dec_pass_payload(dec, داده, inraw, write_cb, write_ctx);
    ws_dec_info(dec, داده, "passing");
    إذا(نتیجه)
      أرجع نتیجه;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    اكسر;
  افتراضي:
    /* we covered all enums above, but some code analyzers are wimps */
    نتیجه = CURLE_FAILED_INIT;
  }
  أرجع نتیجه;
}

ساكن فراغ update_meta(بنية websocket *ws,
                        صحيح قاب_age, صحيح قاب_flags,
                        curl_off_t payload_جابه_جایی,
                        curl_off_t payload_len,
                        size_t cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_جابه_جایی - cur_len);

  ws->recvframe.age = قاب_age;
  ws->recvframe.flags = قاب_flags;
  ws->recvframe.جابه_جایی = payload_جابه_جایی;
  ws->recvframe.len = cur_len;
  ws->recvframe.bytesleft = bytesleft;
}

/* WebSocket decoding client writer */
بنية ws_cw_ctx {
  بنية Curl_cwriter super;
  بنية bufq buf;
};

ساكن CURLcode ws_cw_init(بنية Curl_easy *داده,
                           بنية Curl_cwriter *writer)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  (فراغ)داده;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_اندازه, 1, BUFQ_OPT_SOFT_حد);
  أرجع CURLE_OK;
}

ساكن فراغ ws_cw_close(بنية Curl_easy *داده, بنية Curl_cwriter *writer)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  (فراغ)داده;
  Curl_bufq_free(&ctx->buf);
}

بنية ws_cw_dec_ctx {
  بنية Curl_easy *داده;
  بنية websocket *ws;
  بنية Curl_cwriter *بعدی_writer;
  صحيح cw_type;
};

ساكن CURLcode ws_flush(بنية Curl_easy *داده, بنية websocket *ws,
                         bool blocking);
ساكن CURLcode ws_enc_جمع_کن_pending(بنية Curl_easy *داده,
                                   بنية websocket *ws);

ساكن CURLcode ws_enc_جمع_کن_cntrl(بنية Curl_easy *داده,
                                 بنية websocket *ws,
                                 ثابت uint8_t *payload,
                                 size_t plen,
                                 بیعلامت صحيح قاب_type)
{
  (فراغ)داده;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  إذا(plen > WS_MAX_CNTRL_LEN)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = قاب_type;
  ws->pending.payload_len = plen;
  کپی_حافظه(ws->pending.payload, payload, plen);
  أرجع CURLE_OK;
}

ساكن curl_off_t ws_payload_remain(curl_off_t payload_مجموع,
                                    curl_off_t payload_جابه_جایی,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_مجموع - payload_جابه_جایی;
  إذا((payload_مجموع < 0) || (payload_جابه_جایی < 0) || (remain < 0))
    أرجع -1;
  buffered = curlx_uztoso(payload_buffered);
  إذا(remain < buffered)
    أرجع -1;
  أرجع remain - buffered;
}

ساكن CURLcode ws_cw_dec_بعدی(ثابت uint8_t *buf, size_t buflen,
                               صحيح قاب_age, صحيح قاب_flags,
                               curl_off_t payload_جابه_جایی,
                               curl_off_t payload_len,
                               فراغ *user_داده,
                               size_t *pnwritten)
{
  بنية ws_cw_dec_ctx *ctx = user_داده;
  بنية Curl_easy *داده = ctx->داده;
  بنية websocket *ws = ctx->ws;
  bool auto_pong = !داده->مجموعه.ws_raw_نما && !داده->مجموعه.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode نتیجه;

  (فراغ)قاب_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_جابه_جایی, buflen);
  إذا(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  إذا(auto_pong && (قاب_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(داده, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_جابه_جایی, payload_len);
    /* send back the exact same content as a PONG */
    نتیجه = ws_enc_جمع_کن_cntrl(داده, ws, buf, buflen, CURLWS_PONG);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  وإلا إذا(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, قاب_age, قاب_flags, payload_جابه_جایی,
                payload_len, buflen);

    CURL_TRC_WRITE(داده, "[WS] pass %zu decoded bytes", buflen);
    نتیجه = Curl_cwriter_write(داده, ctx->بعدی_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ثابت حرف *)buf, buflen);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  *pnwritten = buflen;
  أرجع CURLE_OK;
}

ساكن CURLcode ws_cw_write(بنية Curl_easy *داده,
                            بنية Curl_cwriter *writer, صحيح type,
                            ثابت حرف *buf, size_t nbytes)
{
  بنية ws_cw_ctx *ctx = writer->ctx;
  بنية websocket *ws;
  CURLcode نتیجه = CURLE_OK;

  CURL_TRC_WRITE(داده, "[WS] write(len=%zu, type=%d)", nbytes, type);
  إذا(!(type & CLIENTWRITE_BODY) || داده->مجموعه.ws_raw_نما)
    أرجع Curl_cwriter_write(داده, writer->بعدی, type, buf, nbytes);

  ws = Curl_conn_meta_get(داده->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(داده, "[WS] not a websocket transfer");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(nbytes) {
    size_t nwritten;
    نتیجه = Curl_bufq_write(&ctx->buf, (ثابت uint8_t *)buf,
                             nbytes, &nwritten);
    إذا(نتیجه) {
      infof(داده, "[WS] error adding data to buffer %d", (صحيح)نتیجه);
      أرجع نتیجه;
    }
  }

  نتیجه = Curl_cwriter_flush(داده, writer->بعدی);
  إذا(نتیجه)
    اذهب بیرون;

  طالما(!Curl_bufq_is_خالی(&ctx->buf) && !Curl_cwriter_is_paused(داده)) {
    بنية ws_cw_dec_ctx pass_ctx;
    pass_ctx.داده = داده;
    pass_ctx.ws = ws;
    pass_ctx.بعدی_writer = writer->بعدی;
    pass_ctx.cw_type = type;
    نتیجه = ws_dec_pass(&ws->dec, داده, &ctx->buf,
                         ws_cw_dec_بعدی, &pass_ctx);
    إذا(نتیجه == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      نتیجه = CURLE_OK;
      اذهب بیرون;
    }
    وإلا إذا(نتیجه) {
      failf(داده, "[WS] decode payload error %d", (صحيح)نتیجه);
      Curl_bufq_reset(&ctx->buf);
      اذهب بیرون;
    }
  }

بیرون:
  إذا(!نتیجه && !ws->using_reader) {
    نتیجه = ws_flush(داده, ws, Curl_api_is_درون_callback(داده));
    إذا(نتیجه == CURLE_AGAIN)
      نتیجه = CURLE_OK;
  }

  إذا(!نتیجه && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_خالی(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(داده, "[WS] decode ended with incomplete frame");
    نتیجه = CURLE_RECV_ERROR;
  }
  أرجع نتیجه;
}

ساكن CURLcode ws_cw_flush(بنية Curl_easy *داده,
                            بنية Curl_cwriter *writer)
{
  CURLcode نتیجه = CURLE_OK;

  CURL_TRC_WRITE(داده, "[ws] flush");
  إذا(!داده->مجموعه.ws_raw_نما) {
    بنية ws_cw_ctx *ctx = writer->ctx;
    بنية websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    نتیجه = Curl_cwriter_flush(داده, writer->بعدی);
    إذا(نتیجه)
      اذهب بیرون;

    ws = Curl_conn_meta_get(داده->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(داده, "[WS] not a websocket transfer");
      أرجع CURLE_FAILED_INIT;
    }

    طالما(!Curl_bufq_is_خالی(&ctx->buf) && !Curl_cwriter_is_paused(داده)) {
      بنية ws_cw_dec_ctx pass_ctx;
      pass_ctx.داده = داده;
      pass_ctx.ws = ws;
      pass_ctx.بعدی_writer = writer->بعدی;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      نتیجه = ws_dec_pass(&ws->dec, داده, &ctx->buf,
                           ws_cw_dec_بعدی, &pass_ctx);
      إذا(نتیجه == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        نتیجه = CURLE_OK;
        اذهب بیرون;
      }
      وإلا إذا(نتیجه) {
        failf(داده, "[WS] decode payload error %d", (صحيح)نتیجه);
        Curl_bufq_reset(&ctx->buf);
        اذهب بیرون;
      }
    }
  }

بیرون:
  إذا(!نتیجه)
    نتیجه = Curl_cwriter_flush(داده, writer->بعدی);
  أرجع نتیجه;
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

ساكن فراغ ws_enc_info(بنية ws_encoder *enc, بنية Curl_easy *داده,
                        ثابت حرف *پیام)
{
  NOVERBOSE((فراغ)enc);
  NOVERBOSE((فراغ)پیام);
  CURL_TRC_WS(داده, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              پیام, ws_قاب_name_از_op(enc->firstbyte),
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

ساكن CURLcode ws_enc_جمع_کن_قاب(بنية Curl_easy *داده,
                                 بنية ws_encoder *enc,
                                 بیعلامت صحيح flags,
                                 curl_off_t payload_len,
                                 بنية bufq *بیرون)
{
  uint8_t firstb = 0;
  uint8_t سر[14];
  CURLcode نتیجه;
  size_t hlen, nwritten;

  إذا(payload_len < 0) {
    failf(داده, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    أرجع CURLE_SEND_ERROR;
  }

  إذا(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(داده, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    أرجع CURLE_SEND_ERROR;
  }

  نتیجه = ws_قاب_flags2firstbyte(داده, flags, (bool)enc->contfragment,
                                    &firstb);
  إذا(نتیجه)
    أرجع نتیجه;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  إذا(flags & (CURLWS_متن | CURLWS_دودویی)) {
    enc->contfragment = (curl_بیت)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  إذا(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(داده, "[WS] given PING frame is too big");
    أرجع CURLE_TOO_LARGE;
  }
  إذا(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(داده, "[WS] given PONG frame is too big");
    أرجع CURLE_TOO_LARGE;
  }
  إذا(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(داده, "[WS] given CLOSE frame is too big");
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
  ws_enc_info(enc, داده, "sending");

  /* 4 bytes random */

  نتیجه = Curl_rand(داده, (uint8_t *)&enc->ماسک, حجم(enc->ماسک));
  إذا(نتیجه)
    أرجع نتیجه;

#اگر_تعریف_شده DEBUGBUILD
  إذا(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->ماسک, 0, حجم(enc->ماسک));
#پایان_اگر

  /* add 4 bytes mask */
  کپی_حافظه(&سر[hlen], &enc->ماسک, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  نتیجه = Curl_bufq_write(بیرون, سر, hlen, &nwritten);
  إذا(نتیجه)
    أرجع نتیجه;
  إذا(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ws_enc_write_سر(بنية Curl_easy *داده,
                                  بنية websocket *ws,
                                  بنية ws_encoder *enc,
                                  بیعلامت صحيح flags,
                                  curl_off_t payload_len,
                                  بنية bufq *بیرون)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  إذا(ws->pending.type) {
    CURLcode نتیجه = ws_enc_جمع_کن_pending(داده, ws);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  أرجع ws_enc_جمع_کن_قاب(داده, enc, flags, payload_len, بیرون);
}

ساكن CURLcode ws_enc_write_payload(بنية ws_encoder *enc,
                                     بنية Curl_easy *داده,
                                     ثابت uint8_t *buf, size_t buflen,
                                     بنية bufq *بیرون, size_t *pnwritten)
{
  CURLcode نتیجه;
  size_t i, len, n, remain;

  *pnwritten = 0;
  إذا(Curl_bufq_is_پر(بیرون))
    أرجع CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  إذا(remain < len)
    len = remain;

  لكل(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->ماسک[enc->xori];
    نتیجه = Curl_bufq_write(بیرون, &c, 1, &n);
    إذا(نتیجه) {
      إذا((نتیجه != CURLE_AGAIN) || !i)
        أرجع نتیجه;
      اكسر;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, داده, "buffered");
  أرجع CURLE_OK;
}

ساكن CURLcode ws_enc_جمع_کن_pending(بنية Curl_easy *داده,
                                   بنية websocket *ws)
{
  CURLcode نتیجه;
  size_t n;

  إذا(!ws->pending.type) /* no pending frame here */
    أرجع CURLE_OK;
  إذا(ws->enc.payload_remain) /* in the middle of another frame */
    أرجع CURLE_AGAIN;

  نتیجه = ws_enc_جمع_کن_قاب(داده, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  إذا(نتیجه) {
    CURL_TRC_WS(داده, "ws_enc_cntrl(), error adding head: %d",
                (صحيح)نتیجه);
    اذهب بیرون;
  }
  نتیجه = ws_enc_write_payload(&ws->enc, داده, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  إذا(نتیجه) {
    CURL_TRC_WS(داده, "ws_enc_cntrl(), error adding payload: %d",
                (صحيح)نتیجه);
    اذهب بیرون;
  }
  إذا(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(داده, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, حجم(ws->pending));

بیرون:
  أرجع نتیجه;
}

ساكن CURLcode ws_enc_send(بنية Curl_easy *داده,
                            بنية websocket *ws,
                            ثابت uint8_t *میانگیر,
                            size_t buflen,
                            curl_off_t fragsize,
                            بیعلامت صحيح flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(!داده->مجموعه.ws_raw_نما);
  *pnsent = 0;

  إذا(ws->enc.payload_remain || !Curl_bufq_is_خالی(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    إذا(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(داده, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }
    إذا((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(داده, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }
  }
  وإلا {
    نتیجه = ws_flush(داده, ws, Curl_api_is_درون_callback(داده));
    إذا(نتیجه)
      أرجع نتیجه;

    نتیجه = ws_enc_write_سر(داده, ws, &ws->enc, flags,
                               (flags & CURLWS_جابه_جایی) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    إذا(نتیجه) {
      CURL_TRC_WS(داده, "curl_ws_send(), error writing frame head %d",
                  (صحيح)نتیجه);
      أرجع نتیجه;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  طالما(!Curl_bufq_is_خالی(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    إذا(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      نتیجه = ws_enc_write_payload(&ws->enc, داده,
                                    میانگیر + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      إذا(نتیجه && (نتیجه != CURLE_AGAIN))
        أرجع نتیجه;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      إذا(!ws->sendbuf_payload) {
        أرجع CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    نتیجه = ws_flush(داده, ws, Curl_api_is_درون_callback(داده));
    إذا(!نتیجه && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      میانگیر += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    وإلا إذا(نتیجه == CURLE_AGAIN) {
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
        CURL_TRC_WS(داده, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        أرجع CURLE_AGAIN;
      }
    }
    وإلا
      أرجع نتیجه;  /* real error sending the data */
  }
  أرجع CURLE_OK;
}

بنية cr_ws_ctx {
  بنية Curl_creader super;
  بیت(read_eos);  /* we read an EOS from the next reader */
  بیت(eos);       /* we have returned an EOS */
};

ساكن CURLcode cr_ws_init(بنية Curl_easy *داده, بنية Curl_creader *reader)
{
  (فراغ)داده;
  (فراغ)reader;
  أرجع CURLE_OK;
}

ساكن فراغ cr_ws_close(بنية Curl_easy *داده, بنية Curl_creader *reader)
{
  (فراغ)داده;
  (فراغ)reader;
}

ساكن CURLcode cr_ws_read(بنية Curl_easy *داده,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_ws_ctx *ctx = reader->ctx;
  CURLcode نتیجه = CURLE_OK;
  size_t nread, n;
  بنية websocket *ws;
  bool eos;

  *pnread = 0;
  إذا(ctx->eos) {
    *peos = TRUE;
    أرجع CURLE_OK;
  }

  ws = Curl_conn_meta_get(داده->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(داده, "[WS] not a websocket transfer");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(Curl_bufq_is_خالی(&ws->sendbuf)) {
    إذا(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      أرجع CURLE_OK;
    }

    إذا(ws->enc.payload_remain) {
      CURL_TRC_WS(داده, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    نتیجه = Curl_creader_read(داده, reader->بعدی, buf, blen, &nread, &eos);
    إذا(نتیجه)
      أرجع نتیجه;
    ctx->read_eos = eos;

    إذا(!Curl_bufq_is_خالی(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(داده, reader->بعدی);
    }
    وإلا إذا(!nread) {
      /* nothing to convert, return this right away */
      إذا(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      اذهب بیرون;
    }

    إذا(!ws->enc.payload_remain && Curl_bufq_is_خالی(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      نتیجه = ws_enc_write_سر(داده, ws, &ws->enc, CURLWS_دودویی, nread,
                                 &ws->sendbuf);
      إذا(نتیجه)
        اذهب بیرون;
    }

    نتیجه = ws_enc_write_payload(&ws->enc, داده, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    إذا(نتیجه)
      اذهب بیرون;
    CURL_TRC_READ(داده, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_خالی(&ws->sendbuf));
  *peos = FALSE;
  نتیجه = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  إذا(!نتیجه && ctx->read_eos && Curl_bufq_is_خالی(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

بیرون:
  CURL_TRC_READ(داده, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)نتیجه, *pnread, *peos);
  أرجع نتیجه;
}

ساكن curl_off_t cr_ws_مجموع_درازی(بنية Curl_easy *داده,
                                     بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)داده;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_مجموع_درازی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_انجامشده,
  حجم(بنية cr_ws_ctx)
};

بنية wsfield {
  ثابت حرف *name;
  ثابت حرف *val;
};

CURLcode Curl_ws_request(بنية Curl_easy *داده, بنية dynbuf *req)
{
  بیعلامت صحيح i;
  CURLcode نتیجه = CURLE_OK;
  uint8_t rand[16];
  حرف *randstr;
  size_t randlen;
  حرف keyval[40];
  بنية SingleRequest *k = &داده->req;
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
  نتیجه = Curl_rand(داده, rand, حجم(rand));
  إذا(نتیجه)
    أرجع نتیجه;
  نتیجه = curlx_base64_encode(rand, حجم(rand), &randstr, &randlen);
  إذا(نتیجه)
    أرجع نتیجه;
  DEBUGASSERT(randlen < حجم(keyval));
  إذا(randlen >= حجم(keyval)) {
    curlx_free(randstr);
    أرجع CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, حجم(keyval), randstr, randlen);
  curlx_free(randstr);
  لكل(i = 0; !نتیجه && (i < CURL_ARRAYSIZE(heads)); i++) {
    إذا(!Curl_checkheaders(داده, heads[i].name, طول_رشته(heads[i].name))) {
      نتیجه = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  داده->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  داده->conn->بیتها.upgrade_درون_پیشرفت = TRUE;
  أرجع نتیجه;
}

ساكن فراغ ws_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية websocket *ws = مدخل;
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
CURLcode Curl_ws_accept(بنية Curl_easy *داده,
                        ثابت حرف *mem, size_t nread)
{
  بنية SingleRequest *k = &داده->req;
  بنية websocket *ws;
  بنية Curl_cwriter *ws_dec_writer = عدم;
  بنية Curl_creader *ws_enc_reader = عدم;
  CURLcode نتیجه;

  DEBUGASSERT(داده->conn);
  ws = Curl_conn_meta_get(داده->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    size_t chunk_اندازه = WS_CHUNK_اندازه;
    ws = curlx_calloc(1, حجم(*ws));
    إذا(!ws)
      أرجع CURLE_بیرون_از_حافظه;
#اگر_تعریف_شده DEBUGBUILD
    {
      ثابت حرف *p = getenv("CURL_WS_CHUNK_SIZE");
      إذا(p) {
        curl_off_t l;
        إذا(!curlx_str_عدد(&p, &l, 1 * 1024 * 1024))
          chunk_اندازه = (size_t)l;
      }
    }
#پایان_اگر
    CURL_TRC_WS(داده, "WS, using chunk size %zu", chunk_اندازه);
    Curl_bufq_init2(&ws->recvbuf, chunk_اندازه, WS_CHUNK_شمار,
                    BUFQ_OPT_SOFT_حد);
    Curl_bufq_init2(&ws->sendbuf, chunk_اندازه, WS_CHUNK_شمار,
                    BUFQ_OPT_SOFT_حد);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    نتیجه = Curl_conn_meta_مجموعه(داده->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    إذا(نتیجه)
      أرجع نتیجه;
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

  infof(داده, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  نتیجه = Curl_cwriter_create(&ws_dec_writer, داده, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  إذا(نتیجه)
    اذهب بیرون;
  نتیجه = Curl_cwriter_جمع_کن(داده, ws_dec_writer);
  إذا(نتیجه)
    اذهب بیرون;
  ws_dec_writer = عدم; /* owned by transfer now */

  k->سرآیند = FALSE; /* we will not get more response headers */

  إذا(داده->مجموعه.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    إذا(nread) {
      نتیجه = Curl_cf_recvbuf_جمع_کن(داده, داده->conn, FIRSTSOCKET,
                                   (ثابت uint8_t *)mem, nread);
      إذا(نتیجه)
        اذهب بیرون;
    }
    CURL_REQ_CLEAR_RECV(داده); /* read no more content */
  }
  وإلا { /* !connect_only */
    إذا(داده->مجموعه.روش == HTTPREQ_PUT) {
      CURL_TRC_WS(داده, "UPLOAD set, add ws-encode reader");
      نتیجه = Curl_creader_مجموعه_fread(داده, -1);
      إذا(نتیجه)
        اذهب بیرون;

      إذا(!داده->مجموعه.ws_raw_نما) {
        /* Add our client reader encoding WS BINARY frames */
        نتیجه = Curl_creader_create(&ws_enc_reader, داده, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        إذا(نتیجه)
          اذهب بیرون;
        نتیجه = Curl_creader_جمع_کن(داده, ws_enc_reader);
        إذا(نتیجه)
          اذهب بیرون;
        ws_enc_reader = عدم; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      داده->req.eos_read = FALSE;
      داده->req.upload_انجامشده = FALSE;
      CURL_REQ_مجموعه_SEND(داده);
    }

    /* Then pass any additional data to the writers */
    إذا(nread) {
      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, mem, nread);
      إذا(نتیجه)
        اذهب بیرون;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->سرآیند = FALSE; /* we will not get more responses */

بیرون:
  إذا(ws_dec_writer)
    Curl_cwriter_free(داده, ws_dec_writer);
  إذا(ws_enc_reader)
    Curl_creader_free(داده, ws_enc_reader);
  إذا(نتیجه)
    CURL_TRC_WS(داده, "Curl_ws_accept() failed -> %d", (صحيح)نتیجه);
  وإلا
    CURL_TRC_WS(داده, "websocket established, %s mode",
                داده->مجموعه.connect_only ? "connect-only" : "callback");
  أرجع نتیجه;
}

بنية ws_collect {
  بنية Curl_easy *داده;
  بنية websocket *ws;
  uint8_t *میانگیر;
  size_t buflen;
  size_t bufidx;
  صحيح قاب_age;
  صحيح قاب_flags;
  curl_off_t payload_جابه_جایی;
  curl_off_t payload_len;
  bool written;
};

ساكن CURLcode ws_client_collect(ثابت uint8_t *buf, size_t buflen,
                                  صحيح قاب_age, صحيح قاب_flags,
                                  curl_off_t payload_جابه_جایی,
                                  curl_off_t payload_len,
                                  فراغ *userp,
                                  size_t *pnwritten)
{
  بنية ws_collect *ctx = userp;
  بنية Curl_easy *داده = ctx->داده;
  bool auto_pong = !داده->مجموعه.ws_raw_نما && !داده->مجموعه.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode نتیجه = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_جابه_جایی, buflen);
  إذا(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  إذا(!ctx->bufidx) {
    /* first write */
    ctx->قاب_age = قاب_age;
    ctx->قاب_flags = قاب_flags;
    ctx->payload_جابه_جایی = payload_جابه_جایی;
    ctx->payload_len = payload_len;
  }

  إذا(auto_pong && (قاب_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(داده, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_جابه_جایی, payload_len);
    /* send back the exact same content as a PONG */
    نتیجه = ws_enc_جمع_کن_cntrl(ctx->داده, ctx->ws, buf, buflen, CURLWS_PONG);
    إذا(نتیجه)
      أرجع نتیجه;
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
    کپی_حافظه(ctx->میانگیر + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  أرجع نتیجه;
}

ساكن CURLcode nw_درون_recv(فراغ *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  بنية Curl_easy *داده = reader_ctx;
  أرجع Curl_easy_recv(داده, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, فراغ *میانگیر,
                      size_t buflen, size_t *recv,
                      ثابت بنية curl_ws_قاب **metap)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجه = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_recv, &نتیجه)) {
    بنية Curl_easy *داده = curl;
    بنية connectdata *conn;
    بنية websocket *ws;
    بنية ws_collect ctx;

    إذا(!recv || !metap || (buflen && !میانگیر)) {
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
      اذهب بیرون;
    }

    *recv = 0;
    *metap = عدم;

    conn = داده->conn;
    إذا(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      إذا(!داده->مجموعه.connect_only) {
        failf(داده, "[WS] CONNECT_ONLY is required");
        نتیجه = CURLE_UNSUPPORTED_PROTOCOL;
        اذهب بیرون;
      }

      Curl_getconnectinfo(داده, &conn);
      إذا(!conn) {
        failf(داده, "[WS] connection not found");
        نتیجه = CURLE_BAD_FUNCTION_آرگومان;
        اذهب بیرون;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(داده, "[WS] connection is not setup for websocket");
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
      اذهب بیرون;
    }

    memset(&ctx, 0, حجم(ctx));
    ctx.داده = داده;
    ctx.ws = ws;
    ctx.میانگیر = میانگیر;
    ctx.buflen = buflen;

    طالما(1) {
      /* receive more when our buffer is empty */
      إذا(Curl_bufq_is_خالی(&ws->recvbuf)) {
        size_t n;
        نتیجه = Curl_bufq_slurp(&ws->recvbuf, nw_درون_recv, داده, &n);
        إذا(نتیجه)
          اذهب بیرون;
        وإلا إذا(n == 0) {
          /* connection closed */
          infof(داده, "[WS] connection expectedly closed?");
          نتیجه = CURLE_GOT_NOTHING;
          اذهب بیرون;
        }
        CURL_TRC_WS(داده, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      نتیجه = ws_dec_pass(&ws->dec, داده, &ws->recvbuf,
                           ws_client_collect, &ctx);
      إذا(نتیجه == CURLE_AGAIN) {
        إذا(!ctx.written) {
          ws_dec_info(&ws->dec, داده, "need more input");
          استمر;  /* nothing written, try more input */
        }
        اكسر;
      }
      وإلا إذا(نتیجه) {
        اذهب بیرون;
      }
      وإلا إذا(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        اكسر;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.قاب_age, ctx.قاب_flags, ctx.payload_جابه_جایی,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(داده, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.جابه_جایی,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    إذا(!داده->مجموعه.ws_raw_نما && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_جمع_کن_pending(داده, ws);
      إذا(!r2)
        (فراغ)ws_flush(داده, ws, Curl_api_is_درون_callback(داده));
    }
    نتیجه = CURLE_OK;
  }
بیرون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
}

ساكن CURLcode ws_flush(بنية Curl_easy *داده, بنية websocket *ws,
                         bool blocking)
{
  CURLcode نتیجه;

  /* If there is space, add any pending control frame */
  إذا(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_اندازه) {
    نتیجه = ws_enc_جمع_کن_pending(داده, ws);
    إذا(نتیجه && (نتیجه != CURLE_AGAIN))
      أرجع نتیجه;
  }

  إذا(!Curl_bufq_is_خالی(&ws->sendbuf)) {
    ثابت uint8_t *بیرون;
    size_t outlen, n;
#اگر_تعریف_شده DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_بعدی = FALSE;
    size_t chunk_egain = 0;
    ثابت حرف *p = getenv("CURL_WS_CHUNK_EAGAIN");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#پایان_اگر

    طالما(Curl_bufq_peek(&ws->sendbuf, &بیرون, &outlen)) {
#اگر_تعریف_شده DEBUGBUILD
      إذا(eagain_بعدی)
        أرجع CURLE_AGAIN;
      إذا(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_بعدی = TRUE;
      }
#پایان_اگر
      إذا(blocking) {
        نتیجه = ws_send_raw_blocking(داده, ws, (ثابت حرف *)بیرون, outlen);
        n = نتیجه ? 0 : outlen;
      }
      وإلا إذا(داده->مجموعه.connect_only || Curl_api_is_درون_callback(داده))
        نتیجه = Curl_senddata(داده, بیرون, outlen, &n);
      وإلا {
        نتیجه = Curl_xfer_send(داده, بیرون, outlen, FALSE, &n);
        إذا(!نتیجه && !n && outlen)
          نتیجه = CURLE_AGAIN;
      }

      إذا(نتیجه == CURLE_AGAIN) {
        CURL_TRC_WS(داده, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        أرجع نتیجه;
      }
      وإلا إذا(نتیجه) {
        failf(داده, "[WS] flush, write error %d", (صحيح)نتیجه);
        أرجع نتیجه;
      }
      وإلا {
        CURL_TRC_WS(داده, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ws_send_raw_blocking(بنية Curl_easy *داده,
                                     بنية websocket *ws,
                                     ثابت حرف *میانگیر, size_t buflen)
{
  CURLcode نتیجه = CURLE_OK;
  size_t nwritten;

  إذا(!داده)
    أرجع نتیجه;

  (فراغ)ws;
  طالما(buflen) {
    نتیجه = Curl_xfer_send(داده, میانگیر, buflen, FALSE, &nwritten);
    إذا(نتیجه)
      أرجع نتیجه;
    DEBUGASSERT(nwritten <= buflen);
    میانگیر += nwritten;
    buflen -= nwritten;
    إذا(buflen) {
      curl_socket_t sock = داده->conn->sock[FIRSTSOCKET];
      timediff_t چپ_ms;
      صحيح ev;

      CURL_TRC_WS(داده, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      چپ_ms = Curl_timeleft_ms(داده);
      إذا(چپ_ms < 0) {
        failf(داده, "[WS] Timeout waiting for socket becoming writable");
        أرجع CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      إذا(sock == CURL_SOCKET_BAD)
        أرجع CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, چپ_ms ? چپ_ms : 500);
      إذا(ev < 0) {
        failf(داده, "[WS] Error while waiting for socket becoming writable");
        أرجع CURLE_SEND_ERROR;
      }
    }
  }
  أرجع نتیجه;
}

ساكن CURLcode ws_send_raw(بنية Curl_easy *داده, ثابت فراغ *میانگیر,
                            size_t buflen, size_t *pnwritten)
{
  بنية websocket *ws;
  CURLcode نتیجه;

  ws = Curl_conn_meta_get(داده->conn, CURL_META_PROTO_WS_CONN);
  إذا(!ws) {
    failf(داده, "[WS] Not a websocket transfer");
    أرجع CURLE_SEND_ERROR;
  }
  إذا(!buflen)
    أرجع CURLE_OK;

  إذا(Curl_api_is_درون_callback(داده)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    نتیجه = ws_flush(داده, ws, TRUE);
    إذا(نتیجه)
      أرجع نتیجه;
    نتیجه = ws_send_raw_blocking(داده, ws, میانگیر, buflen);
    إذا(!نتیجه)
      *pnwritten = buflen;
  }
  وإلا {
    /* We need any pending data to be sent or EAGAIN this call. */
    نتیجه = ws_flush(داده, ws, FALSE);
    إذا(نتیجه)
      أرجع نتیجه;
    نتیجه = Curl_senddata(داده, میانگیر, buflen, pnwritten);
  }

  CURL_TRC_WS(داده, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (صحيح)نتیجه, *pnwritten);
  أرجع نتیجه;
}

CURLcode curl_ws_send(CURL *curl, ثابت فراغ *میانگیر_arg,
                      size_t buflen, size_t *فرستاده,
                      curl_off_t fragsize,
                      بیعلامت صحيح flags)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجه = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_send, &نتیجه)) {
    بنية websocket *ws;
    ثابت uint8_t *میانگیر = میانگیر_arg;
    بنية Curl_easy *داده = curl;
    size_t ndummy;
    size_t *pnsent = فرستاده ? فرستاده : &ndummy;

    CURL_TRC_WS(داده, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, داده->مجموعه.ws_raw_نما);

    *pnsent = 0;

    إذا(!میانگیر && buflen) {
      failf(داده, "[WS] buffer is NULL when buflen is not");
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
      اذهب بیرون;
    }

    إذا(!داده->conn && داده->مجموعه.connect_only) {
      نتیجه = Curl_connect_only_attach(داده);
      إذا(نتیجه)
        اذهب بیرون;
    }
    إذا(!داده->conn) {
      failf(داده, "[WS] No associated connection");
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
    ws = Curl_conn_meta_get(داده->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(داده, "[WS] Not a websocket transfer");
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }

    إذا(داده->مجموعه.ws_raw_نما) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      نتیجه = ws_flush(داده, ws, FALSE);
      إذا(نتیجه)
        اذهب بیرون;

      إذا(!میانگیر) {
        failf(داده, "[WS] buffer is NULL in raw mode");
        نتیجه = CURLE_BAD_FUNCTION_آرگومان;
        اذهب بیرون;
      }
      إذا(!فرستاده) {
        failf(داده, "[WS] sent is NULL in raw mode");
        نتیجه = CURLE_BAD_FUNCTION_آرگومان;
        اذهب بیرون;
      }
      إذا(fragsize || flags) {
        failf(داده, "[WS] fragsize and flags must be zero in raw mode");
        نتیجه = CURLE_BAD_FUNCTION_آرگومان;
        اذهب بیرون;
      }
      نتیجه = ws_send_raw(داده, میانگیر, buflen, pnsent);
      اذهب بیرون;
    }

    /* Not RAW mode, we do the frame encoding */
    نتیجه = ws_enc_send(داده, ws, میانگیر, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(داده, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, داده->مجموعه.ws_raw_نما, (صحيح)نتیجه,
                *pnsent);
  }
بیرون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
}

ساكن CURLcode ws_راهاندازی_conn(بنية Curl_easy *داده,
                              بنية connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  داده->state.http_neg.accept_09 = FALSE;
  داده->state.http_neg.only_10 = FALSE;
  داده->state.http_neg.wanted = CURL_HTTP_V1x;
  داده->state.http_neg.allowed = CURL_HTTP_V1x;
  أرجع Curl_http_راهاندازی_conn(داده, conn);
}

ثابت بنية curl_ws_قاب *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  بنية Curl_easy *داده = curl;
  إذا(GOOD_EASY_HANDLE(داده) && Curl_api_is_درون_callback(داده) &&
     داده->conn && !داده->مجموعه.ws_raw_نما) {
    بنية websocket *ws;
    ws = Curl_conn_meta_get(داده->conn, CURL_META_PROTO_WS_CONN);
    إذا(ws)
      أرجع &ws->recvframe;
  }
  أرجع عدم;
}

CURL_EXTERN CURLcode curl_ws_شروع_قاب(CURL *curl,
                                         بیعلامت صحيح flags,
                                         curl_off_t قاب_len)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجه = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, ws_شروع_قاب, &نتیجه)) {
    بنية Curl_easy *داده = curl;
    بنية websocket *ws;

    إذا(داده->مجموعه.ws_raw_نما) {
      failf(داده, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      نتیجه = CURLE_FAILED_INIT;
      اذهب بیرون;
    }

    CURL_TRC_WS(داده, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, قاب_len);

    إذا(!داده->conn) {
      failf(داده, "[WS] No associated connection");
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
    ws = Curl_conn_meta_get(داده->conn, CURL_META_PROTO_WS_CONN);
    إذا(!ws) {
      failf(داده, "[WS] Not a websocket transfer");
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }

    إذا(ws->enc.payload_remain) {
      failf(داده, "[WS] previous frame not finished");
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }

    نتیجه = ws_enc_write_سر(داده, ws, &ws->enc, flags, قاب_len,
                               &ws->sendbuf);
    إذا(نتیجه)
      CURL_TRC_WS(داده, "curl_start_frame(), error adding frame head %d",
                  (صحيح)نتیجه);
  }
بیرون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
}

ثابت بنية Curl_protocol Curl_protocol_ws = {
  ws_راهاندازی_conn,                        /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_انجامشده,                       /* done */
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

CURLcode curl_ws_recv(CURL *curl, فراغ *میانگیر, size_t buflen,
                      size_t *recv,
                      ثابت بنية curl_ws_قاب **metap)
{
  (فراغ)curl;
  (فراغ)میانگیر;
  (فراغ)buflen;
  (فراغ)recv;
  (فراغ)metap;
  أرجع CURLE_NOT_BUILT_درون;
}

CURLcode curl_ws_send(CURL *curl, ثابت فراغ *میانگیر,
                      size_t buflen, size_t *فرستاده,
                      curl_off_t fragsize,
                      بیعلامت صحيح flags)
{
  (فراغ)curl;
  (فراغ)میانگیر;
  (فراغ)buflen;
  (فراغ)فرستاده;
  (فراغ)fragsize;
  (فراغ)flags;
  أرجع CURLE_NOT_BUILT_درون;
}

ثابت بنية curl_ws_قاب *curl_ws_meta(CURL *داده)
{
  (فراغ)داده;
  أرجع عدم;
}

CURL_EXTERN CURLcode curl_ws_شروع_قاب(CURL *curl,
                                         بیعلامت صحيح flags,
                                         curl_off_t قاب_len)
{
  (فراغ)curl;
  (فراغ)flags;
  (فراغ)قاب_len;
  أرجع CURLE_NOT_BUILT_درون;
}

#پایان_اگر /* !CURL_DISABLE_WEBSOCKETS */
