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
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "ws.h"

#अगर_अघोषित CURL_DISABLE_WEBSOCKETS

#समावेश "url.h"
#समावेश "bufq.h"
#समावेश "curlx/dynbuf.h"
#समावेश "rand.h"
#समावेश "curlx/base64.h"
#समावेश "cf-recvbuf.h"
#समावेश "connect.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "multiif.h"
#समावेश "easyif.h"
#समावेश "transfer.h"
#समावेश "select.h"
#समावेश "curlx/strparse.h"
#समावेश "curlx/strcopy.h"

/* RFC 6455 Section 5.2

    0 1 2 3 4 5 6 7
   +-+-+-+-+-------+
   |F|R|R|R| opcode|
   |I|S|S|S|  (4)  |
   |N|V|V|V|       |
   | |1|2|3|       |
 */
#घोषणा WSBIT_FIN          0x80
#घोषणा WSBIT_RSV1         0x40
#घोषणा WSBIT_RSV2         0x20
#घोषणा WSBIT_RSV3         0x10
#घोषणा WSBIT_RSV_MASK     (WSBIT_RSV1 | WSBIT_RSV2 | WSBIT_RSV3)
#घोषणा WSBIT_OPCODE_CONT  0x0
#घोषणा WSBIT_OPCODE_TEXT  0x1
#घोषणा WSBIT_OPCODE_BIN   0x2
#घोषणा WSBIT_OPCODE_CLOSE 0x8
#घोषणा WSBIT_OPCODE_PING  0x9
#घोषणा WSBIT_OPCODE_PONG  0xa
#अगर_घोषित CURLVERBOSE
#घोषणा WSBIT_OPCODE_MASK  0xf
#पूर्ण_अगर

#घोषणा WSBIT_MASK 0x80

/* buffer dimensioning */
#घोषणा WS_CHUNK_आकार  65535
#घोषणा WS_CHUNK_गिनती 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
क्रमागत ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_शीर्ष,
  WS_DEC_PAYLOAD
};

काष्ठा ws_decoder {
  पूर्णांक frame_age;        /* zero */
  पूर्णांक frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t शीर्ष[10];
  पूर्णांक शीर्ष_len, शीर्ष_योग;
  क्रमागत ws_dec_state state;
  पूर्णांक cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
काष्ठा ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  अचिन्हित पूर्णांक xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#घोषणा WS_MAX_CNTRL_LEN    125

काष्ठा ws_cntrl_frame {
  अचिन्हित पूर्णांक type;
  माप_प्रकार payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
काष्ठा websocket {
  काष्ठा Curl_easy *तथ्य; /* used for write callback handling */
  काष्ठा ws_decoder dec;  /* decode of ws frames */
  काष्ठा ws_encoder enc;  /* encode of ws frames */
  काष्ठा bufq recvbuf;    /* raw data from the server */
  काष्ठा bufq sendbuf;    /* raw data to be sent to the server */
  काष्ठा curl_ws_frame recvframe;  /* the current WS FRAME received */
  काष्ठा ws_cntrl_frame pending; /* a control frame pending to be sent */
  माप_प्रकार sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#अगर_घोषित CURLVERBOSE
अटल स्थिर अक्षर *ws_frame_name_of_op(uint8_t firstbyte)
{
  चयन(firstbyte & WSBIT_OPCODE_MASK) {
  हाल WSBIT_OPCODE_CONT:
    वापस "CONT";
  हाल WSBIT_OPCODE_TEXT:
    वापस "TEXT";
  हाल WSBIT_OPCODE_BIN:
    वापस "BIN";
  हाल WSBIT_OPCODE_CLOSE:
    वापस "CLOSE";
  हाल WSBIT_OPCODE_PING:
    वापस "PING";
  हाल WSBIT_OPCODE_PONG:
    वापस "PONG";
  शेष:
    वापस "???";
  }
}
#पूर्ण_अगर

अटल पूर्णांक ws_frame_firstbyte2flags(काष्ठा Curl_easy *तथ्य,
                                    uint8_t firstbyte, पूर्णांक cont_flags)
{
  चयन(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  हाल WSBIT_OPCODE_CONT:
    अगर(!(cont_flags & CURLWS_CONT)) {
      failf(तथ्य, "[WS] no ongoing fragmented message to resume");
      वापस 0;
    }
    वापस cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  हाल (WSBIT_OPCODE_CONT | WSBIT_FIN):
    अगर(!(cont_flags & CURLWS_CONT)) {
      failf(तथ्य, "[WS] no ongoing fragmented message to resume");
      वापस 0;
    }
    वापस cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  हाल WSBIT_OPCODE_TEXT:
    अगर(cont_flags & CURLWS_CONT) {
      failf(तथ्य, "[WS] fragmented message interrupted by new TEXT msg");
      वापस 0;
    }
    वापस CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  हाल (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    अगर(cont_flags & CURLWS_CONT) {
      failf(तथ्य, "[WS] fragmented message interrupted by new TEXT msg");
      वापस 0;
    }
    वापस CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  हाल WSBIT_OPCODE_BIN:
    अगर(cont_flags & CURLWS_CONT) {
      failf(तथ्य, "[WS] fragmented message interrupted by new BINARY msg");
      वापस 0;
    }
    वापस CURLWS_द्विआधारी | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  हाल (WSBIT_OPCODE_BIN | WSBIT_FIN):
    अगर(cont_flags & CURLWS_CONT) {
      failf(तथ्य, "[WS] fragmented message interrupted by new BINARY msg");
      वापस 0;
    }
    वापस CURLWS_द्विआधारी;
  /* 0x08 - first CLOSE fragment */
  हाल WSBIT_OPCODE_CLOSE:
    failf(तथ्य, "[WS] invalid fragmented CLOSE frame");
    वापस 0;
  /* 0x88 - unfragmented CLOSE */
  हाल (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    वापस CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  हाल WSBIT_OPCODE_PING:
    failf(तथ्य, "[WS] invalid fragmented PING frame");
    वापस 0;
  /* 0x89 - unfragmented PING */
  हाल (WSBIT_OPCODE_PING | WSBIT_FIN):
    वापस CURLWS_PING;
  /* 0x0a - first PONG fragment */
  हाल WSBIT_OPCODE_PONG:
    failf(तथ्य, "[WS] invalid fragmented PONG frame");
    वापस 0;
  /* 0x8a - unfragmented PONG */
  हाल (WSBIT_OPCODE_PONG | WSBIT_FIN):
    वापस CURLWS_PONG;
  /* invalid first byte */
  शेष:
    अगर(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(तथ्य, "[WS] invalid reserved bits: %02x", firstbyte);
    अन्यथा
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(तथ्य, "[WS] invalid opcode: %02x", firstbyte);
    वापस 0;
  }
}

अटल CURLcode ws_frame_flags2firstbyte(काष्ठा Curl_easy *तथ्य,
                                         अचिन्हित पूर्णांक flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  चयन(flags & ~CURLWS_OFFSET) {
  हाल 0:
    अगर(contfragment) {
      CURL_TRC_WS(तथ्य, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      वापस CURLE_OK;
    }
    failf(तथ्य, "[WS] no flags given");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  हाल CURLWS_CONT:
    अगर(contfragment) {
      infof(तथ्य, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      वापस CURLE_OK;
    }
    failf(तथ्य, "[WS] No ongoing fragmented message to continue");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  हाल CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    वापस CURLE_OK;
  हाल (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    वापस CURLE_OK;
  हाल CURLWS_द्विआधारी:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    वापस CURLE_OK;
  हाल (CURLWS_द्विआधारी | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    वापस CURLE_OK;
  हाल CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    वापस CURLE_OK;
  हाल (CURLWS_CLOSE | CURLWS_CONT):
    failf(तथ्य, "[WS] CLOSE frame must not be fragmented");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  हाल CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    वापस CURLE_OK;
  हाल (CURLWS_PING | CURLWS_CONT):
    failf(तथ्य, "[WS] PING frame must not be fragmented");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  हाल CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    वापस CURLE_OK;
  हाल (CURLWS_PONG | CURLWS_CONT):
    failf(तथ्य, "[WS] PONG frame must not be fragmented");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  शेष:
    failf(तथ्य, "[WS] unknown flags: %x", flags);
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

अटल व्योम ws_dec_info(काष्ठा ws_decoder *dec, काष्ठा Curl_easy *तथ्य,
                        स्थिर अक्षर *संदेश)
{
  NOVERBOSE((व्योम)संदेश);
  चयन(dec->शीर्ष_len) {
  हाल 0:
    अवरोध;
  हाल 1:
    CURL_TRC_WS(तथ्य, "decoded %s [%s%s]", संदेश,
                ws_frame_name_of_op(dec->शीर्ष[0]),
                (dec->शीर्ष[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    अवरोध;
  शेष:
    अगर(dec->शीर्ष_len < dec->शीर्ष_योग) {
      CURL_TRC_WS(तथ्य, "decoded %s [%s%s](%d/%d)", संदेश,
                  ws_frame_name_of_op(dec->शीर्ष[0]),
                  (dec->शीर्ष[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->शीर्ष_len, dec->शीर्ष_योग);
    }
    अन्यथा {
      CURL_TRC_WS(तथ्य, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  संदेश, ws_frame_name_of_op(dec->शीर्ष[0]),
                  (dec->शीर्ष[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    अवरोध;
  }
}

अटल CURLcode ws_send_raw_blocking(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा websocket *ws,
                                     स्थिर अक्षर *बफर, माप_प्रकार buflen);

प्रकार CURLcode ws_write_payload(स्थिर uint8_t *buf, माप_प्रकार buflen,
                                  पूर्णांक frame_age, पूर्णांक frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  व्योम *userp,
                                  माप_प्रकार *pnwritten);

अटल व्योम ws_dec_अग्र_frame(काष्ठा ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->शीर्ष_len = dec->शीर्ष_योग = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

अटल व्योम ws_dec_reset(काष्ठा ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->शीर्ष_len = dec->शीर्ष_योग = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

अटल व्योम ws_dec_init(काष्ठा ws_decoder *dec)
{
  ws_dec_reset(dec);
}

अटल CURLcode ws_dec_read_शीर्ष(काष्ठा ws_decoder *dec,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा bufq *inraw)
{
  स्थिर uint8_t *inbuf;
  माप_प्रकार inlen;

  जबतक(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    अगर(dec->शीर्ष_len == 0) {
      dec->शीर्ष[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(तथ्य, dec->शीर्ष[0],
                                                  dec->cont_flags);
      अगर(!dec->frame_flags) {
        ws_dec_reset(dec);
        वापस CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      अगर(dec->frame_flags & (CURLWS_TEXT | CURLWS_द्विआधारी)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->शीर्ष_len = 1;
#अगर 0
      ws_dec_info(dec, तथ्य, "seeing opcode");
#पूर्ण_अगर
      जारी;
    }
    अन्यथा अगर(dec->शीर्ष_len == 1) {
      dec->शीर्ष[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->शीर्ष_len = 2;

      अगर(dec->शीर्ष[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(तथ्य, "[WS] masked input frame");
        ws_dec_reset(dec);
        वापस CURLE_RECV_ERROR;
      }
      अगर(dec->frame_flags & CURLWS_PING && dec->शीर्ष[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(तथ्य, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        वापस CURLE_RECV_ERROR;
      }
      अगर(dec->frame_flags & CURLWS_PONG && dec->शीर्ष[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(तथ्य, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        वापस CURLE_RECV_ERROR;
      }
      अगर(dec->frame_flags & CURLWS_CLOSE && dec->शीर्ष[1] > WS_MAX_CNTRL_LEN) {
        failf(तथ्य, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        वापस CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      अगर(dec->शीर्ष[1] == 126) {
        dec->शीर्ष_योग = 4;
        जारी;
      }
      अन्यथा अगर(dec->शीर्ष[1] == 127) {
        dec->शीर्ष_योग = 10;
        जारी;
      }
      अन्यथा {
        dec->शीर्ष_योग = 2;
      }
    }

    अगर(dec->शीर्ष_len < dec->शीर्ष_योग) {
      dec->शीर्ष[dec->शीर्ष_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->शीर्ष_len;
      अगर(dec->शीर्ष_len < dec->शीर्ष_योग) {
#अगर 0
        ws_dec_info(dec, तथ्य, "decoding head");
#पूर्ण_अगर
        जारी;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->शीर्ष_len == dec->शीर्ष_योग);
    चयन(dec->शीर्ष_योग) {
    हाल 2:
      dec->payload_len = dec->शीर्ष[1];
      अवरोध;
    हाल 4:
      dec->payload_len = (dec->शीर्ष[2] << 8) | dec->शीर्ष[3];
      अवरोध;
    हाल 10:
      अगर(dec->शीर्ष[2] > 127) {
        failf(तथ्य, "[WS] frame length longer than 63 bits not supported");
        वापस CURLE_RECV_ERROR;
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
      अवरोध;
    शेष:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(तथ्य, "[WS] unexpected frame header length");
      वापस CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, तथ्य, "head");
    वापस CURLE_OK;
  }
  वापस CURLE_AGAIN;
}

अटल CURLcode ws_dec_pass_payload(काष्ठा ws_decoder *dec,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा bufq *inraw,
                                    ws_write_payload *write_cb,
                                    व्योम *write_ctx)
{
  स्थिर uint8_t *inbuf;
  माप_प्रकार inlen;
  माप_प्रकार nwritten;
  CURLcode परिणाम;
  माप_प्रकार remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  जबतक(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(तथ्य)) {
    अगर(inlen > remain)
      inlen = remain;
    परिणाम = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    अगर(परिणाम)
      वापस परिणाम;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(तथ्य, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  वापस remain ? CURLE_AGAIN : CURLE_OK;
}

अटल CURLcode ws_dec_pass(काष्ठा ws_decoder *dec,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा bufq *inraw,
                            ws_write_payload *write_cb,
                            व्योम *write_ctx)
{
  CURLcode परिणाम;

  अगर(Curl_bufq_is_empty(inraw))
    वापस CURLE_AGAIN;

  चयन(dec->state) {
  हाल WS_DEC_INIT:
    ws_dec_अग्र_frame(dec);
    dec->state = WS_DEC_शीर्ष;
    FALLTHROUGH();
  हाल WS_DEC_शीर्ष:
    परिणाम = ws_dec_read_शीर्ष(dec, तथ्य, inraw);
    अगर(परिणाम) {
      अगर(परिणाम != CURLE_AGAIN) {
        failf(तथ्य, "[WS] decode frame error %d", (पूर्णांक)परिणाम);
        अवरोध;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      अवरोध;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    अगर(dec->payload_len == 0) {
      माप_प्रकार nwritten;
      स्थिर uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      परिणाम = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      अगर(परिणाम)
        वापस परिणाम;
      dec->state = WS_DEC_INIT;
      अवरोध;
    }
    FALLTHROUGH();
  हाल WS_DEC_PAYLOAD:
    परिणाम = ws_dec_pass_payload(dec, तथ्य, inraw, write_cb, write_ctx);
    ws_dec_info(dec, तथ्य, "passing");
    अगर(परिणाम)
      वापस परिणाम;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    अवरोध;
  शेष:
    /* we covered all enums above, but some code analyzers are wimps */
    परिणाम = CURLE_FAILED_INIT;
  }
  वापस परिणाम;
}

अटल व्योम update_meta(काष्ठा websocket *ws,
                        पूर्णांक frame_age, पूर्णांक frame_flags,
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
काष्ठा ws_cw_ctx {
  काष्ठा Curl_cwriter super;
  काष्ठा bufq buf;
};

अटल CURLcode ws_cw_init(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_cwriter *writer)
{
  काष्ठा ws_cw_ctx *ctx = writer->ctx;
  (व्योम)तथ्य;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_आकार, 1, BUFQ_OPT_SOFT_LIMIT);
  वापस CURLE_OK;
}

अटल व्योम ws_cw_close(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_cwriter *writer)
{
  काष्ठा ws_cw_ctx *ctx = writer->ctx;
  (व्योम)तथ्य;
  Curl_bufq_free(&ctx->buf);
}

काष्ठा ws_cw_dec_ctx {
  काष्ठा Curl_easy *तथ्य;
  काष्ठा websocket *ws;
  काष्ठा Curl_cwriter *अग्र_writer;
  पूर्णांक cw_type;
};

अटल CURLcode ws_flush(काष्ठा Curl_easy *तथ्य, काष्ठा websocket *ws,
                         bool blocking);
अटल CURLcode ws_enc_add_pending(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा websocket *ws);

अटल CURLcode ws_enc_add_cntrl(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा websocket *ws,
                                 स्थिर uint8_t *payload,
                                 माप_प्रकार plen,
                                 अचिन्हित पूर्णांक frame_type)
{
  (व्योम)तथ्य;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  अगर(plen > WS_MAX_CNTRL_LEN)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  स_नकल(ws->pending.payload, payload, plen);
  वापस CURLE_OK;
}

अटल curl_off_t ws_payload_remain(curl_off_t payload_योग,
                                    curl_off_t payload_offset,
                                    माप_प्रकार payload_buffered)
{
  curl_off_t buffered, remain = payload_योग - payload_offset;
  अगर((payload_योग < 0) || (payload_offset < 0) || (remain < 0))
    वापस -1;
  buffered = curlx_uztoso(payload_buffered);
  अगर(remain < buffered)
    वापस -1;
  वापस remain - buffered;
}

अटल CURLcode ws_cw_dec_अग्र(स्थिर uint8_t *buf, माप_प्रकार buflen,
                               पूर्णांक frame_age, पूर्णांक frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               व्योम *user_तथ्य,
                               माप_प्रकार *pnwritten)
{
  काष्ठा ws_cw_dec_ctx *ctx = user_तथ्य;
  काष्ठा Curl_easy *तथ्य = ctx->तथ्य;
  काष्ठा websocket *ws = ctx->ws;
  bool auto_pong = !तथ्य->set.ws_raw_mode && !तथ्य->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode परिणाम;

  (व्योम)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  अगर(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  अगर(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(तथ्य, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    परिणाम = ws_enc_add_cntrl(तथ्य, ws, buf, buflen, CURLWS_PONG);
    अगर(परिणाम)
      वापस परिणाम;
  }
  अन्यथा अगर(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(तथ्य, "[WS] pass %zu decoded bytes", buflen);
    परिणाम = Curl_cwriter_write(तथ्य, ctx->अग्र_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (स्थिर अक्षर *)buf, buflen);
    अगर(परिणाम)
      वापस परिणाम;
  }
  *pnwritten = buflen;
  वापस CURLE_OK;
}

अटल CURLcode ws_cw_write(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_cwriter *writer, पूर्णांक type,
                            स्थिर अक्षर *buf, माप_प्रकार nbytes)
{
  काष्ठा ws_cw_ctx *ctx = writer->ctx;
  काष्ठा websocket *ws;
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_WRITE(तथ्य, "[WS] write(len=%zu, type=%d)", nbytes, type);
  अगर(!(type & CLIENTWRITE_BODY) || तथ्य->set.ws_raw_mode)
    वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, nbytes);

  ws = Curl_conn_meta_get(तथ्य->conn, CURL_META_PROTO_WS_CONN);
  अगर(!ws) {
    failf(तथ्य, "[WS] not a websocket transfer");
    वापस CURLE_FAILED_INIT;
  }

  अगर(nbytes) {
    माप_प्रकार nwritten;
    परिणाम = Curl_bufq_write(&ctx->buf, (स्थिर uint8_t *)buf,
                             nbytes, &nwritten);
    अगर(परिणाम) {
      infof(तथ्य, "[WS] error adding data to buffer %d", (पूर्णांक)परिणाम);
      वापस परिणाम;
    }
  }

  परिणाम = Curl_cwriter_flush(तथ्य, writer->अग्र);
  अगर(परिणाम)
    जाओ out;

  जबतक(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(तथ्य)) {
    काष्ठा ws_cw_dec_ctx pass_ctx;
    pass_ctx.तथ्य = तथ्य;
    pass_ctx.ws = ws;
    pass_ctx.अग्र_writer = writer->अग्र;
    pass_ctx.cw_type = type;
    परिणाम = ws_dec_pass(&ws->dec, तथ्य, &ctx->buf,
                         ws_cw_dec_अग्र, &pass_ctx);
    अगर(परिणाम == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      परिणाम = CURLE_OK;
      जाओ out;
    }
    अन्यथा अगर(परिणाम) {
      failf(तथ्य, "[WS] decode payload error %d", (पूर्णांक)परिणाम);
      Curl_bufq_reset(&ctx->buf);
      जाओ out;
    }
  }

out:
  अगर(!परिणाम && !ws->using_reader) {
    परिणाम = ws_flush(तथ्य, ws, Curl_api_is_in_callback(तथ्य));
    अगर(परिणाम == CURLE_AGAIN)
      परिणाम = CURLE_OK;
  }

  अगर(!परिणाम && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(तथ्य, "[WS] decode ended with incomplete frame");
    परिणाम = CURLE_RECV_ERROR;
  }
  वापस परिणाम;
}

अटल CURLcode ws_cw_flush(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_cwriter *writer)
{
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_WRITE(तथ्य, "[ws] flush");
  अगर(!तथ्य->set.ws_raw_mode) {
    काष्ठा ws_cw_ctx *ctx = writer->ctx;
    काष्ठा websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    परिणाम = Curl_cwriter_flush(तथ्य, writer->अग्र);
    अगर(परिणाम)
      जाओ out;

    ws = Curl_conn_meta_get(तथ्य->conn, CURL_META_PROTO_WS_CONN);
    अगर(!ws) {
      failf(तथ्य, "[WS] not a websocket transfer");
      वापस CURLE_FAILED_INIT;
    }

    जबतक(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(तथ्य)) {
      काष्ठा ws_cw_dec_ctx pass_ctx;
      pass_ctx.तथ्य = तथ्य;
      pass_ctx.ws = ws;
      pass_ctx.अग्र_writer = writer->अग्र;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      परिणाम = ws_dec_pass(&ws->dec, तथ्य, &ctx->buf,
                           ws_cw_dec_अग्र, &pass_ctx);
      अगर(परिणाम == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        परिणाम = CURLE_OK;
        जाओ out;
      }
      अन्यथा अगर(परिणाम) {
        failf(तथ्य, "[WS] decode payload error %d", (पूर्णांक)परिणाम);
        Curl_bufq_reset(&ctx->buf);
        जाओ out;
      }
    }
  }

out:
  अगर(!परिणाम)
    परिणाम = Curl_cwriter_flush(तथ्य, writer->अग्र);
  वापस परिणाम;
}

/* WebSocket payload decoding client writer. */
अटल स्थिर काष्ठा Curl_cwtype ws_cw_decode = {
  "ws-decode",
  शून्य,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  माप(काष्ठा ws_cw_ctx)
};

अटल व्योम ws_enc_info(काष्ठा ws_encoder *enc, काष्ठा Curl_easy *तथ्य,
                        स्थिर अक्षर *संदेश)
{
  NOVERBOSE((व्योम)enc);
  NOVERBOSE((व्योम)संदेश);
  CURL_TRC_WS(तथ्य, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              संदेश, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

अटल व्योम ws_enc_reset(काष्ठा ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

अटल व्योम ws_enc_init(काष्ठा ws_encoder *enc)
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

अटल CURLcode ws_enc_add_frame(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ws_encoder *enc,
                                 अचिन्हित पूर्णांक flags,
                                 curl_off_t payload_len,
                                 काष्ठा bufq *out)
{
  uint8_t firstb = 0;
  uint8_t शीर्ष[14];
  CURLcode परिणाम;
  माप_प्रकार hlen, nwritten;

  अगर(payload_len < 0) {
    failf(तथ्य, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    वापस CURLE_SEND_ERROR;
  }

  अगर(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(तथ्य, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    वापस CURLE_SEND_ERROR;
  }

  परिणाम = ws_frame_flags2firstbyte(तथ्य, flags, (bool)enc->contfragment,
                                    &firstb);
  अगर(परिणाम)
    वापस परिणाम;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  अगर(flags & (CURLWS_TEXT | CURLWS_द्विआधारी)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  अगर(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(तथ्य, "[WS] given PING frame is too big");
    वापस CURLE_TOO_LARGE;
  }
  अगर(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(तथ्य, "[WS] given PONG frame is too big");
    वापस CURLE_TOO_LARGE;
  }
  अगर(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(तथ्य, "[WS] given CLOSE frame is too big");
    वापस CURLE_TOO_LARGE;
  }

  शीर्ष[0] = enc->firstbyte = firstb;
  अगर(payload_len > 65535) {
    शीर्ष[1] = 127 | WSBIT_MASK;
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
  अन्यथा अगर(payload_len >= 126) {
    शीर्ष[1] = 126 | WSBIT_MASK;
    शीर्ष[2] = (uint8_t)((payload_len >> 8) & 0xff);
    शीर्ष[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  अन्यथा {
    शीर्ष[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, तथ्य, "sending");

  /* 4 bytes random */

  परिणाम = Curl_rand(तथ्य, (uint8_t *)&enc->mask, माप(enc->mask));
  अगर(परिणाम)
    वापस परिणाम;

#अगर_घोषित DEBUGBUILD
  अगर(दो_पर्या("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    स_रखो(&enc->mask, 0, माप(enc->mask));
#पूर्ण_अगर

  /* add 4 bytes mask */
  स_नकल(&शीर्ष[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  परिणाम = Curl_bufq_write(out, शीर्ष, hlen, &nwritten);
  अगर(परिणाम)
    वापस परिणाम;
  अगर(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    वापस CURLE_SEND_ERROR;
  }
  वापस CURLE_OK;
}

अटल CURLcode ws_enc_write_शीर्ष(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा websocket *ws,
                                  काष्ठा ws_encoder *enc,
                                  अचिन्हित पूर्णांक flags,
                                  curl_off_t payload_len,
                                  काष्ठा bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  अगर(ws->pending.type) {
    CURLcode परिणाम = ws_enc_add_pending(तथ्य, ws);
    अगर(परिणाम)
      वापस परिणाम;
  }
  वापस ws_enc_add_frame(तथ्य, enc, flags, payload_len, out);
}

अटल CURLcode ws_enc_write_payload(काष्ठा ws_encoder *enc,
                                     काष्ठा Curl_easy *तथ्य,
                                     स्थिर uint8_t *buf, माप_प्रकार buflen,
                                     काष्ठा bufq *out, माप_प्रकार *pnwritten)
{
  CURLcode परिणाम;
  माप_प्रकार i, len, n, remain;

  *pnwritten = 0;
  अगर(Curl_bufq_is_full(out))
    वापस CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  अगर(remain < len)
    len = remain;

  क्रम(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    परिणाम = Curl_bufq_write(out, &c, 1, &n);
    अगर(परिणाम) {
      अगर((परिणाम != CURLE_AGAIN) || !i)
        वापस परिणाम;
      अवरोध;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, तथ्य, "buffered");
  वापस CURLE_OK;
}

अटल CURLcode ws_enc_add_pending(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा websocket *ws)
{
  CURLcode परिणाम;
  माप_प्रकार n;

  अगर(!ws->pending.type) /* no pending frame here */
    वापस CURLE_OK;
  अगर(ws->enc.payload_remain) /* in the middle of another frame */
    वापस CURLE_AGAIN;

  परिणाम = ws_enc_add_frame(तथ्य, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  अगर(परिणाम) {
    CURL_TRC_WS(तथ्य, "ws_enc_cntrl(), error adding head: %d",
                (पूर्णांक)परिणाम);
    जाओ out;
  }
  परिणाम = ws_enc_write_payload(&ws->enc, तथ्य, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  अगर(परिणाम) {
    CURL_TRC_WS(तथ्य, "ws_enc_cntrl(), error adding payload: %d",
                (पूर्णांक)परिणाम);
    जाओ out;
  }
  अगर(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(तथ्य, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  स_रखो(&ws->pending, 0, माप(ws->pending));

out:
  वापस परिणाम;
}

अटल CURLcode ws_enc_send(काष्ठा Curl_easy *तथ्य,
                            काष्ठा websocket *ws,
                            स्थिर uint8_t *बफर,
                            माप_प्रकार buflen,
                            curl_off_t fragsize,
                            अचिन्हित पूर्णांक flags,
                            माप_प्रकार *pnsent)
{
  माप_प्रकार n;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(!तथ्य->set.ws_raw_mode);
  *pnsent = 0;

  अगर(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    अगर(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(तथ्य, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
    अगर((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(तथ्य, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  अन्यथा {
    परिणाम = ws_flush(तथ्य, ws, Curl_api_is_in_callback(तथ्य));
    अगर(परिणाम)
      वापस परिणाम;

    परिणाम = ws_enc_write_शीर्ष(तथ्य, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    अगर(परिणाम) {
      CURL_TRC_WS(तथ्य, "curl_ws_send(), error writing frame head %d",
                  (पूर्णांक)परिणाम);
      वापस परिणाम;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  जबतक(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    अगर(buflen > ws->sendbuf_payload) {
      माप_प्रकार पूर्व_len = Curl_bufq_len(&ws->sendbuf);
      परिणाम = ws_enc_write_payload(&ws->enc, तथ्य,
                                    बफर + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      अगर(परिणाम && (परिणाम != CURLE_AGAIN))
        वापस परिणाम;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - पूर्व_len;
      अगर(!ws->sendbuf_payload) {
        वापस CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    परिणाम = ws_flush(तथ्य, ws, Curl_api_is_in_callback(तथ्य));
    अगर(!परिणाम && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      बफर += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    अन्यथा अगर(परिणाम == CURLE_AGAIN) {
      अगर(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        माप_प्रकार flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        वापस CURLE_OK;
      }
      अन्यथा {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(तथ्य, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        वापस CURLE_AGAIN;
      }
    }
    अन्यथा
      वापस परिणाम;  /* real error sending the data */
  }
  वापस CURLE_OK;
}

काष्ठा cr_ws_ctx {
  काष्ठा Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

अटल CURLcode cr_ws_init(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader)
{
  (व्योम)तथ्य;
  (व्योम)reader;
  वापस CURLE_OK;
}

अटल व्योम cr_ws_close(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader)
{
  (व्योम)तथ्य;
  (व्योम)reader;
}

अटल CURLcode cr_ws_read(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_creader *reader,
                           अक्षर *buf, माप_प्रकार blen,
                           माप_प्रकार *pnread, bool *peos)
{
  काष्ठा cr_ws_ctx *ctx = reader->ctx;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nread, n;
  काष्ठा websocket *ws;
  bool eos;

  *pnread = 0;
  अगर(ctx->eos) {
    *peos = TRUE;
    वापस CURLE_OK;
  }

  ws = Curl_conn_meta_get(तथ्य->conn, CURL_META_PROTO_WS_CONN);
  अगर(!ws) {
    failf(तथ्य, "[WS] not a websocket transfer");
    वापस CURLE_FAILED_INIT;
  }

  अगर(Curl_bufq_is_empty(&ws->sendbuf)) {
    अगर(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      वापस CURLE_OK;
    }

    अगर(ws->enc.payload_remain) {
      CURL_TRC_WS(तथ्य, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    परिणाम = Curl_creader_read(तथ्य, reader->अग्र, buf, blen, &nread, &eos);
    अगर(परिणाम)
      वापस परिणाम;
    ctx->read_eos = eos;

    अगर(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(तथ्य, reader->अग्र);
    }
    अन्यथा अगर(!nread) {
      /* nothing to convert, return this right away */
      अगर(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      जाओ out;
    }

    अगर(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      परिणाम = ws_enc_write_शीर्ष(तथ्य, ws, &ws->enc, CURLWS_द्विआधारी, nread,
                                 &ws->sendbuf);
      अगर(परिणाम)
        जाओ out;
    }

    परिणाम = ws_enc_write_payload(&ws->enc, तथ्य, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    अगर(परिणाम)
      जाओ out;
    CURL_TRC_READ(तथ्य, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  परिणाम = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  अगर(!परिणाम && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(तथ्य, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (पूर्णांक)परिणाम, *pnread, *peos);
  वापस परिणाम;
}

अटल curl_off_t cr_ws_योग_लंबाई(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (व्योम)तथ्य;
  (व्योम)reader;
  वापस -1;
}

अटल स्थिर काष्ठा Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_योग_लंबाई,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  माप(काष्ठा cr_ws_ctx)
};

काष्ठा wsfield {
  स्थिर अक्षर *name;
  स्थिर अक्षर *val;
};

CURLcode Curl_ws_request(काष्ठा Curl_easy *तथ्य, काष्ठा dynbuf *req)
{
  अचिन्हित पूर्णांक i;
  CURLcode परिणाम = CURLE_OK;
  uint8_t अक्रम[16];
  अक्षर *randstr;
  माप_प्रकार randlen;
  अक्षर keyval[40];
  काष्ठा SingleRequest *k = &तथ्य->req;
  काष्ठा wsfield heads[] = {
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
      "Sec-WebSocket-Key", शून्य,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  परिणाम = Curl_rand(तथ्य, अक्रम, माप(अक्रम));
  अगर(परिणाम)
    वापस परिणाम;
  परिणाम = curlx_base64_encode(अक्रम, माप(अक्रम), &randstr, &randlen);
  अगर(परिणाम)
    वापस परिणाम;
  DEBUGASSERT(randlen < माप(keyval));
  अगर(randlen >= माप(keyval)) {
    curlx_free(randstr);
    वापस CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, माप(keyval), randstr, randlen);
  curlx_free(randstr);
  क्रम(i = 0; !परिणाम && (i < CURL_ARRAYSIZE(heads)); i++) {
    अगर(!Curl_checkheaders(तथ्य, heads[i].name, म_माप(heads[i].name))) {
      परिणाम = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  तथ्य->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  तथ्य->conn->bits.upgrade_in_progress = TRUE;
  वापस परिणाम;
}

अटल व्योम ws_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा websocket *ws = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(काष्ठा Curl_easy *तथ्य,
                        स्थिर अक्षर *mem, माप_प्रकार nread)
{
  काष्ठा SingleRequest *k = &तथ्य->req;
  काष्ठा websocket *ws;
  काष्ठा Curl_cwriter *ws_dec_writer = शून्य;
  काष्ठा Curl_creader *ws_enc_reader = शून्य;
  CURLcode परिणाम;

  DEBUGASSERT(तथ्य->conn);
  ws = Curl_conn_meta_get(तथ्य->conn, CURL_META_PROTO_WS_CONN);
  अगर(!ws) {
    माप_प्रकार chunk_आकार = WS_CHUNK_आकार;
    ws = curlx_calloc(1, माप(*ws));
    अगर(!ws)
      वापस CURLE_OUT_OF_MEMORY;
#अगर_घोषित DEBUGBUILD
    {
      स्थिर अक्षर *p = दो_पर्या("CURL_WS_CHUNK_SIZE");
      अगर(p) {
        curl_off_t l;
        अगर(!curlx_str_संख्या(&p, &l, 1 * 1024 * 1024))
          chunk_आकार = (माप_प्रकार)l;
      }
    }
#पूर्ण_अगर
    CURL_TRC_WS(तथ्य, "WS, using chunk size %zu", chunk_आकार);
    Curl_bufq_init2(&ws->recvbuf, chunk_आकार, WS_CHUNK_गिनती,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_आकार, WS_CHUNK_गिनती,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    परिणाम = Curl_conn_meta_set(तथ्य->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    अगर(परिणाम)
      वापस परिणाम;
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

  infof(तथ्य, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  परिणाम = Curl_cwriter_create(&ws_dec_writer, तथ्य, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  अगर(परिणाम)
    जाओ out;
  परिणाम = Curl_cwriter_add(तथ्य, ws_dec_writer);
  अगर(परिणाम)
    जाओ out;
  ws_dec_writer = शून्य; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  अगर(तथ्य->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    अगर(nread) {
      परिणाम = Curl_cf_recvbuf_add(तथ्य, तथ्य->conn, FIRSTSOCKET,
                                   (स्थिर uint8_t *)mem, nread);
      अगर(परिणाम)
        जाओ out;
    }
    CURL_REQ_CLEAR_RECV(तथ्य); /* read no more content */
  }
  अन्यथा { /* !connect_only */
    अगर(तथ्य->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(तथ्य, "UPLOAD set, add ws-encode reader");
      परिणाम = Curl_creader_set_fread(तथ्य, -1);
      अगर(परिणाम)
        जाओ out;

      अगर(!तथ्य->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        परिणाम = Curl_creader_create(&ws_enc_reader, तथ्य, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        अगर(परिणाम)
          जाओ out;
        परिणाम = Curl_creader_add(तथ्य, ws_enc_reader);
        अगर(परिणाम)
          जाओ out;
        ws_enc_reader = शून्य; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      तथ्य->req.eos_read = FALSE;
      तथ्य->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(तथ्य);
    }

    /* Then pass any additional data to the writers */
    अगर(nread) {
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, mem, nread);
      अगर(परिणाम)
        जाओ out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  अगर(ws_dec_writer)
    Curl_cwriter_free(तथ्य, ws_dec_writer);
  अगर(ws_enc_reader)
    Curl_creader_free(तथ्य, ws_enc_reader);
  अगर(परिणाम)
    CURL_TRC_WS(तथ्य, "Curl_ws_accept() failed -> %d", (पूर्णांक)परिणाम);
  अन्यथा
    CURL_TRC_WS(तथ्य, "websocket established, %s mode",
                तथ्य->set.connect_only ? "connect-only" : "callback");
  वापस परिणाम;
}

काष्ठा ws_collect {
  काष्ठा Curl_easy *तथ्य;
  काष्ठा websocket *ws;
  uint8_t *बफर;
  माप_प्रकार buflen;
  माप_प्रकार bufidx;
  पूर्णांक frame_age;
  पूर्णांक frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

अटल CURLcode ws_client_collect(स्थिर uint8_t *buf, माप_प्रकार buflen,
                                  पूर्णांक frame_age, पूर्णांक frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  व्योम *userp,
                                  माप_प्रकार *pnwritten)
{
  काष्ठा ws_collect *ctx = userp;
  काष्ठा Curl_easy *तथ्य = ctx->तथ्य;
  bool auto_pong = !तथ्य->set.ws_raw_mode && !तथ्य->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode परिणाम = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  अगर(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  अगर(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  अगर(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(तथ्य, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    परिणाम = ws_enc_add_cntrl(ctx->तथ्य, ctx->ws, buf, buflen, CURLWS_PONG);
    अगर(परिणाम)
      वापस परिणाम;
    *pnwritten = buflen;
  }
  अन्यथा {
    माप_प्रकार write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    अगर(!write_len) {
      अगर(!buflen)  /* 0 length write, we accept that */
        वापस CURLE_OK;
      वापस CURLE_AGAIN;  /* no more space */
    }
    स_नकल(ctx->बफर + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  वापस परिणाम;
}

अटल CURLcode nw_in_recv(व्योम *reader_ctx,
                           uint8_t *buf, माप_प्रकार buflen,
                           माप_प्रकार *pnread)
{
  काष्ठा Curl_easy *तथ्य = reader_ctx;
  वापस Curl_easy_recv(तथ्य, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, व्योम *बफर,
                      माप_प्रकार buflen, माप_प्रकार *recv,
                      स्थिर काष्ठा curl_ws_frame **metap)
{
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम = CURLE_OK;

  अगर(CURL_EAPI_ENTER(&guard, curl, ws_recv, &परिणाम)) {
    काष्ठा Curl_easy *तथ्य = curl;
    काष्ठा connectdata *conn;
    काष्ठा websocket *ws;
    काष्ठा ws_collect ctx;

    अगर(!recv || !metap || (buflen && !बफर)) {
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }

    *recv = 0;
    *metap = शून्य;

    conn = तथ्य->conn;
    अगर(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      अगर(!तथ्य->set.connect_only) {
        failf(तथ्य, "[WS] CONNECT_ONLY is required");
        परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
        जाओ out;
      }

      Curl_getconnectinfo(तथ्य, &conn);
      अगर(!conn) {
        failf(तथ्य, "[WS] connection not found");
        परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
        जाओ out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    अगर(!ws) {
      failf(तथ्य, "[WS] connection is not setup for websocket");
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }

    स_रखो(&ctx, 0, माप(ctx));
    ctx.तथ्य = तथ्य;
    ctx.ws = ws;
    ctx.बफर = बफर;
    ctx.buflen = buflen;

    जबतक(1) {
      /* receive more when our buffer is empty */
      अगर(Curl_bufq_is_empty(&ws->recvbuf)) {
        माप_प्रकार n;
        परिणाम = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, तथ्य, &n);
        अगर(परिणाम)
          जाओ out;
        अन्यथा अगर(n == 0) {
          /* connection closed */
          infof(तथ्य, "[WS] connection expectedly closed?");
          परिणाम = CURLE_GOT_NOTHING;
          जाओ out;
        }
        CURL_TRC_WS(तथ्य, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      परिणाम = ws_dec_pass(&ws->dec, तथ्य, &ws->recvbuf,
                           ws_client_collect, &ctx);
      अगर(परिणाम == CURLE_AGAIN) {
        अगर(!ctx.written) {
          ws_dec_info(&ws->dec, तथ्य, "need more input");
          जारी;  /* nothing written, try more input */
        }
        अवरोध;
      }
      अन्यथा अगर(परिणाम) {
        जाओ out;
      }
      अन्यथा अगर(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        अवरोध;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.frame_age, ctx.frame_flags, ctx.payload_offset,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(तथ्य, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.offset,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    अगर(!तथ्य->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(तथ्य, ws);
      अगर(!r2)
        (व्योम)ws_flush(तथ्य, ws, Curl_api_is_in_callback(तथ्य));
    }
    परिणाम = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}

अटल CURLcode ws_flush(काष्ठा Curl_easy *तथ्य, काष्ठा websocket *ws,
                         bool blocking)
{
  CURLcode परिणाम;

  /* If there is space, add any pending control frame */
  अगर(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_आकार) {
    परिणाम = ws_enc_add_pending(तथ्य, ws);
    अगर(परिणाम && (परिणाम != CURLE_AGAIN))
      वापस परिणाम;
  }

  अगर(!Curl_bufq_is_empty(&ws->sendbuf)) {
    स्थिर uint8_t *out;
    माप_प्रकार outlen, n;
#अगर_घोषित DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_अग्र = FALSE;
    माप_प्रकार chunk_egain = 0;
    स्थिर अक्षर *p = दो_पर्या("CURL_WS_CHUNK_EAGAIN");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (माप_प्रकार)l;
    }
#पूर्ण_अगर

    जबतक(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#अगर_घोषित DEBUGBUILD
      अगर(eagain_अग्र)
        वापस CURLE_AGAIN;
      अगर(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_अग्र = TRUE;
      }
#पूर्ण_अगर
      अगर(blocking) {
        परिणाम = ws_send_raw_blocking(तथ्य, ws, (स्थिर अक्षर *)out, outlen);
        n = परिणाम ? 0 : outlen;
      }
      अन्यथा अगर(तथ्य->set.connect_only || Curl_api_is_in_callback(तथ्य))
        परिणाम = Curl_senddata(तथ्य, out, outlen, &n);
      अन्यथा {
        परिणाम = Curl_xfer_send(तथ्य, out, outlen, FALSE, &n);
        अगर(!परिणाम && !n && outlen)
          परिणाम = CURLE_AGAIN;
      }

      अगर(परिणाम == CURLE_AGAIN) {
        CURL_TRC_WS(तथ्य, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        वापस परिणाम;
      }
      अन्यथा अगर(परिणाम) {
        failf(तथ्य, "[WS] flush, write error %d", (पूर्णांक)परिणाम);
        वापस परिणाम;
      }
      अन्यथा {
        CURL_TRC_WS(तथ्य, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  वापस CURLE_OK;
}

अटल CURLcode ws_send_raw_blocking(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा websocket *ws,
                                     स्थिर अक्षर *बफर, माप_प्रकार buflen)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nwritten;

  अगर(!तथ्य)
    वापस परिणाम;

  (व्योम)ws;
  जबतक(buflen) {
    परिणाम = Curl_xfer_send(तथ्य, बफर, buflen, FALSE, &nwritten);
    अगर(परिणाम)
      वापस परिणाम;
    DEBUGASSERT(nwritten <= buflen);
    बफर += nwritten;
    buflen -= nwritten;
    अगर(buflen) {
      curl_socket_t sock = तथ्य->conn->sock[FIRSTSOCKET];
      timediff_t बायाँ_ms;
      पूर्णांक ev;

      CURL_TRC_WS(तथ्य, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      बायाँ_ms = Curl_timeleft_ms(तथ्य);
      अगर(बायाँ_ms < 0) {
        failf(तथ्य, "[WS] Timeout waiting for socket becoming writable");
        वापस CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      अगर(sock == CURL_SOCKET_BAD)
        वापस CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, बायाँ_ms ? बायाँ_ms : 500);
      अगर(ev < 0) {
        failf(तथ्य, "[WS] Error while waiting for socket becoming writable");
        वापस CURLE_SEND_ERROR;
      }
    }
  }
  वापस परिणाम;
}

अटल CURLcode ws_send_raw(काष्ठा Curl_easy *तथ्य, स्थिर व्योम *बफर,
                            माप_प्रकार buflen, माप_प्रकार *pnwritten)
{
  काष्ठा websocket *ws;
  CURLcode परिणाम;

  ws = Curl_conn_meta_get(तथ्य->conn, CURL_META_PROTO_WS_CONN);
  अगर(!ws) {
    failf(तथ्य, "[WS] Not a websocket transfer");
    वापस CURLE_SEND_ERROR;
  }
  अगर(!buflen)
    वापस CURLE_OK;

  अगर(Curl_api_is_in_callback(तथ्य)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    परिणाम = ws_flush(तथ्य, ws, TRUE);
    अगर(परिणाम)
      वापस परिणाम;
    परिणाम = ws_send_raw_blocking(तथ्य, ws, बफर, buflen);
    अगर(!परिणाम)
      *pnwritten = buflen;
  }
  अन्यथा {
    /* We need any pending data to be sent or EAGAIN this call. */
    परिणाम = ws_flush(तथ्य, ws, FALSE);
    अगर(परिणाम)
      वापस परिणाम;
    परिणाम = Curl_senddata(तथ्य, बफर, buflen, pnwritten);
  }

  CURL_TRC_WS(तथ्य, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (पूर्णांक)परिणाम, *pnwritten);
  वापस परिणाम;
}

CURLcode curl_ws_send(CURL *curl, स्थिर व्योम *बफर_arg,
                      माप_प्रकार buflen, माप_प्रकार *sent,
                      curl_off_t fragsize,
                      अचिन्हित पूर्णांक flags)
{
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम = CURLE_OK;

  अगर(CURL_EAPI_ENTER(&guard, curl, ws_send, &परिणाम)) {
    काष्ठा websocket *ws;
    स्थिर uint8_t *बफर = बफर_arg;
    काष्ठा Curl_easy *तथ्य = curl;
    माप_प्रकार ndummy;
    माप_प्रकार *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(तथ्य, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, तथ्य->set.ws_raw_mode);

    *pnsent = 0;

    अगर(!बफर && buflen) {
      failf(तथ्य, "[WS] buffer is NULL when buflen is not");
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }

    अगर(!तथ्य->conn && तथ्य->set.connect_only) {
      परिणाम = Curl_connect_only_attach(तथ्य);
      अगर(परिणाम)
        जाओ out;
    }
    अगर(!तथ्य->conn) {
      failf(तथ्य, "[WS] No associated connection");
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
    ws = Curl_conn_meta_get(तथ्य->conn, CURL_META_PROTO_WS_CONN);
    अगर(!ws) {
      failf(तथ्य, "[WS] Not a websocket transfer");
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }

    अगर(तथ्य->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      परिणाम = ws_flush(तथ्य, ws, FALSE);
      अगर(परिणाम)
        जाओ out;

      अगर(!बफर) {
        failf(तथ्य, "[WS] buffer is NULL in raw mode");
        परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
        जाओ out;
      }
      अगर(!sent) {
        failf(तथ्य, "[WS] sent is NULL in raw mode");
        परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
        जाओ out;
      }
      अगर(fragsize || flags) {
        failf(तथ्य, "[WS] fragsize and flags must be zero in raw mode");
        परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
        जाओ out;
      }
      परिणाम = ws_send_raw(तथ्य, बफर, buflen, pnsent);
      जाओ out;
    }

    /* Not RAW mode, we do the frame encoding */
    परिणाम = ws_enc_send(तथ्य, ws, बफर, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(तथ्य, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, तथ्य->set.ws_raw_mode, (पूर्णांक)परिणाम,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}

अटल CURLcode ws_setup_conn(काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  तथ्य->state.http_neg.accept_09 = FALSE;
  तथ्य->state.http_neg.only_10 = FALSE;
  तथ्य->state.http_neg.wanted = CURL_HTTP_V1x;
  तथ्य->state.http_neg.allowed = CURL_HTTP_V1x;
  वापस Curl_http_setup_conn(तथ्य, conn);
}

स्थिर काष्ठा curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  काष्ठा Curl_easy *तथ्य = curl;
  अगर(GOOD_EASY_HANDLE(तथ्य) && Curl_api_is_in_callback(तथ्य) &&
     तथ्य->conn && !तथ्य->set.ws_raw_mode) {
    काष्ठा websocket *ws;
    ws = Curl_conn_meta_get(तथ्य->conn, CURL_META_PROTO_WS_CONN);
    अगर(ws)
      वापस &ws->recvframe;
  }
  वापस शून्य;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         अचिन्हित पूर्णांक flags,
                                         curl_off_t frame_len)
{
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम = CURLE_OK;

  अगर(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &परिणाम)) {
    काष्ठा Curl_easy *तथ्य = curl;
    काष्ठा websocket *ws;

    अगर(तथ्य->set.ws_raw_mode) {
      failf(तथ्य, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      परिणाम = CURLE_FAILED_INIT;
      जाओ out;
    }

    CURL_TRC_WS(तथ्य, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    अगर(!तथ्य->conn) {
      failf(तथ्य, "[WS] No associated connection");
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
    ws = Curl_conn_meta_get(तथ्य->conn, CURL_META_PROTO_WS_CONN);
    अगर(!ws) {
      failf(तथ्य, "[WS] Not a websocket transfer");
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }

    अगर(ws->enc.payload_remain) {
      failf(तथ्य, "[WS] previous frame not finished");
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }

    परिणाम = ws_enc_write_शीर्ष(तथ्य, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    अगर(परिणाम)
      CURL_TRC_WS(तथ्य, "curl_start_frame(), error adding frame head %d",
                  (पूर्णांक)परिणाम);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}

स्थिर काष्ठा Curl_protocol Curl_protocol_ws = {
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

CURLcode curl_ws_recv(CURL *curl, व्योम *बफर, माप_प्रकार buflen,
                      माप_प्रकार *recv,
                      स्थिर काष्ठा curl_ws_frame **metap)
{
  (व्योम)curl;
  (व्योम)बफर;
  (व्योम)buflen;
  (व्योम)recv;
  (व्योम)metap;
  वापस CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, स्थिर व्योम *बफर,
                      माप_प्रकार buflen, माप_प्रकार *sent,
                      curl_off_t fragsize,
                      अचिन्हित पूर्णांक flags)
{
  (व्योम)curl;
  (व्योम)बफर;
  (व्योम)buflen;
  (व्योम)sent;
  (व्योम)fragsize;
  (व्योम)flags;
  वापस CURLE_NOT_BUILT_IN;
}

स्थिर काष्ठा curl_ws_frame *curl_ws_meta(CURL *तथ्य)
{
  (व्योम)तथ्य;
  वापस शून्य;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         अचिन्हित पूर्णांक flags,
                                         curl_off_t frame_len)
{
  (व्योम)curl;
  (व्योम)flags;
  (व्योम)frame_len;
  वापस CURLE_NOT_BUILT_IN;
}

#पूर्ण_अगर /* !CURL_DISABLE_WEBSOCKETS */
