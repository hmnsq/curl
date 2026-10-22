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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "ws.h"

#ifndef CURL_DISABLE_WEBSOCKETS

#அந்தர்பாவ "url.h"
#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "cf-recvbuf.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "easyif.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "curlx/strcopy.h"

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
#define WS_CHUNK_மொத்தம் 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
எண்ணிக்கை ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_HEAD,
  WS_DEC_PAYLOAD
};

கட்டமைப்பு ws_decoder {
  முழு frame_age;        /* zero */
  முழு frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t head[10];
  முழு head_len, head_total;
  எண்ணிக்கை ws_dec_state state;
  முழு cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
கட்டமைப்பு ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  unsigned முழு xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#define WS_MAX_CNTRL_LEN    125

கட்டமைப்பு ws_cntrl_frame {
  unsigned முழு type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
கட்டமைப்பு websocket {
  கட்டமைப்பு Curl_easy *data; /* used for write callback handling */
  கட்டமைப்பு ws_decoder dec;  /* decode of ws frames */
  கட்டமைப்பு ws_encoder enc;  /* encode of ws frames */
  கட்டமைப்பு bufq recvbuf;    /* raw data from the server */
  கட்டமைப்பு bufq sendbuf;    /* raw data to be sent to the server */
  கட்டமைப்பு curl_ws_frame recvframe;  /* the current WS FRAME received */
  கட்டமைப்பு ws_cntrl_frame pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#ifdef CURLVERBOSE
நிலையான மாறிலி எழுத்து *ws_frame_name_of_op(uint8_t firstbyte)
{
  தேர்வு(firstbyte & WSBIT_OPCODE_MASK) {
  நிலை WSBIT_OPCODE_CONT:
    திரும்பு "CONT";
  நிலை WSBIT_OPCODE_TEXT:
    திரும்பு "TEXT";
  நிலை WSBIT_OPCODE_BIN:
    திரும்பு "BIN";
  நிலை WSBIT_OPCODE_CLOSE:
    திரும்பு "CLOSE";
  நிலை WSBIT_OPCODE_PING:
    திரும்பு "PING";
  நிலை WSBIT_OPCODE_PONG:
    திரும்பு "PONG";
  இயல்பு:
    திரும்பு "???";
  }
}
#endif

நிலையான முழு ws_frame_firstbyte2flags(கட்டமைப்பு Curl_easy *data,
                                    uint8_t firstbyte, முழு cont_flags)
{
  தேர்வு(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  நிலை WSBIT_OPCODE_CONT:
    எனில்(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      திரும்பு 0;
    }
    திரும்பு cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  நிலை (WSBIT_OPCODE_CONT | WSBIT_FIN):
    எனில்(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      திரும்பு 0;
    }
    திரும்பு cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  நிலை WSBIT_OPCODE_TEXT:
    எனில்(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      திரும்பு 0;
    }
    திரும்பு CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  நிலை (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    எனில்(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      திரும்பு 0;
    }
    திரும்பு CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  நிலை WSBIT_OPCODE_BIN:
    எனில்(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      திரும்பு 0;
    }
    திரும்பு CURLWS_இருமம் | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  நிலை (WSBIT_OPCODE_BIN | WSBIT_FIN):
    எனில்(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      திரும்பு 0;
    }
    திரும்பு CURLWS_இருமம்;
  /* 0x08 - first CLOSE fragment */
  நிலை WSBIT_OPCODE_CLOSE:
    failf(data, "[WS] invalid fragmented CLOSE frame");
    திரும்பு 0;
  /* 0x88 - unfragmented CLOSE */
  நிலை (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    திரும்பு CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  நிலை WSBIT_OPCODE_PING:
    failf(data, "[WS] invalid fragmented PING frame");
    திரும்பு 0;
  /* 0x89 - unfragmented PING */
  நிலை (WSBIT_OPCODE_PING | WSBIT_FIN):
    திரும்பு CURLWS_PING;
  /* 0x0a - first PONG fragment */
  நிலை WSBIT_OPCODE_PONG:
    failf(data, "[WS] invalid fragmented PONG frame");
    திரும்பு 0;
  /* 0x8a - unfragmented PONG */
  நிலை (WSBIT_OPCODE_PONG | WSBIT_FIN):
    திரும்பு CURLWS_PONG;
  /* invalid first byte */
  இயல்பு:
    எனில்(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(data, "[WS] invalid reserved bits: %02x", firstbyte);
    இல்லையெனில்
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(data, "[WS] invalid opcode: %02x", firstbyte);
    திரும்பு 0;
  }
}

நிலையான CURLcode ws_frame_flags2firstbyte(கட்டமைப்பு Curl_easy *data,
                                         unsigned முழு flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  தேர்வு(flags & ~CURLWS_OFFSET) {
  நிலை 0:
    எனில்(contfragment) {
      CURL_TRC_WS(data, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      திரும்பு CURLE_OK;
    }
    failf(data, "[WS] no flags given");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  நிலை CURLWS_CONT:
    எனில்(contfragment) {
      infof(data, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      திரும்பு CURLE_OK;
    }
    failf(data, "[WS] No ongoing fragmented message to continue");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  நிலை CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    திரும்பு CURLE_OK;
  நிலை (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    திரும்பு CURLE_OK;
  நிலை CURLWS_இருமம்:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    திரும்பு CURLE_OK;
  நிலை (CURLWS_இருமம் | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    திரும்பு CURLE_OK;
  நிலை CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    திரும்பு CURLE_OK;
  நிலை (CURLWS_CLOSE | CURLWS_CONT):
    failf(data, "[WS] CLOSE frame must not be fragmented");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  நிலை CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    திரும்பு CURLE_OK;
  நிலை (CURLWS_PING | CURLWS_CONT):
    failf(data, "[WS] PING frame must not be fragmented");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  நிலை CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    திரும்பு CURLE_OK;
  நிலை (CURLWS_PONG | CURLWS_CONT):
    failf(data, "[WS] PONG frame must not be fragmented");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  இயல்பு:
    failf(data, "[WS] unknown flags: %x", flags);
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

நிலையான வெறுமை ws_dec_info(கட்டமைப்பு ws_decoder *dec, கட்டமைப்பு Curl_easy *data,
                        மாறிலி எழுத்து *செய்தி)
{
  NOVERBOSE((வெறுமை)செய்தி);
  தேர்வு(dec->head_len) {
  நிலை 0:
    நிறுத்து;
  நிலை 1:
    CURL_TRC_WS(data, "decoded %s [%s%s]", செய்தி,
                ws_frame_name_of_op(dec->head[0]),
                (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    நிறுத்து;
  இயல்பு:
    எனில்(dec->head_len < dec->head_total) {
      CURL_TRC_WS(data, "decoded %s [%s%s](%d/%d)", செய்தி,
                  ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->head_len, dec->head_total);
    }
    இல்லையெனில் {
      CURL_TRC_WS(data, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  செய்தி, ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    நிறுத்து;
  }
}

நிலையான CURLcode ws_send_raw_blocking(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு websocket *ws,
                                     மாறிலி எழுத்து *buffer, size_t buflen);

வகைவரை CURLcode ws_write_payload(மாறிலி uint8_t *buf, size_t buflen,
                                  முழு frame_age, முழு frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  வெறுமை *userp,
                                  size_t *pnwritten);

நிலையான வெறுமை ws_dec_next_frame(கட்டமைப்பு ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

நிலையான வெறுமை ws_dec_reset(கட்டமைப்பு ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

நிலையான வெறுமை ws_dec_init(கட்டமைப்பு ws_decoder *dec)
{
  ws_dec_reset(dec);
}

நிலையான CURLcode ws_dec_read_head(கட்டமைப்பு ws_decoder *dec,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு bufq *inraw)
{
  மாறிலி uint8_t *inbuf;
  size_t inlen;

  போது(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    எனில்(dec->head_len == 0) {
      dec->head[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(data, dec->head[0],
                                                  dec->cont_flags);
      எனில்(!dec->frame_flags) {
        ws_dec_reset(dec);
        திரும்பு CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      எனில்(dec->frame_flags & (CURLWS_TEXT | CURLWS_இருமம்)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->head_len = 1;
#எனில் 0
      ws_dec_info(dec, data, "seeing opcode");
#endif
      தொடர்;
    }
    இல்லையெனில் எனில்(dec->head_len == 1) {
      dec->head[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->head_len = 2;

      எனில்(dec->head[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(data, "[WS] masked input frame");
        ws_dec_reset(dec);
        திரும்பு CURLE_RECV_ERROR;
      }
      எனில்(dec->frame_flags & CURLWS_PING && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(data, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        திரும்பு CURLE_RECV_ERROR;
      }
      எனில்(dec->frame_flags & CURLWS_PONG && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(data, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        திரும்பு CURLE_RECV_ERROR;
      }
      எனில்(dec->frame_flags & CURLWS_CLOSE && dec->head[1] > WS_MAX_CNTRL_LEN) {
        failf(data, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        திரும்பு CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      எனில்(dec->head[1] == 126) {
        dec->head_total = 4;
        தொடர்;
      }
      இல்லையெனில் எனில்(dec->head[1] == 127) {
        dec->head_total = 10;
        தொடர்;
      }
      இல்லையெனில் {
        dec->head_total = 2;
      }
    }

    எனில்(dec->head_len < dec->head_total) {
      dec->head[dec->head_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->head_len;
      எனில்(dec->head_len < dec->head_total) {
#எனில் 0
        ws_dec_info(dec, data, "decoding head");
#endif
        தொடர்;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->head_len == dec->head_total);
    தேர்வு(dec->head_total) {
    நிலை 2:
      dec->payload_len = dec->head[1];
      நிறுத்து;
    நிலை 4:
      dec->payload_len = (dec->head[2] << 8) | dec->head[3];
      நிறுத்து;
    நிலை 10:
      எனில்(dec->head[2] > 127) {
        failf(data, "[WS] frame length longer than 63 bits not supported");
        திரும்பு CURLE_RECV_ERROR;
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
      நிறுத்து;
    இயல்பு:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(data, "[WS] unexpected frame header length");
      திரும்பு CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, data, "head");
    திரும்பு CURLE_OK;
  }
  திரும்பு CURLE_AGAIN;
}

நிலையான CURLcode ws_dec_pass_payload(கட்டமைப்பு ws_decoder *dec,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு bufq *inraw,
                                    ws_write_payload *write_cb,
                                    வெறுமை *write_ctx)
{
  மாறிலி uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode result;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  போது(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(data)) {
    எனில்(inlen > remain)
      inlen = remain;
    result = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    எனில்(result)
      திரும்பு result;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(data, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  திரும்பு remain ? CURLE_AGAIN : CURLE_OK;
}

நிலையான CURLcode ws_dec_pass(கட்டமைப்பு ws_decoder *dec,
                            கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு bufq *inraw,
                            ws_write_payload *write_cb,
                            வெறுமை *write_ctx)
{
  CURLcode result;

  எனில்(Curl_bufq_is_empty(inraw))
    திரும்பு CURLE_AGAIN;

  தேர்வு(dec->state) {
  நிலை WS_DEC_INIT:
    ws_dec_next_frame(dec);
    dec->state = WS_DEC_HEAD;
    FALLTHROUGH();
  நிலை WS_DEC_HEAD:
    result = ws_dec_read_head(dec, data, inraw);
    எனில்(result) {
      எனில்(result != CURLE_AGAIN) {
        failf(data, "[WS] decode frame error %d", (முழு)result);
        நிறுத்து;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      நிறுத்து;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    எனில்(dec->payload_len == 0) {
      size_t nwritten;
      மாறிலி uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      result = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      எனில்(result)
        திரும்பு result;
      dec->state = WS_DEC_INIT;
      நிறுத்து;
    }
    FALLTHROUGH();
  நிலை WS_DEC_PAYLOAD:
    result = ws_dec_pass_payload(dec, data, inraw, write_cb, write_ctx);
    ws_dec_info(dec, data, "passing");
    எனில்(result)
      திரும்பு result;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    நிறுத்து;
  இயல்பு:
    /* we covered all enums above, but some code analyzers are wimps */
    result = CURLE_FAILED_INIT;
  }
  திரும்பு result;
}

நிலையான வெறுமை update_meta(கட்டமைப்பு websocket *ws,
                        முழு frame_age, முழு frame_flags,
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
கட்டமைப்பு ws_cw_ctx {
  கட்டமைப்பு Curl_cwriter super;
  கட்டமைப்பு bufq buf;
};

நிலையான CURLcode ws_cw_init(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு ws_cw_ctx *ctx = writer->ctx;
  (வெறுமை)data;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_SIZE, 1, BUFQ_OPT_SOFT_LIMIT);
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை ws_cw_close(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு ws_cw_ctx *ctx = writer->ctx;
  (வெறுமை)data;
  Curl_bufq_free(&ctx->buf);
}

கட்டமைப்பு ws_cw_dec_ctx {
  கட்டமைப்பு Curl_easy *data;
  கட்டமைப்பு websocket *ws;
  கட்டமைப்பு Curl_cwriter *next_writer;
  முழு cw_type;
};

நிலையான CURLcode ws_flush(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு websocket *ws,
                         bool blocking);
நிலையான CURLcode ws_enc_add_pending(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு websocket *ws);

நிலையான CURLcode ws_enc_add_cntrl(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு websocket *ws,
                                 மாறிலி uint8_t *payload,
                                 size_t plen,
                                 unsigned முழு frame_type)
{
  (வெறுமை)data;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  எனில்(plen > WS_MAX_CNTRL_LEN)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  memcpy(ws->pending.payload, payload, plen);
  திரும்பு CURLE_OK;
}

நிலையான curl_off_t ws_payload_remain(curl_off_t payload_total,
                                    curl_off_t payload_offset,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_total - payload_offset;
  எனில்((payload_total < 0) || (payload_offset < 0) || (remain < 0))
    திரும்பு -1;
  buffered = curlx_uztoso(payload_buffered);
  எனில்(remain < buffered)
    திரும்பு -1;
  திரும்பு remain - buffered;
}

நிலையான CURLcode ws_cw_dec_next(மாறிலி uint8_t *buf, size_t buflen,
                               முழு frame_age, முழு frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               வெறுமை *user_data,
                               size_t *pnwritten)
{
  கட்டமைப்பு ws_cw_dec_ctx *ctx = user_data;
  கட்டமைப்பு Curl_easy *data = ctx->data;
  கட்டமைப்பு websocket *ws = ctx->ws;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result;

  (வெறுமை)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  எனில்(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  எனில்(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(data, ws, buf, buflen, CURLWS_PONG);
    எனில்(result)
      திரும்பு result;
  }
  இல்லையெனில் எனில்(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(data, "[WS] pass %zu decoded bytes", buflen);
    result = Curl_cwriter_write(data, ctx->next_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (மாறிலி எழுத்து *)buf, buflen);
    எனில்(result)
      திரும்பு result;
  }
  *pnwritten = buflen;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ws_cw_write(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_cwriter *writer, முழு type,
                            மாறிலி எழுத்து *buf, size_t nbytes)
{
  கட்டமைப்பு ws_cw_ctx *ctx = writer->ctx;
  கட்டமைப்பு websocket *ws;
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[WS] write(len=%zu, type=%d)", nbytes, type);
  எனில்(!(type & CLIENTWRITE_BODY) || data->set.ws_raw_mode)
    திரும்பு Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  எனில்(!ws) {
    failf(data, "[WS] not a websocket transfer");
    திரும்பு CURLE_FAILED_INIT;
  }

  எனில்(nbytes) {
    size_t nwritten;
    result = Curl_bufq_write(&ctx->buf, (மாறிலி uint8_t *)buf,
                             nbytes, &nwritten);
    எனில்(result) {
      infof(data, "[WS] error adding data to buffer %d", (முழு)result);
      திரும்பு result;
    }
  }

  result = Curl_cwriter_flush(data, writer->next);
  எனில்(result)
    செல் out;

  போது(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
    கட்டமைப்பு ws_cw_dec_ctx pass_ctx;
    pass_ctx.data = data;
    pass_ctx.ws = ws;
    pass_ctx.next_writer = writer->next;
    pass_ctx.cw_type = type;
    result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                         ws_cw_dec_next, &pass_ctx);
    எனில்(result == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      result = CURLE_OK;
      செல் out;
    }
    இல்லையெனில் எனில்(result) {
      failf(data, "[WS] decode payload error %d", (முழு)result);
      Curl_bufq_reset(&ctx->buf);
      செல் out;
    }
  }

out:
  எனில்(!result && !ws->using_reader) {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    எனில்(result == CURLE_AGAIN)
      result = CURLE_OK;
  }

  எனில்(!result && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(data, "[WS] decode ended with incomplete frame");
    result = CURLE_RECV_ERROR;
  }
  திரும்பு result;
}

நிலையான CURLcode ws_cw_flush(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_cwriter *writer)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[ws] flush");
  எனில்(!data->set.ws_raw_mode) {
    கட்டமைப்பு ws_cw_ctx *ctx = writer->ctx;
    கட்டமைப்பு websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    result = Curl_cwriter_flush(data, writer->next);
    எனில்(result)
      செல் out;

    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    எனில்(!ws) {
      failf(data, "[WS] not a websocket transfer");
      திரும்பு CURLE_FAILED_INIT;
    }

    போது(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
      கட்டமைப்பு ws_cw_dec_ctx pass_ctx;
      pass_ctx.data = data;
      pass_ctx.ws = ws;
      pass_ctx.next_writer = writer->next;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                           ws_cw_dec_next, &pass_ctx);
      எனில்(result == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        result = CURLE_OK;
        செல் out;
      }
      இல்லையெனில் எனில்(result) {
        failf(data, "[WS] decode payload error %d", (முழு)result);
        Curl_bufq_reset(&ctx->buf);
        செல் out;
      }
    }
  }

out:
  எனில்(!result)
    result = Curl_cwriter_flush(data, writer->next);
  திரும்பு result;
}

/* WebSocket payload decoding client writer. */
நிலையான மாறிலி கட்டமைப்பு Curl_cwtype ws_cw_decode = {
  "ws-decode",
  வெற்று,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  அளவு(கட்டமைப்பு ws_cw_ctx)
};

நிலையான வெறுமை ws_enc_info(கட்டமைப்பு ws_encoder *enc, கட்டமைப்பு Curl_easy *data,
                        மாறிலி எழுத்து *செய்தி)
{
  NOVERBOSE((வெறுமை)enc);
  NOVERBOSE((வெறுமை)செய்தி);
  CURL_TRC_WS(data, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              செய்தி, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

நிலையான வெறுமை ws_enc_reset(கட்டமைப்பு ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

நிலையான வெறுமை ws_enc_init(கட்டமைப்பு ws_encoder *enc)
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

நிலையான CURLcode ws_enc_add_frame(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு ws_encoder *enc,
                                 unsigned முழு flags,
                                 curl_off_t payload_len,
                                 கட்டமைப்பு bufq *out)
{
  uint8_t firstb = 0;
  uint8_t head[14];
  CURLcode result;
  size_t hlen, nwritten;

  எனில்(payload_len < 0) {
    failf(data, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    திரும்பு CURLE_SEND_ERROR;
  }

  எனில்(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(data, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    திரும்பு CURLE_SEND_ERROR;
  }

  result = ws_frame_flags2firstbyte(data, flags, (bool)enc->contfragment,
                                    &firstb);
  எனில்(result)
    திரும்பு result;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  எனில்(flags & (CURLWS_TEXT | CURLWS_இருமம்)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  எனில்(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PING frame is too big");
    திரும்பு CURLE_TOO_LARGE;
  }
  எனில்(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PONG frame is too big");
    திரும்பு CURLE_TOO_LARGE;
  }
  எனில்(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given CLOSE frame is too big");
    திரும்பு CURLE_TOO_LARGE;
  }

  head[0] = enc->firstbyte = firstb;
  எனில்(payload_len > 65535) {
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
  இல்லையெனில் எனில்(payload_len >= 126) {
    head[1] = 126 | WSBIT_MASK;
    head[2] = (uint8_t)((payload_len >> 8) & 0xff);
    head[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  இல்லையெனில் {
    head[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, data, "sending");

  /* 4 bytes random */

  result = Curl_rand(data, (uint8_t *)&enc->mask, அளவு(enc->mask));
  எனில்(result)
    திரும்பு result;

#ifdef DEBUGBUILD
  எனில்(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->mask, 0, அளவு(enc->mask));
#endif

  /* add 4 bytes mask */
  memcpy(&head[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  result = Curl_bufq_write(out, head, hlen, &nwritten);
  எனில்(result)
    திரும்பு result;
  எனில்(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    திரும்பு CURLE_SEND_ERROR;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ws_enc_write_head(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு websocket *ws,
                                  கட்டமைப்பு ws_encoder *enc,
                                  unsigned முழு flags,
                                  curl_off_t payload_len,
                                  கட்டமைப்பு bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  எனில்(ws->pending.type) {
    CURLcode result = ws_enc_add_pending(data, ws);
    எனில்(result)
      திரும்பு result;
  }
  திரும்பு ws_enc_add_frame(data, enc, flags, payload_len, out);
}

நிலையான CURLcode ws_enc_write_payload(கட்டமைப்பு ws_encoder *enc,
                                     கட்டமைப்பு Curl_easy *data,
                                     மாறிலி uint8_t *buf, size_t buflen,
                                     கட்டமைப்பு bufq *out, size_t *pnwritten)
{
  CURLcode result;
  size_t i, len, n, remain;

  *pnwritten = 0;
  எனில்(Curl_bufq_is_full(out))
    திரும்பு CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  எனில்(remain < len)
    len = remain;

  க்கு(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    result = Curl_bufq_write(out, &c, 1, &n);
    எனில்(result) {
      எனில்((result != CURLE_AGAIN) || !i)
        திரும்பு result;
      நிறுத்து;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, data, "buffered");
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ws_enc_add_pending(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு websocket *ws)
{
  CURLcode result;
  size_t n;

  எனில்(!ws->pending.type) /* no pending frame here */
    திரும்பு CURLE_OK;
  எனில்(ws->enc.payload_remain) /* in the middle of another frame */
    திரும்பு CURLE_AGAIN;

  result = ws_enc_add_frame(data, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  எனில்(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding head: %d",
                (முழு)result);
    செல் out;
  }
  result = ws_enc_write_payload(&ws->enc, data, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  எனில்(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding payload: %d",
                (முழு)result);
    செல் out;
  }
  எனில்(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(data, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    result = CURLE_SEND_ERROR;
    செல் out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, அளவு(ws->pending));

out:
  திரும்பு result;
}

நிலையான CURLcode ws_enc_send(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு websocket *ws,
                            மாறிலி uint8_t *buffer,
                            size_t buflen,
                            curl_off_t fragsize,
                            unsigned முழு flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!data->set.ws_raw_mode);
  *pnsent = 0;

  எனில்(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    எனில்(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(data, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
    எனில்((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(data, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  இல்லையெனில் {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    எனில்(result)
      திரும்பு result;

    result = ws_enc_write_head(data, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    எனில்(result) {
      CURL_TRC_WS(data, "curl_ws_send(), error writing frame head %d",
                  (முழு)result);
      திரும்பு result;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  போது(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    எனில்(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      result = ws_enc_write_payload(&ws->enc, data,
                                    buffer + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      எனில்(result && (result != CURLE_AGAIN))
        திரும்பு result;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      எனில்(!ws->sendbuf_payload) {
        திரும்பு CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    எனில்(!result && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      buffer += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    இல்லையெனில் எனில்(result == CURLE_AGAIN) {
      எனில்(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        திரும்பு CURLE_OK;
      }
      இல்லையெனில் {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(data, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        திரும்பு CURLE_AGAIN;
      }
    }
    இல்லையெனில்
      திரும்பு result;  /* real error sending the data */
  }
  திரும்பு CURLE_OK;
}

கட்டமைப்பு cr_ws_ctx {
  கட்டமைப்பு Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

நிலையான CURLcode cr_ws_init(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_creader *reader)
{
  (வெறுமை)data;
  (வெறுமை)reader;
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cr_ws_close(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_creader *reader)
{
  (வெறுமை)data;
  (வெறுமை)reader;
}

நிலையான CURLcode cr_ws_read(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_creader *reader,
                           எழுத்து *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  கட்டமைப்பு cr_ws_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, n;
  கட்டமைப்பு websocket *ws;
  bool eos;

  *pnread = 0;
  எனில்(ctx->eos) {
    *peos = TRUE;
    திரும்பு CURLE_OK;
  }

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  எனில்(!ws) {
    failf(data, "[WS] not a websocket transfer");
    திரும்பு CURLE_FAILED_INIT;
  }

  எனில்(Curl_bufq_is_empty(&ws->sendbuf)) {
    எனில்(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      திரும்பு CURLE_OK;
    }

    எனில்(ws->enc.payload_remain) {
      CURL_TRC_WS(data, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    எனில்(result)
      திரும்பு result;
    ctx->read_eos = eos;

    எனில்(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(data, reader->next);
    }
    இல்லையெனில் எனில்(!nread) {
      /* nothing to convert, return this right away */
      எனில்(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      செல் out;
    }

    எனில்(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      result = ws_enc_write_head(data, ws, &ws->enc, CURLWS_இருமம், nread,
                                 &ws->sendbuf);
      எனில்(result)
        செல் out;
    }

    result = ws_enc_write_payload(&ws->enc, data, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    எனில்(result)
      செல் out;
    CURL_TRC_READ(data, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  எனில்(!result && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (முழு)result, *pnread, *peos);
  திரும்பு result;
}

நிலையான curl_off_t cr_ws_total_length(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (வெறுமை)data;
  (வெறுமை)reader;
  திரும்பு -1;
}

நிலையான மாறிலி கட்டமைப்பு Curl_crtype ws_cr_encode = {
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
  அளவு(கட்டமைப்பு cr_ws_ctx)
};

கட்டமைப்பு wsfield {
  மாறிலி எழுத்து *name;
  மாறிலி எழுத்து *val;
};

CURLcode Curl_ws_request(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு dynbuf *req)
{
  unsigned முழு i;
  CURLcode result = CURLE_OK;
  uint8_t rand[16];
  எழுத்து *randstr;
  size_t randlen;
  எழுத்து keyval[40];
  கட்டமைப்பு SingleRequest *k = &data->req;
  கட்டமைப்பு wsfield heads[] = {
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
      "Sec-WebSocket-Key", வெற்று,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  result = Curl_rand(data, rand, அளவு(rand));
  எனில்(result)
    திரும்பு result;
  result = curlx_base64_encode(rand, அளவு(rand), &randstr, &randlen);
  எனில்(result)
    திரும்பு result;
  DEBUGASSERT(randlen < அளவு(keyval));
  எனில்(randlen >= அளவு(keyval)) {
    curlx_free(randstr);
    திரும்பு CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, அளவு(keyval), randstr, randlen);
  curlx_free(randstr);
  க்கு(i = 0; !result && (i < CURL_ARRAYSIZE(heads)); i++) {
    எனில்(!Curl_checkheaders(data, heads[i].name, strlen(heads[i].name))) {
      result = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  data->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  data->conn->bits.upgrade_in_progress = TRUE;
  திரும்பு result;
}

நிலையான வெறுமை ws_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு websocket *ws = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(கட்டமைப்பு Curl_easy *data,
                        மாறிலி எழுத்து *mem, size_t nread)
{
  கட்டமைப்பு SingleRequest *k = &data->req;
  கட்டமைப்பு websocket *ws;
  கட்டமைப்பு Curl_cwriter *ws_dec_writer = வெற்று;
  கட்டமைப்பு Curl_creader *ws_enc_reader = வெற்று;
  CURLcode result;

  DEBUGASSERT(data->conn);
  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  எனில்(!ws) {
    size_t chunk_size = WS_CHUNK_SIZE;
    ws = curlx_calloc(1, அளவு(*ws));
    எனில்(!ws)
      திரும்பு CURLE_OUT_OF_MEMORY;
#ifdef DEBUGBUILD
    {
      மாறிலி எழுத்து *p = getenv("CURL_WS_CHUNK_SIZE");
      எனில்(p) {
        curl_off_t l;
        எனில்(!curlx_str_எண்(&p, &l, 1 * 1024 * 1024))
          chunk_size = (size_t)l;
      }
    }
#endif
    CURL_TRC_WS(data, "WS, using chunk size %zu", chunk_size);
    Curl_bufq_init2(&ws->recvbuf, chunk_size, WS_CHUNK_மொத்தம்,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_size, WS_CHUNK_மொத்தம்,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    result = Curl_conn_meta_set(data->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    எனில்(result)
      திரும்பு result;
  }
  இல்லையெனில் {
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
  எனில்(result)
    செல் out;
  result = Curl_cwriter_add(data, ws_dec_writer);
  எனில்(result)
    செல் out;
  ws_dec_writer = வெற்று; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  எனில்(data->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    எனில்(nread) {
      result = Curl_cf_recvbuf_add(data, data->conn, FIRSTSOCKET,
                                   (மாறிலி uint8_t *)mem, nread);
      எனில்(result)
        செல் out;
    }
    CURL_REQ_CLEAR_RECV(data); /* read no more content */
  }
  இல்லையெனில் { /* !connect_only */
    எனில்(data->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(data, "UPLOAD set, add ws-encode reader");
      result = Curl_creader_set_fread(data, -1);
      எனில்(result)
        செல் out;

      எனில்(!data->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        result = Curl_creader_create(&ws_enc_reader, data, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        எனில்(result)
          செல் out;
        result = Curl_creader_add(data, ws_enc_reader);
        எனில்(result)
          செல் out;
        ws_enc_reader = வெற்று; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      data->req.eos_read = FALSE;
      data->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(data);
    }

    /* Then pass any additional data to the writers */
    எனில்(nread) {
      result = Curl_client_write(data, CLIENTWRITE_BODY, mem, nread);
      எனில்(result)
        செல் out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  எனில்(ws_dec_writer)
    Curl_cwriter_free(data, ws_dec_writer);
  எனில்(ws_enc_reader)
    Curl_creader_free(data, ws_enc_reader);
  எனில்(result)
    CURL_TRC_WS(data, "Curl_ws_accept() failed -> %d", (முழு)result);
  இல்லையெனில்
    CURL_TRC_WS(data, "websocket established, %s mode",
                data->set.connect_only ? "connect-only" : "callback");
  திரும்பு result;
}

கட்டமைப்பு ws_collect {
  கட்டமைப்பு Curl_easy *data;
  கட்டமைப்பு websocket *ws;
  uint8_t *buffer;
  size_t buflen;
  size_t bufidx;
  முழு frame_age;
  முழு frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

நிலையான CURLcode ws_client_collect(மாறிலி uint8_t *buf, size_t buflen,
                                  முழு frame_age, முழு frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  வெறுமை *userp,
                                  size_t *pnwritten)
{
  கட்டமைப்பு ws_collect *ctx = userp;
  கட்டமைப்பு Curl_easy *data = ctx->data;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  எனில்(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  எனில்(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  எனில்(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(ctx->data, ctx->ws, buf, buflen, CURLWS_PONG);
    எனில்(result)
      திரும்பு result;
    *pnwritten = buflen;
  }
  இல்லையெனில் {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    எனில்(!write_len) {
      எனில்(!buflen)  /* 0 length write, we accept that */
        திரும்பு CURLE_OK;
      திரும்பு CURLE_AGAIN;  /* no more space */
    }
    memcpy(ctx->buffer + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  திரும்பு result;
}

நிலையான CURLcode nw_in_recv(வெறுமை *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  கட்டமைப்பு Curl_easy *data = reader_ctx;
  திரும்பு Curl_easy_recv(data, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, வெறுமை *buffer,
                      size_t buflen, size_t *recv,
                      மாறிலி கட்டமைப்பு curl_ws_frame **metap)
{
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  எனில்(CURL_EAPI_ENTER(&guard, curl, ws_recv, &result)) {
    கட்டமைப்பு Curl_easy *data = curl;
    கட்டமைப்பு connectdata *conn;
    கட்டமைப்பு websocket *ws;
    கட்டமைப்பு ws_collect ctx;

    எனில்(!recv || !metap || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      செல் out;
    }

    *recv = 0;
    *metap = வெற்று;

    conn = data->conn;
    எனில்(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      எனில்(!data->set.connect_only) {
        failf(data, "[WS] CONNECT_ONLY is required");
        result = CURLE_UNSUPPORTED_PROTOCOL;
        செல் out;
      }

      Curl_getconnectinfo(data, &conn);
      எனில்(!conn) {
        failf(data, "[WS] connection not found");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        செல் out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    எனில்(!ws) {
      failf(data, "[WS] connection is not setup for websocket");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      செல் out;
    }

    memset(&ctx, 0, அளவு(ctx));
    ctx.data = data;
    ctx.ws = ws;
    ctx.buffer = buffer;
    ctx.buflen = buflen;

    போது(1) {
      /* receive more when our buffer is empty */
      எனில்(Curl_bufq_is_empty(&ws->recvbuf)) {
        size_t n;
        result = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, data, &n);
        எனில்(result)
          செல் out;
        இல்லையெனில் எனில்(n == 0) {
          /* connection closed */
          infof(data, "[WS] connection expectedly closed?");
          result = CURLE_GOT_NOTHING;
          செல் out;
        }
        CURL_TRC_WS(data, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      result = ws_dec_pass(&ws->dec, data, &ws->recvbuf,
                           ws_client_collect, &ctx);
      எனில்(result == CURLE_AGAIN) {
        எனில்(!ctx.written) {
          ws_dec_info(&ws->dec, data, "need more input");
          தொடர்;  /* nothing written, try more input */
        }
        நிறுத்து;
      }
      இல்லையெனில் எனில்(result) {
        செல் out;
      }
      இல்லையெனில் எனில்(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        நிறுத்து;
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
    எனில்(!data->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(data, ws);
      எனில்(!r2)
        (வெறுமை)ws_flush(data, ws, Curl_api_is_in_callback(data));
    }
    result = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}

நிலையான CURLcode ws_flush(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு websocket *ws,
                         bool blocking)
{
  CURLcode result;

  /* If there is space, add any pending control frame */
  எனில்(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_size) {
    result = ws_enc_add_pending(data, ws);
    எனில்(result && (result != CURLE_AGAIN))
      திரும்பு result;
  }

  எனில்(!Curl_bufq_is_empty(&ws->sendbuf)) {
    மாறிலி uint8_t *out;
    size_t outlen, n;
#ifdef DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_next = FALSE;
    size_t chunk_egain = 0;
    மாறிலி எழுத்து *p = getenv("CURL_WS_CHUNK_EAGAIN");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#endif

    போது(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#ifdef DEBUGBUILD
      எனில்(eagain_next)
        திரும்பு CURLE_AGAIN;
      எனில்(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_next = TRUE;
      }
#endif
      எனில்(blocking) {
        result = ws_send_raw_blocking(data, ws, (மாறிலி எழுத்து *)out, outlen);
        n = result ? 0 : outlen;
      }
      இல்லையெனில் எனில்(data->set.connect_only || Curl_api_is_in_callback(data))
        result = Curl_senddata(data, out, outlen, &n);
      இல்லையெனில் {
        result = Curl_xfer_send(data, out, outlen, FALSE, &n);
        எனில்(!result && !n && outlen)
          result = CURLE_AGAIN;
      }

      எனில்(result == CURLE_AGAIN) {
        CURL_TRC_WS(data, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        திரும்பு result;
      }
      இல்லையெனில் எனில்(result) {
        failf(data, "[WS] flush, write error %d", (முழு)result);
        திரும்பு result;
      }
      இல்லையெனில் {
        CURL_TRC_WS(data, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ws_send_raw_blocking(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு websocket *ws,
                                     மாறிலி எழுத்து *buffer, size_t buflen)
{
  CURLcode result = CURLE_OK;
  size_t nwritten;

  எனில்(!data)
    திரும்பு result;

  (வெறுமை)ws;
  போது(buflen) {
    result = Curl_xfer_send(data, buffer, buflen, FALSE, &nwritten);
    எனில்(result)
      திரும்பு result;
    DEBUGASSERT(nwritten <= buflen);
    buffer += nwritten;
    buflen -= nwritten;
    எனில்(buflen) {
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      timediff_t இடது_ms;
      முழு ev;

      CURL_TRC_WS(data, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      இடது_ms = Curl_timeleft_ms(data);
      எனில்(இடது_ms < 0) {
        failf(data, "[WS] Timeout waiting for socket becoming writable");
        திரும்பு CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      எனில்(sock == CURL_SOCKET_BAD)
        திரும்பு CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, இடது_ms ? இடது_ms : 500);
      எனில்(ev < 0) {
        failf(data, "[WS] Error while waiting for socket becoming writable");
        திரும்பு CURLE_SEND_ERROR;
      }
    }
  }
  திரும்பு result;
}

நிலையான CURLcode ws_send_raw(கட்டமைப்பு Curl_easy *data, மாறிலி வெறுமை *buffer,
                            size_t buflen, size_t *pnwritten)
{
  கட்டமைப்பு websocket *ws;
  CURLcode result;

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  எனில்(!ws) {
    failf(data, "[WS] Not a websocket transfer");
    திரும்பு CURLE_SEND_ERROR;
  }
  எனில்(!buflen)
    திரும்பு CURLE_OK;

  எனில்(Curl_api_is_in_callback(data)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    result = ws_flush(data, ws, TRUE);
    எனில்(result)
      திரும்பு result;
    result = ws_send_raw_blocking(data, ws, buffer, buflen);
    எனில்(!result)
      *pnwritten = buflen;
  }
  இல்லையெனில் {
    /* We need any pending data to be sent or EAGAIN this call. */
    result = ws_flush(data, ws, FALSE);
    எனில்(result)
      திரும்பு result;
    result = Curl_senddata(data, buffer, buflen, pnwritten);
  }

  CURL_TRC_WS(data, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (முழு)result, *pnwritten);
  திரும்பு result;
}

CURLcode curl_ws_send(CURL *curl, மாறிலி வெறுமை *buffer_arg,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned முழு flags)
{
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  எனில்(CURL_EAPI_ENTER(&guard, curl, ws_send, &result)) {
    கட்டமைப்பு websocket *ws;
    மாறிலி uint8_t *buffer = buffer_arg;
    கட்டமைப்பு Curl_easy *data = curl;
    size_t ndummy;
    size_t *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, data->set.ws_raw_mode);

    *pnsent = 0;

    எனில்(!buffer && buflen) {
      failf(data, "[WS] buffer is NULL when buflen is not");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      செல் out;
    }

    எனில்(!data->conn && data->set.connect_only) {
      result = Curl_connect_only_attach(data);
      எனில்(result)
        செல் out;
    }
    எனில்(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      செல் out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    எனில்(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      செல் out;
    }

    எனில்(data->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      result = ws_flush(data, ws, FALSE);
      எனில்(result)
        செல் out;

      எனில்(!buffer) {
        failf(data, "[WS] buffer is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        செல் out;
      }
      எனில்(!sent) {
        failf(data, "[WS] sent is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        செல் out;
      }
      எனில்(fragsize || flags) {
        failf(data, "[WS] fragsize and flags must be zero in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        செல் out;
      }
      result = ws_send_raw(data, buffer, buflen, pnsent);
      செல் out;
    }

    /* Not RAW mode, we do the frame encoding */
    result = ws_enc_send(data, ws, buffer, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, data->set.ws_raw_mode, (முழு)result,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}

நிலையான CURLcode ws_setup_conn(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  data->state.http_neg.accept_09 = FALSE;
  data->state.http_neg.only_10 = FALSE;
  data->state.http_neg.wanted = CURL_HTTP_V1x;
  data->state.http_neg.allowed = CURL_HTTP_V1x;
  திரும்பு Curl_http_setup_conn(data, conn);
}

மாறிலி கட்டமைப்பு curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  கட்டமைப்பு Curl_easy *data = curl;
  எனில்(GOOD_EASY_HANDLE(data) && Curl_api_is_in_callback(data) &&
     data->conn && !data->set.ws_raw_mode) {
    கட்டமைப்பு websocket *ws;
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    எனில்(ws)
      திரும்பு &ws->recvframe;
  }
  திரும்பு வெற்று;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned முழு flags,
                                         curl_off_t frame_len)
{
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  எனில்(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &result)) {
    கட்டமைப்பு Curl_easy *data = curl;
    கட்டமைப்பு websocket *ws;

    எனில்(data->set.ws_raw_mode) {
      failf(data, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      result = CURLE_FAILED_INIT;
      செல் out;
    }

    CURL_TRC_WS(data, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    எனில்(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      செல் out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    எனில்(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      செல் out;
    }

    எனில்(ws->enc.payload_remain) {
      failf(data, "[WS] previous frame not finished");
      result = CURLE_SEND_ERROR;
      செல் out;
    }

    result = ws_enc_write_head(data, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    எனில்(result)
      CURL_TRC_WS(data, "curl_start_frame(), error adding frame head %d",
                  (முழு)result);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}

மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_ws = {
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

#இல்லையெனில்

CURLcode curl_ws_recv(CURL *curl, வெறுமை *buffer, size_t buflen,
                      size_t *recv,
                      மாறிலி கட்டமைப்பு curl_ws_frame **metap)
{
  (வெறுமை)curl;
  (வெறுமை)buffer;
  (வெறுமை)buflen;
  (வெறுமை)recv;
  (வெறுமை)metap;
  திரும்பு CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, மாறிலி வெறுமை *buffer,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned முழு flags)
{
  (வெறுமை)curl;
  (வெறுமை)buffer;
  (வெறுமை)buflen;
  (வெறுமை)sent;
  (வெறுமை)fragsize;
  (வெறுமை)flags;
  திரும்பு CURLE_NOT_BUILT_IN;
}

மாறிலி கட்டமைப்பு curl_ws_frame *curl_ws_meta(CURL *data)
{
  (வெறுமை)data;
  திரும்பு வெற்று;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned முழு flags,
                                         curl_off_t frame_len)
{
  (வெறுமை)curl;
  (வெறுமை)flags;
  (வெறுமை)frame_len;
  திரும்பு CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
