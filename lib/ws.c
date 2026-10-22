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

#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS

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
#परिभाषय WSBIT_FIN          0x80
#परिभाषय WSBIT_RSV1         0x40
#परिभाषय WSBIT_RSV2         0x20
#परिभाषय WSBIT_RSV3         0x10
#परिभाषय WSBIT_RSV_आवरणम्     (WSBIT_RSV1 | WSBIT_RSV2 | WSBIT_RSV3)
#परिभाषय WSBIT_OPCODE_CONT  0x0
#परिभाषय WSBIT_OPCODE_TEXT  0x1
#परिभाषय WSBIT_OPCODE_BIN   0x2
#परिभाषय WSBIT_OPCODE_CLOSE 0x8
#परिभाषय WSBIT_OPCODE_PING  0x9
#परिभाषय WSBIT_OPCODE_PONG  0xa
#यदि_परिभाषितम् CURLVERBOSE
#परिभाषय WSBIT_OPCODE_आवरणम्  0xf
#यदि_अन्तः

#परिभाषय WSBIT_आवरणम् 0x80

/* buffer dimensioning */
#परिभाषय WS_CHUNK_आकार  65535
#परिभाषय WS_CHUNK_गणनम् 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
गणना ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_शीर्ष,
  WS_DEC_PAYLOAD
};

संरचना ws_decoder {
  पूर्णाङ्क frame_age;        /* zero */
  पूर्णाङ्क frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_विस्थापनम्;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t शीर्ष[10];
  पूर्णाङ्क शीर्ष_len, शीर्ष_योग;
  गणना ws_dec_state state;
  पूर्णाङ्क cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
संरचना ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  अचिह्नित पूर्णाङ्क xori; /* xor index */
  uint8_t आवरणम्[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#परिभाषय WS_MAX_CNTRL_LEN    125

संरचना ws_cntrl_frame {
  अचिह्नित पूर्णाङ्क type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
संरचना websocket {
  संरचना Curl_easy *दत्त; /* used for write callback handling */
  संरचना ws_decoder dec;  /* decode of ws frames */
  संरचना ws_encoder enc;  /* encode of ws frames */
  संरचना bufq recvbuf;    /* raw data from the server */
  संरचना bufq sendbuf;    /* raw data to be sent to the server */
  संरचना curl_ws_frame recvframe;  /* the current WS FRAME received */
  संरचना ws_cntrl_frame pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#यदि_परिभाषितम् CURLVERBOSE
स्थिर ध्रुव वर्ण *ws_frame_name_सम्बन्धि_op(uint8_t firstbyte)
{
  विकल्प(firstbyte & WSBIT_OPCODE_आवरणम्) {
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
#यदि_अन्तः

स्थिर पूर्णाङ्क ws_frame_firstbyte2flags(संरचना Curl_easy *दत्त,
                                    uint8_t firstbyte, पूर्णाङ्क cont_flags)
{
  विकल्प(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  पक्ष WSBIT_OPCODE_CONT:
    यदि(!(cont_flags & CURLWS_CONT)) {
      failf(दत्त, "[WS] no ongoing fragmented message to resume");
      प्रत्यागम 0;
    }
    प्रत्यागम cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  पक्ष (WSBIT_OPCODE_CONT | WSBIT_FIN):
    यदि(!(cont_flags & CURLWS_CONT)) {
      failf(दत्त, "[WS] no ongoing fragmented message to resume");
      प्रत्यागम 0;
    }
    प्रत्यागम cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  पक्ष WSBIT_OPCODE_TEXT:
    यदि(cont_flags & CURLWS_CONT) {
      failf(दत्त, "[WS] fragmented message interrupted by new TEXT msg");
      प्रत्यागम 0;
    }
    प्रत्यागम CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  पक्ष (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    यदि(cont_flags & CURLWS_CONT) {
      failf(दत्त, "[WS] fragmented message interrupted by new TEXT msg");
      प्रत्यागम 0;
    }
    प्रत्यागम CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  पक्ष WSBIT_OPCODE_BIN:
    यदि(cont_flags & CURLWS_CONT) {
      failf(दत्त, "[WS] fragmented message interrupted by new BINARY msg");
      प्रत्यागम 0;
    }
    प्रत्यागम CURLWS_द्विमान | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  पक्ष (WSBIT_OPCODE_BIN | WSBIT_FIN):
    यदि(cont_flags & CURLWS_CONT) {
      failf(दत्त, "[WS] fragmented message interrupted by new BINARY msg");
      प्रत्यागम 0;
    }
    प्रत्यागम CURLWS_द्विमान;
  /* 0x08 - first CLOSE fragment */
  पक्ष WSBIT_OPCODE_CLOSE:
    failf(दत्त, "[WS] invalid fragmented CLOSE frame");
    प्रत्यागम 0;
  /* 0x88 - unfragmented CLOSE */
  पक्ष (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    प्रत्यागम CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  पक्ष WSBIT_OPCODE_PING:
    failf(दत्त, "[WS] invalid fragmented PING frame");
    प्रत्यागम 0;
  /* 0x89 - unfragmented PING */
  पक्ष (WSBIT_OPCODE_PING | WSBIT_FIN):
    प्रत्यागम CURLWS_PING;
  /* 0x0a - first PONG fragment */
  पक्ष WSBIT_OPCODE_PONG:
    failf(दत्त, "[WS] invalid fragmented PONG frame");
    प्रत्यागम 0;
  /* 0x8a - unfragmented PONG */
  पक्ष (WSBIT_OPCODE_PONG | WSBIT_FIN):
    प्रत्यागम CURLWS_PONG;
  /* invalid first byte */
  स्वभाव:
    यदि(firstbyte & WSBIT_RSV_आवरणम्)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(दत्त, "[WS] invalid reserved bits: %02x", firstbyte);
    अन्यथा
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(दत्त, "[WS] invalid opcode: %02x", firstbyte);
    प्रत्यागम 0;
  }
}

स्थिर CURLcode ws_frame_flags2firstbyte(संरचना Curl_easy *दत्त,
                                         अचिह्नित पूर्णाङ्क flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  विकल्प(flags & ~CURLWS_विस्थापनम्) {
  पक्ष 0:
    यदि(contfragment) {
      CURL_TRC_WS(दत्त, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      प्रत्यागम CURLE_OK;
    }
    failf(दत्त, "[WS] no flags given");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  पक्ष CURLWS_CONT:
    यदि(contfragment) {
      infof(दत्त, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      प्रत्यागम CURLE_OK;
    }
    failf(दत्त, "[WS] No ongoing fragmented message to continue");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  पक्ष CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    प्रत्यागम CURLE_OK;
  पक्ष (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    प्रत्यागम CURLE_OK;
  पक्ष CURLWS_द्विमान:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    प्रत्यागम CURLE_OK;
  पक्ष (CURLWS_द्विमान | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    प्रत्यागम CURLE_OK;
  पक्ष CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    प्रत्यागम CURLE_OK;
  पक्ष (CURLWS_CLOSE | CURLWS_CONT):
    failf(दत्त, "[WS] CLOSE frame must not be fragmented");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  पक्ष CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    प्रत्यागम CURLE_OK;
  पक्ष (CURLWS_PING | CURLWS_CONT):
    failf(दत्त, "[WS] PING frame must not be fragmented");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  पक्ष CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    प्रत्यागम CURLE_OK;
  पक्ष (CURLWS_PONG | CURLWS_CONT):
    failf(दत्त, "[WS] PONG frame must not be fragmented");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  स्वभाव:
    failf(दत्त, "[WS] unknown flags: %x", flags);
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
}

स्थिर शून्य ws_dec_info(संरचना ws_decoder *dec, संरचना Curl_easy *दत्त,
                        ध्रुव वर्ण *सन्देशः)
{
  NOVERBOSE((शून्य)सन्देशः);
  विकल्प(dec->शीर्ष_len) {
  पक्ष 0:
    भङ्ग;
  पक्ष 1:
    CURL_TRC_WS(दत्त, "decoded %s [%s%s]", सन्देशः,
                ws_frame_name_सम्बन्धि_op(dec->शीर्ष[0]),
                (dec->शीर्ष[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    भङ्ग;
  स्वभाव:
    यदि(dec->शीर्ष_len < dec->शीर्ष_योग) {
      CURL_TRC_WS(दत्त, "decoded %s [%s%s](%d/%d)", सन्देशः,
                  ws_frame_name_सम्बन्धि_op(dec->शीर्ष[0]),
                  (dec->शीर्ष[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->शीर्ष_len, dec->शीर्ष_योग);
    }
    अन्यथा {
      CURL_TRC_WS(दत्त, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  सन्देशः, ws_frame_name_सम्बन्धि_op(dec->शीर्ष[0]),
                  (dec->शीर्ष[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_विस्थापनम्, dec->payload_len);
    }
    भङ्ग;
  }
}

स्थिर CURLcode ws_send_raw_blocking(संरचना Curl_easy *दत्त,
                                     संरचना websocket *ws,
                                     ध्रुव वर्ण *बफर, size_t buflen);

प्रकारनिर्देश CURLcode ws_write_payload(ध्रुव uint8_t *buf, size_t buflen,
                                  पूर्णाङ्क frame_age, पूर्णाङ्क frame_flags,
                                  curl_off_t payload_विस्थापनम्,
                                  curl_off_t payload_len,
                                  शून्य *userp,
                                  size_t *pnwritten);

स्थिर शून्य ws_dec_अग्र_frame(संरचना ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_विस्थापनम् = 0;
  dec->payload_len = 0;
  dec->शीर्ष_len = dec->शीर्ष_योग = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

स्थिर शून्य ws_dec_reset(संरचना ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_विस्थापनम् = 0;
  dec->payload_len = 0;
  dec->शीर्ष_len = dec->शीर्ष_योग = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

स्थिर शून्य ws_dec_init(संरचना ws_decoder *dec)
{
  ws_dec_reset(dec);
}

स्थिर CURLcode ws_dec_read_शीर्ष(संरचना ws_decoder *dec,
                                 संरचना Curl_easy *दत्त,
                                 संरचना bufq *inraw)
{
  ध्रुव uint8_t *inbuf;
  size_t inlen;

  यावत्(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    यदि(dec->शीर्ष_len == 0) {
      dec->शीर्ष[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(दत्त, dec->शीर्ष[0],
                                                  dec->cont_flags);
      यदि(!dec->frame_flags) {
        ws_dec_reset(dec);
        प्रत्यागम CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      यदि(dec->frame_flags & (CURLWS_TEXT | CURLWS_द्विमान)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->शीर्ष_len = 1;
#यदि 0
      ws_dec_info(dec, दत्त, "seeing opcode");
#यदि_अन्तः
      अनुवर्त;
    }
    अन्यथा यदि(dec->शीर्ष_len == 1) {
      dec->शीर्ष[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->शीर्ष_len = 2;

      यदि(dec->शीर्ष[1] & WSBIT_आवरणम्) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(दत्त, "[WS] masked input frame");
        ws_dec_reset(dec);
        प्रत्यागम CURLE_RECV_ERROR;
      }
      यदि(dec->frame_flags & CURLWS_PING && dec->शीर्ष[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(दत्त, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        प्रत्यागम CURLE_RECV_ERROR;
      }
      यदि(dec->frame_flags & CURLWS_PONG && dec->शीर्ष[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(दत्त, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        प्रत्यागम CURLE_RECV_ERROR;
      }
      यदि(dec->frame_flags & CURLWS_CLOSE && dec->शीर्ष[1] > WS_MAX_CNTRL_LEN) {
        failf(दत्त, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        प्रत्यागम CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      यदि(dec->शीर्ष[1] == 126) {
        dec->शीर्ष_योग = 4;
        अनुवर्त;
      }
      अन्यथा यदि(dec->शीर्ष[1] == 127) {
        dec->शीर्ष_योग = 10;
        अनुवर्त;
      }
      अन्यथा {
        dec->शीर्ष_योग = 2;
      }
    }

    यदि(dec->शीर्ष_len < dec->शीर्ष_योग) {
      dec->शीर्ष[dec->शीर्ष_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->शीर्ष_len;
      यदि(dec->शीर्ष_len < dec->शीर्ष_योग) {
#यदि 0
        ws_dec_info(dec, दत्त, "decoding head");
#यदि_अन्तः
        अनुवर्त;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->शीर्ष_len == dec->शीर्ष_योग);
    विकल्प(dec->शीर्ष_योग) {
    पक्ष 2:
      dec->payload_len = dec->शीर्ष[1];
      भङ्ग;
    पक्ष 4:
      dec->payload_len = (dec->शीर्ष[2] << 8) | dec->शीर्ष[3];
      भङ्ग;
    पक्ष 10:
      यदि(dec->शीर्ष[2] > 127) {
        failf(दत्त, "[WS] frame length longer than 63 bits not supported");
        प्रत्यागम CURLE_RECV_ERROR;
      }
      dec->payload_len =
        (curl_off_t)dec->शीर्ष[2] << 56 |
        (curl_off_t)dec->शीर्ष[3] << 48 |
        (curl_off_t)dec->शीर्ष[4] << 40 |
        (curl_off_t)dec->शीर्ष[5] << 32 |
        (curl_off_t)dec->शीर्ष[6] << 24 |
        (curl_off_t)dec->शीर्ष[7] << 16 |
        (curl_off_t)dec->शीर्ष[8] <<  8 |
        dec->शीर्ष[9];
      भङ्ग;
    स्वभाव:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(दत्त, "[WS] unexpected frame header length");
      प्रत्यागम CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_विस्थापनम् = 0;
    ws_dec_info(dec, दत्त, "head");
    प्रत्यागम CURLE_OK;
  }
  प्रत्यागम CURLE_AGAIN;
}

स्थिर CURLcode ws_dec_pass_payload(संरचना ws_decoder *dec,
                                    संरचना Curl_easy *दत्त,
                                    संरचना bufq *inraw,
                                    ws_write_payload *write_cb,
                                    शून्य *write_ctx)
{
  ध्रुव uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode परिणाम;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_विस्थापनम्,
                                     0, SIZE_MAX);

  यावत्(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(दत्त)) {
    यदि(inlen > remain)
      inlen = remain;
    परिणाम = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_विस्थापनम्, dec->payload_len,
                      write_ctx, &nwritten);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_विस्थापनम् += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_विस्थापनम्,
                                0, SIZE_MAX);
    CURL_TRC_WS(दत्त, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  प्रत्यागम remain ? CURLE_AGAIN : CURLE_OK;
}

स्थिर CURLcode ws_dec_pass(संरचना ws_decoder *dec,
                            संरचना Curl_easy *दत्त,
                            संरचना bufq *inraw,
                            ws_write_payload *write_cb,
                            शून्य *write_ctx)
{
  CURLcode परिणाम;

  यदि(Curl_bufq_is_रिक्तानि(inraw))
    प्रत्यागम CURLE_AGAIN;

  विकल्प(dec->state) {
  पक्ष WS_DEC_INIT:
    ws_dec_अग्र_frame(dec);
    dec->state = WS_DEC_शीर्ष;
    FALLTHROUGH();
  पक्ष WS_DEC_शीर्ष:
    परिणाम = ws_dec_read_शीर्ष(dec, दत्त, inraw);
    यदि(परिणाम) {
      यदि(परिणाम != CURLE_AGAIN) {
        failf(दत्त, "[WS] decode frame error %d", (पूर्णाङ्क)परिणाम);
        भङ्ग;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_रिक्तानि(inraw));
      भङ्ग;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    यदि(dec->payload_len == 0) {
      size_t nwritten;
      ध्रुव uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      परिणाम = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      dec->state = WS_DEC_INIT;
      भङ्ग;
    }
    FALLTHROUGH();
  पक्ष WS_DEC_PAYLOAD:
    परिणाम = ws_dec_pass_payload(dec, दत्त, inraw, write_cb, write_ctx);
    ws_dec_info(dec, दत्त, "passing");
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    भङ्ग;
  स्वभाव:
    /* we covered all enums above, but some code analyzers are wimps */
    परिणाम = CURLE_FAILED_INIT;
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य update_meta(संरचना websocket *ws,
                        पूर्णाङ्क frame_age, पूर्णाङ्क frame_flags,
                        curl_off_t payload_विस्थापनम्,
                        curl_off_t payload_len,
                        size_t cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_विस्थापनम् - cur_len);

  ws->recvframe.age = frame_age;
  ws->recvframe.flags = frame_flags;
  ws->recvframe.विस्थापनम् = payload_विस्थापनम्;
  ws->recvframe.len = cur_len;
  ws->recvframe.bytesleft = bytesleft;
}

/* WebSocket decoding client writer */
संरचना ws_cw_ctx {
  संरचना Curl_cwriter super;
  संरचना bufq buf;
};

स्थिर CURLcode ws_cw_init(संरचना Curl_easy *दत्त,
                           संरचना Curl_cwriter *writer)
{
  संरचना ws_cw_ctx *ctx = writer->ctx;
  (शून्य)दत्त;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_आकार, 1, BUFQ_OPT_SOFT_सीमा);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य ws_cw_close(संरचना Curl_easy *दत्त, संरचना Curl_cwriter *writer)
{
  संरचना ws_cw_ctx *ctx = writer->ctx;
  (शून्य)दत्त;
  Curl_bufq_free(&ctx->buf);
}

संरचना ws_cw_dec_ctx {
  संरचना Curl_easy *दत्त;
  संरचना websocket *ws;
  संरचना Curl_cwriter *अग्र_writer;
  पूर्णाङ्क cw_type;
};

स्थिर CURLcode ws_flush(संरचना Curl_easy *दत्त, संरचना websocket *ws,
                         bool blocking);
स्थिर CURLcode ws_enc_सङ्कलय_pending(संरचना Curl_easy *दत्त,
                                   संरचना websocket *ws);

स्थिर CURLcode ws_enc_सङ्कलय_cntrl(संरचना Curl_easy *दत्त,
                                 संरचना websocket *ws,
                                 ध्रुव uint8_t *payload,
                                 size_t plen,
                                 अचिह्नित पूर्णाङ्क frame_type)
{
  (शून्य)दत्त;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  यदि(plen > WS_MAX_CNTRL_LEN)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  स्मृतिं_प्रतिलिख(ws->pending.payload, payload, plen);
  प्रत्यागम CURLE_OK;
}

स्थिर curl_off_t ws_payload_remain(curl_off_t payload_योग,
                                    curl_off_t payload_विस्थापनम्,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_योग - payload_विस्थापनम्;
  यदि((payload_योग < 0) || (payload_विस्थापनम् < 0) || (remain < 0))
    प्रत्यागम -1;
  buffered = curlx_uztoso(payload_buffered);
  यदि(remain < buffered)
    प्रत्यागम -1;
  प्रत्यागम remain - buffered;
}

स्थिर CURLcode ws_cw_dec_अग्र(ध्रुव uint8_t *buf, size_t buflen,
                               पूर्णाङ्क frame_age, पूर्णाङ्क frame_flags,
                               curl_off_t payload_विस्थापनम्,
                               curl_off_t payload_len,
                               शून्य *user_दत्त,
                               size_t *pnwritten)
{
  संरचना ws_cw_dec_ctx *ctx = user_दत्त;
  संरचना Curl_easy *दत्त = ctx->दत्त;
  संरचना websocket *ws = ctx->ws;
  bool auto_pong = !दत्त->समुच्चयः.ws_raw_बहुलकम् && !दत्त->समुच्चयः.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode परिणाम;

  (शून्य)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_विस्थापनम्, buflen);
  यदि(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  यदि(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(दत्त, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_विस्थापनम्, payload_len);
    /* send back the exact same content as a PONG */
    परिणाम = ws_enc_सङ्कलय_cntrl(दत्त, ws, buf, buflen, CURLWS_PONG);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  अन्यथा यदि(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_विस्थापनम्,
                payload_len, buflen);

    CURL_TRC_WRITE(दत्त, "[WS] pass %zu decoded bytes", buflen);
    परिणाम = Curl_cwriter_write(दत्त, ctx->अग्र_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ध्रुव वर्ण *)buf, buflen);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  *pnwritten = buflen;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ws_cw_write(संरचना Curl_easy *दत्त,
                            संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                            ध्रुव वर्ण *buf, size_t nbytes)
{
  संरचना ws_cw_ctx *ctx = writer->ctx;
  संरचना websocket *ws;
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_WRITE(दत्त, "[WS] write(len=%zu, type=%d)", nbytes, type);
  यदि(!(type & CLIENTWRITE_BODY) || दत्त->समुच्चयः.ws_raw_बहुलकम्)
    प्रत्यागम Curl_cwriter_write(दत्त, writer->अग्र, type, buf, nbytes);

  ws = Curl_conn_meta_get(दत्त->conn, CURL_META_PROTO_WS_CONN);
  यदि(!ws) {
    failf(दत्त, "[WS] not a websocket transfer");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  यदि(nbytes) {
    size_t nwritten;
    परिणाम = Curl_bufq_write(&ctx->buf, (ध्रुव uint8_t *)buf,
                             nbytes, &nwritten);
    यदि(परिणाम) {
      infof(दत्त, "[WS] error adding data to buffer %d", (पूर्णाङ्क)परिणाम);
      प्रत्यागम परिणाम;
    }
  }

  परिणाम = Curl_cwriter_flush(दत्त, writer->अग्र);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यावत्(!Curl_bufq_is_रिक्तानि(&ctx->buf) && !Curl_cwriter_is_paused(दत्त)) {
    संरचना ws_cw_dec_ctx pass_ctx;
    pass_ctx.दत्त = दत्त;
    pass_ctx.ws = ws;
    pass_ctx.अग्र_writer = writer->अग्र;
    pass_ctx.cw_type = type;
    परिणाम = ws_dec_pass(&ws->dec, दत्त, &ctx->buf,
                         ws_cw_dec_अग्र, &pass_ctx);
    यदि(परिणाम == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      परिणाम = CURLE_OK;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(परिणाम) {
      failf(दत्त, "[WS] decode payload error %d", (पूर्णाङ्क)परिणाम);
      Curl_bufq_reset(&ctx->buf);
      गच्छ बहिर्गमनम्;
    }
  }

बहिर्गमनम्:
  यदि(!परिणाम && !ws->using_reader) {
    परिणाम = ws_flush(दत्त, ws, Curl_api_is_अन्तर्_callback(दत्त));
    यदि(परिणाम == CURLE_AGAIN)
      परिणाम = CURLE_OK;
  }

  यदि(!परिणाम && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_रिक्तानि(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(दत्त, "[WS] decode ended with incomplete frame");
    परिणाम = CURLE_RECV_ERROR;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ws_cw_flush(संरचना Curl_easy *दत्त,
                            संरचना Curl_cwriter *writer)
{
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_WRITE(दत्त, "[ws] flush");
  यदि(!दत्त->समुच्चयः.ws_raw_बहुलकम्) {
    संरचना ws_cw_ctx *ctx = writer->ctx;
    संरचना websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    परिणाम = Curl_cwriter_flush(दत्त, writer->अग्र);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    ws = Curl_conn_meta_get(दत्त->conn, CURL_META_PROTO_WS_CONN);
    यदि(!ws) {
      failf(दत्त, "[WS] not a websocket transfer");
      प्रत्यागम CURLE_FAILED_INIT;
    }

    यावत्(!Curl_bufq_is_रिक्तानि(&ctx->buf) && !Curl_cwriter_is_paused(दत्त)) {
      संरचना ws_cw_dec_ctx pass_ctx;
      pass_ctx.दत्त = दत्त;
      pass_ctx.ws = ws;
      pass_ctx.अग्र_writer = writer->अग्र;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      परिणाम = ws_dec_pass(&ws->dec, दत्त, &ctx->buf,
                           ws_cw_dec_अग्र, &pass_ctx);
      यदि(परिणाम == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        परिणाम = CURLE_OK;
        गच्छ बहिर्गमनम्;
      }
      अन्यथा यदि(परिणाम) {
        failf(दत्त, "[WS] decode payload error %d", (पूर्णाङ्क)परिणाम);
        Curl_bufq_reset(&ctx->buf);
        गच्छ बहिर्गमनम्;
      }
    }
  }

बहिर्गमनम्:
  यदि(!परिणाम)
    परिणाम = Curl_cwriter_flush(दत्त, writer->अग्र);
  प्रत्यागम परिणाम;
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

स्थिर शून्य ws_enc_info(संरचना ws_encoder *enc, संरचना Curl_easy *दत्त,
                        ध्रुव वर्ण *सन्देशः)
{
  NOVERBOSE((शून्य)enc);
  NOVERBOSE((शून्य)सन्देशः);
  CURL_TRC_WS(दत्त, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              सन्देशः, ws_frame_name_सम्बन्धि_op(enc->firstbyte),
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

स्थिर CURLcode ws_enc_सङ्कलय_frame(संरचना Curl_easy *दत्त,
                                 संरचना ws_encoder *enc,
                                 अचिह्नित पूर्णाङ्क flags,
                                 curl_off_t payload_len,
                                 संरचना bufq *बहिर्गमनम्)
{
  uint8_t firstb = 0;
  uint8_t शीर्ष[14];
  CURLcode परिणाम;
  size_t hlen, nwritten;

  यदि(payload_len < 0) {
    failf(दत्त, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    प्रत्यागम CURLE_SEND_ERROR;
  }

  यदि(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(दत्त, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    प्रत्यागम CURLE_SEND_ERROR;
  }

  परिणाम = ws_frame_flags2firstbyte(दत्त, flags, (bool)enc->contfragment,
                                    &firstb);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  यदि(flags & (CURLWS_TEXT | CURLWS_द्विमान)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  यदि(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(दत्त, "[WS] given PING frame is too big");
    प्रत्यागम CURLE_TOO_LARGE;
  }
  यदि(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(दत्त, "[WS] given PONG frame is too big");
    प्रत्यागम CURLE_TOO_LARGE;
  }
  यदि(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(दत्त, "[WS] given CLOSE frame is too big");
    प्रत्यागम CURLE_TOO_LARGE;
  }

  शीर्ष[0] = enc->firstbyte = firstb;
  यदि(payload_len > 65535) {
    शीर्ष[1] = 127 | WSBIT_आवरणम्;
    शीर्ष[2] = (uint8_t)((payload_len >> 56) & 0xff);
    शीर्ष[3] = (uint8_t)((payload_len >> 48) & 0xff);
    शीर्ष[4] = (uint8_t)((payload_len >> 40) & 0xff);
    शीर्ष[5] = (uint8_t)((payload_len >> 32) & 0xff);
    शीर्ष[6] = (uint8_t)((payload_len >> 24) & 0xff);
    शीर्ष[7] = (uint8_t)((payload_len >> 16) & 0xff);
    शीर्ष[8] = (uint8_t)((payload_len >> 8) & 0xff);
    शीर्ष[9] = (uint8_t)(payload_len & 0xff);
    hlen = 10;
  }
  अन्यथा यदि(payload_len >= 126) {
    शीर्ष[1] = 126 | WSBIT_आवरणम्;
    शीर्ष[2] = (uint8_t)((payload_len >> 8) & 0xff);
    शीर्ष[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  अन्यथा {
    शीर्ष[1] = (uint8_t)payload_len | WSBIT_आवरणम्;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, दत्त, "sending");

  /* 4 bytes random */

  परिणाम = Curl_rand(दत्त, (uint8_t *)&enc->आवरणम्, परिमाण(enc->आवरणम्));
  यदि(परिणाम)
    प्रत्यागम परिणाम;

#यदि_परिभाषितम् DEBUGBUILD
  यदि(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    स्मृतिं_पूरय(&enc->आवरणम्, 0, परिमाण(enc->आवरणम्));
#यदि_अन्तः

  /* add 4 bytes mask */
  स्मृतिं_प्रतिलिख(&शीर्ष[hlen], &enc->आवरणम्, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  परिणाम = Curl_bufq_write(बहिर्गमनम्, शीर्ष, hlen, &nwritten);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  यदि(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ws_enc_write_शीर्ष(संरचना Curl_easy *दत्त,
                                  संरचना websocket *ws,
                                  संरचना ws_encoder *enc,
                                  अचिह्नित पूर्णाङ्क flags,
                                  curl_off_t payload_len,
                                  संरचना bufq *बहिर्गमनम्)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  यदि(ws->pending.type) {
    CURLcode परिणाम = ws_enc_सङ्कलय_pending(दत्त, ws);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  प्रत्यागम ws_enc_सङ्कलय_frame(दत्त, enc, flags, payload_len, बहिर्गमनम्);
}

स्थिर CURLcode ws_enc_write_payload(संरचना ws_encoder *enc,
                                     संरचना Curl_easy *दत्त,
                                     ध्रुव uint8_t *buf, size_t buflen,
                                     संरचना bufq *बहिर्गमनम्, size_t *pnwritten)
{
  CURLcode परिणाम;
  size_t i, len, n, remain;

  *pnwritten = 0;
  यदि(Curl_bufq_is_पूर्णम्(बहिर्गमनम्))
    प्रत्यागम CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  यदि(remain < len)
    len = remain;

  प्रति(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->आवरणम्[enc->xori];
    परिणाम = Curl_bufq_write(बहिर्गमनम्, &c, 1, &n);
    यदि(परिणाम) {
      यदि((परिणाम != CURLE_AGAIN) || !i)
        प्रत्यागम परिणाम;
      भङ्ग;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, दत्त, "buffered");
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ws_enc_सङ्कलय_pending(संरचना Curl_easy *दत्त,
                                   संरचना websocket *ws)
{
  CURLcode परिणाम;
  size_t n;

  यदि(!ws->pending.type) /* no pending frame here */
    प्रत्यागम CURLE_OK;
  यदि(ws->enc.payload_remain) /* in the middle of another frame */
    प्रत्यागम CURLE_AGAIN;

  परिणाम = ws_enc_सङ्कलय_frame(दत्त, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  यदि(परिणाम) {
    CURL_TRC_WS(दत्त, "ws_enc_cntrl(), error adding head: %d",
                (पूर्णाङ्क)परिणाम);
    गच्छ बहिर्गमनम्;
  }
  परिणाम = ws_enc_write_payload(&ws->enc, दत्त, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  यदि(परिणाम) {
    CURL_TRC_WS(दत्त, "ws_enc_cntrl(), error adding payload: %d",
                (पूर्णाङ्क)परिणाम);
    गच्छ बहिर्गमनम्;
  }
  यदि(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(दत्त, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  स्मृतिं_पूरय(&ws->pending, 0, परिमाण(ws->pending));

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ws_enc_send(संरचना Curl_easy *दत्त,
                            संरचना websocket *ws,
                            ध्रुव uint8_t *बफर,
                            size_t buflen,
                            curl_off_t fragsize,
                            अचिह्नित पूर्णाङ्क flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(!दत्त->समुच्चयः.ws_raw_बहुलकम्);
  *pnsent = 0;

  यदि(ws->enc.payload_remain || !Curl_bufq_is_रिक्तानि(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    यदि(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(दत्त, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
    यदि((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(दत्त, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
  }
  अन्यथा {
    परिणाम = ws_flush(दत्त, ws, Curl_api_is_अन्तर्_callback(दत्त));
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    परिणाम = ws_enc_write_शीर्ष(दत्त, ws, &ws->enc, flags,
                               (flags & CURLWS_विस्थापनम्) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    यदि(परिणाम) {
      CURL_TRC_WS(दत्त, "curl_ws_send(), error writing frame head %d",
                  (पूर्णाङ्क)परिणाम);
      प्रत्यागम परिणाम;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  यावत्(!Curl_bufq_is_रिक्तानि(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    यदि(buflen > ws->sendbuf_payload) {
      size_t पूर्व_len = Curl_bufq_len(&ws->sendbuf);
      परिणाम = ws_enc_write_payload(&ws->enc, दत्त,
                                    बफर + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      यदि(परिणाम && (परिणाम != CURLE_AGAIN))
        प्रत्यागम परिणाम;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - पूर्व_len;
      यदि(!ws->sendbuf_payload) {
        प्रत्यागम CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    परिणाम = ws_flush(दत्त, ws, Curl_api_is_अन्तर्_callback(दत्त));
    यदि(!परिणाम && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      बफर += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    अन्यथा यदि(परिणाम == CURLE_AGAIN) {
      यदि(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        प्रत्यागम CURLE_OK;
      }
      अन्यथा {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(दत्त, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        प्रत्यागम CURLE_AGAIN;
      }
    }
    अन्यथा
      प्रत्यागम परिणाम;  /* real error sending the data */
  }
  प्रत्यागम CURLE_OK;
}

संरचना cr_ws_ctx {
  संरचना Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

स्थिर CURLcode cr_ws_init(संरचना Curl_easy *दत्त, संरचना Curl_creader *reader)
{
  (शून्य)दत्त;
  (शून्य)reader;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cr_ws_close(संरचना Curl_easy *दत्त, संरचना Curl_creader *reader)
{
  (शून्य)दत्त;
  (शून्य)reader;
}

स्थिर CURLcode cr_ws_read(संरचना Curl_easy *दत्त,
                           संरचना Curl_creader *reader,
                           वर्ण *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  संरचना cr_ws_ctx *ctx = reader->ctx;
  CURLcode परिणाम = CURLE_OK;
  size_t nread, n;
  संरचना websocket *ws;
  bool eos;

  *pnread = 0;
  यदि(ctx->eos) {
    *peos = TRUE;
    प्रत्यागम CURLE_OK;
  }

  ws = Curl_conn_meta_get(दत्त->conn, CURL_META_PROTO_WS_CONN);
  यदि(!ws) {
    failf(दत्त, "[WS] not a websocket transfer");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  यदि(Curl_bufq_is_रिक्तानि(&ws->sendbuf)) {
    यदि(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      प्रत्यागम CURLE_OK;
    }

    यदि(ws->enc.payload_remain) {
      CURL_TRC_WS(दत्त, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    परिणाम = Curl_creader_read(दत्त, reader->अग्र, buf, blen, &nread, &eos);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    ctx->read_eos = eos;

    यदि(!Curl_bufq_is_रिक्तानि(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(दत्त, reader->अग्र);
    }
    अन्यथा यदि(!nread) {
      /* nothing to convert, return this right away */
      यदि(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      गच्छ बहिर्गमनम्;
    }

    यदि(!ws->enc.payload_remain && Curl_bufq_is_रिक्तानि(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      परिणाम = ws_enc_write_शीर्ष(दत्त, ws, &ws->enc, CURLWS_द्विमान, nread,
                                 &ws->sendbuf);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }

    परिणाम = ws_enc_write_payload(&ws->enc, दत्त, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    CURL_TRC_READ(दत्त, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_रिक्तानि(&ws->sendbuf));
  *peos = FALSE;
  परिणाम = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  यदि(!परिणाम && ctx->read_eos && Curl_bufq_is_रिक्तानि(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

बहिर्गमनम्:
  CURL_TRC_READ(दत्त, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (पूर्णाङ्क)परिणाम, *pnread, *peos);
  प्रत्यागम परिणाम;
}

स्थिर curl_off_t cr_ws_योग_दीर्घता(संरचना Curl_easy *दत्त,
                                     संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)दत्त;
  (शून्य)reader;
  प्रत्यागम -1;
}

स्थिर ध्रुव संरचना Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_योग_दीर्घता,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_समाप्तम्,
  परिमाण(संरचना cr_ws_ctx)
};

संरचना wsfield {
  ध्रुव वर्ण *name;
  ध्रुव वर्ण *val;
};

CURLcode Curl_ws_request(संरचना Curl_easy *दत्त, संरचना dynbuf *req)
{
  अचिह्नित पूर्णाङ्क i;
  CURLcode परिणाम = CURLE_OK;
  uint8_t यादृच्छिकम्[16];
  वर्ण *randstr;
  size_t randlen;
  वर्ण keyval[40];
  संरचना SingleRequest *k = &दत्त->req;
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
  परिणाम = Curl_rand(दत्त, यादृच्छिकम्, परिमाण(यादृच्छिकम्));
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  परिणाम = curlx_base64_encode(यादृच्छिकम्, परिमाण(यादृच्छिकम्), &randstr, &randlen);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  DEBUGASSERT(randlen < परिमाण(keyval));
  यदि(randlen >= परिमाण(keyval)) {
    curlx_free(randstr);
    प्रत्यागम CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, परिमाण(keyval), randstr, randlen);
  curlx_free(randstr);
  प्रति(i = 0; !परिणाम && (i < CURL_ARRAYSIZE(heads)); i++) {
    यदि(!Curl_checkheaders(दत्त, heads[i].name, वाक्यदैर्घ्यम्(heads[i].name))) {
      परिणाम = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  दत्त->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  दत्त->conn->द्वयङ्काः.upgrade_अन्तर्_प्रगतिः = TRUE;
  प्रत्यागम परिणाम;
}

स्थिर शून्य ws_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना websocket *ws = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(संरचना Curl_easy *दत्त,
                        ध्रुव वर्ण *mem, size_t nread)
{
  संरचना SingleRequest *k = &दत्त->req;
  संरचना websocket *ws;
  संरचना Curl_cwriter *ws_dec_writer = रिक्त;
  संरचना Curl_creader *ws_enc_reader = रिक्त;
  CURLcode परिणाम;

  DEBUGASSERT(दत्त->conn);
  ws = Curl_conn_meta_get(दत्त->conn, CURL_META_PROTO_WS_CONN);
  यदि(!ws) {
    size_t chunk_आकार = WS_CHUNK_आकार;
    ws = curlx_calloc(1, परिमाण(*ws));
    यदि(!ws)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
#यदि_परिभाषितम् DEBUGBUILD
    {
      ध्रुव वर्ण *p = getenv("CURL_WS_CHUNK_SIZE");
      यदि(p) {
        curl_off_t l;
        यदि(!curlx_str_सङ्ख्या(&p, &l, 1 * 1024 * 1024))
          chunk_आकार = (size_t)l;
      }
    }
#यदि_अन्तः
    CURL_TRC_WS(दत्त, "WS, using chunk size %zu", chunk_आकार);
    Curl_bufq_init2(&ws->recvbuf, chunk_आकार, WS_CHUNK_गणनम्,
                    BUFQ_OPT_SOFT_सीमा);
    Curl_bufq_init2(&ws->sendbuf, chunk_आकार, WS_CHUNK_गणनम्,
                    BUFQ_OPT_SOFT_सीमा);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    परिणाम = Curl_conn_meta_समुच्चयः(दत्त->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
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

  infof(दत्त, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  परिणाम = Curl_cwriter_create(&ws_dec_writer, दत्त, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  परिणाम = Curl_cwriter_सङ्कलय(दत्त, ws_dec_writer);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  ws_dec_writer = रिक्त; /* owned by transfer now */

  k->शीर्षकम् = FALSE; /* we will not get more response headers */

  यदि(दत्त->समुच्चयः.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    यदि(nread) {
      परिणाम = Curl_cf_recvbuf_सङ्कलय(दत्त, दत्त->conn, FIRSTSOCKET,
                                   (ध्रुव uint8_t *)mem, nread);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
    CURL_REQ_CLEAR_RECV(दत्त); /* read no more content */
  }
  अन्यथा { /* !connect_only */
    यदि(दत्त->समुच्चयः.विधानम् == HTTPREQ_PUT) {
      CURL_TRC_WS(दत्त, "UPLOAD set, add ws-encode reader");
      परिणाम = Curl_creader_समुच्चयः_fread(दत्त, -1);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;

      यदि(!दत्त->समुच्चयः.ws_raw_बहुलकम्) {
        /* Add our client reader encoding WS BINARY frames */
        परिणाम = Curl_creader_create(&ws_enc_reader, दत्त, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
        परिणाम = Curl_creader_सङ्कलय(दत्त, ws_enc_reader);
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
        ws_enc_reader = रिक्त; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      दत्त->req.eos_read = FALSE;
      दत्त->req.upload_समाप्तम् = FALSE;
      CURL_REQ_समुच्चयः_SEND(दत्त);
    }

    /* Then pass any additional data to the writers */
    यदि(nread) {
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, mem, nread);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->शीर्षकम् = FALSE; /* we will not get more responses */

बहिर्गमनम्:
  यदि(ws_dec_writer)
    Curl_cwriter_free(दत्त, ws_dec_writer);
  यदि(ws_enc_reader)
    Curl_creader_free(दत्त, ws_enc_reader);
  यदि(परिणाम)
    CURL_TRC_WS(दत्त, "Curl_ws_accept() failed -> %d", (पूर्णाङ्क)परिणाम);
  अन्यथा
    CURL_TRC_WS(दत्त, "websocket established, %s mode",
                दत्त->समुच्चयः.connect_only ? "connect-only" : "callback");
  प्रत्यागम परिणाम;
}

संरचना ws_collect {
  संरचना Curl_easy *दत्त;
  संरचना websocket *ws;
  uint8_t *बफर;
  size_t buflen;
  size_t bufidx;
  पूर्णाङ्क frame_age;
  पूर्णाङ्क frame_flags;
  curl_off_t payload_विस्थापनम्;
  curl_off_t payload_len;
  bool written;
};

स्थिर CURLcode ws_client_collect(ध्रुव uint8_t *buf, size_t buflen,
                                  पूर्णाङ्क frame_age, पूर्णाङ्क frame_flags,
                                  curl_off_t payload_विस्थापनम्,
                                  curl_off_t payload_len,
                                  शून्य *userp,
                                  size_t *pnwritten)
{
  संरचना ws_collect *ctx = userp;
  संरचना Curl_easy *दत्त = ctx->दत्त;
  bool auto_pong = !दत्त->समुच्चयः.ws_raw_बहुलकम् && !दत्त->समुच्चयः.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode परिणाम = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_विस्थापनम्, buflen);
  यदि(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  यदि(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_विस्थापनम् = payload_विस्थापनम्;
    ctx->payload_len = payload_len;
  }

  यदि(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(दत्त, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_विस्थापनम्, payload_len);
    /* send back the exact same content as a PONG */
    परिणाम = ws_enc_सङ्कलय_cntrl(ctx->दत्त, ctx->ws, buf, buflen, CURLWS_PONG);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    *pnwritten = buflen;
  }
  अन्यथा {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    यदि(!write_len) {
      यदि(!buflen)  /* 0 length write, we accept that */
        प्रत्यागम CURLE_OK;
      प्रत्यागम CURLE_AGAIN;  /* no more space */
    }
    स्मृतिं_प्रतिलिख(ctx->बफर + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode nw_अन्तर्_recv(शून्य *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  संरचना Curl_easy *दत्त = reader_ctx;
  प्रत्यागम Curl_easy_recv(दत्त, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, शून्य *बफर,
                      size_t buflen, size_t *recv,
                      ध्रुव संरचना curl_ws_frame **metap)
{
  संरचना Curl_eapi_guard guard;
  CURLcode परिणाम = CURLE_OK;

  यदि(CURL_EAPI_ENTER(&guard, curl, ws_recv, &परिणाम)) {
    संरचना Curl_easy *दत्त = curl;
    संरचना connectdata *conn;
    संरचना websocket *ws;
    संरचना ws_collect ctx;

    यदि(!recv || !metap || (buflen && !बफर)) {
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }

    *recv = 0;
    *metap = रिक्त;

    conn = दत्त->conn;
    यदि(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      यदि(!दत्त->समुच्चयः.connect_only) {
        failf(दत्त, "[WS] CONNECT_ONLY is required");
        परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
        गच्छ बहिर्गमनम्;
      }

      Curl_getconnectinfo(दत्त, &conn);
      यदि(!conn) {
        failf(दत्त, "[WS] connection not found");
        परिणाम = CURLE_BAD_FUNCTION_तर्कः;
        गच्छ बहिर्गमनम्;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    यदि(!ws) {
      failf(दत्त, "[WS] connection is not setup for websocket");
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }

    स्मृतिं_पूरय(&ctx, 0, परिमाण(ctx));
    ctx.दत्त = दत्त;
    ctx.ws = ws;
    ctx.बफर = बफर;
    ctx.buflen = buflen;

    यावत्(1) {
      /* receive more when our buffer is empty */
      यदि(Curl_bufq_is_रिक्तानि(&ws->recvbuf)) {
        size_t n;
        परिणाम = Curl_bufq_slurp(&ws->recvbuf, nw_अन्तर्_recv, दत्त, &n);
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
        अन्यथा यदि(n == 0) {
          /* connection closed */
          infof(दत्त, "[WS] connection expectedly closed?");
          परिणाम = CURLE_GOT_NOTHING;
          गच्छ बहिर्गमनम्;
        }
        CURL_TRC_WS(दत्त, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      परिणाम = ws_dec_pass(&ws->dec, दत्त, &ws->recvbuf,
                           ws_client_collect, &ctx);
      यदि(परिणाम == CURLE_AGAIN) {
        यदि(!ctx.written) {
          ws_dec_info(&ws->dec, दत्त, "need more input");
          अनुवर्त;  /* nothing written, try more input */
        }
        भङ्ग;
      }
      अन्यथा यदि(परिणाम) {
        गच्छ बहिर्गमनम्;
      }
      अन्यथा यदि(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        भङ्ग;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.frame_age, ctx.frame_flags, ctx.payload_विस्थापनम्,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(दत्त, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.विस्थापनम्,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    यदि(!दत्त->समुच्चयः.ws_raw_बहुलकम् && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_सङ्कलय_pending(दत्त, ws);
      यदि(!r2)
        (शून्य)ws_flush(दत्त, ws, Curl_api_is_अन्तर्_callback(दत्त));
    }
    परिणाम = CURLE_OK;
  }
बहिर्गमनम्:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ws_flush(संरचना Curl_easy *दत्त, संरचना websocket *ws,
                         bool blocking)
{
  CURLcode परिणाम;

  /* If there is space, add any pending control frame */
  यदि(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_आकार) {
    परिणाम = ws_enc_सङ्कलय_pending(दत्त, ws);
    यदि(परिणाम && (परिणाम != CURLE_AGAIN))
      प्रत्यागम परिणाम;
  }

  यदि(!Curl_bufq_is_रिक्तानि(&ws->sendbuf)) {
    ध्रुव uint8_t *बहिर्गमनम्;
    size_t outlen, n;
#यदि_परिभाषितम् DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_अग्र = FALSE;
    size_t chunk_egain = 0;
    ध्रुव वर्ण *p = getenv("CURL_WS_CHUNK_EAGAIN");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#यदि_अन्तः

    यावत्(Curl_bufq_peek(&ws->sendbuf, &बहिर्गमनम्, &outlen)) {
#यदि_परिभाषितम् DEBUGBUILD
      यदि(eagain_अग्र)
        प्रत्यागम CURLE_AGAIN;
      यदि(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_अग्र = TRUE;
      }
#यदि_अन्तः
      यदि(blocking) {
        परिणाम = ws_send_raw_blocking(दत्त, ws, (ध्रुव वर्ण *)बहिर्गमनम्, outlen);
        n = परिणाम ? 0 : outlen;
      }
      अन्यथा यदि(दत्त->समुच्चयः.connect_only || Curl_api_is_अन्तर्_callback(दत्त))
        परिणाम = Curl_senddata(दत्त, बहिर्गमनम्, outlen, &n);
      अन्यथा {
        परिणाम = Curl_xfer_send(दत्त, बहिर्गमनम्, outlen, FALSE, &n);
        यदि(!परिणाम && !n && outlen)
          परिणाम = CURLE_AGAIN;
      }

      यदि(परिणाम == CURLE_AGAIN) {
        CURL_TRC_WS(दत्त, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        प्रत्यागम परिणाम;
      }
      अन्यथा यदि(परिणाम) {
        failf(दत्त, "[WS] flush, write error %d", (पूर्णाङ्क)परिणाम);
        प्रत्यागम परिणाम;
      }
      अन्यथा {
        CURL_TRC_WS(दत्त, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ws_send_raw_blocking(संरचना Curl_easy *दत्त,
                                     संरचना websocket *ws,
                                     ध्रुव वर्ण *बफर, size_t buflen)
{
  CURLcode परिणाम = CURLE_OK;
  size_t nwritten;

  यदि(!दत्त)
    प्रत्यागम परिणाम;

  (शून्य)ws;
  यावत्(buflen) {
    परिणाम = Curl_xfer_send(दत्त, बफर, buflen, FALSE, &nwritten);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    DEBUGASSERT(nwritten <= buflen);
    बफर += nwritten;
    buflen -= nwritten;
    यदि(buflen) {
      curl_socket_t sock = दत्त->conn->sock[FIRSTSOCKET];
      timediff_t वामः_ms;
      पूर्णाङ्क ev;

      CURL_TRC_WS(दत्त, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      वामः_ms = Curl_timeleft_ms(दत्त);
      यदि(वामः_ms < 0) {
        failf(दत्त, "[WS] Timeout waiting for socket becoming writable");
        प्रत्यागम CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      यदि(sock == CURL_SOCKET_BAD)
        प्रत्यागम CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, वामः_ms ? वामः_ms : 500);
      यदि(ev < 0) {
        failf(दत्त, "[WS] Error while waiting for socket becoming writable");
        प्रत्यागम CURLE_SEND_ERROR;
      }
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ws_send_raw(संरचना Curl_easy *दत्त, ध्रुव शून्य *बफर,
                            size_t buflen, size_t *pnwritten)
{
  संरचना websocket *ws;
  CURLcode परिणाम;

  ws = Curl_conn_meta_get(दत्त->conn, CURL_META_PROTO_WS_CONN);
  यदि(!ws) {
    failf(दत्त, "[WS] Not a websocket transfer");
    प्रत्यागम CURLE_SEND_ERROR;
  }
  यदि(!buflen)
    प्रत्यागम CURLE_OK;

  यदि(Curl_api_is_अन्तर्_callback(दत्त)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    परिणाम = ws_flush(दत्त, ws, TRUE);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    परिणाम = ws_send_raw_blocking(दत्त, ws, बफर, buflen);
    यदि(!परिणाम)
      *pnwritten = buflen;
  }
  अन्यथा {
    /* We need any pending data to be sent or EAGAIN this call. */
    परिणाम = ws_flush(दत्त, ws, FALSE);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    परिणाम = Curl_senddata(दत्त, बफर, buflen, pnwritten);
  }

  CURL_TRC_WS(दत्त, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (पूर्णाङ्क)परिणाम, *pnwritten);
  प्रत्यागम परिणाम;
}

CURLcode curl_ws_send(CURL *curl, ध्रुव शून्य *बफर_arg,
                      size_t buflen, size_t *प्रेषितम्,
                      curl_off_t fragsize,
                      अचिह्नित पूर्णाङ्क flags)
{
  संरचना Curl_eapi_guard guard;
  CURLcode परिणाम = CURLE_OK;

  यदि(CURL_EAPI_ENTER(&guard, curl, ws_send, &परिणाम)) {
    संरचना websocket *ws;
    ध्रुव uint8_t *बफर = बफर_arg;
    संरचना Curl_easy *दत्त = curl;
    size_t ndummy;
    size_t *pnsent = प्रेषितम् ? प्रेषितम् : &ndummy;

    CURL_TRC_WS(दत्त, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, दत्त->समुच्चयः.ws_raw_बहुलकम्);

    *pnsent = 0;

    यदि(!बफर && buflen) {
      failf(दत्त, "[WS] buffer is NULL when buflen is not");
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }

    यदि(!दत्त->conn && दत्त->समुच्चयः.connect_only) {
      परिणाम = Curl_connect_only_attach(दत्त);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
    यदि(!दत्त->conn) {
      failf(दत्त, "[WS] No associated connection");
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
    ws = Curl_conn_meta_get(दत्त->conn, CURL_META_PROTO_WS_CONN);
    यदि(!ws) {
      failf(दत्त, "[WS] Not a websocket transfer");
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }

    यदि(दत्त->समुच्चयः.ws_raw_बहुलकम्) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      परिणाम = ws_flush(दत्त, ws, FALSE);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;

      यदि(!बफर) {
        failf(दत्त, "[WS] buffer is NULL in raw mode");
        परिणाम = CURLE_BAD_FUNCTION_तर्कः;
        गच्छ बहिर्गमनम्;
      }
      यदि(!प्रेषितम्) {
        failf(दत्त, "[WS] sent is NULL in raw mode");
        परिणाम = CURLE_BAD_FUNCTION_तर्कः;
        गच्छ बहिर्गमनम्;
      }
      यदि(fragsize || flags) {
        failf(दत्त, "[WS] fragsize and flags must be zero in raw mode");
        परिणाम = CURLE_BAD_FUNCTION_तर्कः;
        गच्छ बहिर्गमनम्;
      }
      परिणाम = ws_send_raw(दत्त, बफर, buflen, pnsent);
      गच्छ बहिर्गमनम्;
    }

    /* Not RAW mode, we do the frame encoding */
    परिणाम = ws_enc_send(दत्त, ws, बफर, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(दत्त, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, दत्त->समुच्चयः.ws_raw_बहुलकम्, (पूर्णाङ्क)परिणाम,
                *pnsent);
  }
बहिर्गमनम्:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ws_सज्जता_conn(संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  दत्त->state.http_neg.accept_09 = FALSE;
  दत्त->state.http_neg.only_10 = FALSE;
  दत्त->state.http_neg.wanted = CURL_HTTP_V1x;
  दत्त->state.http_neg.allowed = CURL_HTTP_V1x;
  प्रत्यागम Curl_http_सज्जता_conn(दत्त, conn);
}

ध्रुव संरचना curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  संरचना Curl_easy *दत्त = curl;
  यदि(GOOD_EASY_HANDLE(दत्त) && Curl_api_is_अन्तर्_callback(दत्त) &&
     दत्त->conn && !दत्त->समुच्चयः.ws_raw_बहुलकम्) {
    संरचना websocket *ws;
    ws = Curl_conn_meta_get(दत्त->conn, CURL_META_PROTO_WS_CONN);
    यदि(ws)
      प्रत्यागम &ws->recvframe;
  }
  प्रत्यागम रिक्त;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         अचिह्नित पूर्णाङ्क flags,
                                         curl_off_t frame_len)
{
  संरचना Curl_eapi_guard guard;
  CURLcode परिणाम = CURLE_OK;

  यदि(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &परिणाम)) {
    संरचना Curl_easy *दत्त = curl;
    संरचना websocket *ws;

    यदि(दत्त->समुच्चयः.ws_raw_बहुलकम्) {
      failf(दत्त, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      परिणाम = CURLE_FAILED_INIT;
      गच्छ बहिर्गमनम्;
    }

    CURL_TRC_WS(दत्त, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    यदि(!दत्त->conn) {
      failf(दत्त, "[WS] No associated connection");
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
    ws = Curl_conn_meta_get(दत्त->conn, CURL_META_PROTO_WS_CONN);
    यदि(!ws) {
      failf(दत्त, "[WS] Not a websocket transfer");
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }

    यदि(ws->enc.payload_remain) {
      failf(दत्त, "[WS] previous frame not finished");
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }

    परिणाम = ws_enc_write_शीर्ष(दत्त, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    यदि(परिणाम)
      CURL_TRC_WS(दत्त, "curl_start_frame(), error adding frame head %d",
                  (पूर्णाङ्क)परिणाम);
  }
बहिर्गमनम्:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
}

ध्रुव संरचना Curl_protocol Curl_protocol_ws = {
  ws_सज्जता_conn,                        /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_समाप्तम्,                       /* done */
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

CURLcode curl_ws_recv(CURL *curl, शून्य *बफर, size_t buflen,
                      size_t *recv,
                      ध्रुव संरचना curl_ws_frame **metap)
{
  (शून्य)curl;
  (शून्य)बफर;
  (शून्य)buflen;
  (शून्य)recv;
  (शून्य)metap;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

CURLcode curl_ws_send(CURL *curl, ध्रुव शून्य *बफर,
                      size_t buflen, size_t *प्रेषितम्,
                      curl_off_t fragsize,
                      अचिह्नित पूर्णाङ्क flags)
{
  (शून्य)curl;
  (शून्य)बफर;
  (शून्य)buflen;
  (शून्य)प्रेषितम्;
  (शून्य)fragsize;
  (शून्य)flags;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

ध्रुव संरचना curl_ws_frame *curl_ws_meta(CURL *दत्त)
{
  (शून्य)दत्त;
  प्रत्यागम रिक्त;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         अचिह्नित पूर्णाङ्क flags,
                                         curl_off_t frame_len)
{
  (शून्य)curl;
  (शून्य)flags;
  (शून्य)frame_len;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

#यदि_अन्तः /* !CURL_DISABLE_WEBSOCKETS */
