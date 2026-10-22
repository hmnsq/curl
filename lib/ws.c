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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "ws.h"

#ifndef CURL_DISABLE_WEBSOCKETS

#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "cf-recvbuf.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "easyif.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

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
#define WS_CHUNK_ಎಣಿಕೆ 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
ಗಣನಾ ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_HEAD,
  WS_DEC_PAYLOAD
};

ಸಂರಚನಾ ws_decoder {
  ಪೂರ್ಣಾಙ್ಕ frame_age;        /* zero */
  ಪೂರ್ಣಾಙ್ಕ frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t head[10];
  ಪೂರ್ಣಾಙ್ಕ head_len, head_total;
  ಗಣನಾ ws_dec_state state;
  ಪೂರ್ಣಾಙ್ಕ cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
ಸಂರಚನಾ ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  unsigned ಪೂರ್ಣಾಙ್ಕ xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#define WS_MAX_CNTRL_LEN    125

ಸಂರಚನಾ ws_cntrl_frame {
  unsigned ಪೂರ್ಣಾಙ್ಕ type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
ಸಂರಚನಾ websocket {
  ಸಂರಚನಾ Curl_easy *data; /* used for write callback handling */
  ಸಂರಚನಾ ws_decoder dec;  /* decode of ws frames */
  ಸಂರಚನಾ ws_encoder enc;  /* encode of ws frames */
  ಸಂರಚನಾ bufq recvbuf;    /* raw data from the server */
  ಸಂರಚನಾ bufq sendbuf;    /* raw data to be sent to the server */
  ಸಂರಚನಾ curl_ws_frame recvframe;  /* the current WS FRAME received */
  ಸಂರಚನಾ ws_cntrl_frame pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#ifdef CURLVERBOSE
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ws_frame_name_of_op(uint8_t firstbyte)
{
  ವಿಕಲ್ಪ(firstbyte & WSBIT_OPCODE_MASK) {
  ಪಕ್ಷ WSBIT_OPCODE_CONT:
    ಪ್ರತ್ಯಾಗಮ "CONT";
  ಪಕ್ಷ WSBIT_OPCODE_TEXT:
    ಪ್ರತ್ಯಾಗಮ "TEXT";
  ಪಕ್ಷ WSBIT_OPCODE_BIN:
    ಪ್ರತ್ಯಾಗಮ "BIN";
  ಪಕ್ಷ WSBIT_OPCODE_CLOSE:
    ಪ್ರತ್ಯಾಗಮ "CLOSE";
  ಪಕ್ಷ WSBIT_OPCODE_PING:
    ಪ್ರತ್ಯಾಗಮ "PING";
  ಪಕ್ಷ WSBIT_OPCODE_PONG:
    ಪ್ರತ್ಯಾಗಮ "PONG";
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ "???";
  }
}
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ws_frame_firstbyte2flags(ಸಂರಚನಾ Curl_easy *data,
                                    uint8_t firstbyte, ಪೂರ್ಣಾಙ್ಕ cont_flags)
{
  ವಿಕಲ್ಪ(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  ಪಕ್ಷ WSBIT_OPCODE_CONT:
    ಯದಿ(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  ಪಕ್ಷ (WSBIT_OPCODE_CONT | WSBIT_FIN):
    ಯದಿ(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  ಪಕ್ಷ WSBIT_OPCODE_TEXT:
    ಯದಿ(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  ಪಕ್ಷ (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    ಯದಿ(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  ಪಕ್ಷ WSBIT_OPCODE_BIN:
    ಯದಿ(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ CURLWS_ದ್ವಿಮಾನ | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  ಪಕ್ಷ (WSBIT_OPCODE_BIN | WSBIT_FIN):
    ಯದಿ(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ CURLWS_ದ್ವಿಮಾನ;
  /* 0x08 - first CLOSE fragment */
  ಪಕ್ಷ WSBIT_OPCODE_CLOSE:
    failf(data, "[WS] invalid fragmented CLOSE frame");
    ಪ್ರತ್ಯಾಗಮ 0;
  /* 0x88 - unfragmented CLOSE */
  ಪಕ್ಷ (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    ಪ್ರತ್ಯಾಗಮ CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  ಪಕ್ಷ WSBIT_OPCODE_PING:
    failf(data, "[WS] invalid fragmented PING frame");
    ಪ್ರತ್ಯಾಗಮ 0;
  /* 0x89 - unfragmented PING */
  ಪಕ್ಷ (WSBIT_OPCODE_PING | WSBIT_FIN):
    ಪ್ರತ್ಯಾಗಮ CURLWS_PING;
  /* 0x0a - first PONG fragment */
  ಪಕ್ಷ WSBIT_OPCODE_PONG:
    failf(data, "[WS] invalid fragmented PONG frame");
    ಪ್ರತ್ಯಾಗಮ 0;
  /* 0x8a - unfragmented PONG */
  ಪಕ್ಷ (WSBIT_OPCODE_PONG | WSBIT_FIN):
    ಪ್ರತ್ಯಾಗಮ CURLWS_PONG;
  /* invalid first byte */
  ಸ್ವಭಾವ:
    ಯದಿ(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(data, "[WS] invalid reserved bits: %02x", firstbyte);
    ಅನ್ಯಥಾ
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(data, "[WS] invalid opcode: %02x", firstbyte);
    ಪ್ರತ್ಯಾಗಮ 0;
  }
}

ಸ್ಥಿರ CURLcode ws_frame_flags2firstbyte(ಸಂರಚನಾ Curl_easy *data,
                                         unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  ವಿಕಲ್ಪ(flags & ~CURLWS_OFFSET) {
  ಪಕ್ಷ 0:
    ಯದಿ(contfragment) {
      CURL_TRC_WS(data, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    failf(data, "[WS] no flags given");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಪಕ್ಷ CURLWS_CONT:
    ಯದಿ(contfragment) {
      infof(data, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    failf(data, "[WS] No ongoing fragmented message to continue");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಪಕ್ಷ CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURLWS_ದ್ವಿಮಾನ:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ (CURLWS_ದ್ವಿಮಾನ | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ (CURLWS_CLOSE | CURLWS_CONT):
    failf(data, "[WS] CLOSE frame must not be fragmented");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಪಕ್ಷ CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ (CURLWS_PING | CURLWS_CONT):
    failf(data, "[WS] PING frame must not be fragmented");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಪಕ್ಷ CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ (CURLWS_PONG | CURLWS_CONT):
    failf(data, "[WS] PONG frame must not be fragmented");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಸ್ವಭಾವ:
    failf(data, "[WS] unknown flags: %x", flags);
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_dec_info(ಸಂರಚನಾ ws_decoder *dec, ಸಂರಚನಾ Curl_easy *data,
                        ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ)
{
  NOVERBOSE((ಶೂನ್ಯ)ಸಂದೇಶ);
  ವಿಕಲ್ಪ(dec->head_len) {
  ಪಕ್ಷ 0:
    ಭಙ್ಗ;
  ಪಕ್ಷ 1:
    CURL_TRC_WS(data, "decoded %s [%s%s]", ಸಂದೇಶ,
                ws_frame_name_of_op(dec->head[0]),
                (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಯದಿ(dec->head_len < dec->head_total) {
      CURL_TRC_WS(data, "decoded %s [%s%s](%d/%d)", ಸಂದೇಶ,
                  ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->head_len, dec->head_total);
    }
    ಅನ್ಯಥಾ {
      CURL_TRC_WS(data, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  ಸಂದೇಶ, ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    ಭಙ್ಗ;
  }
}

ಸ್ಥಿರ CURLcode ws_send_raw_blocking(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ websocket *ws,
                                     ಧ್ರುವ ವರ್ಣ *buffer, size_t buflen);

ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode ws_write_payload(ಧ್ರುವ uint8_t *buf, size_t buflen,
                                  ಪೂರ್ಣಾಙ್ಕ frame_age, ಪೂರ್ಣಾಙ್ಕ frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  ಶೂನ್ಯ *userp,
                                  size_t *pnwritten);

ಸ್ಥಿರ ಶೂನ್ಯ ws_dec_next_frame(ಸಂರಚನಾ ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_dec_reset(ಸಂರಚನಾ ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_dec_init(ಸಂರಚನಾ ws_decoder *dec)
{
  ws_dec_reset(dec);
}

ಸ್ಥಿರ CURLcode ws_dec_read_head(ಸಂರಚನಾ ws_decoder *dec,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ bufq *inraw)
{
  ಧ್ರುವ uint8_t *inbuf;
  size_t inlen;

  ಯಾವತ್(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    ಯದಿ(dec->head_len == 0) {
      dec->head[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(data, dec->head[0],
                                                  dec->cont_flags);
      ಯದಿ(!dec->frame_flags) {
        ws_dec_reset(dec);
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      ಯದಿ(dec->frame_flags & (CURLWS_TEXT | CURLWS_ದ್ವಿಮಾನ)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->head_len = 1;
#ಯದಿ 0
      ws_dec_info(dec, data, "seeing opcode");
#endif
      ಅನುವರ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ(dec->head_len == 1) {
      dec->head[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->head_len = 2;

      ಯದಿ(dec->head[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(data, "[WS] masked input frame");
        ws_dec_reset(dec);
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
      ಯದಿ(dec->frame_flags & CURLWS_PING && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(data, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
      ಯದಿ(dec->frame_flags & CURLWS_PONG && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(data, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
      ಯದಿ(dec->frame_flags & CURLWS_CLOSE && dec->head[1] > WS_MAX_CNTRL_LEN) {
        failf(data, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      ಯದಿ(dec->head[1] == 126) {
        dec->head_total = 4;
        ಅನುವರ್ತ;
      }
      ಅನ್ಯಥಾ ಯದಿ(dec->head[1] == 127) {
        dec->head_total = 10;
        ಅನುವರ್ತ;
      }
      ಅನ್ಯಥಾ {
        dec->head_total = 2;
      }
    }

    ಯದಿ(dec->head_len < dec->head_total) {
      dec->head[dec->head_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->head_len;
      ಯದಿ(dec->head_len < dec->head_total) {
#ಯದಿ 0
        ws_dec_info(dec, data, "decoding head");
#endif
        ಅನುವರ್ತ;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->head_len == dec->head_total);
    ವಿಕಲ್ಪ(dec->head_total) {
    ಪಕ್ಷ 2:
      dec->payload_len = dec->head[1];
      ಭಙ್ಗ;
    ಪಕ್ಷ 4:
      dec->payload_len = (dec->head[2] << 8) | dec->head[3];
      ಭಙ್ಗ;
    ಪಕ್ಷ 10:
      ಯದಿ(dec->head[2] > 127) {
        failf(data, "[WS] frame length longer than 63 bits not supported");
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
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
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(data, "[WS] unexpected frame header length");
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, data, "head");
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
}

ಸ್ಥಿರ CURLcode ws_dec_pass_payload(ಸಂರಚನಾ ws_decoder *dec,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ bufq *inraw,
                                    ws_write_payload *write_cb,
                                    ಶೂನ್ಯ *write_ctx)
{
  ಧ್ರುವ uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode result;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  ಯಾವತ್(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(data)) {
    ಯದಿ(inlen > remain)
      inlen = remain;
    result = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(data, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  ಪ್ರತ್ಯಾಗಮ remain ? CURLE_AGAIN : CURLE_OK;
}

ಸ್ಥಿರ CURLcode ws_dec_pass(ಸಂರಚನಾ ws_decoder *dec,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ bufq *inraw,
                            ws_write_payload *write_cb,
                            ಶೂನ್ಯ *write_ctx)
{
  CURLcode result;

  ಯದಿ(Curl_bufq_is_empty(inraw))
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ವಿಕಲ್ಪ(dec->state) {
  ಪಕ್ಷ WS_DEC_INIT:
    ws_dec_next_frame(dec);
    dec->state = WS_DEC_HEAD;
    FALLTHROUGH();
  ಪಕ್ಷ WS_DEC_HEAD:
    result = ws_dec_read_head(dec, data, inraw);
    ಯದಿ(result) {
      ಯದಿ(result != CURLE_AGAIN) {
        failf(data, "[WS] decode frame error %d", (ಪೂರ್ಣಾಙ್ಕ)result);
        ಭಙ್ಗ;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      ಭಙ್ಗ;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    ಯದಿ(dec->payload_len == 0) {
      size_t nwritten;
      ಧ್ರುವ uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      result = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      dec->state = WS_DEC_INIT;
      ಭಙ್ಗ;
    }
    FALLTHROUGH();
  ಪಕ್ಷ WS_DEC_PAYLOAD:
    result = ws_dec_pass_payload(dec, data, inraw, write_cb, write_ctx);
    ws_dec_info(dec, data, "passing");
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    /* we covered all enums above, but some code analyzers are wimps */
    result = CURLE_FAILED_INIT;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ update_meta(ಸಂರಚನಾ websocket *ws,
                        ಪೂರ್ಣಾಙ್ಕ frame_age, ಪೂರ್ಣಾಙ್ಕ frame_flags,
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
ಸಂರಚನಾ ws_cw_ctx {
  ಸಂರಚನಾ Curl_cwriter super;
  ಸಂರಚನಾ bufq buf;
};

ಸ್ಥಿರ CURLcode ws_cw_init(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ ws_cw_ctx *ctx = writer->ctx;
  (ಶೂನ್ಯ)data;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_SIZE, 1, BUFQ_OPT_SOFT_LIMIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_cw_close(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ ws_cw_ctx *ctx = writer->ctx;
  (ಶೂನ್ಯ)data;
  Curl_bufq_free(&ctx->buf);
}

ಸಂರಚನಾ ws_cw_dec_ctx {
  ಸಂರಚನಾ Curl_easy *data;
  ಸಂರಚನಾ websocket *ws;
  ಸಂರಚನಾ Curl_cwriter *next_writer;
  ಪೂರ್ಣಾಙ್ಕ cw_type;
};

ಸ್ಥಿರ CURLcode ws_flush(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ websocket *ws,
                         bool blocking);
ಸ್ಥಿರ CURLcode ws_enc_add_pending(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ websocket *ws);

ಸ್ಥಿರ CURLcode ws_enc_add_cntrl(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ websocket *ws,
                                 ಧ್ರುವ uint8_t *payload,
                                 size_t plen,
                                 unsigned ಪೂರ್ಣಾಙ್ಕ frame_type)
{
  (ಶೂನ್ಯ)data;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  ಯದಿ(plen > WS_MAX_CNTRL_LEN)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  memcpy(ws->pending.payload, payload, plen);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ curl_off_t ws_payload_remain(curl_off_t payload_total,
                                    curl_off_t payload_offset,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_total - payload_offset;
  ಯದಿ((payload_total < 0) || (payload_offset < 0) || (remain < 0))
    ಪ್ರತ್ಯಾಗಮ -1;
  buffered = curlx_uztoso(payload_buffered);
  ಯದಿ(remain < buffered)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಪ್ರತ್ಯಾಗಮ remain - buffered;
}

ಸ್ಥಿರ CURLcode ws_cw_dec_next(ಧ್ರುವ uint8_t *buf, size_t buflen,
                               ಪೂರ್ಣಾಙ್ಕ frame_age, ಪೂರ್ಣಾಙ್ಕ frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               ಶೂನ್ಯ *user_data,
                               size_t *pnwritten)
{
  ಸಂರಚನಾ ws_cw_dec_ctx *ctx = user_data;
  ಸಂರಚನಾ Curl_easy *data = ctx->data;
  ಸಂರಚನಾ websocket *ws = ctx->ws;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result;

  (ಶೂನ್ಯ)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  ಯದಿ(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ಯದಿ(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(data, ws, buf, buflen, CURLWS_PONG);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ ಯದಿ(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(data, "[WS] pass %zu decoded bytes", buflen);
    result = Curl_cwriter_write(data, ctx->next_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ಧ್ರುವ ವರ್ಣ *)buf, buflen);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  *pnwritten = buflen;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ws_cw_write(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                            ಧ್ರುವ ವರ್ಣ *buf, size_t nbytes)
{
  ಸಂರಚನಾ ws_cw_ctx *ctx = writer->ctx;
  ಸಂರಚನಾ websocket *ws;
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[WS] write(len=%zu, type=%d)", nbytes, type);
  ಯದಿ(!(type & CLIENTWRITE_BODY) || data->set.ws_raw_mode)
    ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ಯದಿ(!ws) {
    failf(data, "[WS] not a websocket transfer");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  ಯದಿ(nbytes) {
    size_t nwritten;
    result = Curl_bufq_write(&ctx->buf, (ಧ್ರುವ uint8_t *)buf,
                             nbytes, &nwritten);
    ಯದಿ(result) {
      infof(data, "[WS] error adding data to buffer %d", (ಪೂರ್ಣಾಙ್ಕ)result);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  result = Curl_cwriter_flush(data, writer->next);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯಾವತ್(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
    ಸಂರಚನಾ ws_cw_dec_ctx pass_ctx;
    pass_ctx.data = data;
    pass_ctx.ws = ws;
    pass_ctx.next_writer = writer->next;
    pass_ctx.cw_type = type;
    result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                         ws_cw_dec_next, &pass_ctx);
    ಯದಿ(result == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      result = CURLE_OK;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(result) {
      failf(data, "[WS] decode payload error %d", (ಪೂರ್ಣಾಙ್ಕ)result);
      Curl_bufq_reset(&ctx->buf);
      ಗಚ್ಛ out;
    }
  }

out:
  ಯದಿ(!result && !ws->using_reader) {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    ಯದಿ(result == CURLE_AGAIN)
      result = CURLE_OK;
  }

  ಯದಿ(!result && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(data, "[WS] decode ended with incomplete frame");
    result = CURLE_RECV_ERROR;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ws_cw_flush(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_cwriter *writer)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[ws] flush");
  ಯದಿ(!data->set.ws_raw_mode) {
    ಸಂರಚನಾ ws_cw_ctx *ctx = writer->ctx;
    ಸಂರಚನಾ websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    result = Curl_cwriter_flush(data, writer->next);
    ಯದಿ(result)
      ಗಚ್ಛ out;

    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ಯದಿ(!ws) {
      failf(data, "[WS] not a websocket transfer");
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    }

    ಯಾವತ್(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
      ಸಂರಚನಾ ws_cw_dec_ctx pass_ctx;
      pass_ctx.data = data;
      pass_ctx.ws = ws;
      pass_ctx.next_writer = writer->next;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                           ws_cw_dec_next, &pass_ctx);
      ಯದಿ(result == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        result = CURLE_OK;
        ಗಚ್ಛ out;
      }
      ಅನ್ಯಥಾ ಯದಿ(result) {
        failf(data, "[WS] decode payload error %d", (ಪೂರ್ಣಾಙ್ಕ)result);
        Curl_bufq_reset(&ctx->buf);
        ಗಚ್ಛ out;
      }
    }
  }

out:
  ಯದಿ(!result)
    result = Curl_cwriter_flush(data, writer->next);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* WebSocket payload decoding client writer. */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype ws_cw_decode = {
  "ws-decode",
  ರಿಕ್ತ,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ ws_cw_ctx)
};

ಸ್ಥಿರ ಶೂನ್ಯ ws_enc_info(ಸಂರಚನಾ ws_encoder *enc, ಸಂರಚನಾ Curl_easy *data,
                        ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ)
{
  NOVERBOSE((ಶೂನ್ಯ)enc);
  NOVERBOSE((ಶೂನ್ಯ)ಸಂದೇಶ);
  CURL_TRC_WS(data, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              ಸಂದೇಶ, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_enc_reset(ಸಂರಚನಾ ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_enc_init(ಸಂರಚನಾ ws_encoder *enc)
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

ಸ್ಥಿರ CURLcode ws_enc_add_frame(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ws_encoder *enc,
                                 unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                                 curl_off_t payload_len,
                                 ಸಂರಚನಾ bufq *out)
{
  uint8_t firstb = 0;
  uint8_t head[14];
  CURLcode result;
  size_t hlen, nwritten;

  ಯದಿ(payload_len < 0) {
    failf(data, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }

  ಯದಿ(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(data, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }

  result = ws_frame_flags2firstbyte(data, flags, (bool)enc->contfragment,
                                    &firstb);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  ಯದಿ(flags & (CURLWS_TEXT | CURLWS_ದ್ವಿಮಾನ)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  ಯದಿ(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PING frame is too big");
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  }
  ಯದಿ(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PONG frame is too big");
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  }
  ಯದಿ(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given CLOSE frame is too big");
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  }

  head[0] = enc->firstbyte = firstb;
  ಯದಿ(payload_len > 65535) {
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
  ಅನ್ಯಥಾ ಯದಿ(payload_len >= 126) {
    head[1] = 126 | WSBIT_MASK;
    head[2] = (uint8_t)((payload_len >> 8) & 0xff);
    head[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  ಅನ್ಯಥಾ {
    head[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, data, "sending");

  /* 4 bytes random */

  result = Curl_rand(data, (uint8_t *)&enc->mask, ಪರಿಮಾಣ(enc->mask));
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

#ifdef DEBUGBUILD
  ಯದಿ(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->mask, 0, ಪರಿಮಾಣ(enc->mask));
#endif

  /* add 4 bytes mask */
  memcpy(&head[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  result = Curl_bufq_write(out, head, hlen, &nwritten);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಯದಿ(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ws_enc_write_head(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ websocket *ws,
                                  ಸಂರಚನಾ ws_encoder *enc,
                                  unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                                  curl_off_t payload_len,
                                  ಸಂರಚನಾ bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  ಯದಿ(ws->pending.type) {
    CURLcode result = ws_enc_add_pending(data, ws);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ ws_enc_add_frame(data, enc, flags, payload_len, out);
}

ಸ್ಥಿರ CURLcode ws_enc_write_payload(ಸಂರಚನಾ ws_encoder *enc,
                                     ಸಂರಚನಾ Curl_easy *data,
                                     ಧ್ರುವ uint8_t *buf, size_t buflen,
                                     ಸಂರಚನಾ bufq *out, size_t *pnwritten)
{
  CURLcode result;
  size_t i, len, n, remain;

  *pnwritten = 0;
  ಯದಿ(Curl_bufq_is_full(out))
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  ಯದಿ(remain < len)
    len = remain;

  ಪ್ರತಿ(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    result = Curl_bufq_write(out, &c, 1, &n);
    ಯದಿ(result) {
      ಯದಿ((result != CURLE_AGAIN) || !i)
        ಪ್ರತ್ಯಾಗಮ result;
      ಭಙ್ಗ;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, data, "buffered");
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ws_enc_add_pending(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ websocket *ws)
{
  CURLcode result;
  size_t n;

  ಯದಿ(!ws->pending.type) /* no pending frame here */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಯದಿ(ws->enc.payload_remain) /* in the middle of another frame */
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  result = ws_enc_add_frame(data, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  ಯದಿ(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding head: %d",
                (ಪೂರ್ಣಾಙ್ಕ)result);
    ಗಚ್ಛ out;
  }
  result = ws_enc_write_payload(&ws->enc, data, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  ಯದಿ(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding payload: %d",
                (ಪೂರ್ಣಾಙ್ಕ)result);
    ಗಚ್ಛ out;
  }
  ಯದಿ(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(data, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, ಪರಿಮಾಣ(ws->pending));

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ws_enc_send(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ websocket *ws,
                            ಧ್ರುವ uint8_t *buffer,
                            size_t buflen,
                            curl_off_t fragsize,
                            unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!data->set.ws_raw_mode);
  *pnsent = 0;

  ಯದಿ(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    ಯದಿ(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(data, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ಯದಿ((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(data, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  ಅನ್ಯಥಾ {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    result = ws_enc_write_head(data, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    ಯದಿ(result) {
      CURL_TRC_WS(data, "curl_ws_send(), error writing frame head %d",
                  (ಪೂರ್ಣಾಙ್ಕ)result);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  ಯಾವತ್(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    ಯದಿ(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      result = ws_enc_write_payload(&ws->enc, data,
                                    buffer + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      ಯದಿ(result && (result != CURLE_AGAIN))
        ಪ್ರತ್ಯಾಗಮ result;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      ಯದಿ(!ws->sendbuf_payload) {
        ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    ಯದಿ(!result && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      buffer += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ(result == CURLE_AGAIN) {
      ಯದಿ(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಅನ್ಯಥಾ {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(data, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
      }
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ result;  /* real error sending the data */
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸಂರಚನಾ cr_ws_ctx {
  ಸಂರಚನಾ Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

ಸ್ಥಿರ CURLcode cr_ws_init(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_creader *reader)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_ws_close(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_creader *reader)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
}

ಸ್ಥಿರ CURLcode cr_ws_read(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_creader *reader,
                           ವರ್ಣ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ಸಂರಚನಾ cr_ws_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, n;
  ಸಂರಚನಾ websocket *ws;
  bool eos;

  *pnread = 0;
  ಯದಿ(ctx->eos) {
    *peos = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ಯದಿ(!ws) {
    failf(data, "[WS] not a websocket transfer");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  ಯದಿ(Curl_bufq_is_empty(&ws->sendbuf)) {
    ಯದಿ(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }

    ಯದಿ(ws->enc.payload_remain) {
      CURL_TRC_WS(data, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ctx->read_eos = eos;

    ಯದಿ(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(data, reader->next);
    }
    ಅನ್ಯಥಾ ಯದಿ(!nread) {
      /* nothing to convert, return this right away */
      ಯದಿ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      ಗಚ್ಛ out;
    }

    ಯದಿ(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      result = ws_enc_write_head(data, ws, &ws->enc, CURLWS_ದ್ವಿಮಾನ, nread,
                                 &ws->sendbuf);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }

    result = ws_enc_write_payload(&ws->enc, data, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    CURL_TRC_READ(data, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  ಯದಿ(!result && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread, *peos);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ curl_off_t cr_ws_total_length(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_crtype ws_cr_encode = {
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
  ಪರಿಮಾಣ(ಸಂರಚನಾ cr_ws_ctx)
};

ಸಂರಚನಾ wsfield {
  ಧ್ರುವ ವರ್ಣ *name;
  ಧ್ರುವ ವರ್ಣ *val;
};

CURLcode Curl_ws_request(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ dynbuf *req)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ i;
  CURLcode result = CURLE_OK;
  uint8_t rand[16];
  ವರ್ಣ *randstr;
  size_t randlen;
  ವರ್ಣ keyval[40];
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  ಸಂರಚನಾ wsfield heads[] = {
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
      "Sec-WebSocket-Key", ರಿಕ್ತ,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  result = Curl_rand(data, rand, ಪರಿಮಾಣ(rand));
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  result = curlx_base64_encode(rand, ಪರಿಮಾಣ(rand), &randstr, &randlen);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  DEBUGASSERT(randlen < ಪರಿಮಾಣ(keyval));
  ಯದಿ(randlen >= ಪರಿಮಾಣ(keyval)) {
    curlx_free(randstr);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, ಪರಿಮಾಣ(keyval), randstr, randlen);
  curlx_free(randstr);
  ಪ್ರತಿ(i = 0; !result && (i < CURL_ARRAYSIZE(heads)); i++) {
    ಯದಿ(!Curl_checkheaders(data, heads[i].name, strlen(heads[i].name))) {
      result = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  data->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  data->conn->bits.upgrade_in_progress = TRUE;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ websocket *ws = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(ಸಂರಚನಾ Curl_easy *data,
                        ಧ್ರುವ ವರ್ಣ *mem, size_t nread)
{
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  ಸಂರಚನಾ websocket *ws;
  ಸಂರಚನಾ Curl_cwriter *ws_dec_writer = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_creader *ws_enc_reader = ರಿಕ್ತ;
  CURLcode result;

  DEBUGASSERT(data->conn);
  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ಯದಿ(!ws) {
    size_t chunk_size = WS_CHUNK_SIZE;
    ws = curlx_calloc(1, ಪರಿಮಾಣ(*ws));
    ಯದಿ(!ws)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
#ifdef DEBUGBUILD
    {
      ಧ್ರುವ ವರ್ಣ *p = getenv("CURL_WS_CHUNK_SIZE");
      ಯದಿ(p) {
        curl_off_t l;
        ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 1 * 1024 * 1024))
          chunk_size = (size_t)l;
      }
    }
#endif
    CURL_TRC_WS(data, "WS, using chunk size %zu", chunk_size);
    Curl_bufq_init2(&ws->recvbuf, chunk_size, WS_CHUNK_ಎಣಿಕೆ,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_size, WS_CHUNK_ಎಣಿಕೆ,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    result = Curl_conn_meta_set(data->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ {
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
  ಯದಿ(result)
    ಗಚ್ಛ out;
  result = Curl_cwriter_add(data, ws_dec_writer);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ws_dec_writer = ರಿಕ್ತ; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  ಯದಿ(data->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    ಯದಿ(nread) {
      result = Curl_cf_recvbuf_add(data, data->conn, FIRSTSOCKET,
                                   (ಧ್ರುವ uint8_t *)mem, nread);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
    CURL_REQ_CLEAR_RECV(data); /* read no more content */
  }
  ಅನ್ಯಥಾ { /* !connect_only */
    ಯದಿ(data->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(data, "UPLOAD set, add ws-encode reader");
      result = Curl_creader_set_fread(data, -1);
      ಯದಿ(result)
        ಗಚ್ಛ out;

      ಯದಿ(!data->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        result = Curl_creader_create(&ws_enc_reader, data, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        ಯದಿ(result)
          ಗಚ್ಛ out;
        result = Curl_creader_add(data, ws_enc_reader);
        ಯದಿ(result)
          ಗಚ್ಛ out;
        ws_enc_reader = ರಿಕ್ತ; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      data->req.eos_read = FALSE;
      data->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(data);
    }

    /* Then pass any additional data to the writers */
    ಯದಿ(nread) {
      result = Curl_client_write(data, CLIENTWRITE_BODY, mem, nread);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  ಯದಿ(ws_dec_writer)
    Curl_cwriter_free(data, ws_dec_writer);
  ಯದಿ(ws_enc_reader)
    Curl_creader_free(data, ws_enc_reader);
  ಯದಿ(result)
    CURL_TRC_WS(data, "Curl_ws_accept() failed -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
  ಅನ್ಯಥಾ
    CURL_TRC_WS(data, "websocket established, %s mode",
                data->set.connect_only ? "connect-only" : "callback");
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸಂರಚನಾ ws_collect {
  ಸಂರಚನಾ Curl_easy *data;
  ಸಂರಚನಾ websocket *ws;
  uint8_t *buffer;
  size_t buflen;
  size_t bufidx;
  ಪೂರ್ಣಾಙ್ಕ frame_age;
  ಪೂರ್ಣಾಙ್ಕ frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

ಸ್ಥಿರ CURLcode ws_client_collect(ಧ್ರುವ uint8_t *buf, size_t buflen,
                                  ಪೂರ್ಣಾಙ್ಕ frame_age, ಪೂರ್ಣಾಙ್ಕ frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  ಶೂನ್ಯ *userp,
                                  size_t *pnwritten)
{
  ಸಂರಚನಾ ws_collect *ctx = userp;
  ಸಂರಚನಾ Curl_easy *data = ctx->data;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  ಯದಿ(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ಯದಿ(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  ಯದಿ(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(ctx->data, ctx->ws, buf, buflen, CURLWS_PONG);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    *pnwritten = buflen;
  }
  ಅನ್ಯಥಾ {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    ಯದಿ(!write_len) {
      ಯದಿ(!buflen)  /* 0 length write, we accept that */
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;  /* no more space */
    }
    memcpy(ctx->buffer + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode nw_in_recv(ಶೂನ್ಯ *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  ಸಂರಚನಾ Curl_easy *data = reader_ctx;
  ಪ್ರತ್ಯಾಗಮ Curl_easy_recv(data, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, ಶೂನ್ಯ *buffer,
                      size_t buflen, size_t *recv,
                      ಧ್ರುವ ಸಂರಚನಾ curl_ws_frame **metap)
{
  ಸಂರಚನಾ Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  ಯದಿ(CURL_EAPI_ENTER(&guard, curl, ws_recv, &result)) {
    ಸಂರಚನಾ Curl_easy *data = curl;
    ಸಂರಚನಾ connectdata *conn;
    ಸಂರಚನಾ websocket *ws;
    ಸಂರಚನಾ ws_collect ctx;

    ಯದಿ(!recv || !metap || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ಗಚ್ಛ out;
    }

    *recv = 0;
    *metap = ರಿಕ್ತ;

    conn = data->conn;
    ಯದಿ(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      ಯದಿ(!data->set.connect_only) {
        failf(data, "[WS] CONNECT_ONLY is required");
        result = CURLE_UNSUPPORTED_PROTOCOL;
        ಗಚ್ಛ out;
      }

      Curl_getconnectinfo(data, &conn);
      ಯದಿ(!conn) {
        failf(data, "[WS] connection not found");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ಗಚ್ಛ out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    ಯದಿ(!ws) {
      failf(data, "[WS] connection is not setup for websocket");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ಗಚ್ಛ out;
    }

    memset(&ctx, 0, ಪರಿಮಾಣ(ctx));
    ctx.data = data;
    ctx.ws = ws;
    ctx.buffer = buffer;
    ctx.buflen = buflen;

    ಯಾವತ್(1) {
      /* receive more when our buffer is empty */
      ಯದಿ(Curl_bufq_is_empty(&ws->recvbuf)) {
        size_t n;
        result = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, data, &n);
        ಯದಿ(result)
          ಗಚ್ಛ out;
        ಅನ್ಯಥಾ ಯದಿ(n == 0) {
          /* connection closed */
          infof(data, "[WS] connection expectedly closed?");
          result = CURLE_GOT_NOTHING;
          ಗಚ್ಛ out;
        }
        CURL_TRC_WS(data, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      result = ws_dec_pass(&ws->dec, data, &ws->recvbuf,
                           ws_client_collect, &ctx);
      ಯದಿ(result == CURLE_AGAIN) {
        ಯದಿ(!ctx.written) {
          ws_dec_info(&ws->dec, data, "need more input");
          ಅನುವರ್ತ;  /* nothing written, try more input */
        }
        ಭಙ್ಗ;
      }
      ಅನ್ಯಥಾ ಯದಿ(result) {
        ಗಚ್ಛ out;
      }
      ಅನ್ಯಥಾ ಯದಿ(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        ಭಙ್ಗ;
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
    ಯದಿ(!data->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(data, ws);
      ಯದಿ(!r2)
        (ಶೂನ್ಯ)ws_flush(data, ws, Curl_api_is_in_callback(data));
    }
    result = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ws_flush(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ websocket *ws,
                         bool blocking)
{
  CURLcode result;

  /* If there is space, add any pending control frame */
  ಯದಿ(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_size) {
    result = ws_enc_add_pending(data, ws);
    ಯದಿ(result && (result != CURLE_AGAIN))
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(!Curl_bufq_is_empty(&ws->sendbuf)) {
    ಧ್ರುವ uint8_t *out;
    size_t outlen, n;
#ifdef DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_next = FALSE;
    size_t chunk_egain = 0;
    ಧ್ರುವ ವರ್ಣ *p = getenv("CURL_WS_CHUNK_EAGAIN");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#endif

    ಯಾವತ್(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#ifdef DEBUGBUILD
      ಯದಿ(eagain_next)
        ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
      ಯದಿ(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_next = TRUE;
      }
#endif
      ಯದಿ(blocking) {
        result = ws_send_raw_blocking(data, ws, (ಧ್ರುವ ವರ್ಣ *)out, outlen);
        n = result ? 0 : outlen;
      }
      ಅನ್ಯಥಾ ಯದಿ(data->set.connect_only || Curl_api_is_in_callback(data))
        result = Curl_senddata(data, out, outlen, &n);
      ಅನ್ಯಥಾ {
        result = Curl_xfer_send(data, out, outlen, FALSE, &n);
        ಯದಿ(!result && !n && outlen)
          result = CURLE_AGAIN;
      }

      ಯದಿ(result == CURLE_AGAIN) {
        CURL_TRC_WS(data, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        ಪ್ರತ್ಯಾಗಮ result;
      }
      ಅನ್ಯಥಾ ಯದಿ(result) {
        failf(data, "[WS] flush, write error %d", (ಪೂರ್ಣಾಙ್ಕ)result);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      ಅನ್ಯಥಾ {
        CURL_TRC_WS(data, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ws_send_raw_blocking(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ websocket *ws,
                                     ಧ್ರುವ ವರ್ಣ *buffer, size_t buflen)
{
  CURLcode result = CURLE_OK;
  size_t nwritten;

  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ result;

  (ಶೂನ್ಯ)ws;
  ಯಾವತ್(buflen) {
    result = Curl_xfer_send(data, buffer, buflen, FALSE, &nwritten);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    DEBUGASSERT(nwritten <= buflen);
    buffer += nwritten;
    buflen -= nwritten;
    ಯದಿ(buflen) {
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      timediff_t ಎಡ_ms;
      ಪೂರ್ಣಾಙ್ಕ ev;

      CURL_TRC_WS(data, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      ಎಡ_ms = Curl_timeleft_ms(data);
      ಯದಿ(ಎಡ_ms < 0) {
        failf(data, "[WS] Timeout waiting for socket becoming writable");
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      ಯದಿ(sock == CURL_SOCKET_BAD)
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, ಎಡ_ms ? ಎಡ_ms : 500);
      ಯದಿ(ev < 0) {
        failf(data, "[WS] Error while waiting for socket becoming writable");
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ws_send_raw(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ಶೂನ್ಯ *buffer,
                            size_t buflen, size_t *pnwritten)
{
  ಸಂರಚನಾ websocket *ws;
  CURLcode result;

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ಯದಿ(!ws) {
    failf(data, "[WS] Not a websocket transfer");
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }
  ಯದಿ(!buflen)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(Curl_api_is_in_callback(data)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    result = ws_flush(data, ws, TRUE);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    result = ws_send_raw_blocking(data, ws, buffer, buflen);
    ಯದಿ(!result)
      *pnwritten = buflen;
  }
  ಅನ್ಯಥಾ {
    /* We need any pending data to be sent or EAGAIN this call. */
    result = ws_flush(data, ws, FALSE);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    result = Curl_senddata(data, buffer, buflen, pnwritten);
  }

  CURL_TRC_WS(data, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode curl_ws_send(CURL *curl, ಧ್ರುವ ಶೂನ್ಯ *buffer_arg,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  ಸಂರಚನಾ Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  ಯದಿ(CURL_EAPI_ENTER(&guard, curl, ws_send, &result)) {
    ಸಂರಚನಾ websocket *ws;
    ಧ್ರುವ uint8_t *buffer = buffer_arg;
    ಸಂರಚನಾ Curl_easy *data = curl;
    size_t ndummy;
    size_t *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, data->set.ws_raw_mode);

    *pnsent = 0;

    ಯದಿ(!buffer && buflen) {
      failf(data, "[WS] buffer is NULL when buflen is not");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ಗಚ್ಛ out;
    }

    ಯದಿ(!data->conn && data->set.connect_only) {
      result = Curl_connect_only_attach(data);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
    ಯದಿ(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ಯದಿ(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }

    ಯದಿ(data->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      result = ws_flush(data, ws, FALSE);
      ಯದಿ(result)
        ಗಚ್ಛ out;

      ಯದಿ(!buffer) {
        failf(data, "[WS] buffer is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ಗಚ್ಛ out;
      }
      ಯದಿ(!sent) {
        failf(data, "[WS] sent is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ಗಚ್ಛ out;
      }
      ಯದಿ(fragsize || flags) {
        failf(data, "[WS] fragsize and flags must be zero in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ಗಚ್ಛ out;
      }
      result = ws_send_raw(data, buffer, buflen, pnsent);
      ಗಚ್ಛ out;
    }

    /* Not RAW mode, we do the frame encoding */
    result = ws_enc_send(data, ws, buffer, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, data->set.ws_raw_mode, (ಪೂರ್ಣಾಙ್ಕ)result,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ws_setup_conn(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  data->state.http_neg.accept_09 = FALSE;
  data->state.http_neg.only_10 = FALSE;
  data->state.http_neg.wanted = CURL_HTTP_V1x;
  data->state.http_neg.allowed = CURL_HTTP_V1x;
  ಪ್ರತ್ಯಾಗಮ Curl_http_setup_conn(data, conn);
}

ಧ್ರುವ ಸಂರಚನಾ curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  ಸಂರಚನಾ Curl_easy *data = curl;
  ಯದಿ(GOOD_EASY_HANDLE(data) && Curl_api_is_in_callback(data) &&
     data->conn && !data->set.ws_raw_mode) {
    ಸಂರಚನಾ websocket *ws;
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ಯದಿ(ws)
      ಪ್ರತ್ಯಾಗಮ &ws->recvframe;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                                         curl_off_t frame_len)
{
  ಸಂರಚನಾ Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  ಯದಿ(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &result)) {
    ಸಂರಚನಾ Curl_easy *data = curl;
    ಸಂರಚನಾ websocket *ws;

    ಯದಿ(data->set.ws_raw_mode) {
      failf(data, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      result = CURLE_FAILED_INIT;
      ಗಚ್ಛ out;
    }

    CURL_TRC_WS(data, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    ಯದಿ(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ಯದಿ(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }

    ಯದಿ(ws->enc.payload_remain) {
      failf(data, "[WS] previous frame not finished");
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }

    result = ws_enc_write_head(data, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    ಯದಿ(result)
      CURL_TRC_WS(data, "curl_start_frame(), error adding frame head %d",
                  (ಪೂರ್ಣಾಙ್ಕ)result);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_ws = {
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

#ಅನ್ಯಥಾ

CURLcode curl_ws_recv(CURL *curl, ಶೂನ್ಯ *buffer, size_t buflen,
                      size_t *recv,
                      ಧ್ರುವ ಸಂರಚನಾ curl_ws_frame **metap)
{
  (ಶೂನ್ಯ)curl;
  (ಶೂನ್ಯ)buffer;
  (ಶೂನ್ಯ)buflen;
  (ಶೂನ್ಯ)recv;
  (ಶೂನ್ಯ)metap;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, ಧ್ರುವ ಶೂನ್ಯ *buffer,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  (ಶೂನ್ಯ)curl;
  (ಶೂನ್ಯ)buffer;
  (ಶೂನ್ಯ)buflen;
  (ಶೂನ್ಯ)sent;
  (ಶೂನ್ಯ)fragsize;
  (ಶೂನ್ಯ)flags;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

ಧ್ರುವ ಸಂರಚನಾ curl_ws_frame *curl_ws_meta(CURL *data)
{
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                                         curl_off_t frame_len)
{
  (ಶೂನ್ಯ)curl;
  (ಶೂನ್ಯ)flags;
  (ಶೂನ್ಯ)frame_len;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
