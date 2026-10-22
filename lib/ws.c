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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ws.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS

#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-recvbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "easyif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

/* RFC 6455 Section 5.2

    0 1 2 3 4 5 6 7
   +-+-+-+-+-------+
   |F|R|R|R| opcode|
   |I|S|S|S|  (4)  |
   |N|V|V|V|       |
   | |1|2|3|       |
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_FIN          0x80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_RSV1         0x40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_RSV2         0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_RSV3         0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_RSV_ਨਕਾਬ     (WSBIT_RSV1 | WSBIT_RSV2 | WSBIT_RSV3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_OPCODE_CONT  0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_OPCODE_ਪਾਠ  0x1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_OPCODE_BIN   0x2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_OPCODE_CLOSE 0x8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_OPCODE_PING  0x9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_OPCODE_PONG  0xa
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_OPCODE_ਨਕਾਬ  0xf
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSBIT_ਨਕਾਬ 0x80

/* buffer dimensioning */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WS_CHUNK_ਮਾਪ  65535
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WS_CHUNK_ਗਿਣਤੀ 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
ਗਿਣਤੀ ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_ਸਿਰ,
  WS_DEC_PAYLOAD
};

ਬਣਤਰ ws_decoder {
  ਪੂਰਨ ਚੌਖਟਾ_age;        /* zero */
  ਪੂਰਨ ਚੌਖਟਾ_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_ਵਿੱਥ;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t ਸਿਰ[10];
  ਪੂਰਨ ਸਿਰ_len, ਸਿਰ_ਕੁੱਲ;
  ਗਿਣਤੀ ws_dec_state state;
  ਪੂਰਨ cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
ਬਣਤਰ ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ xori; /* xor index */
  uint8_t ਨਕਾਬ[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  ਦੋਆਧਾਰੀ_ਅੰਕ(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WS_MAX_CNTRL_LEN    125

ਬਣਤਰ ws_cntrl_ਚੌਖਟਾ {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
ਬਣਤਰ websocket {
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ; /* used for write callback handling */
  ਬਣਤਰ ws_decoder dec;  /* decode of ws frames */
  ਬਣਤਰ ws_encoder enc;  /* encode of ws frames */
  ਬਣਤਰ bufq recvbuf;    /* raw data from the server */
  ਬਣਤਰ bufq sendbuf;    /* raw data to be sent to the server */
  ਬਣਤਰ curl_ws_ਚੌਖਟਾ recvframe;  /* the current WS FRAME received */
  ਬਣਤਰ ws_cntrl_ਚੌਖਟਾ pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  ਦੋਆਧਾਰੀ_ਅੰਕ(using_reader);      /* sending WS data is handled via a client reader */
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ws_ਚੌਖਟਾ_name_ਦਾ_op(uint8_t firstbyte)
{
  ਚੋਣ(firstbyte & WSBIT_OPCODE_ਨਕਾਬ) {
  ਹਾਲਤ WSBIT_OPCODE_CONT:
    ਵਾਪਸ "CONT";
  ਹਾਲਤ WSBIT_OPCODE_ਪਾਠ:
    ਵਾਪਸ "TEXT";
  ਹਾਲਤ WSBIT_OPCODE_BIN:
    ਵਾਪਸ "BIN";
  ਹਾਲਤ WSBIT_OPCODE_CLOSE:
    ਵਾਪਸ "CLOSE";
  ਹਾਲਤ WSBIT_OPCODE_PING:
    ਵਾਪਸ "PING";
  ਹਾਲਤ WSBIT_OPCODE_PONG:
    ਵਾਪਸ "PONG";
  ਮੂਲ:
    ਵਾਪਸ "???";
  }
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ ws_ਚੌਖਟਾ_firstbyte2flags(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    uint8_t firstbyte, ਪੂਰਨ cont_flags)
{
  ਚੋਣ(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  ਹਾਲਤ WSBIT_OPCODE_CONT:
    ਜੇ(!(cont_flags & CURLWS_CONT)) {
      failf(ਅੰਕੜੇ, "[WS] no ongoing fragmented message to resume");
      ਵਾਪਸ 0;
    }
    ਵਾਪਸ cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  ਹਾਲਤ (WSBIT_OPCODE_CONT | WSBIT_FIN):
    ਜੇ(!(cont_flags & CURLWS_CONT)) {
      failf(ਅੰਕੜੇ, "[WS] no ongoing fragmented message to resume");
      ਵਾਪਸ 0;
    }
    ਵਾਪਸ cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  ਹਾਲਤ WSBIT_OPCODE_ਪਾਠ:
    ਜੇ(cont_flags & CURLWS_CONT) {
      failf(ਅੰਕੜੇ, "[WS] fragmented message interrupted by new TEXT msg");
      ਵਾਪਸ 0;
    }
    ਵਾਪਸ CURLWS_ਪਾਠ | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  ਹਾਲਤ (WSBIT_OPCODE_ਪਾਠ | WSBIT_FIN):
    ਜੇ(cont_flags & CURLWS_CONT) {
      failf(ਅੰਕੜੇ, "[WS] fragmented message interrupted by new TEXT msg");
      ਵਾਪਸ 0;
    }
    ਵਾਪਸ CURLWS_ਪਾਠ;
  /* 0x02 - first BINARY fragment */
  ਹਾਲਤ WSBIT_OPCODE_BIN:
    ਜੇ(cont_flags & CURLWS_CONT) {
      failf(ਅੰਕੜੇ, "[WS] fragmented message interrupted by new BINARY msg");
      ਵਾਪਸ 0;
    }
    ਵਾਪਸ CURLWS_ਦੁਆਧਾਰੀ | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  ਹਾਲਤ (WSBIT_OPCODE_BIN | WSBIT_FIN):
    ਜੇ(cont_flags & CURLWS_CONT) {
      failf(ਅੰਕੜੇ, "[WS] fragmented message interrupted by new BINARY msg");
      ਵਾਪਸ 0;
    }
    ਵਾਪਸ CURLWS_ਦੁਆਧਾਰੀ;
  /* 0x08 - first CLOSE fragment */
  ਹਾਲਤ WSBIT_OPCODE_CLOSE:
    failf(ਅੰਕੜੇ, "[WS] invalid fragmented CLOSE frame");
    ਵਾਪਸ 0;
  /* 0x88 - unfragmented CLOSE */
  ਹਾਲਤ (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    ਵਾਪਸ CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  ਹਾਲਤ WSBIT_OPCODE_PING:
    failf(ਅੰਕੜੇ, "[WS] invalid fragmented PING frame");
    ਵਾਪਸ 0;
  /* 0x89 - unfragmented PING */
  ਹਾਲਤ (WSBIT_OPCODE_PING | WSBIT_FIN):
    ਵਾਪਸ CURLWS_PING;
  /* 0x0a - first PONG fragment */
  ਹਾਲਤ WSBIT_OPCODE_PONG:
    failf(ਅੰਕੜੇ, "[WS] invalid fragmented PONG frame");
    ਵਾਪਸ 0;
  /* 0x8a - unfragmented PONG */
  ਹਾਲਤ (WSBIT_OPCODE_PONG | WSBIT_FIN):
    ਵਾਪਸ CURLWS_PONG;
  /* invalid first byte */
  ਮੂਲ:
    ਜੇ(firstbyte & WSBIT_RSV_ਨਕਾਬ)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(ਅੰਕੜੇ, "[WS] invalid reserved bits: %02x", firstbyte);
    ਨਹੀਂਤਾਂ
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(ਅੰਕੜੇ, "[WS] invalid opcode: %02x", firstbyte);
    ਵਾਪਸ 0;
  }
}

ਸਥਿਰ CURLcode ws_ਚੌਖਟਾ_flags2firstbyte(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  ਚੋਣ(flags & ~CURLWS_ਵਿੱਥ) {
  ਹਾਲਤ 0:
    ਜੇ(contfragment) {
      CURL_TRC_WS(ਅੰਕੜੇ, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      ਵਾਪਸ CURLE_OK;
    }
    failf(ਅੰਕੜੇ, "[WS] no flags given");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਹਾਲਤ CURLWS_CONT:
    ਜੇ(contfragment) {
      infof(ਅੰਕੜੇ, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      ਵਾਪਸ CURLE_OK;
    }
    failf(ਅੰਕੜੇ, "[WS] No ongoing fragmented message to continue");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਹਾਲਤ CURLWS_ਪਾਠ:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_ਪਾਠ | WSBIT_FIN);
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ (CURLWS_ਪਾਠ | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_ਪਾਠ;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURLWS_ਦੁਆਧਾਰੀ:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ (CURLWS_ਦੁਆਧਾਰੀ | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ (CURLWS_CLOSE | CURLWS_CONT):
    failf(ਅੰਕੜੇ, "[WS] CLOSE frame must not be fragmented");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਹਾਲਤ CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ (CURLWS_PING | CURLWS_CONT):
    failf(ਅੰਕੜੇ, "[WS] PING frame must not be fragmented");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਹਾਲਤ CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ (CURLWS_PONG | CURLWS_CONT):
    failf(ਅੰਕੜੇ, "[WS] PONG frame must not be fragmented");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "[WS] unknown flags: %x", flags);
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
}

ਸਥਿਰ ਖਾਲੀ ws_dec_info(ਬਣਤਰ ws_decoder *dec, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ)
{
  NOVERBOSE((ਖਾਲੀ)ਸੁਨੇਹਾ);
  ਚੋਣ(dec->ਸਿਰ_len) {
  ਹਾਲਤ 0:
    ਰੋਕੋ;
  ਹਾਲਤ 1:
    CURL_TRC_WS(ਅੰਕੜੇ, "decoded %s [%s%s]", ਸੁਨੇਹਾ,
                ws_ਚੌਖਟਾ_name_ਦਾ_op(dec->ਸਿਰ[0]),
                (dec->ਸਿਰ[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    ਰੋਕੋ;
  ਮੂਲ:
    ਜੇ(dec->ਸਿਰ_len < dec->ਸਿਰ_ਕੁੱਲ) {
      CURL_TRC_WS(ਅੰਕੜੇ, "decoded %s [%s%s](%d/%d)", ਸੁਨੇਹਾ,
                  ws_ਚੌਖਟਾ_name_ਦਾ_op(dec->ਸਿਰ[0]),
                  (dec->ਸਿਰ[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->ਸਿਰ_len, dec->ਸਿਰ_ਕੁੱਲ);
    }
    ਨਹੀਂਤਾਂ {
      CURL_TRC_WS(ਅੰਕੜੇ, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  ਸੁਨੇਹਾ, ws_ਚੌਖਟਾ_name_ਦਾ_op(dec->ਸਿਰ[0]),
                  (dec->ਸਿਰ[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_ਵਿੱਥ, dec->payload_len);
    }
    ਰੋਕੋ;
  }
}

ਸਥਿਰ CURLcode ws_send_raw_blocking(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ websocket *ws,
                                     ਅਚਲ ਅੱਖਰ *ਬਫਰ, size_t buflen);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode ws_write_payload(ਅਚਲ uint8_t *buf, size_t buflen,
                                  ਪੂਰਨ ਚੌਖਟਾ_age, ਪੂਰਨ ਚੌਖਟਾ_flags,
                                  curl_off_t payload_ਵਿੱਥ,
                                  curl_off_t payload_len,
                                  ਖਾਲੀ *userp,
                                  size_t *pnwritten);

ਸਥਿਰ ਖਾਲੀ ws_dec_ਅਗਲਾ_ਚੌਖਟਾ(ਬਣਤਰ ws_decoder *dec)
{
  dec->ਚੌਖਟਾ_age = 0;
  dec->ਚੌਖਟਾ_flags = 0;
  dec->payload_ਵਿੱਥ = 0;
  dec->payload_len = 0;
  dec->ਸਿਰ_len = dec->ਸਿਰ_ਕੁੱਲ = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ਸਥਿਰ ਖਾਲੀ ws_dec_reset(ਬਣਤਰ ws_decoder *dec)
{
  dec->ਚੌਖਟਾ_age = 0;
  dec->ਚੌਖਟਾ_flags = 0;
  dec->payload_ਵਿੱਥ = 0;
  dec->payload_len = 0;
  dec->ਸਿਰ_len = dec->ਸਿਰ_ਕੁੱਲ = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

ਸਥਿਰ ਖਾਲੀ ws_dec_init(ਬਣਤਰ ws_decoder *dec)
{
  ws_dec_reset(dec);
}

ਸਥਿਰ CURLcode ws_dec_read_ਸਿਰ(ਬਣਤਰ ws_decoder *dec,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ bufq *inraw)
{
  ਅਚਲ uint8_t *inbuf;
  size_t inlen;

  ਜਦੋਂ(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    ਜੇ(dec->ਸਿਰ_len == 0) {
      dec->ਸਿਰ[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->ਚੌਖਟਾ_flags = ws_ਚੌਖਟਾ_firstbyte2flags(ਅੰਕੜੇ, dec->ਸਿਰ[0],
                                                  dec->cont_flags);
      ਜੇ(!dec->ਚੌਖਟਾ_flags) {
        ws_dec_reset(dec);
        ਵਾਪਸ CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      ਜੇ(dec->ਚੌਖਟਾ_flags & (CURLWS_ਪਾਠ | CURLWS_ਦੁਆਧਾਰੀ)) {
        dec->cont_flags = dec->ਚੌਖਟਾ_flags;
      }

      dec->ਸਿਰ_len = 1;
#ਜੇ 0
      ws_dec_info(dec, ਅੰਕੜੇ, "seeing opcode");
#ਜੇ_ਅੰਤ
      ਜਾਰੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(dec->ਸਿਰ_len == 1) {
      dec->ਸਿਰ[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->ਸਿਰ_len = 2;

      ਜੇ(dec->ਸਿਰ[1] & WSBIT_ਨਕਾਬ) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(ਅੰਕੜੇ, "[WS] masked input frame");
        ws_dec_reset(dec);
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
      ਜੇ(dec->ਚੌਖਟਾ_flags & CURLWS_PING && dec->ਸਿਰ[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(ਅੰਕੜੇ, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
      ਜੇ(dec->ਚੌਖਟਾ_flags & CURLWS_PONG && dec->ਸਿਰ[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(ਅੰਕੜੇ, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
      ਜੇ(dec->ਚੌਖਟਾ_flags & CURLWS_CLOSE && dec->ਸਿਰ[1] > WS_MAX_CNTRL_LEN) {
        failf(ਅੰਕੜੇ, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        ਵਾਪਸ CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      ਜੇ(dec->ਸਿਰ[1] == 126) {
        dec->ਸਿਰ_ਕੁੱਲ = 4;
        ਜਾਰੀ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(dec->ਸਿਰ[1] == 127) {
        dec->ਸਿਰ_ਕੁੱਲ = 10;
        ਜਾਰੀ;
      }
      ਨਹੀਂਤਾਂ {
        dec->ਸਿਰ_ਕੁੱਲ = 2;
      }
    }

    ਜੇ(dec->ਸਿਰ_len < dec->ਸਿਰ_ਕੁੱਲ) {
      dec->ਸਿਰ[dec->ਸਿਰ_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->ਸਿਰ_len;
      ਜੇ(dec->ਸਿਰ_len < dec->ਸਿਰ_ਕੁੱਲ) {
#ਜੇ 0
        ws_dec_info(dec, ਅੰਕੜੇ, "decoding head");
#ਜੇ_ਅੰਤ
        ਜਾਰੀ;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->ਸਿਰ_len == dec->ਸਿਰ_ਕੁੱਲ);
    ਚੋਣ(dec->ਸਿਰ_ਕੁੱਲ) {
    ਹਾਲਤ 2:
      dec->payload_len = dec->ਸਿਰ[1];
      ਰੋਕੋ;
    ਹਾਲਤ 4:
      dec->payload_len = (dec->ਸਿਰ[2] << 8) | dec->ਸਿਰ[3];
      ਰੋਕੋ;
    ਹਾਲਤ 10:
      ਜੇ(dec->ਸਿਰ[2] > 127) {
        failf(ਅੰਕੜੇ, "[WS] frame length longer than 63 bits not supported");
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
      dec->payload_len =
        (curl_off_t)dec->ਸਿਰ[2] << 56 |
        (curl_off_t)dec->ਸਿਰ[3] << 48 |
        (curl_off_t)dec->ਸਿਰ[4] << 40 |
        (curl_off_t)dec->ਸਿਰ[5] << 32 |
        (curl_off_t)dec->ਸਿਰ[6] << 24 |
        (curl_off_t)dec->ਸਿਰ[7] << 16 |
        (curl_off_t)dec->ਸਿਰ[8] <<  8 |
        dec->ਸਿਰ[9];
      ਰੋਕੋ;
    ਮੂਲ:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(ਅੰਕੜੇ, "[WS] unexpected frame header length");
      ਵਾਪਸ CURLE_RECV_ERROR;
    }

    dec->ਚੌਖਟਾ_age = 0;
    dec->payload_ਵਿੱਥ = 0;
    ws_dec_info(dec, ਅੰਕੜੇ, "head");
    ਵਾਪਸ CURLE_OK;
  }
  ਵਾਪਸ CURLE_AGAIN;
}

ਸਥਿਰ CURLcode ws_dec_pass_payload(ਬਣਤਰ ws_decoder *dec,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ bufq *inraw,
                                    ws_write_payload *write_cb,
                                    ਖਾਲੀ *write_ctx)
{
  ਅਚਲ uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_ਵਿੱਥ,
                                     0, SIZE_MAX);

  ਜਦੋਂ(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(ਅੰਕੜੇ)) {
    ਜੇ(inlen > remain)
      inlen = remain;
    ਨਤੀਜਾ = write_cb(inbuf, inlen, dec->ਚੌਖਟਾ_age, dec->ਚੌਖਟਾ_flags,
                      dec->payload_ਵਿੱਥ, dec->payload_len,
                      write_ctx, &nwritten);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_ਵਿੱਥ += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_ਵਿੱਥ,
                                0, SIZE_MAX);
    CURL_TRC_WS(ਅੰਕੜੇ, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  ਵਾਪਸ remain ? CURLE_AGAIN : CURLE_OK;
}

ਸਥਿਰ CURLcode ws_dec_pass(ਬਣਤਰ ws_decoder *dec,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ bufq *inraw,
                            ws_write_payload *write_cb,
                            ਖਾਲੀ *write_ctx)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(inraw))
    ਵਾਪਸ CURLE_AGAIN;

  ਚੋਣ(dec->state) {
  ਹਾਲਤ WS_DEC_INIT:
    ws_dec_ਅਗਲਾ_ਚੌਖਟਾ(dec);
    dec->state = WS_DEC_ਸਿਰ;
    FALLTHROUGH();
  ਹਾਲਤ WS_DEC_ਸਿਰ:
    ਨਤੀਜਾ = ws_dec_read_ਸਿਰ(dec, ਅੰਕੜੇ, inraw);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN) {
        failf(ਅੰਕੜੇ, "[WS] decode frame error %d", (ਪੂਰਨ)ਨਤੀਜਾ);
        ਰੋਕੋ;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(inraw));
      ਰੋਕੋ;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    ਜੇ(dec->payload_len == 0) {
      size_t nwritten;
      ਅਚਲ uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      ਨਤੀਜਾ = write_cb(&tmp, 0, dec->ਚੌਖਟਾ_age, dec->ਚੌਖਟਾ_flags,
                        0, 0, write_ctx, &nwritten);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      dec->state = WS_DEC_INIT;
      ਰੋਕੋ;
    }
    FALLTHROUGH();
  ਹਾਲਤ WS_DEC_PAYLOAD:
    ਨਤੀਜਾ = ws_dec_pass_payload(dec, ਅੰਕੜੇ, inraw, write_cb, write_ctx);
    ws_dec_info(dec, ਅੰਕੜੇ, "passing");
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    ਰੋਕੋ;
  ਮੂਲ:
    /* we covered all enums above, but some code analyzers are wimps */
    ਨਤੀਜਾ = CURLE_FAILED_INIT;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ update_meta(ਬਣਤਰ websocket *ws,
                        ਪੂਰਨ ਚੌਖਟਾ_age, ਪੂਰਨ ਚੌਖਟਾ_flags,
                        curl_off_t payload_ਵਿੱਥ,
                        curl_off_t payload_len,
                        size_t cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_ਵਿੱਥ - cur_len);

  ws->recvframe.age = ਚੌਖਟਾ_age;
  ws->recvframe.flags = ਚੌਖਟਾ_flags;
  ws->recvframe.ਵਿੱਥ = payload_ਵਿੱਥ;
  ws->recvframe.len = cur_len;
  ws->recvframe.bytesleft = bytesleft;
}

/* WebSocket decoding client writer */
ਬਣਤਰ ws_cw_ctx {
  ਬਣਤਰ Curl_cwriter super;
  ਬਣਤਰ bufq buf;
};

ਸਥਿਰ CURLcode ws_cw_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ ws_cw_ctx *ctx = writer->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_ਮਾਪ, 1, BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ ws_cw_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ ws_cw_ctx *ctx = writer->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  Curl_bufq_free(&ctx->buf);
}

ਬਣਤਰ ws_cw_dec_ctx {
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਬਣਤਰ websocket *ws;
  ਬਣਤਰ Curl_cwriter *ਅਗਲਾ_writer;
  ਪੂਰਨ cw_type;
};

ਸਥਿਰ CURLcode ws_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ websocket *ws,
                         bool blocking);
ਸਥਿਰ CURLcode ws_enc_ਜੋੜੋ_pending(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ websocket *ws);

ਸਥਿਰ CURLcode ws_enc_ਜੋੜੋ_cntrl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ websocket *ws,
                                 ਅਚਲ uint8_t *payload,
                                 size_t plen,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਚੌਖਟਾ_type)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  ਜੇ(plen > WS_MAX_CNTRL_LEN)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = ਚੌਖਟਾ_type;
  ws->pending.payload_len = plen;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ws->pending.payload, payload, plen);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ curl_off_t ws_payload_remain(curl_off_t payload_ਕੁੱਲ,
                                    curl_off_t payload_ਵਿੱਥ,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_ਕੁੱਲ - payload_ਵਿੱਥ;
  ਜੇ((payload_ਕੁੱਲ < 0) || (payload_ਵਿੱਥ < 0) || (remain < 0))
    ਵਾਪਸ -1;
  buffered = curlx_uztoso(payload_buffered);
  ਜੇ(remain < buffered)
    ਵਾਪਸ -1;
  ਵਾਪਸ remain - buffered;
}

ਸਥਿਰ CURLcode ws_cw_dec_ਅਗਲਾ(ਅਚਲ uint8_t *buf, size_t buflen,
                               ਪੂਰਨ ਚੌਖਟਾ_age, ਪੂਰਨ ਚੌਖਟਾ_flags,
                               curl_off_t payload_ਵਿੱਥ,
                               curl_off_t payload_len,
                               ਖਾਲੀ *user_ਅੰਕੜੇ,
                               size_t *pnwritten)
{
  ਬਣਤਰ ws_cw_dec_ctx *ctx = user_ਅੰਕੜੇ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = ctx->ਅੰਕੜੇ;
  ਬਣਤਰ websocket *ws = ctx->ws;
  bool auto_pong = !ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ && !ਅੰਕੜੇ->ਸੈੱਟ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਚੌਖਟਾ_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_ਵਿੱਥ, buflen);
  ਜੇ(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  ਜੇ(auto_pong && (ਚੌਖਟਾ_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(ਅੰਕੜੇ, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_ਵਿੱਥ, payload_len);
    /* send back the exact same content as a PONG */
    ਨਤੀਜਾ = ws_enc_ਜੋੜੋ_cntrl(ਅੰਕੜੇ, ws, buf, buflen, CURLWS_PONG);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, ਚੌਖਟਾ_age, ਚੌਖਟਾ_flags, payload_ਵਿੱਥ,
                payload_len, buflen);

    CURL_TRC_WRITE(ਅੰਕੜੇ, "[WS] pass %zu decoded bytes", buflen);
    ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, ctx->ਅਗਲਾ_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ਅਚਲ ਅੱਖਰ *)buf, buflen);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  *pnwritten = buflen;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ws_cw_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                            ਅਚਲ ਅੱਖਰ *buf, size_t nbytes)
{
  ਬਣਤਰ ws_cw_ctx *ctx = writer->ctx;
  ਬਣਤਰ websocket *ws;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CURL_TRC_WRITE(ਅੰਕੜੇ, "[WS] write(len=%zu, type=%d)", nbytes, type);
  ਜੇ(!(type & CLIENTWRITE_BODY) || ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ)
    ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, nbytes);

  ws = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_PROTO_WS_CONN);
  ਜੇ(!ws) {
    failf(ਅੰਕੜੇ, "[WS] not a websocket transfer");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਜੇ(nbytes) {
    size_t nwritten;
    ਨਤੀਜਾ = Curl_bufq_write(&ctx->buf, (ਅਚਲ uint8_t *)buf,
                             nbytes, &nwritten);
    ਜੇ(ਨਤੀਜਾ) {
      infof(ਅੰਕੜੇ, "[WS] error adding data to buffer %d", (ਪੂਰਨ)ਨਤੀਜਾ);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  ਨਤੀਜਾ = Curl_cwriter_flush(ਅੰਕੜੇ, writer->ਅਗਲਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜਦੋਂ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->buf) && !Curl_cwriter_is_paused(ਅੰਕੜੇ)) {
    ਬਣਤਰ ws_cw_dec_ctx pass_ctx;
    pass_ctx.ਅੰਕੜੇ = ਅੰਕੜੇ;
    pass_ctx.ws = ws;
    pass_ctx.ਅਗਲਾ_writer = writer->ਅਗਲਾ;
    pass_ctx.cw_type = type;
    ਨਤੀਜਾ = ws_dec_pass(&ws->dec, ਅੰਕੜੇ, &ctx->buf,
                         ws_cw_dec_ਅਗਲਾ, &pass_ctx);
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      ਨਤੀਜਾ = CURLE_OK;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "[WS] decode payload error %d", (ਪੂਰਨ)ਨਤੀਜਾ);
      Curl_bufq_reset(&ctx->buf);
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ && !ws->using_reader) {
    ਨਤੀਜਾ = ws_flush(ਅੰਕੜੇ, ws, Curl_api_is_ਵਿੱਚ_callback(ਅੰਕੜੇ));
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਨਤੀਜਾ = CURLE_OK;
  }

  ਜੇ(!ਨਤੀਜਾ && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(ਅੰਕੜੇ, "[WS] decode ended with incomplete frame");
    ਨਤੀਜਾ = CURLE_RECV_ERROR;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ws_cw_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_cwriter *writer)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CURL_TRC_WRITE(ਅੰਕੜੇ, "[ws] flush");
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ) {
    ਬਣਤਰ ws_cw_ctx *ctx = writer->ctx;
    ਬਣਤਰ websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    ਨਤੀਜਾ = Curl_cwriter_flush(ਅੰਕੜੇ, writer->ਅਗਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ws = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_PROTO_WS_CONN);
    ਜੇ(!ws) {
      failf(ਅੰਕੜੇ, "[WS] not a websocket transfer");
      ਵਾਪਸ CURLE_FAILED_INIT;
    }

    ਜਦੋਂ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->buf) && !Curl_cwriter_is_paused(ਅੰਕੜੇ)) {
      ਬਣਤਰ ws_cw_dec_ctx pass_ctx;
      pass_ctx.ਅੰਕੜੇ = ਅੰਕੜੇ;
      pass_ctx.ws = ws;
      pass_ctx.ਅਗਲਾ_writer = writer->ਅਗਲਾ;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      ਨਤੀਜਾ = ws_dec_pass(&ws->dec, ਅੰਕੜੇ, &ctx->buf,
                           ws_cw_dec_ਅਗਲਾ, &pass_ctx);
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        ਨਤੀਜਾ = CURLE_OK;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
        failf(ਅੰਕੜੇ, "[WS] decode payload error %d", (ਪੂਰਨ)ਨਤੀਜਾ);
        Curl_bufq_reset(&ctx->buf);
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_cwriter_flush(ਅੰਕੜੇ, writer->ਅਗਲਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* WebSocket payload decoding client writer. */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype ws_cw_decode = {
  "ws-decode",
  ਨਿਰਾ,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  ਆਕਾਰ(ਬਣਤਰ ws_cw_ctx)
};

ਸਥਿਰ ਖਾਲੀ ws_enc_info(ਬਣਤਰ ws_encoder *enc, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ)
{
  NOVERBOSE((ਖਾਲੀ)enc);
  NOVERBOSE((ਖਾਲੀ)ਸੁਨੇਹਾ);
  CURL_TRC_WS(ਅੰਕੜੇ, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              ਸੁਨੇਹਾ, ws_ਚੌਖਟਾ_name_ਦਾ_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

ਸਥਿਰ ਖਾਲੀ ws_enc_reset(ਬਣਤਰ ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

ਸਥਿਰ ਖਾਲੀ ws_enc_init(ਬਣਤਰ ws_encoder *enc)
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

ਸਥਿਰ CURLcode ws_enc_ਜੋੜੋ_ਚੌਖਟਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ws_encoder *enc,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                                 curl_off_t payload_len,
                                 ਬਣਤਰ bufq *ਬਾਹਰਲਾ)
{
  uint8_t firstb = 0;
  uint8_t ਸਿਰ[14];
  CURLcode ਨਤੀਜਾ;
  size_t hlen, nwritten;

  ਜੇ(payload_len < 0) {
    failf(ਅੰਕੜੇ, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    ਵਾਪਸ CURLE_SEND_ERROR;
  }

  ਜੇ(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(ਅੰਕੜੇ, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    ਵਾਪਸ CURLE_SEND_ERROR;
  }

  ਨਤੀਜਾ = ws_ਚੌਖਟਾ_flags2firstbyte(ਅੰਕੜੇ, flags, (bool)enc->contfragment,
                                    &firstb);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  ਜੇ(flags & (CURLWS_ਪਾਠ | CURLWS_ਦੁਆਧਾਰੀ)) {
    enc->contfragment = (curl_ਦੋਆਧਾਰੀ_ਅੰਕ)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  ਜੇ(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(ਅੰਕੜੇ, "[WS] given PING frame is too big");
    ਵਾਪਸ CURLE_TOO_LARGE;
  }
  ਜੇ(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(ਅੰਕੜੇ, "[WS] given PONG frame is too big");
    ਵਾਪਸ CURLE_TOO_LARGE;
  }
  ਜੇ(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(ਅੰਕੜੇ, "[WS] given CLOSE frame is too big");
    ਵਾਪਸ CURLE_TOO_LARGE;
  }

  ਸਿਰ[0] = enc->firstbyte = firstb;
  ਜੇ(payload_len > 65535) {
    ਸਿਰ[1] = 127 | WSBIT_ਨਕਾਬ;
    ਸਿਰ[2] = (uint8_t)((payload_len >> 56) & 0xff);
    ਸਿਰ[3] = (uint8_t)((payload_len >> 48) & 0xff);
    ਸਿਰ[4] = (uint8_t)((payload_len >> 40) & 0xff);
    ਸਿਰ[5] = (uint8_t)((payload_len >> 32) & 0xff);
    ਸਿਰ[6] = (uint8_t)((payload_len >> 24) & 0xff);
    ਸਿਰ[7] = (uint8_t)((payload_len >> 16) & 0xff);
    ਸਿਰ[8] = (uint8_t)((payload_len >> 8) & 0xff);
    ਸਿਰ[9] = (uint8_t)(payload_len & 0xff);
    hlen = 10;
  }
  ਨਹੀਂਤਾਂ ਜੇ(payload_len >= 126) {
    ਸਿਰ[1] = 126 | WSBIT_ਨਕਾਬ;
    ਸਿਰ[2] = (uint8_t)((payload_len >> 8) & 0xff);
    ਸਿਰ[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  ਨਹੀਂਤਾਂ {
    ਸਿਰ[1] = (uint8_t)payload_len | WSBIT_ਨਕਾਬ;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, ਅੰਕੜੇ, "sending");

  /* 4 bytes random */

  ਨਤੀਜਾ = Curl_rand(ਅੰਕੜੇ, (uint8_t *)&enc->ਨਕਾਬ, ਆਕਾਰ(enc->ਨਕਾਬ));
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->ਨਕਾਬ, 0, ਆਕਾਰ(enc->ਨਕਾਬ));
#ਜੇ_ਅੰਤ

  /* add 4 bytes mask */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ਸਿਰ[hlen], &enc->ਨਕਾਬ, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  ਨਤੀਜਾ = Curl_bufq_write(ਬਾਹਰਲਾ, ਸਿਰ, hlen, &nwritten);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਜੇ(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    ਵਾਪਸ CURLE_SEND_ERROR;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ws_enc_write_ਸਿਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ websocket *ws,
                                  ਬਣਤਰ ws_encoder *enc,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                                  curl_off_t payload_len,
                                  ਬਣਤਰ bufq *ਬਾਹਰਲਾ)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  ਜੇ(ws->pending.type) {
    CURLcode ਨਤੀਜਾ = ws_enc_ਜੋੜੋ_pending(ਅੰਕੜੇ, ws);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ ws_enc_ਜੋੜੋ_ਚੌਖਟਾ(ਅੰਕੜੇ, enc, flags, payload_len, ਬਾਹਰਲਾ);
}

ਸਥਿਰ CURLcode ws_enc_write_payload(ਬਣਤਰ ws_encoder *enc,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਅਚਲ uint8_t *buf, size_t buflen,
                                     ਬਣਤਰ bufq *ਬਾਹਰਲਾ, size_t *pnwritten)
{
  CURLcode ਨਤੀਜਾ;
  size_t i, len, n, remain;

  *pnwritten = 0;
  ਜੇ(Curl_bufq_is_ਭਰਿਆ(ਬਾਹਰਲਾ))
    ਵਾਪਸ CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  ਜੇ(remain < len)
    len = remain;

  ਲਈ(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->ਨਕਾਬ[enc->xori];
    ਨਤੀਜਾ = Curl_bufq_write(ਬਾਹਰਲਾ, &c, 1, &n);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ((ਨਤੀਜਾ != CURLE_AGAIN) || !i)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਰੋਕੋ;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, ਅੰਕੜੇ, "buffered");
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ws_enc_ਜੋੜੋ_pending(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ websocket *ws)
{
  CURLcode ਨਤੀਜਾ;
  size_t n;

  ਜੇ(!ws->pending.type) /* no pending frame here */
    ਵਾਪਸ CURLE_OK;
  ਜੇ(ws->enc.payload_remain) /* in the middle of another frame */
    ਵਾਪਸ CURLE_AGAIN;

  ਨਤੀਜਾ = ws_enc_ਜੋੜੋ_ਚੌਖਟਾ(ਅੰਕੜੇ, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  ਜੇ(ਨਤੀਜਾ) {
    CURL_TRC_WS(ਅੰਕੜੇ, "ws_enc_cntrl(), error adding head: %d",
                (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = ws_enc_write_payload(&ws->enc, ਅੰਕੜੇ, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  ਜੇ(ਨਤੀਜਾ) {
    CURL_TRC_WS(ਅੰਕੜੇ, "ws_enc_cntrl(), error adding payload: %d",
                (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(ਅੰਕੜੇ, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, ਆਕਾਰ(ws->pending));

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ws_enc_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ websocket *ws,
                            ਅਚਲ uint8_t *ਬਫਰ,
                            size_t buflen,
                            curl_off_t fragsize,
                            ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(!ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ);
  *pnsent = 0;

  ਜੇ(ws->enc.payload_remain || !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    ਜੇ(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(ਅੰਕੜੇ, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
    ਜੇ((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(ਅੰਕੜੇ, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = ws_flush(ਅੰਕੜੇ, ws, Curl_api_is_ਵਿੱਚ_callback(ਅੰਕੜੇ));
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਨਤੀਜਾ = ws_enc_write_ਸਿਰ(ਅੰਕੜੇ, ws, &ws->enc, flags,
                               (flags & CURLWS_ਵਿੱਥ) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_WS(ਅੰਕੜੇ, "curl_ws_send(), error writing frame head %d",
                  (ਪੂਰਨ)ਨਤੀਜਾ);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  ਜਦੋਂ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    ਜੇ(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      ਨਤੀਜਾ = ws_enc_write_payload(&ws->enc, ਅੰਕੜੇ,
                                    ਬਫਰ + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN))
        ਵਾਪਸ ਨਤੀਜਾ;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      ਜੇ(!ws->sendbuf_payload) {
        ਵਾਪਸ CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    ਨਤੀਜਾ = ws_flush(ਅੰਕੜੇ, ws, Curl_api_is_ਵਿੱਚ_callback(ਅੰਕੜੇ));
    ਜੇ(!ਨਤੀਜਾ && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      ਬਫਰ += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      ਜੇ(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        ਵਾਪਸ CURLE_OK;
      }
      ਨਹੀਂਤਾਂ {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(ਅੰਕੜੇ, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        ਵਾਪਸ CURLE_AGAIN;
      }
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ ਨਤੀਜਾ;  /* real error sending the data */
  }
  ਵਾਪਸ CURLE_OK;
}

ਬਣਤਰ cr_ws_ctx {
  ਬਣਤਰ Curl_creader super;
  ਦੋਆਧਾਰੀ_ਅੰਕ(read_eos);  /* we read an EOS from the next reader */
  ਦੋਆਧਾਰੀ_ਅੰਕ(eos);       /* we have returned an EOS */
};

ਸਥਿਰ CURLcode cr_ws_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cr_ws_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
}

ਸਥਿਰ CURLcode cr_ws_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_creader *reader,
                           ਅੱਖਰ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ਬਣਤਰ cr_ws_ctx *ctx = reader->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nread, n;
  ਬਣਤਰ websocket *ws;
  bool eos;

  *pnread = 0;
  ਜੇ(ctx->eos) {
    *peos = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  ws = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_PROTO_WS_CONN);
  ਜੇ(!ws) {
    failf(ਅੰਕੜੇ, "[WS] not a websocket transfer");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ws->sendbuf)) {
    ਜੇ(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      ਵਾਪਸ CURLE_OK;
    }

    ਜੇ(ws->enc.payload_remain) {
      CURL_TRC_WS(ਅੰਕੜੇ, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    ਨਤੀਜਾ = Curl_creader_read(ਅੰਕੜੇ, reader->ਅਗਲਾ, buf, blen, &nread, &eos);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ctx->read_eos = eos;

    ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(ਅੰਕੜੇ, reader->ਅਗਲਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ(!nread) {
      /* nothing to convert, return this right away */
      ਜੇ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(!ws->enc.payload_remain && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      ਨਤੀਜਾ = ws_enc_write_ਸਿਰ(ਅੰਕੜੇ, ws, &ws->enc, CURLWS_ਦੁਆਧਾਰੀ, nread,
                                 &ws->sendbuf);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਨਤੀਜਾ = ws_enc_write_payload(&ws->enc, ਅੰਕੜੇ, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    CURL_TRC_READ(ਅੰਕੜੇ, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ws->sendbuf));
  *peos = FALSE;
  ਨਤੀਜਾ = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  ਜੇ(!ਨਤੀਜਾ && ctx->read_eos && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

ਬਾਹਰਲਾ:
  CURL_TRC_READ(ਅੰਕੜੇ, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread, *peos);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ curl_off_t cr_ws_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  ਵਾਪਸ -1;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_ਕੁੱਲ_ਲੰਮਾਈ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ਪੂਰਾ,
  ਆਕਾਰ(ਬਣਤਰ cr_ws_ctx)
};

ਬਣਤਰ wsfield {
  ਅਚਲ ਅੱਖਰ *name;
  ਅਚਲ ਅੱਖਰ *val;
};

CURLcode Curl_ws_request(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ dynbuf *req)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  uint8_t rand[16];
  ਅੱਖਰ *randstr;
  size_t randlen;
  ਅੱਖਰ keyval[40];
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  ਬਣਤਰ wsfield heads[] = {
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
      "Sec-WebSocket-Key", ਨਿਰਾ,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  ਨਤੀਜਾ = Curl_rand(ਅੰਕੜੇ, rand, ਆਕਾਰ(rand));
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਨਤੀਜਾ = curlx_base64_encode(rand, ਆਕਾਰ(rand), &randstr, &randlen);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  DEBUGASSERT(randlen < ਆਕਾਰ(keyval));
  ਜੇ(randlen >= ਆਕਾਰ(keyval)) {
    curlx_free(randstr);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, ਆਕਾਰ(keyval), randstr, randlen);
  curlx_free(randstr);
  ਲਈ(i = 0; !ਨਤੀਜਾ && (i < CURL_ARRAYSIZE(heads)); i++) {
    ਜੇ(!Curl_checkheaders(ਅੰਕੜੇ, heads[i].name, ਲੜੀ_ਲੰਬਾਈ(heads[i].name))) {
      ਨਤੀਜਾ = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  ਅੰਕੜੇ->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  ਅੰਕੜੇ->conn->ਬਿੱਟ.upgrade_ਵਿੱਚ_ਤਰੱਕੀ = TRUE;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ ws_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ websocket *ws = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *mem, size_t nread)
{
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  ਬਣਤਰ websocket *ws;
  ਬਣਤਰ Curl_cwriter *ws_dec_writer = ਨਿਰਾ;
  ਬਣਤਰ Curl_creader *ws_enc_reader = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ਅੰਕੜੇ->conn);
  ws = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_PROTO_WS_CONN);
  ਜੇ(!ws) {
    size_t chunk_ਮਾਪ = WS_CHUNK_ਮਾਪ;
    ws = curlx_calloc(1, ਆਕਾਰ(*ws));
    ਜੇ(!ws)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    {
      ਅਚਲ ਅੱਖਰ *p = getenv("CURL_WS_CHUNK_SIZE");
      ਜੇ(p) {
        curl_off_t l;
        ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, 1 * 1024 * 1024))
          chunk_ਮਾਪ = (size_t)l;
      }
    }
#ਜੇ_ਅੰਤ
    CURL_TRC_WS(ਅੰਕੜੇ, "WS, using chunk size %zu", chunk_ਮਾਪ);
    Curl_bufq_init2(&ws->recvbuf, chunk_ਮਾਪ, WS_CHUNK_ਗਿਣਤੀ,
                    BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
    Curl_bufq_init2(&ws->sendbuf, chunk_ਮਾਪ, WS_CHUNK_ਗਿਣਤੀ,
                    BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    ਨਤੀਜਾ = Curl_conn_meta_ਸੈੱਟ(ਅੰਕੜੇ->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ {
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

  infof(ਅੰਕੜੇ, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  ਨਤੀਜਾ = Curl_cwriter_create(&ws_dec_writer, ਅੰਕੜੇ, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਨਤੀਜਾ = Curl_cwriter_ਜੋੜੋ(ਅੰਕੜੇ, ws_dec_writer);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ws_dec_writer = ਨਿਰਾ; /* owned by transfer now */

  k->ਸਿਰਲੇਖ = FALSE; /* we will not get more response headers */

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    ਜੇ(nread) {
      ਨਤੀਜਾ = Curl_cf_recvbuf_ਜੋੜੋ(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, FIRSTSOCKET,
                                   (ਅਚਲ uint8_t *)mem, nread);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
    CURL_REQ_CLEAR_RECV(ਅੰਕੜੇ); /* read no more content */
  }
  ਨਹੀਂਤਾਂ { /* !connect_only */
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ਵਿਧੀ == HTTPREQ_PUT) {
      CURL_TRC_WS(ਅੰਕੜੇ, "UPLOAD set, add ws-encode reader");
      ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_fread(ਅੰਕੜੇ, -1);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;

      ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ) {
        /* Add our client reader encoding WS BINARY frames */
        ਨਤੀਜਾ = Curl_creader_create(&ws_enc_reader, ਅੰਕੜੇ, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
        ਨਤੀਜਾ = Curl_creader_ਜੋੜੋ(ਅੰਕੜੇ, ws_enc_reader);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
        ws_enc_reader = ਨਿਰਾ; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      ਅੰਕੜੇ->req.eos_read = FALSE;
      ਅੰਕੜੇ->req.upload_ਪੂਰਾ = FALSE;
      CURL_REQ_ਸੈੱਟ_SEND(ਅੰਕੜੇ);
    }

    /* Then pass any additional data to the writers */
    ਜੇ(nread) {
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, mem, nread);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->ਸਿਰਲੇਖ = FALSE; /* we will not get more responses */

ਬਾਹਰਲਾ:
  ਜੇ(ws_dec_writer)
    Curl_cwriter_free(ਅੰਕੜੇ, ws_dec_writer);
  ਜੇ(ws_enc_reader)
    Curl_creader_free(ਅੰਕੜੇ, ws_enc_reader);
  ਜੇ(ਨਤੀਜਾ)
    CURL_TRC_WS(ਅੰਕੜੇ, "Curl_ws_accept() failed -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
  ਨਹੀਂਤਾਂ
    CURL_TRC_WS(ਅੰਕੜੇ, "websocket established, %s mode",
                ਅੰਕੜੇ->ਸੈੱਟ.connect_only ? "connect-only" : "callback");
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਬਣਤਰ ws_collect {
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਬਣਤਰ websocket *ws;
  uint8_t *ਬਫਰ;
  size_t buflen;
  size_t bufidx;
  ਪੂਰਨ ਚੌਖਟਾ_age;
  ਪੂਰਨ ਚੌਖਟਾ_flags;
  curl_off_t payload_ਵਿੱਥ;
  curl_off_t payload_len;
  bool written;
};

ਸਥਿਰ CURLcode ws_client_collect(ਅਚਲ uint8_t *buf, size_t buflen,
                                  ਪੂਰਨ ਚੌਖਟਾ_age, ਪੂਰਨ ਚੌਖਟਾ_flags,
                                  curl_off_t payload_ਵਿੱਥ,
                                  curl_off_t payload_len,
                                  ਖਾਲੀ *userp,
                                  size_t *pnwritten)
{
  ਬਣਤਰ ws_collect *ctx = userp;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = ctx->ਅੰਕੜੇ;
  bool auto_pong = !ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ && !ਅੰਕੜੇ->ਸੈੱਟ.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_ਵਿੱਥ, buflen);
  ਜੇ(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  ਜੇ(!ctx->bufidx) {
    /* first write */
    ctx->ਚੌਖਟਾ_age = ਚੌਖਟਾ_age;
    ctx->ਚੌਖਟਾ_flags = ਚੌਖਟਾ_flags;
    ctx->payload_ਵਿੱਥ = payload_ਵਿੱਥ;
    ctx->payload_len = payload_len;
  }

  ਜੇ(auto_pong && (ਚੌਖਟਾ_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(ਅੰਕੜੇ, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_ਵਿੱਥ, payload_len);
    /* send back the exact same content as a PONG */
    ਨਤੀਜਾ = ws_enc_ਜੋੜੋ_cntrl(ctx->ਅੰਕੜੇ, ctx->ws, buf, buflen, CURLWS_PONG);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    *pnwritten = buflen;
  }
  ਨਹੀਂਤਾਂ {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    ਜੇ(!write_len) {
      ਜੇ(!buflen)  /* 0 length write, we accept that */
        ਵਾਪਸ CURLE_OK;
      ਵਾਪਸ CURLE_AGAIN;  /* no more space */
    }
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ctx->ਬਫਰ + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode nw_ਵਿੱਚ_recv(ਖਾਲੀ *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = reader_ctx;
  ਵਾਪਸ Curl_easy_recv(ਅੰਕੜੇ, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, ਖਾਲੀ *ਬਫਰ,
                      size_t buflen, size_t *recv,
                      ਅਚਲ ਬਣਤਰ curl_ws_ਚੌਖਟਾ **metap)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, ws_recv, &ਨਤੀਜਾ)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    ਬਣਤਰ connectdata *conn;
    ਬਣਤਰ websocket *ws;
    ਬਣਤਰ ws_collect ctx;

    ਜੇ(!recv || !metap || (buflen && !ਬਫਰ)) {
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    *recv = 0;
    *metap = ਨਿਰਾ;

    conn = ਅੰਕੜੇ->conn;
    ਜੇ(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.connect_only) {
        failf(ਅੰਕੜੇ, "[WS] CONNECT_ONLY is required");
        ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;
        ਜਾਓ ਬਾਹਰਲਾ;
      }

      Curl_getconnectinfo(ਅੰਕੜੇ, &conn);
      ਜੇ(!conn) {
        failf(ਅੰਕੜੇ, "[WS] connection not found");
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    ਜੇ(!ws) {
      failf(ਅੰਕੜੇ, "[WS] connection is not setup for websocket");
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    memset(&ctx, 0, ਆਕਾਰ(ctx));
    ctx.ਅੰਕੜੇ = ਅੰਕੜੇ;
    ctx.ws = ws;
    ctx.ਬਫਰ = ਬਫਰ;
    ctx.buflen = buflen;

    ਜਦੋਂ(1) {
      /* receive more when our buffer is empty */
      ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ws->recvbuf)) {
        size_t n;
        ਨਤੀਜਾ = Curl_bufq_slurp(&ws->recvbuf, nw_ਵਿੱਚ_recv, ਅੰਕੜੇ, &n);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
        ਨਹੀਂਤਾਂ ਜੇ(n == 0) {
          /* connection closed */
          infof(ਅੰਕੜੇ, "[WS] connection expectedly closed?");
          ਨਤੀਜਾ = CURLE_GOT_NOTHING;
          ਜਾਓ ਬਾਹਰਲਾ;
        }
        CURL_TRC_WS(ਅੰਕੜੇ, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      ਨਤੀਜਾ = ws_dec_pass(&ws->dec, ਅੰਕੜੇ, &ws->recvbuf,
                           ws_client_collect, &ctx);
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
        ਜੇ(!ctx.written) {
          ws_dec_info(&ws->dec, ਅੰਕੜੇ, "need more input");
          ਜਾਰੀ;  /* nothing written, try more input */
        }
        ਰੋਕੋ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        ਰੋਕੋ;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.ਚੌਖਟਾ_age, ctx.ਚੌਖਟਾ_flags, ctx.payload_ਵਿੱਥ,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(ਅੰਕੜੇ, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.ਵਿੱਥ,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_ਜੋੜੋ_pending(ਅੰਕੜੇ, ws);
      ਜੇ(!r2)
        (ਖਾਲੀ)ws_flush(ਅੰਕੜੇ, ws, Curl_api_is_ਵਿੱਚ_callback(ਅੰਕੜੇ));
    }
    ਨਤੀਜਾ = CURLE_OK;
  }
ਬਾਹਰਲਾ:
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ws_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ websocket *ws,
                         bool blocking)
{
  CURLcode ਨਤੀਜਾ;

  /* If there is space, add any pending control frame */
  ਜੇ(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_ਮਾਪ) {
    ਨਤੀਜਾ = ws_enc_ਜੋੜੋ_pending(ਅੰਕੜੇ, ws);
    ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN))
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ws->sendbuf)) {
    ਅਚਲ uint8_t *ਬਾਹਰਲਾ;
    size_t outlen, n;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_ਅਗਲਾ = FALSE;
    size_t chunk_egain = 0;
    ਅਚਲ ਅੱਖਰ *p = getenv("CURL_WS_CHUNK_EAGAIN");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#ਜੇ_ਅੰਤ

    ਜਦੋਂ(Curl_bufq_peek(&ws->sendbuf, &ਬਾਹਰਲਾ, &outlen)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
      ਜੇ(eagain_ਅਗਲਾ)
        ਵਾਪਸ CURLE_AGAIN;
      ਜੇ(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_ਅਗਲਾ = TRUE;
      }
#ਜੇ_ਅੰਤ
      ਜੇ(blocking) {
        ਨਤੀਜਾ = ws_send_raw_blocking(ਅੰਕੜੇ, ws, (ਅਚਲ ਅੱਖਰ *)ਬਾਹਰਲਾ, outlen);
        n = ਨਤੀਜਾ ? 0 : outlen;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.connect_only || Curl_api_is_ਵਿੱਚ_callback(ਅੰਕੜੇ))
        ਨਤੀਜਾ = Curl_senddata(ਅੰਕੜੇ, ਬਾਹਰਲਾ, outlen, &n);
      ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, ਬਾਹਰਲਾ, outlen, FALSE, &n);
        ਜੇ(!ਨਤੀਜਾ && !n && outlen)
          ਨਤੀਜਾ = CURLE_AGAIN;
      }

      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
        CURL_TRC_WS(ਅੰਕੜੇ, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
        failf(ਅੰਕੜੇ, "[WS] flush, write error %d", (ਪੂਰਨ)ਨਤੀਜਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਨਹੀਂਤਾਂ {
        CURL_TRC_WS(ਅੰਕੜੇ, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ws_send_raw_blocking(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ websocket *ws,
                                     ਅਚਲ ਅੱਖਰ *ਬਫਰ, size_t buflen)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nwritten;

  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ ਨਤੀਜਾ;

  (ਖਾਲੀ)ws;
  ਜਦੋਂ(buflen) {
    ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, ਬਫਰ, buflen, FALSE, &nwritten);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    DEBUGASSERT(nwritten <= buflen);
    ਬਫਰ += nwritten;
    buflen -= nwritten;
    ਜੇ(buflen) {
      curl_socket_t sock = ਅੰਕੜੇ->conn->sock[FIRSTSOCKET];
      timediff_t ਖੱਬਾ_ms;
      ਪੂਰਨ ev;

      CURL_TRC_WS(ਅੰਕੜੇ, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      ਖੱਬਾ_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
      ਜੇ(ਖੱਬਾ_ms < 0) {
        failf(ਅੰਕੜੇ, "[WS] Timeout waiting for socket becoming writable");
        ਵਾਪਸ CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      ਜੇ(sock == CURL_SOCKET_BAD)
        ਵਾਪਸ CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, ਖੱਬਾ_ms ? ਖੱਬਾ_ms : 500);
      ਜੇ(ev < 0) {
        failf(ਅੰਕੜੇ, "[WS] Error while waiting for socket becoming writable");
        ਵਾਪਸ CURLE_SEND_ERROR;
      }
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ws_send_raw(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਖਾਲੀ *ਬਫਰ,
                            size_t buflen, size_t *pnwritten)
{
  ਬਣਤਰ websocket *ws;
  CURLcode ਨਤੀਜਾ;

  ws = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_PROTO_WS_CONN);
  ਜੇ(!ws) {
    failf(ਅੰਕੜੇ, "[WS] Not a websocket transfer");
    ਵਾਪਸ CURLE_SEND_ERROR;
  }
  ਜੇ(!buflen)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(Curl_api_is_ਵਿੱਚ_callback(ਅੰਕੜੇ)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    ਨਤੀਜਾ = ws_flush(ਅੰਕੜੇ, ws, TRUE);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਨਤੀਜਾ = ws_send_raw_blocking(ਅੰਕੜੇ, ws, ਬਫਰ, buflen);
    ਜੇ(!ਨਤੀਜਾ)
      *pnwritten = buflen;
  }
  ਨਹੀਂਤਾਂ {
    /* We need any pending data to be sent or EAGAIN this call. */
    ਨਤੀਜਾ = ws_flush(ਅੰਕੜੇ, ws, FALSE);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਨਤੀਜਾ = Curl_senddata(ਅੰਕੜੇ, ਬਫਰ, buflen, pnwritten);
  }

  CURL_TRC_WS(ਅੰਕੜੇ, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode curl_ws_send(CURL *curl, ਅਚਲ ਖਾਲੀ *ਬਫਰ_arg,
                      size_t buflen, size_t *ਭੇਜਿਆ,
                      curl_off_t fragsize,
                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, ws_send, &ਨਤੀਜਾ)) {
    ਬਣਤਰ websocket *ws;
    ਅਚਲ uint8_t *ਬਫਰ = ਬਫਰ_arg;
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    size_t ndummy;
    size_t *pnsent = ਭੇਜਿਆ ? ਭੇਜਿਆ : &ndummy;

    CURL_TRC_WS(ਅੰਕੜੇ, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ);

    *pnsent = 0;

    ਜੇ(!ਬਫਰ && buflen) {
      failf(ਅੰਕੜੇ, "[WS] buffer is NULL when buflen is not");
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(!ਅੰਕੜੇ->conn && ਅੰਕੜੇ->ਸੈੱਟ.connect_only) {
      ਨਤੀਜਾ = Curl_connect_only_attach(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ(!ਅੰਕੜੇ->conn) {
      failf(ਅੰਕੜੇ, "[WS] No associated connection");
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ws = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_PROTO_WS_CONN);
    ਜੇ(!ws) {
      failf(ਅੰਕੜੇ, "[WS] Not a websocket transfer");
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      ਨਤੀਜਾ = ws_flush(ਅੰਕੜੇ, ws, FALSE);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;

      ਜੇ(!ਬਫਰ) {
        failf(ਅੰਕੜੇ, "[WS] buffer is NULL in raw mode");
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਜੇ(!ਭੇਜਿਆ) {
        failf(ਅੰਕੜੇ, "[WS] sent is NULL in raw mode");
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਜੇ(fragsize || flags) {
        failf(ਅੰਕੜੇ, "[WS] fragsize and flags must be zero in raw mode");
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਤੀਜਾ = ws_send_raw(ਅੰਕੜੇ, ਬਫਰ, buflen, pnsent);
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    /* Not RAW mode, we do the frame encoding */
    ਨਤੀਜਾ = ws_enc_send(ਅੰਕੜੇ, ws, ਬਫਰ, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(ਅੰਕੜੇ, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ, (ਪੂਰਨ)ਨਤੀਜਾ,
                *pnsent);
  }
ਬਾਹਰਲਾ:
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ws_ਤਿਆਰੀ_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  ਅੰਕੜੇ->state.http_neg.accept_09 = FALSE;
  ਅੰਕੜੇ->state.http_neg.only_10 = FALSE;
  ਅੰਕੜੇ->state.http_neg.wanted = CURL_HTTP_V1x;
  ਅੰਕੜੇ->state.http_neg.allowed = CURL_HTTP_V1x;
  ਵਾਪਸ Curl_http_ਤਿਆਰੀ_conn(ਅੰਕੜੇ, conn);
}

ਅਚਲ ਬਣਤਰ curl_ws_ਚੌਖਟਾ *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
  ਜੇ(GOOD_EASY_HANDLE(ਅੰਕੜੇ) && Curl_api_is_ਵਿੱਚ_callback(ਅੰਕੜੇ) &&
     ਅੰਕੜੇ->conn && !ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ) {
    ਬਣਤਰ websocket *ws;
    ws = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_PROTO_WS_CONN);
    ਜੇ(ws)
      ਵਾਪਸ &ws->recvframe;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

CURL_EXTERN CURLcode curl_ws_ਸ਼ੁਰੂ_ਚੌਖਟਾ(CURL *curl,
                                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                                         curl_off_t ਚੌਖਟਾ_len)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, ws_ਸ਼ੁਰੂ_ਚੌਖਟਾ, &ਨਤੀਜਾ)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    ਬਣਤਰ websocket *ws;

    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ws_raw_ਬਹੁਲਕ) {
      failf(ਅੰਕੜੇ, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      ਨਤੀਜਾ = CURLE_FAILED_INIT;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    CURL_TRC_WS(ਅੰਕੜੇ, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, ਚੌਖਟਾ_len);

    ਜੇ(!ਅੰਕੜੇ->conn) {
      failf(ਅੰਕੜੇ, "[WS] No associated connection");
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ws = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_PROTO_WS_CONN);
    ਜੇ(!ws) {
      failf(ਅੰਕੜੇ, "[WS] Not a websocket transfer");
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(ws->enc.payload_remain) {
      failf(ਅੰਕੜੇ, "[WS] previous frame not finished");
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਨਤੀਜਾ = ws_enc_write_ਸਿਰ(ਅੰਕੜੇ, ws, &ws->enc, flags, ਚੌਖਟਾ_len,
                               &ws->sendbuf);
    ਜੇ(ਨਤੀਜਾ)
      CURL_TRC_WS(ਅੰਕੜੇ, "curl_start_frame(), error adding frame head %d",
                  (ਪੂਰਨ)ਨਤੀਜਾ);
  }
ਬਾਹਰਲਾ:
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_ws = {
  ws_ਤਿਆਰੀ_conn,                        /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_ਪੂਰਾ,                       /* done */
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

#ਨਹੀਂਤਾਂ

CURLcode curl_ws_recv(CURL *curl, ਖਾਲੀ *ਬਫਰ, size_t buflen,
                      size_t *recv,
                      ਅਚਲ ਬਣਤਰ curl_ws_ਚੌਖਟਾ **metap)
{
  (ਖਾਲੀ)curl;
  (ਖਾਲੀ)ਬਫਰ;
  (ਖਾਲੀ)buflen;
  (ਖਾਲੀ)recv;
  (ਖਾਲੀ)metap;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

CURLcode curl_ws_send(CURL *curl, ਅਚਲ ਖਾਲੀ *ਬਫਰ,
                      size_t buflen, size_t *ਭੇਜਿਆ,
                      curl_off_t fragsize,
                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  (ਖਾਲੀ)curl;
  (ਖਾਲੀ)ਬਫਰ;
  (ਖਾਲੀ)buflen;
  (ਖਾਲੀ)ਭੇਜਿਆ;
  (ਖਾਲੀ)fragsize;
  (ਖਾਲੀ)flags;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

ਅਚਲ ਬਣਤਰ curl_ws_ਚੌਖਟਾ *curl_ws_meta(CURL *ਅੰਕੜੇ)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ ਨਿਰਾ;
}

CURL_EXTERN CURLcode curl_ws_ਸ਼ੁਰੂ_ਚੌਖਟਾ(CURL *curl,
                                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                                         curl_off_t ਚੌਖਟਾ_len)
{
  (ਖਾਲੀ)curl;
  (ਖਾਲੀ)flags;
  (ਖਾਲੀ)ਚੌਖਟਾ_len;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_WEBSOCKETS */
