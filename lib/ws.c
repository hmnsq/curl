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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS

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
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_FIN          0x80
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_RSV1         0x40
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_RSV2         0x20
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_RSV3         0x10
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_RSV_ಮುಖವಾಡ     (WSBIT_RSV1 | WSBIT_RSV2 | WSBIT_RSV3)
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_OPCODE_CONT  0x0
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_OPCODE_ಪಠ್ಯ  0x1
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_OPCODE_BIN   0x2
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_OPCODE_CLOSE 0x8
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_OPCODE_PING  0x9
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_OPCODE_PONG  0xa
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_OPCODE_ಮುಖವಾಡ  0xf
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು WSBIT_ಮುಖವಾಡ 0x80

/* buffer dimensioning */
#ವ್ಯಾಖ್ಯಾನಿಸು WS_CHUNK_ಗಾತ್ರ  65535
#ವ್ಯಾಖ್ಯಾನಿಸು WS_CHUNK_ಎಣಿಕೆ 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
ಎಣಿಕೆ ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_ತಲೆ,
  WS_DEC_PAYLOAD
};

ರಚನೆ ws_decoder {
  ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_age;        /* zero */
  ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_ಸ್ಥಾನಾಂತರ;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t ತಲೆ[10];
  ಪೂರ್ಣಾಂಕ ತಲೆ_len, ತಲೆ_ಒಟ್ಟು;
  ಎಣಿಕೆ ws_dec_state state;
  ಪೂರ್ಣಾಂಕ cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
ರಚನೆ ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ xori; /* xor index */
  uint8_t ಮುಖವಾಡ[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  ಬಿಟ್(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#ವ್ಯಾಖ್ಯಾನಿಸು WS_MAX_CNTRL_LEN    125

ರಚನೆ ws_cntrl_ಚೌಕಟ್ಟು {
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
ರಚನೆ websocket {
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ; /* used for write callback handling */
  ರಚನೆ ws_decoder dec;  /* decode of ws frames */
  ರಚನೆ ws_encoder enc;  /* encode of ws frames */
  ರಚನೆ bufq recvbuf;    /* raw data from the server */
  ರಚನೆ bufq sendbuf;    /* raw data to be sent to the server */
  ರಚನೆ curl_ws_ಚೌಕಟ್ಟು recvframe;  /* the current WS FRAME received */
  ರಚನೆ ws_cntrl_ಚೌಕಟ್ಟು pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  ಬಿಟ್(using_reader);      /* sending WS data is handled via a client reader */
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ws_ಚೌಕಟ್ಟು_name_ದ_op(uint8_t firstbyte)
{
  ಆಯ್ಕೆ(firstbyte & WSBIT_OPCODE_ಮುಖವಾಡ) {
  ಸ್ಥಿತಿ WSBIT_OPCODE_CONT:
    ಹಿಂತಿರುಗಿ "CONT";
  ಸ್ಥಿತಿ WSBIT_OPCODE_ಪಠ್ಯ:
    ಹಿಂತಿರುಗಿ "TEXT";
  ಸ್ಥಿತಿ WSBIT_OPCODE_BIN:
    ಹಿಂತಿರುಗಿ "BIN";
  ಸ್ಥಿತಿ WSBIT_OPCODE_CLOSE:
    ಹಿಂತಿರುಗಿ "CLOSE";
  ಸ್ಥಿತಿ WSBIT_OPCODE_PING:
    ಹಿಂತಿರುಗಿ "PING";
  ಸ್ಥಿತಿ WSBIT_OPCODE_PONG:
    ಹಿಂತಿರುಗಿ "PONG";
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ "???";
  }
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ws_ಚೌಕಟ್ಟು_firstbyte2flags(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    uint8_t firstbyte, ಪೂರ್ಣಾಂಕ cont_flags)
{
  ಆಯ್ಕೆ(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  ಸ್ಥಿತಿ WSBIT_OPCODE_CONT:
    ಆದರೆ(!(cont_flags & CURLWS_CONT)) {
      failf(ದತ್ತಾಂಶ, "[WS] no ongoing fragmented message to resume");
      ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  ಸ್ಥಿತಿ (WSBIT_OPCODE_CONT | WSBIT_FIN):
    ಆದರೆ(!(cont_flags & CURLWS_CONT)) {
      failf(ದತ್ತಾಂಶ, "[WS] no ongoing fragmented message to resume");
      ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  ಸ್ಥಿತಿ WSBIT_OPCODE_ಪಠ್ಯ:
    ಆದರೆ(cont_flags & CURLWS_CONT) {
      failf(ದತ್ತಾಂಶ, "[WS] fragmented message interrupted by new TEXT msg");
      ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ CURLWS_ಪಠ್ಯ | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  ಸ್ಥಿತಿ (WSBIT_OPCODE_ಪಠ್ಯ | WSBIT_FIN):
    ಆದರೆ(cont_flags & CURLWS_CONT) {
      failf(ದತ್ತಾಂಶ, "[WS] fragmented message interrupted by new TEXT msg");
      ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ CURLWS_ಪಠ್ಯ;
  /* 0x02 - first BINARY fragment */
  ಸ್ಥಿತಿ WSBIT_OPCODE_BIN:
    ಆದರೆ(cont_flags & CURLWS_CONT) {
      failf(ದತ್ತಾಂಶ, "[WS] fragmented message interrupted by new BINARY msg");
      ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ CURLWS_ದ್ವಿಮಾನ | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  ಸ್ಥಿತಿ (WSBIT_OPCODE_BIN | WSBIT_FIN):
    ಆದರೆ(cont_flags & CURLWS_CONT) {
      failf(ದತ್ತಾಂಶ, "[WS] fragmented message interrupted by new BINARY msg");
      ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ CURLWS_ದ್ವಿಮಾನ;
  /* 0x08 - first CLOSE fragment */
  ಸ್ಥಿತಿ WSBIT_OPCODE_CLOSE:
    failf(ದತ್ತಾಂಶ, "[WS] invalid fragmented CLOSE frame");
    ಹಿಂತಿರುಗಿ 0;
  /* 0x88 - unfragmented CLOSE */
  ಸ್ಥಿತಿ (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    ಹಿಂತಿರುಗಿ CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  ಸ್ಥಿತಿ WSBIT_OPCODE_PING:
    failf(ದತ್ತಾಂಶ, "[WS] invalid fragmented PING frame");
    ಹಿಂತಿರುಗಿ 0;
  /* 0x89 - unfragmented PING */
  ಸ್ಥಿತಿ (WSBIT_OPCODE_PING | WSBIT_FIN):
    ಹಿಂತಿರುಗಿ CURLWS_PING;
  /* 0x0a - first PONG fragment */
  ಸ್ಥಿತಿ WSBIT_OPCODE_PONG:
    failf(ದತ್ತಾಂಶ, "[WS] invalid fragmented PONG frame");
    ಹಿಂತಿರುಗಿ 0;
  /* 0x8a - unfragmented PONG */
  ಸ್ಥಿತಿ (WSBIT_OPCODE_PONG | WSBIT_FIN):
    ಹಿಂತಿರುಗಿ CURLWS_PONG;
  /* invalid first byte */
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ(firstbyte & WSBIT_RSV_ಮುಖವಾಡ)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(ದತ್ತಾಂಶ, "[WS] invalid reserved bits: %02x", firstbyte);
    ಇಲ್ಲದಿದ್ದರೆ
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(ದತ್ತಾಂಶ, "[WS] invalid opcode: %02x", firstbyte);
    ಹಿಂತಿರುಗಿ 0;
  }
}

ಸ್ಥಿರ CURLcode ws_ಚೌಕಟ್ಟು_flags2firstbyte(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  ಆಯ್ಕೆ(flags & ~CURLWS_ಸ್ಥಾನಾಂತರ) {
  ಸ್ಥಿತಿ 0:
    ಆದರೆ(contfragment) {
      CURL_TRC_WS(ದತ್ತಾಂಶ, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    failf(ದತ್ತಾಂಶ, "[WS] no flags given");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಸ್ಥಿತಿ CURLWS_CONT:
    ಆದರೆ(contfragment) {
      infof(ದತ್ತಾಂಶ, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    failf(ದತ್ತಾಂಶ, "[WS] No ongoing fragmented message to continue");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಸ್ಥಿತಿ CURLWS_ಪಠ್ಯ:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_ಪಠ್ಯ | WSBIT_FIN);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ (CURLWS_ಪಠ್ಯ | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_ಪಠ್ಯ;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURLWS_ದ್ವಿಮಾನ:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ (CURLWS_ದ್ವಿಮಾನ | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ (CURLWS_CLOSE | CURLWS_CONT):
    failf(ದತ್ತಾಂಶ, "[WS] CLOSE frame must not be fragmented");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಸ್ಥಿತಿ CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ (CURLWS_PING | CURLWS_CONT):
    failf(ದತ್ತಾಂಶ, "[WS] PING frame must not be fragmented");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಸ್ಥಿತಿ CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ (CURLWS_PONG | CURLWS_CONT):
    failf(ದತ್ತಾಂಶ, "[WS] PONG frame must not be fragmented");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "[WS] unknown flags: %x", flags);
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_dec_info(ರಚನೆ ws_decoder *dec, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ)
{
  NOVERBOSE((ಶೂನ್ಯ)ಸಂದೇಶ);
  ಆಯ್ಕೆ(dec->ತಲೆ_len) {
  ಸ್ಥಿತಿ 0:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 1:
    CURL_TRC_WS(ದತ್ತಾಂಶ, "decoded %s [%s%s]", ಸಂದೇಶ,
                ws_ಚೌಕಟ್ಟು_name_ದ_op(dec->ತಲೆ[0]),
                (dec->ತಲೆ[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ(dec->ತಲೆ_len < dec->ತಲೆ_ಒಟ್ಟು) {
      CURL_TRC_WS(ದತ್ತಾಂಶ, "decoded %s [%s%s](%d/%d)", ಸಂದೇಶ,
                  ws_ಚೌಕಟ್ಟು_name_ದ_op(dec->ತಲೆ[0]),
                  (dec->ತಲೆ[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->ತಲೆ_len, dec->ತಲೆ_ಒಟ್ಟು);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      CURL_TRC_WS(ದತ್ತಾಂಶ, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  ಸಂದೇಶ, ws_ಚೌಕಟ್ಟು_name_ದ_op(dec->ತಲೆ[0]),
                  (dec->ತಲೆ[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_ಸ್ಥಾನಾಂತರ, dec->payload_len);
    }
    ನಿಲ್ಲಿಸು;
  }
}

ಸ್ಥಿರ CURLcode ws_send_raw_blocking(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ websocket *ws,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್, size_t buflen);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode ws_write_payload(ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                                  ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_age, ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_flags,
                                  curl_off_t payload_ಸ್ಥಾನಾಂತರ,
                                  curl_off_t payload_len,
                                  ಶೂನ್ಯ *userp,
                                  size_t *pnwritten);

ಸ್ಥಿರ ಶೂನ್ಯ ws_dec_ಮುಂದಿನ_ಚೌಕಟ್ಟು(ರಚನೆ ws_decoder *dec)
{
  dec->ಚೌಕಟ್ಟು_age = 0;
  dec->ಚೌಕಟ್ಟು_flags = 0;
  dec->payload_ಸ್ಥಾನಾಂತರ = 0;
  dec->payload_len = 0;
  dec->ತಲೆ_len = dec->ತಲೆ_ಒಟ್ಟು = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_dec_reset(ರಚನೆ ws_decoder *dec)
{
  dec->ಚೌಕಟ್ಟು_age = 0;
  dec->ಚೌಕಟ್ಟು_flags = 0;
  dec->payload_ಸ್ಥಾನಾಂತರ = 0;
  dec->payload_len = 0;
  dec->ತಲೆ_len = dec->ತಲೆ_ಒಟ್ಟು = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_dec_init(ರಚನೆ ws_decoder *dec)
{
  ws_dec_reset(dec);
}

ಸ್ಥಿರ CURLcode ws_dec_read_ತಲೆ(ರಚನೆ ws_decoder *dec,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ bufq *inraw)
{
  ಸ್ಥಿರಾಂಕ uint8_t *inbuf;
  size_t inlen;

  ಆಗ(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    ಆದರೆ(dec->ತಲೆ_len == 0) {
      dec->ತಲೆ[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->ಚೌಕಟ್ಟು_flags = ws_ಚೌಕಟ್ಟು_firstbyte2flags(ದತ್ತಾಂಶ, dec->ತಲೆ[0],
                                                  dec->cont_flags);
      ಆದರೆ(!dec->ಚೌಕಟ್ಟು_flags) {
        ws_dec_reset(dec);
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      ಆದರೆ(dec->ಚೌಕಟ್ಟು_flags & (CURLWS_ಪಠ್ಯ | CURLWS_ದ್ವಿಮಾನ)) {
        dec->cont_flags = dec->ಚೌಕಟ್ಟು_flags;
      }

      dec->ತಲೆ_len = 1;
#ಆದರೆ 0
      ws_dec_info(dec, ದತ್ತಾಂಶ, "seeing opcode");
#ಆದರೆ_ಅಂತ್ಯ
      ಮುಂದುವರಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dec->ತಲೆ_len == 1) {
      dec->ತಲೆ[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->ತಲೆ_len = 2;

      ಆದರೆ(dec->ತಲೆ[1] & WSBIT_ಮುಖವಾಡ) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(ದತ್ತಾಂಶ, "[WS] masked input frame");
        ws_dec_reset(dec);
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
      ಆದರೆ(dec->ಚೌಕಟ್ಟು_flags & CURLWS_PING && dec->ತಲೆ[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(ದತ್ತಾಂಶ, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
      ಆದರೆ(dec->ಚೌಕಟ್ಟು_flags & CURLWS_PONG && dec->ತಲೆ[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(ದತ್ತಾಂಶ, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
      ಆದರೆ(dec->ಚೌಕಟ್ಟು_flags & CURLWS_CLOSE && dec->ತಲೆ[1] > WS_MAX_CNTRL_LEN) {
        failf(ದತ್ತಾಂಶ, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      ಆದರೆ(dec->ತಲೆ[1] == 126) {
        dec->ತಲೆ_ಒಟ್ಟು = 4;
        ಮುಂದುವರಿ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dec->ತಲೆ[1] == 127) {
        dec->ತಲೆ_ಒಟ್ಟು = 10;
        ಮುಂದುವರಿ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        dec->ತಲೆ_ಒಟ್ಟು = 2;
      }
    }

    ಆದರೆ(dec->ತಲೆ_len < dec->ತಲೆ_ಒಟ್ಟು) {
      dec->ತಲೆ[dec->ತಲೆ_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->ತಲೆ_len;
      ಆದರೆ(dec->ತಲೆ_len < dec->ತಲೆ_ಒಟ್ಟು) {
#ಆದರೆ 0
        ws_dec_info(dec, ದತ್ತಾಂಶ, "decoding head");
#ಆದರೆ_ಅಂತ್ಯ
        ಮುಂದುವರಿ;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->ತಲೆ_len == dec->ತಲೆ_ಒಟ್ಟು);
    ಆಯ್ಕೆ(dec->ತಲೆ_ಒಟ್ಟು) {
    ಸ್ಥಿತಿ 2:
      dec->payload_len = dec->ತಲೆ[1];
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 4:
      dec->payload_len = (dec->ತಲೆ[2] << 8) | dec->ತಲೆ[3];
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 10:
      ಆದರೆ(dec->ತಲೆ[2] > 127) {
        failf(ದತ್ತಾಂಶ, "[WS] frame length longer than 63 bits not supported");
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
      dec->payload_len =
        (curl_off_t)dec->ತಲೆ[2] << 56 |
        (curl_off_t)dec->ತಲೆ[3] << 48 |
        (curl_off_t)dec->ತಲೆ[4] << 40 |
        (curl_off_t)dec->ತಲೆ[5] << 32 |
        (curl_off_t)dec->ತಲೆ[6] << 24 |
        (curl_off_t)dec->ತಲೆ[7] << 16 |
        (curl_off_t)dec->ತಲೆ[8] <<  8 |
        dec->ತಲೆ[9];
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(ದತ್ತಾಂಶ, "[WS] unexpected frame header length");
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    }

    dec->ಚೌಕಟ್ಟು_age = 0;
    dec->payload_ಸ್ಥಾನಾಂತರ = 0;
    ws_dec_info(dec, ದತ್ತಾಂಶ, "head");
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಹಿಂತಿರುಗಿ CURLE_AGAIN;
}

ಸ್ಥಿರ CURLcode ws_dec_pass_payload(ರಚನೆ ws_decoder *dec,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ bufq *inraw,
                                    ws_write_payload *write_cb,
                                    ಶೂನ್ಯ *write_ctx)
{
  ಸ್ಥಿರಾಂಕ uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_ಸ್ಥಾನಾಂತರ,
                                     0, SIZE_MAX);

  ಆಗ(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(ದತ್ತಾಂಶ)) {
    ಆದರೆ(inlen > remain)
      inlen = remain;
    ಫಲಿತಾಂಶ = write_cb(inbuf, inlen, dec->ಚೌಕಟ್ಟು_age, dec->ಚೌಕಟ್ಟು_flags,
                      dec->payload_ಸ್ಥಾನಾಂತರ, dec->payload_len,
                      write_ctx, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_ಸ್ಥಾನಾಂತರ += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_ಸ್ಥಾನಾಂತರ,
                                0, SIZE_MAX);
    CURL_TRC_WS(ದತ್ತಾಂಶ, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  ಹಿಂತಿರುಗಿ remain ? CURLE_AGAIN : CURLE_OK;
}

ಸ್ಥಿರ CURLcode ws_dec_pass(ರಚನೆ ws_decoder *dec,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ bufq *inraw,
                            ws_write_payload *write_cb,
                            ಶೂನ್ಯ *write_ctx)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(inraw))
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆಯ್ಕೆ(dec->state) {
  ಸ್ಥಿತಿ WS_DEC_INIT:
    ws_dec_ಮುಂದಿನ_ಚೌಕಟ್ಟು(dec);
    dec->state = WS_DEC_ತಲೆ;
    FALLTHROUGH();
  ಸ್ಥಿತಿ WS_DEC_ತಲೆ:
    ಫಲಿತಾಂಶ = ws_dec_read_ತಲೆ(dec, ದತ್ತಾಂಶ, inraw);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN) {
        failf(ದತ್ತಾಂಶ, "[WS] decode frame error %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ನಿಲ್ಲಿಸು;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_ಖಾಲಿಗಳು(inraw));
      ನಿಲ್ಲಿಸು;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    ಆದರೆ(dec->payload_len == 0) {
      size_t nwritten;
      ಸ್ಥಿರಾಂಕ uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      ಫಲಿತಾಂಶ = write_cb(&tmp, 0, dec->ಚೌಕಟ್ಟು_age, dec->ಚೌಕಟ್ಟು_flags,
                        0, 0, write_ctx, &nwritten);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      dec->state = WS_DEC_INIT;
      ನಿಲ್ಲಿಸು;
    }
    FALLTHROUGH();
  ಸ್ಥಿತಿ WS_DEC_PAYLOAD:
    ಫಲಿತಾಂಶ = ws_dec_pass_payload(dec, ದತ್ತಾಂಶ, inraw, write_cb, write_ctx);
    ws_dec_info(dec, ದತ್ತಾಂಶ, "passing");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* we covered all enums above, but some code analyzers are wimps */
    ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ update_meta(ರಚನೆ websocket *ws,
                        ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_age, ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_flags,
                        curl_off_t payload_ಸ್ಥಾನಾಂತರ,
                        curl_off_t payload_len,
                        size_t cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_ಸ್ಥಾನಾಂತರ - cur_len);

  ws->recvframe.age = ಚೌಕಟ್ಟು_age;
  ws->recvframe.flags = ಚೌಕಟ್ಟು_flags;
  ws->recvframe.ಸ್ಥಾನಾಂತರ = payload_ಸ್ಥಾನಾಂತರ;
  ws->recvframe.len = cur_len;
  ws->recvframe.bytesleft = bytesleft;
}

/* WebSocket decoding client writer */
ರಚನೆ ws_cw_ctx {
  ರಚನೆ Curl_cwriter super;
  ರಚನೆ bufq buf;
};

ಸ್ಥಿರ CURLcode ws_cw_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ ws_cw_ctx *ctx = writer->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_ಗಾತ್ರ, 1, BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_cw_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ ws_cw_ctx *ctx = writer->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  Curl_bufq_free(&ctx->buf);
}

ರಚನೆ ws_cw_dec_ctx {
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ರಚನೆ websocket *ws;
  ರಚನೆ Curl_cwriter *ಮುಂದಿನ_writer;
  ಪೂರ್ಣಾಂಕ cw_type;
};

ಸ್ಥಿರ CURLcode ws_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ websocket *ws,
                         bool blocking);
ಸ್ಥಿರ CURLcode ws_enc_ಕೂಡಿಸು_pending(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ websocket *ws);

ಸ್ಥಿರ CURLcode ws_enc_ಕೂಡಿಸು_cntrl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ websocket *ws,
                                 ಸ್ಥಿರಾಂಕ uint8_t *payload,
                                 size_t plen,
                                 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_type)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  ಆದರೆ(plen > WS_MAX_CNTRL_LEN)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = ಚೌಕಟ್ಟು_type;
  ws->pending.payload_len = plen;
  ಸ್ಮರಣೆ_ನಕಲು(ws->pending.payload, payload, plen);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ curl_off_t ws_payload_remain(curl_off_t payload_ಒಟ್ಟು,
                                    curl_off_t payload_ಸ್ಥಾನಾಂತರ,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_ಒಟ್ಟು - payload_ಸ್ಥಾನಾಂತರ;
  ಆದರೆ((payload_ಒಟ್ಟು < 0) || (payload_ಸ್ಥಾನಾಂತರ < 0) || (remain < 0))
    ಹಿಂತಿರುಗಿ -1;
  buffered = curlx_uztoso(payload_buffered);
  ಆದರೆ(remain < buffered)
    ಹಿಂತಿರುಗಿ -1;
  ಹಿಂತಿರುಗಿ remain - buffered;
}

ಸ್ಥಿರ CURLcode ws_cw_dec_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                               ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_age, ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_flags,
                               curl_off_t payload_ಸ್ಥಾನಾಂತರ,
                               curl_off_t payload_len,
                               ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                               size_t *pnwritten)
{
  ರಚನೆ ws_cw_dec_ctx *ctx = user_ದತ್ತಾಂಶ;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = ctx->ದತ್ತಾಂಶ;
  ರಚನೆ websocket *ws = ctx->ws;
  bool auto_pong = !ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ && !ದತ್ತಾಂಶ->ಗಣ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ಚೌಕಟ್ಟು_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_ಸ್ಥಾನಾಂತರ, buflen);
  ಆದರೆ(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  ಆದರೆ(auto_pong && (ಚೌಕಟ್ಟು_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(ದತ್ತಾಂಶ, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_ಸ್ಥಾನಾಂತರ, payload_len);
    /* send back the exact same content as a PONG */
    ಫಲಿತಾಂಶ = ws_enc_ಕೂಡಿಸು_cntrl(ದತ್ತಾಂಶ, ws, buf, buflen, CURLWS_PONG);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, ಚೌಕಟ್ಟು_age, ಚೌಕಟ್ಟು_flags, payload_ಸ್ಥಾನಾಂತರ,
                payload_len, buflen);

    CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[WS] pass %zu decoded bytes", buflen);
    ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, ctx->ಮುಂದಿನ_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf, buflen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  *pnwritten = buflen;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ws_cw_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t nbytes)
{
  ರಚನೆ ws_cw_ctx *ctx = writer->ctx;
  ರಚನೆ websocket *ws;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[WS] write(len=%zu, type=%d)", nbytes, type);
  ಆದರೆ(!(type & CLIENTWRITE_BODY) || ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ)
    ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, nbytes);

  ws = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_PROTO_WS_CONN);
  ಆದರೆ(!ws) {
    failf(ದತ್ತಾಂಶ, "[WS] not a websocket transfer");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಆದರೆ(nbytes) {
    size_t nwritten;
    ಫಲಿತಾಂಶ = Curl_bufq_write(&ctx->buf, (ಸ್ಥಿರಾಂಕ uint8_t *)buf,
                             nbytes, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      infof(ದತ್ತಾಂಶ, "[WS] error adding data to buffer %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  ಫಲಿತಾಂಶ = Curl_cwriter_flush(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆಗ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->buf) && !Curl_cwriter_is_paused(ದತ್ತಾಂಶ)) {
    ರಚನೆ ws_cw_dec_ctx pass_ctx;
    pass_ctx.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
    pass_ctx.ws = ws;
    pass_ctx.ಮುಂದಿನ_writer = writer->ಮುಂದಿನ;
    pass_ctx.cw_type = type;
    ಫಲಿತಾಂಶ = ws_dec_pass(&ws->dec, ದತ್ತಾಂಶ, &ctx->buf,
                         ws_cw_dec_ಮುಂದಿನ, &pass_ctx);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      ಫಲಿತಾಂಶ = CURLE_OK;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "[WS] decode payload error %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      Curl_bufq_reset(&ctx->buf);
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ && !ws->using_reader) {
    ಫಲಿತಾಂಶ = ws_flush(ದತ್ತಾಂಶ, ws, Curl_api_is_ಒಳ_callback(ದತ್ತಾಂಶ));
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ಫಲಿತಾಂಶ = CURLE_OK;
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(ದತ್ತಾಂಶ, "[WS] decode ended with incomplete frame");
    ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ws_cw_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_cwriter *writer)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[ws] flush");
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ) {
    ರಚನೆ ws_cw_ctx *ctx = writer->ctx;
    ರಚನೆ websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    ಫಲಿತಾಂಶ = Curl_cwriter_flush(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    ws = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_PROTO_WS_CONN);
    ಆದರೆ(!ws) {
      failf(ದತ್ತಾಂಶ, "[WS] not a websocket transfer");
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }

    ಆಗ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->buf) && !Curl_cwriter_is_paused(ದತ್ತಾಂಶ)) {
      ರಚನೆ ws_cw_dec_ctx pass_ctx;
      pass_ctx.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
      pass_ctx.ws = ws;
      pass_ctx.ಮುಂದಿನ_writer = writer->ಮುಂದಿನ;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      ಫಲಿತಾಂಶ = ws_dec_pass(&ws->dec, ದತ್ತಾಂಶ, &ctx->buf,
                           ws_cw_dec_ಮುಂದಿನ, &pass_ctx);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        ಫಲಿತಾಂಶ = CURLE_OK;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
        failf(ದತ್ತಾಂಶ, "[WS] decode payload error %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        Curl_bufq_reset(&ctx->buf);
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_cwriter_flush(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* WebSocket payload decoding client writer. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype ws_cw_decode = {
  "ws-decode",
  ಖಾಲಿ,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  ಗಾತ್ರ(ರಚನೆ ws_cw_ctx)
};

ಸ್ಥಿರ ಶೂನ್ಯ ws_enc_info(ರಚನೆ ws_encoder *enc, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ)
{
  NOVERBOSE((ಶೂನ್ಯ)enc);
  NOVERBOSE((ಶೂನ್ಯ)ಸಂದೇಶ);
  CURL_TRC_WS(ದತ್ತಾಂಶ, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              ಸಂದೇಶ, ws_ಚೌಕಟ್ಟು_name_ದ_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_enc_reset(ರಚನೆ ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_enc_init(ರಚನೆ ws_encoder *enc)
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

ಸ್ಥಿರ CURLcode ws_enc_ಕೂಡಿಸು_ಚೌಕಟ್ಟು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ws_encoder *enc,
                                 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                                 curl_off_t payload_len,
                                 ರಚನೆ bufq *ಹೊರಹೋಗು)
{
  uint8_t firstb = 0;
  uint8_t ತಲೆ[14];
  CURLcode ಫಲಿತಾಂಶ;
  size_t hlen, nwritten;

  ಆದರೆ(payload_len < 0) {
    failf(ದತ್ತಾಂಶ, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }

  ಆದರೆ(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(ದತ್ತಾಂಶ, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }

  ಫಲಿತಾಂಶ = ws_ಚೌಕಟ್ಟು_flags2firstbyte(ದತ್ತಾಂಶ, flags, (bool)enc->contfragment,
                                    &firstb);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  ಆದರೆ(flags & (CURLWS_ಪಠ್ಯ | CURLWS_ದ್ವಿಮಾನ)) {
    enc->contfragment = (curl_ಬಿಟ್)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  ಆದರೆ(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(ದತ್ತಾಂಶ, "[WS] given PING frame is too big");
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  }
  ಆದರೆ(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(ದತ್ತಾಂಶ, "[WS] given PONG frame is too big");
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  }
  ಆದರೆ(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(ದತ್ತಾಂಶ, "[WS] given CLOSE frame is too big");
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  }

  ತಲೆ[0] = enc->firstbyte = firstb;
  ಆದರೆ(payload_len > 65535) {
    ತಲೆ[1] = 127 | WSBIT_ಮುಖವಾಡ;
    ತಲೆ[2] = (uint8_t)((payload_len >> 56) & 0xff);
    ತಲೆ[3] = (uint8_t)((payload_len >> 48) & 0xff);
    ತಲೆ[4] = (uint8_t)((payload_len >> 40) & 0xff);
    ತಲೆ[5] = (uint8_t)((payload_len >> 32) & 0xff);
    ತಲೆ[6] = (uint8_t)((payload_len >> 24) & 0xff);
    ತಲೆ[7] = (uint8_t)((payload_len >> 16) & 0xff);
    ತಲೆ[8] = (uint8_t)((payload_len >> 8) & 0xff);
    ತಲೆ[9] = (uint8_t)(payload_len & 0xff);
    hlen = 10;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(payload_len >= 126) {
    ತಲೆ[1] = 126 | WSBIT_ಮುಖವಾಡ;
    ತಲೆ[2] = (uint8_t)((payload_len >> 8) & 0xff);
    ತಲೆ[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ತಲೆ[1] = (uint8_t)payload_len | WSBIT_ಮುಖವಾಡ;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, ದತ್ತಾಂಶ, "sending");

  /* 4 bytes random */

  ಫಲಿತಾಂಶ = Curl_rand(ದತ್ತಾಂಶ, (uint8_t *)&enc->ಮುಖವಾಡ, ಗಾತ್ರ(enc->ಮುಖವಾಡ));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->ಮುಖವಾಡ, 0, ಗಾತ್ರ(enc->ಮುಖವಾಡ));
#ಆದರೆ_ಅಂತ್ಯ

  /* add 4 bytes mask */
  ಸ್ಮರಣೆ_ನಕಲು(&ತಲೆ[hlen], &enc->ಮುಖವಾಡ, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  ಫಲಿತಾಂಶ = Curl_bufq_write(ಹೊರಹೋಗು, ತಲೆ, hlen, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಆದರೆ(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ws_enc_write_ತಲೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ websocket *ws,
                                  ರಚನೆ ws_encoder *enc,
                                  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                                  curl_off_t payload_len,
                                  ರಚನೆ bufq *ಹೊರಹೋಗು)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  ಆದರೆ(ws->pending.type) {
    CURLcode ಫಲಿತಾಂಶ = ws_enc_ಕೂಡಿಸು_pending(ದತ್ತಾಂಶ, ws);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ ws_enc_ಕೂಡಿಸು_ಚೌಕಟ್ಟು(ದತ್ತಾಂಶ, enc, flags, payload_len, ಹೊರಹೋಗು);
}

ಸ್ಥಿರ CURLcode ws_enc_write_payload(ರಚನೆ ws_encoder *enc,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                                     ರಚನೆ bufq *ಹೊರಹೋಗು, size_t *pnwritten)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t i, len, n, remain;

  *pnwritten = 0;
  ಆದರೆ(Curl_bufq_is_ತುಂಬಿದ(ಹೊರಹೋಗು))
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  ಆದರೆ(remain < len)
    len = remain;

  ಗಾಗಿ(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->ಮುಖವಾಡ[enc->xori];
    ಫಲಿತಾಂಶ = Curl_bufq_write(ಹೊರಹೋಗು, &c, 1, &n);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ((ಫಲಿತಾಂಶ != CURLE_AGAIN) || !i)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ನಿಲ್ಲಿಸು;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, ದತ್ತಾಂಶ, "buffered");
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ws_enc_ಕೂಡಿಸು_pending(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ websocket *ws)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t n;

  ಆದರೆ(!ws->pending.type) /* no pending frame here */
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಆದರೆ(ws->enc.payload_remain) /* in the middle of another frame */
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಫಲಿತಾಂಶ = ws_enc_ಕೂಡಿಸು_ಚೌಕಟ್ಟು(ದತ್ತಾಂಶ, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    CURL_TRC_WS(ದತ್ತಾಂಶ, "ws_enc_cntrl(), error adding head: %d",
                (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = ws_enc_write_payload(&ws->enc, ದತ್ತಾಂಶ, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    CURL_TRC_WS(ದತ್ತಾಂಶ, "ws_enc_cntrl(), error adding payload: %d",
                (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(ದತ್ತಾಂಶ, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, ಗಾತ್ರ(ws->pending));

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ws_enc_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ websocket *ws,
                            ಸ್ಥಿರಾಂಕ uint8_t *ಬಫರ್,
                            size_t buflen,
                            curl_off_t fragsize,
                            ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(!ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ);
  *pnsent = 0;

  ಆದರೆ(ws->enc.payload_remain || !Curl_bufq_is_ಖಾಲಿಗಳು(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    ಆದರೆ(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(ದತ್ತಾಂಶ, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
    ಆದರೆ((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(ದತ್ತಾಂಶ, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = ws_flush(ದತ್ತಾಂಶ, ws, Curl_api_is_ಒಳ_callback(ದತ್ತಾಂಶ));
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಫಲಿತಾಂಶ = ws_enc_write_ತಲೆ(ದತ್ತಾಂಶ, ws, &ws->enc, flags,
                               (flags & CURLWS_ಸ್ಥಾನಾಂತರ) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_WS(ದತ್ತಾಂಶ, "curl_ws_send(), error writing frame head %d",
                  (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  ಆಗ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    ಆದರೆ(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      ಫಲಿತಾಂಶ = ws_enc_write_payload(&ws->enc, ದತ್ತಾಂಶ,
                                    ಬಫರ್ + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN))
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      ಆದರೆ(!ws->sendbuf_payload) {
        ಹಿಂತಿರುಗಿ CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    ಫಲಿತಾಂಶ = ws_flush(ದತ್ತಾಂಶ, ws, Curl_api_is_ಒಳ_callback(ದತ್ತಾಂಶ));
    ಆದರೆ(!ಫಲಿತಾಂಶ && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      ಬಫರ್ += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      ಆದರೆ(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(ದತ್ತಾಂಶ, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        ಹಿಂತಿರುಗಿ CURLE_AGAIN;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;  /* real error sending the data */
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ರಚನೆ cr_ws_ctx {
  ರಚನೆ Curl_creader super;
  ಬಿಟ್(read_eos);  /* we read an EOS from the next reader */
  ಬಿಟ್(eos);       /* we have returned an EOS */
};

ಸ್ಥಿರ CURLcode cr_ws_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_creader *reader)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_ws_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_creader *reader)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
}

ಸ್ಥಿರ CURLcode cr_ws_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_creader *reader,
                           ಅಕ್ಷರ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ರಚನೆ cr_ws_ctx *ctx = reader->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nread, n;
  ರಚನೆ websocket *ws;
  bool eos;

  *pnread = 0;
  ಆದರೆ(ctx->eos) {
    *peos = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ws = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_PROTO_WS_CONN);
  ಆದರೆ(!ws) {
    failf(ದತ್ತಾಂಶ, "[WS] not a websocket transfer");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ws->sendbuf)) {
    ಆದರೆ(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }

    ಆದರೆ(ws->enc.payload_remain) {
      CURL_TRC_WS(ದತ್ತಾಂಶ, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    ಫಲಿತಾಂಶ = Curl_creader_read(ದತ್ತಾಂಶ, reader->ಮುಂದಿನ, buf, blen, &nread, &eos);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ctx->read_eos = eos;

    ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(ದತ್ತಾಂಶ, reader->ಮುಂದಿನ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!nread) {
      /* nothing to convert, return this right away */
      ಆದರೆ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(!ws->enc.payload_remain && Curl_bufq_is_ಖಾಲಿಗಳು(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      ಫಲಿತಾಂಶ = ws_enc_write_ತಲೆ(ದತ್ತಾಂಶ, ws, &ws->enc, CURLWS_ದ್ವಿಮಾನ, nread,
                                 &ws->sendbuf);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }

    ಫಲಿತಾಂಶ = ws_enc_write_payload(&ws->enc, ದತ್ತಾಂಶ, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_ಖಾಲಿಗಳು(&ws->sendbuf));
  *peos = FALSE;
  ಫಲಿತಾಂಶ = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  ಆದರೆ(!ಫಲಿತಾಂಶ && ctx->read_eos && Curl_bufq_is_ಖಾಲಿಗಳು(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

ಹೊರಹೋಗು:
  CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread, *peos);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ curl_off_t cr_ws_ಒಟ್ಟು_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_ಒಟ್ಟು_ಉದ್ದಳತೆ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ಮುಗಿದ,
  ಗಾತ್ರ(ರಚನೆ cr_ws_ctx)
};

ರಚನೆ wsfield {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val;
};

CURLcode Curl_ws_request(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ dynbuf *req)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  uint8_t rand[16];
  ಅಕ್ಷರ *randstr;
  size_t randlen;
  ಅಕ್ಷರ keyval[40];
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  ರಚನೆ wsfield heads[] = {
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
      "Sec-WebSocket-Key", ಖಾಲಿ,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  ಫಲಿತಾಂಶ = Curl_rand(ದತ್ತಾಂಶ, rand, ಗಾತ್ರ(rand));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಫಲಿತಾಂಶ = curlx_base64_encode(rand, ಗಾತ್ರ(rand), &randstr, &randlen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  DEBUGASSERT(randlen < ಗಾತ್ರ(keyval));
  ಆದರೆ(randlen >= ಗಾತ್ರ(keyval)) {
    curlx_free(randstr);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, ಗಾತ್ರ(keyval), randstr, randlen);
  curlx_free(randstr);
  ಗಾಗಿ(i = 0; !ಫಲಿತಾಂಶ && (i < CURL_ARRAYSIZE(heads)); i++) {
    ಆದರೆ(!Curl_checkheaders(ದತ್ತಾಂಶ, heads[i].name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(heads[i].name))) {
      ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  ದತ್ತಾಂಶ->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.upgrade_ಒಳ_ಪ್ರಗತಿ = TRUE;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ws_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ websocket *ws = ನಮೂದು;
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
CURLcode Curl_ws_accept(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mem, size_t nread)
{
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  ರಚನೆ websocket *ws;
  ರಚನೆ Curl_cwriter *ws_dec_writer = ಖಾಲಿ;
  ರಚನೆ Curl_creader *ws_enc_reader = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  ws = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_PROTO_WS_CONN);
  ಆದರೆ(!ws) {
    size_t chunk_ಗಾತ್ರ = WS_CHUNK_ಗಾತ್ರ;
    ws = curlx_calloc(1, ಗಾತ್ರ(*ws));
    ಆದರೆ(!ws)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_WS_CHUNK_SIZE");
      ಆದರೆ(p) {
        curl_off_t l;
        ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 1 * 1024 * 1024))
          chunk_ಗಾತ್ರ = (size_t)l;
      }
    }
#ಆದರೆ_ಅಂತ್ಯ
    CURL_TRC_WS(ದತ್ತಾಂಶ, "WS, using chunk size %zu", chunk_ಗಾತ್ರ);
    Curl_bufq_init2(&ws->recvbuf, chunk_ಗಾತ್ರ, WS_CHUNK_ಎಣಿಕೆ,
                    BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
    Curl_bufq_init2(&ws->sendbuf, chunk_ಗಾತ್ರ, WS_CHUNK_ಎಣಿಕೆ,
                    BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    ಫಲಿತಾಂಶ = Curl_conn_meta_ಗಣ(ದತ್ತಾಂಶ->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
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

  infof(ದತ್ತಾಂಶ, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  ಫಲಿತಾಂಶ = Curl_cwriter_create(&ws_dec_writer, ದತ್ತಾಂಶ, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಫಲಿತಾಂಶ = Curl_cwriter_ಕೂಡಿಸು(ದತ್ತಾಂಶ, ws_dec_writer);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ws_dec_writer = ಖಾಲಿ; /* owned by transfer now */

  k->ಶೀರ್ಷಿಕೆ = FALSE; /* we will not get more response headers */

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    ಆದರೆ(nread) {
      ಫಲಿತಾಂಶ = Curl_cf_recvbuf_ಕೂಡಿಸು(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, FIRSTSOCKET,
                                   (ಸ್ಥಿರಾಂಕ uint8_t *)mem, nread);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
    CURL_REQ_CLEAR_RECV(ದತ್ತಾಂಶ); /* read no more content */
  }
  ಇಲ್ಲದಿದ್ದರೆ { /* !connect_only */
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ವಿಧಾನ == HTTPREQ_PUT) {
      CURL_TRC_WS(ದತ್ತಾಂಶ, "UPLOAD set, add ws-encode reader");
      ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_fread(ದತ್ತಾಂಶ, -1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;

      ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ) {
        /* Add our client reader encoding WS BINARY frames */
        ಫಲಿತಾಂಶ = Curl_creader_create(&ws_enc_reader, ದತ್ತಾಂಶ, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
        ಫಲಿತಾಂಶ = Curl_creader_ಕೂಡಿಸು(ದತ್ತಾಂಶ, ws_enc_reader);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
        ws_enc_reader = ಖಾಲಿ; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      ದತ್ತಾಂಶ->req.eos_read = FALSE;
      ದತ್ತಾಂಶ->req.upload_ಮುಗಿದ = FALSE;
      CURL_REQ_ಗಣ_SEND(ದತ್ತಾಂಶ);
    }

    /* Then pass any additional data to the writers */
    ಆದರೆ(nread) {
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, mem, nread);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->ಶೀರ್ಷಿಕೆ = FALSE; /* we will not get more responses */

ಹೊರಹೋಗು:
  ಆದರೆ(ws_dec_writer)
    Curl_cwriter_free(ದತ್ತಾಂಶ, ws_dec_writer);
  ಆದರೆ(ws_enc_reader)
    Curl_creader_free(ದತ್ತಾಂಶ, ws_enc_reader);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    CURL_TRC_WS(ದತ್ತಾಂಶ, "Curl_ws_accept() failed -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಇಲ್ಲದಿದ್ದರೆ
    CURL_TRC_WS(ದತ್ತಾಂಶ, "websocket established, %s mode",
                ದತ್ತಾಂಶ->ಗಣ.connect_only ? "connect-only" : "callback");
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ರಚನೆ ws_collect {
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ರಚನೆ websocket *ws;
  uint8_t *ಬಫರ್;
  size_t buflen;
  size_t bufidx;
  ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_age;
  ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_flags;
  curl_off_t payload_ಸ್ಥಾನಾಂತರ;
  curl_off_t payload_len;
  bool written;
};

ಸ್ಥಿರ CURLcode ws_client_collect(ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                                  ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_age, ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_flags,
                                  curl_off_t payload_ಸ್ಥಾನಾಂತರ,
                                  curl_off_t payload_len,
                                  ಶೂನ್ಯ *userp,
                                  size_t *pnwritten)
{
  ರಚನೆ ws_collect *ctx = userp;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = ctx->ದತ್ತಾಂಶ;
  bool auto_pong = !ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ && !ದತ್ತಾಂಶ->ಗಣ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_ಸ್ಥಾನಾಂತರ, buflen);
  ಆದರೆ(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  ಆದರೆ(!ctx->bufidx) {
    /* first write */
    ctx->ಚೌಕಟ್ಟು_age = ಚೌಕಟ್ಟು_age;
    ctx->ಚೌಕಟ್ಟು_flags = ಚೌಕಟ್ಟು_flags;
    ctx->payload_ಸ್ಥಾನಾಂತರ = payload_ಸ್ಥಾನಾಂತರ;
    ctx->payload_len = payload_len;
  }

  ಆದರೆ(auto_pong && (ಚೌಕಟ್ಟು_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(ದತ್ತಾಂಶ, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_ಸ್ಥಾನಾಂತರ, payload_len);
    /* send back the exact same content as a PONG */
    ಫಲಿತಾಂಶ = ws_enc_ಕೂಡಿಸು_cntrl(ctx->ದತ್ತಾಂಶ, ctx->ws, buf, buflen, CURLWS_PONG);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    *pnwritten = buflen;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    ಆದರೆ(!write_len) {
      ಆದರೆ(!buflen)  /* 0 length write, we accept that */
        ಹಿಂತಿರುಗಿ CURLE_OK;
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;  /* no more space */
    }
    ಸ್ಮರಣೆ_ನಕಲು(ctx->ಬಫರ್ + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode nw_ಒಳ_recv(ಶೂನ್ಯ *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = reader_ctx;
  ಹಿಂತಿರುಗಿ Curl_easy_recv(ದತ್ತಾಂಶ, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, ಶೂನ್ಯ *ಬಫರ್,
                      size_t buflen, size_t *recv,
                      ಸ್ಥಿರಾಂಕ ರಚನೆ curl_ws_ಚೌಕಟ್ಟು **metap)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, ws_recv, &ಫಲಿತಾಂಶ)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    ರಚನೆ connectdata *conn;
    ರಚನೆ websocket *ws;
    ರಚನೆ ws_collect ctx;

    ಆದರೆ(!recv || !metap || (buflen && !ಬಫರ್)) {
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    *recv = 0;
    *metap = ಖಾಲಿ;

    conn = ದತ್ತಾಂಶ->conn;
    ಆದರೆ(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.connect_only) {
        failf(ದತ್ತಾಂಶ, "[WS] CONNECT_ONLY is required");
        ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;
        ಹೋಗು ಹೊರಹೋಗು;
      }

      Curl_getconnectinfo(ದತ್ತಾಂಶ, &conn);
      ಆದರೆ(!conn) {
        failf(ದತ್ತಾಂಶ, "[WS] connection not found");
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    ಆದರೆ(!ws) {
      failf(ದತ್ತಾಂಶ, "[WS] connection is not setup for websocket");
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    memset(&ctx, 0, ಗಾತ್ರ(ctx));
    ctx.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
    ctx.ws = ws;
    ctx.ಬಫರ್ = ಬಫರ್;
    ctx.buflen = buflen;

    ಆಗ(1) {
      /* receive more when our buffer is empty */
      ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ws->recvbuf)) {
        size_t n;
        ಫಲಿತಾಂಶ = Curl_bufq_slurp(&ws->recvbuf, nw_ಒಳ_recv, ದತ್ತಾಂಶ, &n);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(n == 0) {
          /* connection closed */
          infof(ದತ್ತಾಂಶ, "[WS] connection expectedly closed?");
          ಫಲಿತಾಂಶ = CURLE_GOT_NOTHING;
          ಹೋಗು ಹೊರಹೋಗು;
        }
        CURL_TRC_WS(ದತ್ತಾಂಶ, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      ಫಲಿತಾಂಶ = ws_dec_pass(&ws->dec, ದತ್ತಾಂಶ, &ws->recvbuf,
                           ws_client_collect, &ctx);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        ಆದರೆ(!ctx.written) {
          ws_dec_info(&ws->dec, ದತ್ತಾಂಶ, "need more input");
          ಮುಂದುವರಿ;  /* nothing written, try more input */
        }
        ನಿಲ್ಲಿಸು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        ನಿಲ್ಲಿಸು;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.ಚೌಕಟ್ಟು_age, ctx.ಚೌಕಟ್ಟು_flags, ctx.payload_ಸ್ಥಾನಾಂತರ,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(ದತ್ತಾಂಶ, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.ಸ್ಥಾನಾಂತರ,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_ಕೂಡಿಸು_pending(ದತ್ತಾಂಶ, ws);
      ಆದರೆ(!r2)
        (ಶೂನ್ಯ)ws_flush(ದತ್ತಾಂಶ, ws, Curl_api_is_ಒಳ_callback(ದತ್ತಾಂಶ));
    }
    ಫಲಿತಾಂಶ = CURLE_OK;
  }
ಹೊರಹೋಗು:
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ws_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ websocket *ws,
                         bool blocking)
{
  CURLcode ಫಲಿತಾಂಶ;

  /* If there is space, add any pending control frame */
  ಆದರೆ(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_ಗಾತ್ರ) {
    ಫಲಿತಾಂಶ = ws_enc_ಕೂಡಿಸು_pending(ದತ್ತಾಂಶ, ws);
    ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN))
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ws->sendbuf)) {
    ಸ್ಥಿರಾಂಕ uint8_t *ಹೊರಹೋಗು;
    size_t outlen, n;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_ಮುಂದಿನ = FALSE;
    size_t chunk_egain = 0;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_WS_CHUNK_EAGAIN");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆಗ(Curl_bufq_peek(&ws->sendbuf, &ಹೊರಹೋಗು, &outlen)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
      ಆದರೆ(eagain_ಮುಂದಿನ)
        ಹಿಂತಿರುಗಿ CURLE_AGAIN;
      ಆದರೆ(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_ಮುಂದಿನ = TRUE;
      }
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(blocking) {
        ಫಲಿತಾಂಶ = ws_send_raw_blocking(ದತ್ತಾಂಶ, ws, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಹೊರಹೋಗು, outlen);
        n = ಫಲಿತಾಂಶ ? 0 : outlen;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.connect_only || Curl_api_is_ಒಳ_callback(ದತ್ತಾಂಶ))
        ಫಲಿತಾಂಶ = Curl_senddata(ದತ್ತಾಂಶ, ಹೊರಹೋಗು, outlen, &n);
      ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, ಹೊರಹೋಗು, outlen, FALSE, &n);
        ಆದರೆ(!ಫಲಿತಾಂಶ && !n && outlen)
          ಫಲಿತಾಂಶ = CURLE_AGAIN;
      }

      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        CURL_TRC_WS(ದತ್ತಾಂಶ, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
        failf(ದತ್ತಾಂಶ, "[WS] flush, write error %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        CURL_TRC_WS(ದತ್ತಾಂಶ, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ws_send_raw_blocking(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ websocket *ws,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್, size_t buflen)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nwritten;

  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ws;
  ಆಗ(buflen) {
    ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, ಬಫರ್, buflen, FALSE, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    DEBUGASSERT(nwritten <= buflen);
    ಬಫರ್ += nwritten;
    buflen -= nwritten;
    ಆದರೆ(buflen) {
      curl_socket_t sock = ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET];
      timediff_t ಎಡ_ms;
      ಪೂರ್ಣಾಂಕ ev;

      CURL_TRC_WS(ದತ್ತಾಂಶ, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      ಎಡ_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
      ಆದರೆ(ಎಡ_ms < 0) {
        failf(ದತ್ತಾಂಶ, "[WS] Timeout waiting for socket becoming writable");
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      ಆದರೆ(sock == CURL_SOCKET_BAD)
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, ಎಡ_ms ? ಎಡ_ms : 500);
      ಆದರೆ(ev < 0) {
        failf(ದತ್ತಾಂಶ, "[WS] Error while waiting for socket becoming writable");
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      }
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ws_send_raw(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಫರ್,
                            size_t buflen, size_t *pnwritten)
{
  ರಚನೆ websocket *ws;
  CURLcode ಫಲಿತಾಂಶ;

  ws = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_PROTO_WS_CONN);
  ಆದರೆ(!ws) {
    failf(ದತ್ತಾಂಶ, "[WS] Not a websocket transfer");
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }
  ಆದರೆ(!buflen)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(Curl_api_is_ಒಳ_callback(ದತ್ತಾಂಶ)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    ಫಲಿತಾಂಶ = ws_flush(ದತ್ತಾಂಶ, ws, TRUE);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಫಲಿತಾಂಶ = ws_send_raw_blocking(ದತ್ತಾಂಶ, ws, ಬಫರ್, buflen);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      *pnwritten = buflen;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* We need any pending data to be sent or EAGAIN this call. */
    ಫಲಿತಾಂಶ = ws_flush(ದತ್ತಾಂಶ, ws, FALSE);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಫಲಿತಾಂಶ = Curl_senddata(ದತ್ತಾಂಶ, ಬಫರ್, buflen, pnwritten);
  }

  CURL_TRC_WS(ದತ್ತಾಂಶ, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode curl_ws_send(CURL *curl, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಫರ್_arg,
                      size_t buflen, size_t *ಕಳುಹಿಸಿದ,
                      curl_off_t fragsize,
                      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, ws_send, &ಫಲಿತಾಂಶ)) {
    ರಚನೆ websocket *ws;
    ಸ್ಥಿರಾಂಕ uint8_t *ಬಫರ್ = ಬಫರ್_arg;
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    size_t ndummy;
    size_t *pnsent = ಕಳುಹಿಸಿದ ? ಕಳುಹಿಸಿದ : &ndummy;

    CURL_TRC_WS(ದತ್ತಾಂಶ, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ);

    *pnsent = 0;

    ಆದರೆ(!ಬಫರ್ && buflen) {
      failf(ದತ್ತಾಂಶ, "[WS] buffer is NULL when buflen is not");
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(!ದತ್ತಾಂಶ->conn && ದತ್ತಾಂಶ->ಗಣ.connect_only) {
      ಫಲಿತಾಂಶ = Curl_connect_only_attach(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ(!ದತ್ತಾಂಶ->conn) {
      failf(ದತ್ತಾಂಶ, "[WS] No associated connection");
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ws = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_PROTO_WS_CONN);
    ಆದರೆ(!ws) {
      failf(ದತ್ತಾಂಶ, "[WS] Not a websocket transfer");
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      ಫಲಿತಾಂಶ = ws_flush(ದತ್ತಾಂಶ, ws, FALSE);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;

      ಆದರೆ(!ಬಫರ್) {
        failf(ದತ್ತಾಂಶ, "[WS] buffer is NULL in raw mode");
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಆದರೆ(!ಕಳುಹಿಸಿದ) {
        failf(ದತ್ತಾಂಶ, "[WS] sent is NULL in raw mode");
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಆದರೆ(fragsize || flags) {
        failf(ದತ್ತಾಂಶ, "[WS] fragsize and flags must be zero in raw mode");
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಫಲಿತಾಂಶ = ws_send_raw(ದತ್ತಾಂಶ, ಬಫರ್, buflen, pnsent);
      ಹೋಗು ಹೊರಹೋಗು;
    }

    /* Not RAW mode, we do the frame encoding */
    ಫಲಿತಾಂಶ = ws_enc_send(ದತ್ತಾಂಶ, ws, ಬಫರ್, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(ದತ್ತಾಂಶ, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
                *pnsent);
  }
ಹೊರಹೋಗು:
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ws_ಸಿದ್ಧತೆ_conn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  ದತ್ತಾಂಶ->state.http_neg.accept_09 = FALSE;
  ದತ್ತಾಂಶ->state.http_neg.only_10 = FALSE;
  ದತ್ತಾಂಶ->state.http_neg.wanted = CURL_HTTP_V1x;
  ದತ್ತಾಂಶ->state.http_neg.allowed = CURL_HTTP_V1x;
  ಹಿಂತಿರುಗಿ Curl_http_ಸಿದ್ಧತೆ_conn(ದತ್ತಾಂಶ, conn);
}

ಸ್ಥಿರಾಂಕ ರಚನೆ curl_ws_ಚೌಕಟ್ಟು *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
  ಆದರೆ(GOOD_EASY_HANDLE(ದತ್ತಾಂಶ) && Curl_api_is_ಒಳ_callback(ದತ್ತಾಂಶ) &&
     ದತ್ತಾಂಶ->conn && !ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ) {
    ರಚನೆ websocket *ws;
    ws = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_PROTO_WS_CONN);
    ಆದರೆ(ws)
      ಹಿಂತಿರುಗಿ &ws->recvframe;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

CURL_EXTERN CURLcode curl_ws_ಆರಂಭ_ಚೌಕಟ್ಟು(CURL *curl,
                                         ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                                         curl_off_t ಚೌಕಟ್ಟು_len)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, ws_ಆರಂಭ_ಚೌಕಟ್ಟು, &ಫಲಿತಾಂಶ)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    ರಚನೆ websocket *ws;

    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ws_raw_ಬಹುಲಕ) {
      failf(ದತ್ತಾಂಶ, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    CURL_TRC_WS(ದತ್ತಾಂಶ, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, ಚೌಕಟ್ಟು_len);

    ಆದರೆ(!ದತ್ತಾಂಶ->conn) {
      failf(ದತ್ತಾಂಶ, "[WS] No associated connection");
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ws = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_PROTO_WS_CONN);
    ಆದರೆ(!ws) {
      failf(ದತ್ತಾಂಶ, "[WS] Not a websocket transfer");
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(ws->enc.payload_remain) {
      failf(ದತ್ತಾಂಶ, "[WS] previous frame not finished");
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಫಲಿತಾಂಶ = ws_enc_write_ತಲೆ(ದತ್ತಾಂಶ, ws, &ws->enc, flags, ಚೌಕಟ್ಟು_len,
                               &ws->sendbuf);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      CURL_TRC_WS(ದತ್ತಾಂಶ, "curl_start_frame(), error adding frame head %d",
                  (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  }
ಹೊರಹೋಗು:
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_ws = {
  ws_ಸಿದ್ಧತೆ_conn,                        /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_ಮುಗಿದ,                       /* done */
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

#ಇಲ್ಲದಿದ್ದರೆ

CURLcode curl_ws_recv(CURL *curl, ಶೂನ್ಯ *ಬಫರ್, size_t buflen,
                      size_t *recv,
                      ಸ್ಥಿರಾಂಕ ರಚನೆ curl_ws_ಚೌಕಟ್ಟು **metap)
{
  (ಶೂನ್ಯ)curl;
  (ಶೂನ್ಯ)ಬಫರ್;
  (ಶೂನ್ಯ)buflen;
  (ಶೂನ್ಯ)recv;
  (ಶೂನ್ಯ)metap;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

CURLcode curl_ws_send(CURL *curl, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಫರ್,
                      size_t buflen, size_t *ಕಳುಹಿಸಿದ,
                      curl_off_t fragsize,
                      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  (ಶೂನ್ಯ)curl;
  (ಶೂನ್ಯ)ಬಫರ್;
  (ಶೂನ್ಯ)buflen;
  (ಶೂನ್ಯ)ಕಳುಹಿಸಿದ;
  (ಶೂನ್ಯ)fragsize;
  (ಶೂನ್ಯ)flags;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ curl_ws_ಚೌಕಟ್ಟು *curl_ws_meta(CURL *ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

CURL_EXTERN CURLcode curl_ws_ಆರಂಭ_ಚೌಕಟ್ಟು(CURL *curl,
                                         ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                                         curl_off_t ಚೌಕಟ್ಟು_len)
{
  (ಶೂನ್ಯ)curl;
  (ಶೂನ್ಯ)flags;
  (ಶೂನ್ಯ)ಚೌಕಟ್ಟು_len;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_WEBSOCKETS */
