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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "ws.h"

#ifndef CURL_DISABLE_WEBSOCKETS

#अन्तर्भाव "url.h"
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "cf-recvbuf.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "easyif.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/strcopy.h"

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
#define WS_CHUNK_गिनती 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
गणना ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_HEAD,
  WS_DEC_PAYLOAD
};

संरचना ws_decoder {
  पूर्णाङ्क frame_age;        /* zero */
  पूर्णाङ्क frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t head[10];
  पूर्णाङ्क head_len, head_total;
  गणना ws_dec_state state;
  पूर्णाङ्क cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
संरचना ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  अचिन्हित पूर्णाङ्क xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#define WS_MAX_CNTRL_LEN    125

संरचना ws_cntrl_frame {
  अचिन्हित पूर्णाङ्क type;
  माप_प्रकार payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
संरचना websocket {
  संरचना Curl_easy *data; /* used for write callback handling */
  संरचना ws_decoder dec;  /* decode of ws frames */
  संरचना ws_encoder enc;  /* encode of ws frames */
  संरचना bufq recvbuf;    /* raw data from the server */
  संरचना bufq sendbuf;    /* raw data to be sent to the server */
  संरचना curl_ws_frame recvframe;  /* the current WS FRAME received */
  संरचना ws_cntrl_frame pending; /* a control frame pending to be sent */
  माप_प्रकार sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#ifdef CURLVERBOSE
स्थिर ध्रुव वर्ण *ws_frame_name_of_op(uint8_t firstbyte)
{
  विकल्प(firstbyte & WSBIT_OPCODE_MASK) {
  पक्ष WSBIT_OPCODE_CONT:
    प्रत्यागम "CONT";
  पक्ष WSBIT_OPCODE_TEXT:
    प्रत्यागम "TEXT";
  पक्ष WSBIT_OPCODE_BIN:
    प्रत्यागम "BIN";
  पक्ष WSBIT_OPCODE_CLOSE:
    प्रत्यागम "CLOSE";
  पक्ष WSBIT_OPCODE_PING:
    प्रत्यागम "PING";
  पक्ष WSBIT_OPCODE_PONG:
    प्रत्यागम "PONG";
  स्वभाव:
    प्रत्यागम "???";
  }
}
#endif

स्थिर पूर्णाङ्क ws_frame_firstbyte2flags(संरचना Curl_easy *data,
                                    uint8_t firstbyte, पूर्णाङ्क cont_flags)
{
  विकल्प(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  पक्ष WSBIT_OPCODE_CONT:
    यदि(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      प्रत्यागम 0;
    }
    प्रत्यागम cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  पक्ष (WSBIT_OPCODE_CONT | WSBIT_FIN):
    यदि(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      प्रत्यागम 0;
    }
    प्रत्यागम cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  पक्ष WSBIT_OPCODE_TEXT:
    यदि(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      प्रत्यागम 0;
    }
    प्रत्यागम CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  पक्ष (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    यदि(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      प्रत्यागम 0;
    }
    प्रत्यागम CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  पक्ष WSBIT_OPCODE_BIN:
    यदि(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      प्रत्यागम 0;
    }
    प्रत्यागम CURLWS_द्विआधारी | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  पक्ष (WSBIT_OPCODE_BIN | WSBIT_FIN):
    यदि(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      प्रत्यागम 0;
    }
    प्रत्यागम CURLWS_द्विआधारी;
  /* 0x08 - first CLOSE fragment */
  पक्ष WSBIT_OPCODE_CLOSE:
    failf(data, "[WS] invalid fragmented CLOSE frame");
    प्रत्यागम 0;
  /* 0x88 - unfragmented CLOSE */
  पक्ष (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    प्रत्यागम CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  पक्ष WSBIT_OPCODE_PING:
    failf(data, "[WS] invalid fragmented PING frame");
    प्रत्यागम 0;
  /* 0x89 - unfragmented PING */
  पक्ष (WSBIT_OPCODE_PING | WSBIT_FIN):
    प्रत्यागम CURLWS_PING;
  /* 0x0a - first PONG fragment */
  पक्ष WSBIT_OPCODE_PONG:
    failf(data, "[WS] invalid fragmented PONG frame");
    प्रत्यागम 0;
  /* 0x8a - unfragmented PONG */
  पक्ष (WSBIT_OPCODE_PONG | WSBIT_FIN):
    प्रत्यागम CURLWS_PONG;
  /* invalid first byte */
  स्वभाव:
    यदि(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(data, "[WS] invalid reserved bits: %02x", firstbyte);
    अन्यथा
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(data, "[WS] invalid opcode: %02x", firstbyte);
    प्रत्यागम 0;
  }
}

स्थिर CURLcode ws_frame_flags2firstbyte(संरचना Curl_easy *data,
                                         अचिन्हित पूर्णाङ्क flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  विकल्प(flags & ~CURLWS_OFFSET) {
  पक्ष 0:
    यदि(contfragment) {
      CURL_TRC_WS(data, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      प्रत्यागम CURLE_OK;
    }
    failf(data, "[WS] no flags given");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  पक्ष CURLWS_CONT:
    यदि(contfragment) {
      infof(data, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      प्रत्यागम CURLE_OK;
    }
    failf(data, "[WS] No ongoing fragmented message to continue");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  पक्ष CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    प्रत्यागम CURLE_OK;
  पक्ष (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    प्रत्यागम CURLE_OK;
  पक्ष CURLWS_द्विआधारी:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    प्रत्यागम CURLE_OK;
  पक्ष (CURLWS_द्विआधारी | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    प्रत्यागम CURLE_OK;
  पक्ष CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    प्रत्यागम CURLE_OK;
  पक्ष (CURLWS_CLOSE | CURLWS_CONT):
    failf(data, "[WS] CLOSE frame must not be fragmented");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  पक्ष CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    प्रत्यागम CURLE_OK;
  पक्ष (CURLWS_PING | CURLWS_CONT):
    failf(data, "[WS] PING frame must not be fragmented");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  पक्ष CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    प्रत्यागम CURLE_OK;
  पक्ष (CURLWS_PONG | CURLWS_CONT):
    failf(data, "[WS] PONG frame must not be fragmented");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  स्वभाव:
    failf(data, "[WS] unknown flags: %x", flags);
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

स्थिर शून्य ws_dec_info(संरचना ws_decoder *dec, संरचना Curl_easy *data,
                        ध्रुव वर्ण *संदेश)
{
  NOVERBOSE((शून्य)संदेश);
  विकल्प(dec->head_len) {
  पक्ष 0:
    भङ्ग;
  पक्ष 1:
    CURL_TRC_WS(data, "decoded %s [%s%s]", संदेश,
                ws_frame_name_of_op(dec->head[0]),
                (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    भङ्ग;
  स्वभाव:
    यदि(dec->head_len < dec->head_total) {
      CURL_TRC_WS(data, "decoded %s [%s%s](%d/%d)", संदेश,
                  ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->head_len, dec->head_total);
    }
    अन्यथा {
      CURL_TRC_WS(data, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  संदेश, ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    भङ्ग;
  }
}

स्थिर CURLcode ws_send_raw_blocking(संरचना Curl_easy *data,
                                     संरचना websocket *ws,
                                     ध्रुव वर्ण *buffer, माप_प्रकार buflen);

प्रकारनिर्देश CURLcode ws_write_payload(ध्रुव uint8_t *buf, माप_प्रकार buflen,
                                  पूर्णाङ्क frame_age, पूर्णाङ्क frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  शून्य *userp,
                                  माप_प्रकार *pnwritten);

स्थिर शून्य ws_dec_next_frame(संरचना ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

स्थिर शून्य ws_dec_reset(संरचना ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

स्थिर शून्य ws_dec_init(संरचना ws_decoder *dec)
{
  ws_dec_reset(dec);
}

स्थिर CURLcode ws_dec_read_head(संरचना ws_decoder *dec,
                                 संरचना Curl_easy *data,
                                 संरचना bufq *inraw)
{
  ध्रुव uint8_t *inbuf;
  माप_प्रकार inlen;

  यावत्(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    यदि(dec->head_len == 0) {
      dec->head[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(data, dec->head[0],
                                                  dec->cont_flags);
      यदि(!dec->frame_flags) {
        ws_dec_reset(dec);
        प्रत्यागम CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      यदि(dec->frame_flags & (CURLWS_TEXT | CURLWS_द्विआधारी)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->head_len = 1;
#यदि 0
      ws_dec_info(dec, data, "seeing opcode");
#endif
      अनुवर्त;
    }
    अन्यथा यदि(dec->head_len == 1) {
      dec->head[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->head_len = 2;

      यदि(dec->head[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(data, "[WS] masked input frame");
        ws_dec_reset(dec);
        प्रत्यागम CURLE_RECV_ERROR;
      }
      यदि(dec->frame_flags & CURLWS_PING && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(data, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        प्रत्यागम CURLE_RECV_ERROR;
      }
      यदि(dec->frame_flags & CURLWS_PONG && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(data, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        प्रत्यागम CURLE_RECV_ERROR;
      }
      यदि(dec->frame_flags & CURLWS_CLOSE && dec->head[1] > WS_MAX_CNTRL_LEN) {
        failf(data, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        प्रत्यागम CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      यदि(dec->head[1] == 126) {
        dec->head_total = 4;
        अनुवर्त;
      }
      अन्यथा यदि(dec->head[1] == 127) {
        dec->head_total = 10;
        अनुवर्त;
      }
      अन्यथा {
        dec->head_total = 2;
      }
    }

    यदि(dec->head_len < dec->head_total) {
      dec->head[dec->head_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->head_len;
      यदि(dec->head_len < dec->head_total) {
#यदि 0
        ws_dec_info(dec, data, "decoding head");
#endif
        अनुवर्त;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->head_len == dec->head_total);
    विकल्प(dec->head_total) {
    पक्ष 2:
      dec->payload_len = dec->head[1];
      भङ्ग;
    पक्ष 4:
      dec->payload_len = (dec->head[2] << 8) | dec->head[3];
      भङ्ग;
    पक्ष 10:
      यदि(dec->head[2] > 127) {
        failf(data, "[WS] frame length longer than 63 bits not supported");
        प्रत्यागम CURLE_RECV_ERROR;
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
      भङ्ग;
    स्वभाव:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(data, "[WS] unexpected frame header length");
      प्रत्यागम CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, data, "head");
    प्रत्यागम CURLE_OK;
  }
  प्रत्यागम CURLE_AGAIN;
}

स्थिर CURLcode ws_dec_pass_payload(संरचना ws_decoder *dec,
                                    संरचना Curl_easy *data,
                                    संरचना bufq *inraw,
                                    ws_write_payload *write_cb,
                                    शून्य *write_ctx)
{
  ध्रुव uint8_t *inbuf;
  माप_प्रकार inlen;
  माप_प्रकार nwritten;
  CURLcode result;
  माप_प्रकार remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  यावत्(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(data)) {
    यदि(inlen > remain)
      inlen = remain;
    result = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    यदि(result)
      प्रत्यागम result;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(data, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  प्रत्यागम remain ? CURLE_AGAIN : CURLE_OK;
}

स्थिर CURLcode ws_dec_pass(संरचना ws_decoder *dec,
                            संरचना Curl_easy *data,
                            संरचना bufq *inraw,
                            ws_write_payload *write_cb,
                            शून्य *write_ctx)
{
  CURLcode result;

  यदि(Curl_bufq_is_empty(inraw))
    प्रत्यागम CURLE_AGAIN;

  विकल्प(dec->state) {
  पक्ष WS_DEC_INIT:
    ws_dec_next_frame(dec);
    dec->state = WS_DEC_HEAD;
    FALLTHROUGH();
  पक्ष WS_DEC_HEAD:
    result = ws_dec_read_head(dec, data, inraw);
    यदि(result) {
      यदि(result != CURLE_AGAIN) {
        failf(data, "[WS] decode frame error %d", (पूर्णाङ्क)result);
        भङ्ग;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      भङ्ग;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    यदि(dec->payload_len == 0) {
      माप_प्रकार nwritten;
      ध्रुव uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      result = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      यदि(result)
        प्रत्यागम result;
      dec->state = WS_DEC_INIT;
      भङ्ग;
    }
    FALLTHROUGH();
  पक्ष WS_DEC_PAYLOAD:
    result = ws_dec_pass_payload(dec, data, inraw, write_cb, write_ctx);
    ws_dec_info(dec, data, "passing");
    यदि(result)
      प्रत्यागम result;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    भङ्ग;
  स्वभाव:
    /* we covered all enums above, but some code analyzers are wimps */
    result = CURLE_FAILED_INIT;
  }
  प्रत्यागम result;
}

स्थिर शून्य update_meta(संरचना websocket *ws,
                        पूर्णाङ्क frame_age, पूर्णाङ्क frame_flags,
                        curl_off_t payload_offset,
                        curl_off_t payload_len,
                        माप_प्रकार cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_offset - cur_len);

  ws->recvframe.age = frame_age;
  ws->recvframe.flags = frame_flags;
  ws->recvframe.offset = payload_offset;
  ws->recvframe.len = cur_len;
  ws->recvframe.bytesleft = bytesleft;
}

/* WebSocket decoding client writer */
संरचना ws_cw_ctx {
  संरचना Curl_cwriter super;
  संरचना bufq buf;
};

स्थिर CURLcode ws_cw_init(संरचना Curl_easy *data,
                           संरचना Curl_cwriter *writer)
{
  संरचना ws_cw_ctx *ctx = writer->ctx;
  (शून्य)data;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_SIZE, 1, BUFQ_OPT_SOFT_LIMIT);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य ws_cw_close(संरचना Curl_easy *data, संरचना Curl_cwriter *writer)
{
  संरचना ws_cw_ctx *ctx = writer->ctx;
  (शून्य)data;
  Curl_bufq_free(&ctx->buf);
}

संरचना ws_cw_dec_ctx {
  संरचना Curl_easy *data;
  संरचना websocket *ws;
  संरचना Curl_cwriter *next_writer;
  पूर्णाङ्क cw_type;
};

स्थिर CURLcode ws_flush(संरचना Curl_easy *data, संरचना websocket *ws,
                         bool blocking);
स्थिर CURLcode ws_enc_add_pending(संरचना Curl_easy *data,
                                   संरचना websocket *ws);

स्थिर CURLcode ws_enc_add_cntrl(संरचना Curl_easy *data,
                                 संरचना websocket *ws,
                                 ध्रुव uint8_t *payload,
                                 माप_प्रकार plen,
                                 अचिन्हित पूर्णाङ्क frame_type)
{
  (शून्य)data;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  यदि(plen > WS_MAX_CNTRL_LEN)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  स_नकल(ws->pending.payload, payload, plen);
  प्रत्यागम CURLE_OK;
}

स्थिर curl_off_t ws_payload_remain(curl_off_t payload_total,
                                    curl_off_t payload_offset,
                                    माप_प्रकार payload_buffered)
{
  curl_off_t buffered, remain = payload_total - payload_offset;
  यदि((payload_total < 0) || (payload_offset < 0) || (remain < 0))
    प्रत्यागम -1;
  buffered = curlx_uztoso(payload_buffered);
  यदि(remain < buffered)
    प्रत्यागम -1;
  प्रत्यागम remain - buffered;
}

स्थिर CURLcode ws_cw_dec_next(ध्रुव uint8_t *buf, माप_प्रकार buflen,
                               पूर्णाङ्क frame_age, पूर्णाङ्क frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               शून्य *user_data,
                               माप_प्रकार *pnwritten)
{
  संरचना ws_cw_dec_ctx *ctx = user_data;
  संरचना Curl_easy *data = ctx->data;
  संरचना websocket *ws = ctx->ws;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result;

  (शून्य)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  यदि(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }

  यदि(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(data, ws, buf, buflen, CURLWS_PONG);
    यदि(result)
      प्रत्यागम result;
  }
  अन्यथा यदि(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(data, "[WS] pass %zu decoded bytes", buflen);
    result = Curl_cwriter_write(data, ctx->next_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ध्रुव वर्ण *)buf, buflen);
    यदि(result)
      प्रत्यागम result;
  }
  *pnwritten = buflen;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ws_cw_write(संरचना Curl_easy *data,
                            संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                            ध्रुव वर्ण *buf, माप_प्रकार nbytes)
{
  संरचना ws_cw_ctx *ctx = writer->ctx;
  संरचना websocket *ws;
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[WS] write(len=%zu, type=%d)", nbytes, type);
  यदि(!(type & CLIENTWRITE_BODY) || data->set.ws_raw_mode)
    प्रत्यागम Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  यदि(!ws) {
    failf(data, "[WS] not a websocket transfer");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  यदि(nbytes) {
    माप_प्रकार nwritten;
    result = Curl_bufq_write(&ctx->buf, (ध्रुव uint8_t *)buf,
                             nbytes, &nwritten);
    यदि(result) {
      infof(data, "[WS] error adding data to buffer %d", (पूर्णाङ्क)result);
      प्रत्यागम result;
    }
  }

  result = Curl_cwriter_flush(data, writer->next);
  यदि(result)
    गच्छ out;

  यावत्(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
    संरचना ws_cw_dec_ctx pass_ctx;
    pass_ctx.data = data;
    pass_ctx.ws = ws;
    pass_ctx.next_writer = writer->next;
    pass_ctx.cw_type = type;
    result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                         ws_cw_dec_next, &pass_ctx);
    यदि(result == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      result = CURLE_OK;
      गच्छ out;
    }
    अन्यथा यदि(result) {
      failf(data, "[WS] decode payload error %d", (पूर्णाङ्क)result);
      Curl_bufq_reset(&ctx->buf);
      गच्छ out;
    }
  }

out:
  यदि(!result && !ws->using_reader) {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    यदि(result == CURLE_AGAIN)
      result = CURLE_OK;
  }

  यदि(!result && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(data, "[WS] decode ended with incomplete frame");
    result = CURLE_RECV_ERROR;
  }
  प्रत्यागम result;
}

स्थिर CURLcode ws_cw_flush(संरचना Curl_easy *data,
                            संरचना Curl_cwriter *writer)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[ws] flush");
  यदि(!data->set.ws_raw_mode) {
    संरचना ws_cw_ctx *ctx = writer->ctx;
    संरचना websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    result = Curl_cwriter_flush(data, writer->next);
    यदि(result)
      गच्छ out;

    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    यदि(!ws) {
      failf(data, "[WS] not a websocket transfer");
      प्रत्यागम CURLE_FAILED_INIT;
    }

    यावत्(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
      संरचना ws_cw_dec_ctx pass_ctx;
      pass_ctx.data = data;
      pass_ctx.ws = ws;
      pass_ctx.next_writer = writer->next;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                           ws_cw_dec_next, &pass_ctx);
      यदि(result == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        result = CURLE_OK;
        गच्छ out;
      }
      अन्यथा यदि(result) {
        failf(data, "[WS] decode payload error %d", (पूर्णाङ्क)result);
        Curl_bufq_reset(&ctx->buf);
        गच्छ out;
      }
    }
  }

out:
  यदि(!result)
    result = Curl_cwriter_flush(data, writer->next);
  प्रत्यागम result;
}

/* WebSocket payload decoding client writer. */
स्थिर ध्रुव संरचना Curl_cwtype ws_cw_decode = {
  "ws-decode",
  रिक्त,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  परिमाण(संरचना ws_cw_ctx)
};

स्थिर शून्य ws_enc_info(संरचना ws_encoder *enc, संरचना Curl_easy *data,
                        ध्रुव वर्ण *संदेश)
{
  NOVERBOSE((शून्य)enc);
  NOVERBOSE((शून्य)संदेश);
  CURL_TRC_WS(data, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              संदेश, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

स्थिर शून्य ws_enc_reset(संरचना ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

स्थिर शून्य ws_enc_init(संरचना ws_encoder *enc)
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

स्थिर CURLcode ws_enc_add_frame(संरचना Curl_easy *data,
                                 संरचना ws_encoder *enc,
                                 अचिन्हित पूर्णाङ्क flags,
                                 curl_off_t payload_len,
                                 संरचना bufq *out)
{
  uint8_t firstb = 0;
  uint8_t head[14];
  CURLcode result;
  माप_प्रकार hlen, nwritten;

  यदि(payload_len < 0) {
    failf(data, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    प्रत्यागम CURLE_SEND_ERROR;
  }

  यदि(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(data, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    प्रत्यागम CURLE_SEND_ERROR;
  }

  result = ws_frame_flags2firstbyte(data, flags, (bool)enc->contfragment,
                                    &firstb);
  यदि(result)
    प्रत्यागम result;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  यदि(flags & (CURLWS_TEXT | CURLWS_द्विआधारी)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  यदि(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PING frame is too big");
    प्रत्यागम CURLE_TOO_LARGE;
  }
  यदि(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PONG frame is too big");
    प्रत्यागम CURLE_TOO_LARGE;
  }
  यदि(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given CLOSE frame is too big");
    प्रत्यागम CURLE_TOO_LARGE;
  }

  head[0] = enc->firstbyte = firstb;
  यदि(payload_len > 65535) {
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
  अन्यथा यदि(payload_len >= 126) {
    head[1] = 126 | WSBIT_MASK;
    head[2] = (uint8_t)((payload_len >> 8) & 0xff);
    head[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  अन्यथा {
    head[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, data, "sending");

  /* 4 bytes random */

  result = Curl_rand(data, (uint8_t *)&enc->mask, परिमाण(enc->mask));
  यदि(result)
    प्रत्यागम result;

#ifdef DEBUGBUILD
  यदि(दो_पर्या("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    स_रखो(&enc->mask, 0, परिमाण(enc->mask));
#endif

  /* add 4 bytes mask */
  स_नकल(&head[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  result = Curl_bufq_write(out, head, hlen, &nwritten);
  यदि(result)
    प्रत्यागम result;
  यदि(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ws_enc_write_head(संरचना Curl_easy *data,
                                  संरचना websocket *ws,
                                  संरचना ws_encoder *enc,
                                  अचिन्हित पूर्णाङ्क flags,
                                  curl_off_t payload_len,
                                  संरचना bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  यदि(ws->pending.type) {
    CURLcode result = ws_enc_add_pending(data, ws);
    यदि(result)
      प्रत्यागम result;
  }
  प्रत्यागम ws_enc_add_frame(data, enc, flags, payload_len, out);
}

स्थिर CURLcode ws_enc_write_payload(संरचना ws_encoder *enc,
                                     संरचना Curl_easy *data,
                                     ध्रुव uint8_t *buf, माप_प्रकार buflen,
                                     संरचना bufq *out, माप_प्रकार *pnwritten)
{
  CURLcode result;
  माप_प्रकार i, len, n, remain;

  *pnwritten = 0;
  यदि(Curl_bufq_is_full(out))
    प्रत्यागम CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  यदि(remain < len)
    len = remain;

  प्रति(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    result = Curl_bufq_write(out, &c, 1, &n);
    यदि(result) {
      यदि((result != CURLE_AGAIN) || !i)
        प्रत्यागम result;
      भङ्ग;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, data, "buffered");
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ws_enc_add_pending(संरचना Curl_easy *data,
                                   संरचना websocket *ws)
{
  CURLcode result;
  माप_प्रकार n;

  यदि(!ws->pending.type) /* no pending frame here */
    प्रत्यागम CURLE_OK;
  यदि(ws->enc.payload_remain) /* in the middle of another frame */
    प्रत्यागम CURLE_AGAIN;

  result = ws_enc_add_frame(data, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  यदि(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding head: %d",
                (पूर्णाङ्क)result);
    गच्छ out;
  }
  result = ws_enc_write_payload(&ws->enc, data, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  यदि(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding payload: %d",
                (पूर्णाङ्क)result);
    गच्छ out;
  }
  यदि(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(data, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    result = CURLE_SEND_ERROR;
    गच्छ out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  स_रखो(&ws->pending, 0, परिमाण(ws->pending));

out:
  प्रत्यागम result;
}

स्थिर CURLcode ws_enc_send(संरचना Curl_easy *data,
                            संरचना websocket *ws,
                            ध्रुव uint8_t *buffer,
                            माप_प्रकार buflen,
                            curl_off_t fragsize,
                            अचिन्हित पूर्णाङ्क flags,
                            माप_प्रकार *pnsent)
{
  माप_प्रकार n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!data->set.ws_raw_mode);
  *pnsent = 0;

  यदि(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    यदि(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(data, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    }
    यदि((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(data, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  अन्यथा {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    यदि(result)
      प्रत्यागम result;

    result = ws_enc_write_head(data, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    यदि(result) {
      CURL_TRC_WS(data, "curl_ws_send(), error writing frame head %d",
                  (पूर्णाङ्क)result);
      प्रत्यागम result;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  यावत्(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    यदि(buflen > ws->sendbuf_payload) {
      माप_प्रकार prev_len = Curl_bufq_len(&ws->sendbuf);
      result = ws_enc_write_payload(&ws->enc, data,
                                    buffer + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      यदि(result && (result != CURLE_AGAIN))
        प्रत्यागम result;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      यदि(!ws->sendbuf_payload) {
        प्रत्यागम CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    यदि(!result && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      buffer += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    अन्यथा यदि(result == CURLE_AGAIN) {
      यदि(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        माप_प्रकार flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        प्रत्यागम CURLE_OK;
      }
      अन्यथा {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(data, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        प्रत्यागम CURLE_AGAIN;
      }
    }
    अन्यथा
      प्रत्यागम result;  /* real error sending the data */
  }
  प्रत्यागम CURLE_OK;
}

संरचना cr_ws_ctx {
  संरचना Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

स्थिर CURLcode cr_ws_init(संरचना Curl_easy *data, संरचना Curl_creader *reader)
{
  (शून्य)data;
  (शून्य)reader;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cr_ws_close(संरचना Curl_easy *data, संरचना Curl_creader *reader)
{
  (शून्य)data;
  (शून्य)reader;
}

स्थिर CURLcode cr_ws_read(संरचना Curl_easy *data,
                           संरचना Curl_creader *reader,
                           वर्ण *buf, माप_प्रकार blen,
                           माप_प्रकार *pnread, bool *peos)
{
  संरचना cr_ws_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  माप_प्रकार nread, n;
  संरचना websocket *ws;
  bool eos;

  *pnread = 0;
  यदि(ctx->eos) {
    *peos = TRUE;
    प्रत्यागम CURLE_OK;
  }

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  यदि(!ws) {
    failf(data, "[WS] not a websocket transfer");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  यदि(Curl_bufq_is_empty(&ws->sendbuf)) {
    यदि(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      प्रत्यागम CURLE_OK;
    }

    यदि(ws->enc.payload_remain) {
      CURL_TRC_WS(data, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    यदि(result)
      प्रत्यागम result;
    ctx->read_eos = eos;

    यदि(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(data, reader->next);
    }
    अन्यथा यदि(!nread) {
      /* nothing to convert, return this right away */
      यदि(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      गच्छ out;
    }

    यदि(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      result = ws_enc_write_head(data, ws, &ws->enc, CURLWS_द्विआधारी, nread,
                                 &ws->sendbuf);
      यदि(result)
        गच्छ out;
    }

    result = ws_enc_write_payload(&ws->enc, data, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    यदि(result)
      गच्छ out;
    CURL_TRC_READ(data, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  यदि(!result && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (पूर्णाङ्क)result, *pnread, *peos);
  प्रत्यागम result;
}

स्थिर curl_off_t cr_ws_total_length(संरचना Curl_easy *data,
                                     संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)data;
  (शून्य)reader;
  प्रत्यागम -1;
}

स्थिर ध्रुव संरचना Curl_crtype ws_cr_encode = {
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
  परिमाण(संरचना cr_ws_ctx)
};

संरचना wsfield {
  ध्रुव वर्ण *name;
  ध्रुव वर्ण *val;
};

CURLcode Curl_ws_request(संरचना Curl_easy *data, संरचना dynbuf *req)
{
  अचिन्हित पूर्णाङ्क i;
  CURLcode result = CURLE_OK;
  uint8_t अक्रम[16];
  वर्ण *randstr;
  माप_प्रकार randlen;
  वर्ण keyval[40];
  संरचना SingleRequest *k = &data->req;
  संरचना wsfield heads[] = {
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
      "Sec-WebSocket-Key", रिक्त,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  result = Curl_rand(data, अक्रम, परिमाण(अक्रम));
  यदि(result)
    प्रत्यागम result;
  result = curlx_base64_encode(अक्रम, परिमाण(अक्रम), &randstr, &randlen);
  यदि(result)
    प्रत्यागम result;
  DEBUGASSERT(randlen < परिमाण(keyval));
  यदि(randlen >= परिमाण(keyval)) {
    curlx_free(randstr);
    प्रत्यागम CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, परिमाण(keyval), randstr, randlen);
  curlx_free(randstr);
  प्रति(i = 0; !result && (i < CURL_ARRAYSIZE(heads)); i++) {
    यदि(!Curl_checkheaders(data, heads[i].name, म_माप(heads[i].name))) {
      result = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  data->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  data->conn->bits.upgrade_in_progress = TRUE;
  प्रत्यागम result;
}

स्थिर शून्य ws_conn_dtor(ध्रुव शून्य *कुंजी, माप_प्रकार klen, शून्य *entry)
{
  संरचना websocket *ws = entry;
  (शून्य)कुंजी;
  (शून्य)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(संरचना Curl_easy *data,
                        ध्रुव वर्ण *mem, माप_प्रकार nread)
{
  संरचना SingleRequest *k = &data->req;
  संरचना websocket *ws;
  संरचना Curl_cwriter *ws_dec_writer = रिक्त;
  संरचना Curl_creader *ws_enc_reader = रिक्त;
  CURLcode result;

  DEBUGASSERT(data->conn);
  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  यदि(!ws) {
    माप_प्रकार chunk_size = WS_CHUNK_SIZE;
    ws = curlx_calloc(1, परिमाण(*ws));
    यदि(!ws)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
#ifdef DEBUGBUILD
    {
      ध्रुव वर्ण *p = दो_पर्या("CURL_WS_CHUNK_SIZE");
      यदि(p) {
        curl_off_t l;
        यदि(!curlx_str_संख्या(&p, &l, 1 * 1024 * 1024))
          chunk_size = (माप_प्रकार)l;
      }
    }
#endif
    CURL_TRC_WS(data, "WS, using chunk size %zu", chunk_size);
    Curl_bufq_init2(&ws->recvbuf, chunk_size, WS_CHUNK_गिनती,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_size, WS_CHUNK_गिनती,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    result = Curl_conn_meta_set(data->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    यदि(result)
      प्रत्यागम result;
  }
  अन्यथा {
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
  यदि(result)
    गच्छ out;
  result = Curl_cwriter_add(data, ws_dec_writer);
  यदि(result)
    गच्छ out;
  ws_dec_writer = रिक्त; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  यदि(data->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    यदि(nread) {
      result = Curl_cf_recvbuf_add(data, data->conn, FIRSTSOCKET,
                                   (ध्रुव uint8_t *)mem, nread);
      यदि(result)
        गच्छ out;
    }
    CURL_REQ_CLEAR_RECV(data); /* read no more content */
  }
  अन्यथा { /* !connect_only */
    यदि(data->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(data, "UPLOAD set, add ws-encode reader");
      result = Curl_creader_set_fread(data, -1);
      यदि(result)
        गच्छ out;

      यदि(!data->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        result = Curl_creader_create(&ws_enc_reader, data, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        यदि(result)
          गच्छ out;
        result = Curl_creader_add(data, ws_enc_reader);
        यदि(result)
          गच्छ out;
        ws_enc_reader = रिक्त; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      data->req.eos_read = FALSE;
      data->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(data);
    }

    /* Then pass any additional data to the writers */
    यदि(nread) {
      result = Curl_client_write(data, CLIENTWRITE_BODY, mem, nread);
      यदि(result)
        गच्छ out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  यदि(ws_dec_writer)
    Curl_cwriter_free(data, ws_dec_writer);
  यदि(ws_enc_reader)
    Curl_creader_free(data, ws_enc_reader);
  यदि(result)
    CURL_TRC_WS(data, "Curl_ws_accept() failed -> %d", (पूर्णाङ्क)result);
  अन्यथा
    CURL_TRC_WS(data, "websocket established, %s mode",
                data->set.connect_only ? "connect-only" : "callback");
  प्रत्यागम result;
}

संरचना ws_collect {
  संरचना Curl_easy *data;
  संरचना websocket *ws;
  uint8_t *buffer;
  माप_प्रकार buflen;
  माप_प्रकार bufidx;
  पूर्णाङ्क frame_age;
  पूर्णाङ्क frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

स्थिर CURLcode ws_client_collect(ध्रुव uint8_t *buf, माप_प्रकार buflen,
                                  पूर्णाङ्क frame_age, पूर्णाङ्क frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  शून्य *userp,
                                  माप_प्रकार *pnwritten)
{
  संरचना ws_collect *ctx = userp;
  संरचना Curl_easy *data = ctx->data;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  यदि(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }

  यदि(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  यदि(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(ctx->data, ctx->ws, buf, buflen, CURLWS_PONG);
    यदि(result)
      प्रत्यागम result;
    *pnwritten = buflen;
  }
  अन्यथा {
    माप_प्रकार write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    यदि(!write_len) {
      यदि(!buflen)  /* 0 length write, we accept that */
        प्रत्यागम CURLE_OK;
      प्रत्यागम CURLE_AGAIN;  /* no more space */
    }
    स_नकल(ctx->buffer + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  प्रत्यागम result;
}

स्थिर CURLcode nw_in_recv(शून्य *reader_ctx,
                           uint8_t *buf, माप_प्रकार buflen,
                           माप_प्रकार *pnread)
{
  संरचना Curl_easy *data = reader_ctx;
  प्रत्यागम Curl_easy_recv(data, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, शून्य *buffer,
                      माप_प्रकार buflen, माप_प्रकार *recv,
                      ध्रुव संरचना curl_ws_frame **metap)
{
  संरचना Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  यदि(CURL_EAPI_ENTER(&guard, curl, ws_recv, &result)) {
    संरचना Curl_easy *data = curl;
    संरचना connectdata *conn;
    संरचना websocket *ws;
    संरचना ws_collect ctx;

    यदि(!recv || !metap || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      गच्छ out;
    }

    *recv = 0;
    *metap = रिक्त;

    conn = data->conn;
    यदि(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      यदि(!data->set.connect_only) {
        failf(data, "[WS] CONNECT_ONLY is required");
        result = CURLE_UNSUPPORTED_PROTOCOL;
        गच्छ out;
      }

      Curl_getconnectinfo(data, &conn);
      यदि(!conn) {
        failf(data, "[WS] connection not found");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        गच्छ out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    यदि(!ws) {
      failf(data, "[WS] connection is not setup for websocket");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      गच्छ out;
    }

    स_रखो(&ctx, 0, परिमाण(ctx));
    ctx.data = data;
    ctx.ws = ws;
    ctx.buffer = buffer;
    ctx.buflen = buflen;

    यावत्(1) {
      /* receive more when our buffer is empty */
      यदि(Curl_bufq_is_empty(&ws->recvbuf)) {
        माप_प्रकार n;
        result = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, data, &n);
        यदि(result)
          गच्छ out;
        अन्यथा यदि(n == 0) {
          /* connection closed */
          infof(data, "[WS] connection expectedly closed?");
          result = CURLE_GOT_NOTHING;
          गच्छ out;
        }
        CURL_TRC_WS(data, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      result = ws_dec_pass(&ws->dec, data, &ws->recvbuf,
                           ws_client_collect, &ctx);
      यदि(result == CURLE_AGAIN) {
        यदि(!ctx.written) {
          ws_dec_info(&ws->dec, data, "need more input");
          अनुवर्त;  /* nothing written, try more input */
        }
        भङ्ग;
      }
      अन्यथा यदि(result) {
        गच्छ out;
      }
      अन्यथा यदि(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        भङ्ग;
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
    यदि(!data->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(data, ws);
      यदि(!r2)
        (शून्य)ws_flush(data, ws, Curl_api_is_in_callback(data));
    }
    result = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम result;
}

स्थिर CURLcode ws_flush(संरचना Curl_easy *data, संरचना websocket *ws,
                         bool blocking)
{
  CURLcode result;

  /* If there is space, add any pending control frame */
  यदि(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_size) {
    result = ws_enc_add_pending(data, ws);
    यदि(result && (result != CURLE_AGAIN))
      प्रत्यागम result;
  }

  यदि(!Curl_bufq_is_empty(&ws->sendbuf)) {
    ध्रुव uint8_t *out;
    माप_प्रकार outlen, n;
#ifdef DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_next = FALSE;
    माप_प्रकार chunk_egain = 0;
    ध्रुव वर्ण *p = दो_पर्या("CURL_WS_CHUNK_EAGAIN");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_संख्या(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (माप_प्रकार)l;
    }
#endif

    यावत्(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#ifdef DEBUGBUILD
      यदि(eagain_next)
        प्रत्यागम CURLE_AGAIN;
      यदि(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_next = TRUE;
      }
#endif
      यदि(blocking) {
        result = ws_send_raw_blocking(data, ws, (ध्रुव वर्ण *)out, outlen);
        n = result ? 0 : outlen;
      }
      अन्यथा यदि(data->set.connect_only || Curl_api_is_in_callback(data))
        result = Curl_senddata(data, out, outlen, &n);
      अन्यथा {
        result = Curl_xfer_send(data, out, outlen, FALSE, &n);
        यदि(!result && !n && outlen)
          result = CURLE_AGAIN;
      }

      यदि(result == CURLE_AGAIN) {
        CURL_TRC_WS(data, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        प्रत्यागम result;
      }
      अन्यथा यदि(result) {
        failf(data, "[WS] flush, write error %d", (पूर्णाङ्क)result);
        प्रत्यागम result;
      }
      अन्यथा {
        CURL_TRC_WS(data, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ws_send_raw_blocking(संरचना Curl_easy *data,
                                     संरचना websocket *ws,
                                     ध्रुव वर्ण *buffer, माप_प्रकार buflen)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार nwritten;

  यदि(!data)
    प्रत्यागम result;

  (शून्य)ws;
  यावत्(buflen) {
    result = Curl_xfer_send(data, buffer, buflen, FALSE, &nwritten);
    यदि(result)
      प्रत्यागम result;
    DEBUGASSERT(nwritten <= buflen);
    buffer += nwritten;
    buflen -= nwritten;
    यदि(buflen) {
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      timediff_t बायाँ_ms;
      पूर्णाङ्क ev;

      CURL_TRC_WS(data, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      बायाँ_ms = Curl_timeleft_ms(data);
      यदि(बायाँ_ms < 0) {
        failf(data, "[WS] Timeout waiting for socket becoming writable");
        प्रत्यागम CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      यदि(sock == CURL_SOCKET_BAD)
        प्रत्यागम CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, बायाँ_ms ? बायाँ_ms : 500);
      यदि(ev < 0) {
        failf(data, "[WS] Error while waiting for socket becoming writable");
        प्रत्यागम CURLE_SEND_ERROR;
      }
    }
  }
  प्रत्यागम result;
}

स्थिर CURLcode ws_send_raw(संरचना Curl_easy *data, ध्रुव शून्य *buffer,
                            माप_प्रकार buflen, माप_प्रकार *pnwritten)
{
  संरचना websocket *ws;
  CURLcode result;

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  यदि(!ws) {
    failf(data, "[WS] Not a websocket transfer");
    प्रत्यागम CURLE_SEND_ERROR;
  }
  यदि(!buflen)
    प्रत्यागम CURLE_OK;

  यदि(Curl_api_is_in_callback(data)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    result = ws_flush(data, ws, TRUE);
    यदि(result)
      प्रत्यागम result;
    result = ws_send_raw_blocking(data, ws, buffer, buflen);
    यदि(!result)
      *pnwritten = buflen;
  }
  अन्यथा {
    /* We need any pending data to be sent or EAGAIN this call. */
    result = ws_flush(data, ws, FALSE);
    यदि(result)
      प्रत्यागम result;
    result = Curl_senddata(data, buffer, buflen, pnwritten);
  }

  CURL_TRC_WS(data, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (पूर्णाङ्क)result, *pnwritten);
  प्रत्यागम result;
}

CURLcode curl_ws_send(CURL *curl, ध्रुव शून्य *buffer_arg,
                      माप_प्रकार buflen, माप_प्रकार *sent,
                      curl_off_t fragsize,
                      अचिन्हित पूर्णाङ्क flags)
{
  संरचना Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  यदि(CURL_EAPI_ENTER(&guard, curl, ws_send, &result)) {
    संरचना websocket *ws;
    ध्रुव uint8_t *buffer = buffer_arg;
    संरचना Curl_easy *data = curl;
    माप_प्रकार ndummy;
    माप_प्रकार *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, data->set.ws_raw_mode);

    *pnsent = 0;

    यदि(!buffer && buflen) {
      failf(data, "[WS] buffer is NULL when buflen is not");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      गच्छ out;
    }

    यदि(!data->conn && data->set.connect_only) {
      result = Curl_connect_only_attach(data);
      यदि(result)
        गच्छ out;
    }
    यदि(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    यदि(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }

    यदि(data->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      result = ws_flush(data, ws, FALSE);
      यदि(result)
        गच्छ out;

      यदि(!buffer) {
        failf(data, "[WS] buffer is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        गच्छ out;
      }
      यदि(!sent) {
        failf(data, "[WS] sent is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        गच्छ out;
      }
      यदि(fragsize || flags) {
        failf(data, "[WS] fragsize and flags must be zero in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        गच्छ out;
      }
      result = ws_send_raw(data, buffer, buflen, pnsent);
      गच्छ out;
    }

    /* Not RAW mode, we do the frame encoding */
    result = ws_enc_send(data, ws, buffer, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, data->set.ws_raw_mode, (पूर्णाङ्क)result,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम result;
}

स्थिर CURLcode ws_setup_conn(संरचना Curl_easy *data,
                              संरचना connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  data->state.http_neg.accept_09 = FALSE;
  data->state.http_neg.only_10 = FALSE;
  data->state.http_neg.wanted = CURL_HTTP_V1x;
  data->state.http_neg.allowed = CURL_HTTP_V1x;
  प्रत्यागम Curl_http_setup_conn(data, conn);
}

ध्रुव संरचना curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  संरचना Curl_easy *data = curl;
  यदि(GOOD_EASY_HANDLE(data) && Curl_api_is_in_callback(data) &&
     data->conn && !data->set.ws_raw_mode) {
    संरचना websocket *ws;
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    यदि(ws)
      प्रत्यागम &ws->recvframe;
  }
  प्रत्यागम रिक्त;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         अचिन्हित पूर्णाङ्क flags,
                                         curl_off_t frame_len)
{
  संरचना Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  यदि(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &result)) {
    संरचना Curl_easy *data = curl;
    संरचना websocket *ws;

    यदि(data->set.ws_raw_mode) {
      failf(data, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      result = CURLE_FAILED_INIT;
      गच्छ out;
    }

    CURL_TRC_WS(data, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    यदि(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    यदि(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }

    यदि(ws->enc.payload_remain) {
      failf(data, "[WS] previous frame not finished");
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }

    result = ws_enc_write_head(data, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    यदि(result)
      CURL_TRC_WS(data, "curl_start_frame(), error adding frame head %d",
                  (पूर्णाङ्क)result);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम result;
}

ध्रुव संरचना Curl_protocol Curl_protocol_ws = {
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

#अन्यथा

CURLcode curl_ws_recv(CURL *curl, शून्य *buffer, माप_प्रकार buflen,
                      माप_प्रकार *recv,
                      ध्रुव संरचना curl_ws_frame **metap)
{
  (शून्य)curl;
  (शून्य)buffer;
  (शून्य)buflen;
  (शून्य)recv;
  (शून्य)metap;
  प्रत्यागम CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, ध्रुव शून्य *buffer,
                      माप_प्रकार buflen, माप_प्रकार *sent,
                      curl_off_t fragsize,
                      अचिन्हित पूर्णाङ्क flags)
{
  (शून्य)curl;
  (शून्य)buffer;
  (शून्य)buflen;
  (शून्य)sent;
  (शून्य)fragsize;
  (शून्य)flags;
  प्रत्यागम CURLE_NOT_BUILT_IN;
}

ध्रुव संरचना curl_ws_frame *curl_ws_meta(CURL *data)
{
  (शून्य)data;
  प्रत्यागम रिक्त;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         अचिन्हित पूर्णाङ्क flags,
                                         curl_off_t frame_len)
{
  (शून्य)curl;
  (शून्य)flags;
  (शून्य)frame_len;
  प्रत्यागम CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
