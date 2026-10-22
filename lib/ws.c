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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "ws.h"

#ifndef CURL_DISABLE_WEBSOCKETS

#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "curlx/base64.h"
#અન્તર્ભાવ "cf-recvbuf.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "easyif.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curlx/strcopy.h"

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
#define WS_CHUNK_ગણતરી 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
ગણના ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_HEAD,
  WS_DEC_PAYLOAD
};

સંરચના ws_decoder {
  પૂર્ણાંક frame_age;        /* zero */
  પૂર્ણાંક frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t head[10];
  પૂર્ણાંક head_len, head_total;
  ગણના ws_dec_state state;
  પૂર્ણાંક cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
સંરચના ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  unsigned પૂર્ણાંક xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#define WS_MAX_CNTRL_LEN    125

સંરચના ws_cntrl_frame {
  unsigned પૂર્ણાંક type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
સંરચના websocket {
  સંરચના Curl_easy *data; /* used for write callback handling */
  સંરચના ws_decoder dec;  /* decode of ws frames */
  સંરચના ws_encoder enc;  /* encode of ws frames */
  સંરચના bufq recvbuf;    /* raw data from the server */
  સંરચના bufq sendbuf;    /* raw data to be sent to the server */
  સંરચના curl_ws_frame recvframe;  /* the current WS FRAME received */
  સંરચના ws_cntrl_frame pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#ifdef CURLVERBOSE
સ્થિર અચલ અક્ષર *ws_frame_name_of_op(uint8_t firstbyte)
{
  પસંદગી(firstbyte & WSBIT_OPCODE_MASK) {
  સ્થિતિ WSBIT_OPCODE_CONT:
    પાછા "CONT";
  સ્થિતિ WSBIT_OPCODE_TEXT:
    પાછા "TEXT";
  સ્થિતિ WSBIT_OPCODE_BIN:
    પાછા "BIN";
  સ્થિતિ WSBIT_OPCODE_CLOSE:
    પાછા "CLOSE";
  સ્થિતિ WSBIT_OPCODE_PING:
    પાછા "PING";
  સ્થિતિ WSBIT_OPCODE_PONG:
    પાછા "PONG";
  મૂળભૂત:
    પાછા "???";
  }
}
#endif

સ્થિર પૂર્ણાંક ws_frame_firstbyte2flags(સંરચના Curl_easy *data,
                                    uint8_t firstbyte, પૂર્ણાંક cont_flags)
{
  પસંદગી(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  સ્થિતિ WSBIT_OPCODE_CONT:
    જો(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      પાછા 0;
    }
    પાછા cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  સ્થિતિ (WSBIT_OPCODE_CONT | WSBIT_FIN):
    જો(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      પાછા 0;
    }
    પાછા cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  સ્થિતિ WSBIT_OPCODE_TEXT:
    જો(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      પાછા 0;
    }
    પાછા CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  સ્થિતિ (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    જો(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      પાછા 0;
    }
    પાછા CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  સ્થિતિ WSBIT_OPCODE_BIN:
    જો(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      પાછા 0;
    }
    પાછા CURLWS_દ્વિઅંકી | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  સ્થિતિ (WSBIT_OPCODE_BIN | WSBIT_FIN):
    જો(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      પાછા 0;
    }
    પાછા CURLWS_દ્વિઅંકી;
  /* 0x08 - first CLOSE fragment */
  સ્થિતિ WSBIT_OPCODE_CLOSE:
    failf(data, "[WS] invalid fragmented CLOSE frame");
    પાછા 0;
  /* 0x88 - unfragmented CLOSE */
  સ્થિતિ (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    પાછા CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  સ્થિતિ WSBIT_OPCODE_PING:
    failf(data, "[WS] invalid fragmented PING frame");
    પાછા 0;
  /* 0x89 - unfragmented PING */
  સ્થિતિ (WSBIT_OPCODE_PING | WSBIT_FIN):
    પાછા CURLWS_PING;
  /* 0x0a - first PONG fragment */
  સ્થિતિ WSBIT_OPCODE_PONG:
    failf(data, "[WS] invalid fragmented PONG frame");
    પાછા 0;
  /* 0x8a - unfragmented PONG */
  સ્થિતિ (WSBIT_OPCODE_PONG | WSBIT_FIN):
    પાછા CURLWS_PONG;
  /* invalid first byte */
  મૂળભૂત:
    જો(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(data, "[WS] invalid reserved bits: %02x", firstbyte);
    નહીંતર
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(data, "[WS] invalid opcode: %02x", firstbyte);
    પાછા 0;
  }
}

સ્થિર CURLcode ws_frame_flags2firstbyte(સંરચના Curl_easy *data,
                                         unsigned પૂર્ણાંક flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  પસંદગી(flags & ~CURLWS_OFFSET) {
  સ્થિતિ 0:
    જો(contfragment) {
      CURL_TRC_WS(data, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      પાછા CURLE_OK;
    }
    failf(data, "[WS] no flags given");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  સ્થિતિ CURLWS_CONT:
    જો(contfragment) {
      infof(data, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      પાછા CURLE_OK;
    }
    failf(data, "[WS] No ongoing fragmented message to continue");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  સ્થિતિ CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    પાછા CURLE_OK;
  સ્થિતિ (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    પાછા CURLE_OK;
  સ્થિતિ CURLWS_દ્વિઅંકી:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    પાછા CURLE_OK;
  સ્થિતિ (CURLWS_દ્વિઅંકી | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    પાછા CURLE_OK;
  સ્થિતિ CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    પાછા CURLE_OK;
  સ્થિતિ (CURLWS_CLOSE | CURLWS_CONT):
    failf(data, "[WS] CLOSE frame must not be fragmented");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  સ્થિતિ CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    પાછા CURLE_OK;
  સ્થિતિ (CURLWS_PING | CURLWS_CONT):
    failf(data, "[WS] PING frame must not be fragmented");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  સ્થિતિ CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    પાછા CURLE_OK;
  સ્થિતિ (CURLWS_PONG | CURLWS_CONT):
    failf(data, "[WS] PONG frame must not be fragmented");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  મૂળભૂત:
    failf(data, "[WS] unknown flags: %x", flags);
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

સ્થિર શૂન્ય ws_dec_info(સંરચના ws_decoder *dec, સંરચના Curl_easy *data,
                        અચલ અક્ષર *સંદેશ)
{
  NOVERBOSE((શૂન્ય)સંદેશ);
  પસંદગી(dec->head_len) {
  સ્થિતિ 0:
    તોડો;
  સ્થિતિ 1:
    CURL_TRC_WS(data, "decoded %s [%s%s]", સંદેશ,
                ws_frame_name_of_op(dec->head[0]),
                (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    તોડો;
  મૂળભૂત:
    જો(dec->head_len < dec->head_total) {
      CURL_TRC_WS(data, "decoded %s [%s%s](%d/%d)", સંદેશ,
                  ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->head_len, dec->head_total);
    }
    નહીંતર {
      CURL_TRC_WS(data, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  સંદેશ, ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    તોડો;
  }
}

સ્થિર CURLcode ws_send_raw_blocking(સંરચના Curl_easy *data,
                                     સંરચના websocket *ws,
                                     અચલ અક્ષર *buffer, size_t buflen);

પ્રકારવ્યાખ્યા CURLcode ws_write_payload(અચલ uint8_t *buf, size_t buflen,
                                  પૂર્ણાંક frame_age, પૂર્ણાંક frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  શૂન્ય *userp,
                                  size_t *pnwritten);

સ્થિર શૂન્ય ws_dec_next_frame(સંરચના ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

સ્થિર શૂન્ય ws_dec_reset(સંરચના ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

સ્થિર શૂન્ય ws_dec_init(સંરચના ws_decoder *dec)
{
  ws_dec_reset(dec);
}

સ્થિર CURLcode ws_dec_read_head(સંરચના ws_decoder *dec,
                                 સંરચના Curl_easy *data,
                                 સંરચના bufq *inraw)
{
  અચલ uint8_t *inbuf;
  size_t inlen;

  જ્યારે(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    જો(dec->head_len == 0) {
      dec->head[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(data, dec->head[0],
                                                  dec->cont_flags);
      જો(!dec->frame_flags) {
        ws_dec_reset(dec);
        પાછા CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      જો(dec->frame_flags & (CURLWS_TEXT | CURLWS_દ્વિઅંકી)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->head_len = 1;
#જો 0
      ws_dec_info(dec, data, "seeing opcode");
#endif
      ચાલુ;
    }
    નહીંતર જો(dec->head_len == 1) {
      dec->head[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->head_len = 2;

      જો(dec->head[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(data, "[WS] masked input frame");
        ws_dec_reset(dec);
        પાછા CURLE_RECV_ERROR;
      }
      જો(dec->frame_flags & CURLWS_PING && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(data, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        પાછા CURLE_RECV_ERROR;
      }
      જો(dec->frame_flags & CURLWS_PONG && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(data, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        પાછા CURLE_RECV_ERROR;
      }
      જો(dec->frame_flags & CURLWS_CLOSE && dec->head[1] > WS_MAX_CNTRL_LEN) {
        failf(data, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        પાછા CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      જો(dec->head[1] == 126) {
        dec->head_total = 4;
        ચાલુ;
      }
      નહીંતર જો(dec->head[1] == 127) {
        dec->head_total = 10;
        ચાલુ;
      }
      નહીંતર {
        dec->head_total = 2;
      }
    }

    જો(dec->head_len < dec->head_total) {
      dec->head[dec->head_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->head_len;
      જો(dec->head_len < dec->head_total) {
#જો 0
        ws_dec_info(dec, data, "decoding head");
#endif
        ચાલુ;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->head_len == dec->head_total);
    પસંદગી(dec->head_total) {
    સ્થિતિ 2:
      dec->payload_len = dec->head[1];
      તોડો;
    સ્થિતિ 4:
      dec->payload_len = (dec->head[2] << 8) | dec->head[3];
      તોડો;
    સ્થિતિ 10:
      જો(dec->head[2] > 127) {
        failf(data, "[WS] frame length longer than 63 bits not supported");
        પાછા CURLE_RECV_ERROR;
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
      તોડો;
    મૂળભૂત:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(data, "[WS] unexpected frame header length");
      પાછા CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, data, "head");
    પાછા CURLE_OK;
  }
  પાછા CURLE_AGAIN;
}

સ્થિર CURLcode ws_dec_pass_payload(સંરચના ws_decoder *dec,
                                    સંરચના Curl_easy *data,
                                    સંરચના bufq *inraw,
                                    ws_write_payload *write_cb,
                                    શૂન્ય *write_ctx)
{
  અચલ uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode result;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  જ્યારે(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(data)) {
    જો(inlen > remain)
      inlen = remain;
    result = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    જો(result)
      પાછા result;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(data, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  પાછા remain ? CURLE_AGAIN : CURLE_OK;
}

સ્થિર CURLcode ws_dec_pass(સંરચના ws_decoder *dec,
                            સંરચના Curl_easy *data,
                            સંરચના bufq *inraw,
                            ws_write_payload *write_cb,
                            શૂન્ય *write_ctx)
{
  CURLcode result;

  જો(Curl_bufq_is_empty(inraw))
    પાછા CURLE_AGAIN;

  પસંદગી(dec->state) {
  સ્થિતિ WS_DEC_INIT:
    ws_dec_next_frame(dec);
    dec->state = WS_DEC_HEAD;
    FALLTHROUGH();
  સ્થિતિ WS_DEC_HEAD:
    result = ws_dec_read_head(dec, data, inraw);
    જો(result) {
      જો(result != CURLE_AGAIN) {
        failf(data, "[WS] decode frame error %d", (પૂર્ણાંક)result);
        તોડો;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      તોડો;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    જો(dec->payload_len == 0) {
      size_t nwritten;
      અચલ uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      result = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      જો(result)
        પાછા result;
      dec->state = WS_DEC_INIT;
      તોડો;
    }
    FALLTHROUGH();
  સ્થિતિ WS_DEC_PAYLOAD:
    result = ws_dec_pass_payload(dec, data, inraw, write_cb, write_ctx);
    ws_dec_info(dec, data, "passing");
    જો(result)
      પાછા result;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    તોડો;
  મૂળભૂત:
    /* we covered all enums above, but some code analyzers are wimps */
    result = CURLE_FAILED_INIT;
  }
  પાછા result;
}

સ્થિર શૂન્ય update_meta(સંરચના websocket *ws,
                        પૂર્ણાંક frame_age, પૂર્ણાંક frame_flags,
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
સંરચના ws_cw_ctx {
  સંરચના Curl_cwriter super;
  સંરચના bufq buf;
};

સ્થિર CURLcode ws_cw_init(સંરચના Curl_easy *data,
                           સંરચના Curl_cwriter *writer)
{
  સંરચના ws_cw_ctx *ctx = writer->ctx;
  (શૂન્ય)data;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_SIZE, 1, BUFQ_OPT_SOFT_LIMIT);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય ws_cw_close(સંરચના Curl_easy *data, સંરચના Curl_cwriter *writer)
{
  સંરચના ws_cw_ctx *ctx = writer->ctx;
  (શૂન્ય)data;
  Curl_bufq_free(&ctx->buf);
}

સંરચના ws_cw_dec_ctx {
  સંરચના Curl_easy *data;
  સંરચના websocket *ws;
  સંરચના Curl_cwriter *next_writer;
  પૂર્ણાંક cw_type;
};

સ્થિર CURLcode ws_flush(સંરચના Curl_easy *data, સંરચના websocket *ws,
                         bool blocking);
સ્થિર CURLcode ws_enc_add_pending(સંરચના Curl_easy *data,
                                   સંરચના websocket *ws);

સ્થિર CURLcode ws_enc_add_cntrl(સંરચના Curl_easy *data,
                                 સંરચના websocket *ws,
                                 અચલ uint8_t *payload,
                                 size_t plen,
                                 unsigned પૂર્ણાંક frame_type)
{
  (શૂન્ય)data;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  જો(plen > WS_MAX_CNTRL_LEN)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  memcpy(ws->pending.payload, payload, plen);
  પાછા CURLE_OK;
}

સ્થિર curl_off_t ws_payload_remain(curl_off_t payload_total,
                                    curl_off_t payload_offset,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_total - payload_offset;
  જો((payload_total < 0) || (payload_offset < 0) || (remain < 0))
    પાછા -1;
  buffered = curlx_uztoso(payload_buffered);
  જો(remain < buffered)
    પાછા -1;
  પાછા remain - buffered;
}

સ્થિર CURLcode ws_cw_dec_next(અચલ uint8_t *buf, size_t buflen,
                               પૂર્ણાંક frame_age, પૂર્ણાંક frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               શૂન્ય *user_data,
                               size_t *pnwritten)
{
  સંરચના ws_cw_dec_ctx *ctx = user_data;
  સંરચના Curl_easy *data = ctx->data;
  સંરચના websocket *ws = ctx->ws;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result;

  (શૂન્ય)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  જો(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  જો(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(data, ws, buf, buflen, CURLWS_PONG);
    જો(result)
      પાછા result;
  }
  નહીંતર જો(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(data, "[WS] pass %zu decoded bytes", buflen);
    result = Curl_cwriter_write(data, ctx->next_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (અચલ અક્ષર *)buf, buflen);
    જો(result)
      પાછા result;
  }
  *pnwritten = buflen;
  પાછા CURLE_OK;
}

સ્થિર CURLcode ws_cw_write(સંરચના Curl_easy *data,
                            સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                            અચલ અક્ષર *buf, size_t nbytes)
{
  સંરચના ws_cw_ctx *ctx = writer->ctx;
  સંરચના websocket *ws;
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[WS] write(len=%zu, type=%d)", nbytes, type);
  જો(!(type & CLIENTWRITE_BODY) || data->set.ws_raw_mode)
    પાછા Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  જો(!ws) {
    failf(data, "[WS] not a websocket transfer");
    પાછા CURLE_FAILED_INIT;
  }

  જો(nbytes) {
    size_t nwritten;
    result = Curl_bufq_write(&ctx->buf, (અચલ uint8_t *)buf,
                             nbytes, &nwritten);
    જો(result) {
      infof(data, "[WS] error adding data to buffer %d", (પૂર્ણાંક)result);
      પાછા result;
    }
  }

  result = Curl_cwriter_flush(data, writer->next);
  જો(result)
    જાઓ out;

  જ્યારે(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
    સંરચના ws_cw_dec_ctx pass_ctx;
    pass_ctx.data = data;
    pass_ctx.ws = ws;
    pass_ctx.next_writer = writer->next;
    pass_ctx.cw_type = type;
    result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                         ws_cw_dec_next, &pass_ctx);
    જો(result == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      result = CURLE_OK;
      જાઓ out;
    }
    નહીંતર જો(result) {
      failf(data, "[WS] decode payload error %d", (પૂર્ણાંક)result);
      Curl_bufq_reset(&ctx->buf);
      જાઓ out;
    }
  }

out:
  જો(!result && !ws->using_reader) {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    જો(result == CURLE_AGAIN)
      result = CURLE_OK;
  }

  જો(!result && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(data, "[WS] decode ended with incomplete frame");
    result = CURLE_RECV_ERROR;
  }
  પાછા result;
}

સ્થિર CURLcode ws_cw_flush(સંરચના Curl_easy *data,
                            સંરચના Curl_cwriter *writer)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[ws] flush");
  જો(!data->set.ws_raw_mode) {
    સંરચના ws_cw_ctx *ctx = writer->ctx;
    સંરચના websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    result = Curl_cwriter_flush(data, writer->next);
    જો(result)
      જાઓ out;

    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    જો(!ws) {
      failf(data, "[WS] not a websocket transfer");
      પાછા CURLE_FAILED_INIT;
    }

    જ્યારે(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
      સંરચના ws_cw_dec_ctx pass_ctx;
      pass_ctx.data = data;
      pass_ctx.ws = ws;
      pass_ctx.next_writer = writer->next;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                           ws_cw_dec_next, &pass_ctx);
      જો(result == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        result = CURLE_OK;
        જાઓ out;
      }
      નહીંતર જો(result) {
        failf(data, "[WS] decode payload error %d", (પૂર્ણાંક)result);
        Curl_bufq_reset(&ctx->buf);
        જાઓ out;
      }
    }
  }

out:
  જો(!result)
    result = Curl_cwriter_flush(data, writer->next);
  પાછા result;
}

/* WebSocket payload decoding client writer. */
સ્થિર અચલ સંરચના Curl_cwtype ws_cw_decode = {
  "ws-decode",
  ખાલી,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  કદ(સંરચના ws_cw_ctx)
};

સ્થિર શૂન્ય ws_enc_info(સંરચના ws_encoder *enc, સંરચના Curl_easy *data,
                        અચલ અક્ષર *સંદેશ)
{
  NOVERBOSE((શૂન્ય)enc);
  NOVERBOSE((શૂન્ય)સંદેશ);
  CURL_TRC_WS(data, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              સંદેશ, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

સ્થિર શૂન્ય ws_enc_reset(સંરચના ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

સ્થિર શૂન્ય ws_enc_init(સંરચના ws_encoder *enc)
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

સ્થિર CURLcode ws_enc_add_frame(સંરચના Curl_easy *data,
                                 સંરચના ws_encoder *enc,
                                 unsigned પૂર્ણાંક flags,
                                 curl_off_t payload_len,
                                 સંરચના bufq *out)
{
  uint8_t firstb = 0;
  uint8_t head[14];
  CURLcode result;
  size_t hlen, nwritten;

  જો(payload_len < 0) {
    failf(data, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    પાછા CURLE_SEND_ERROR;
  }

  જો(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(data, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    પાછા CURLE_SEND_ERROR;
  }

  result = ws_frame_flags2firstbyte(data, flags, (bool)enc->contfragment,
                                    &firstb);
  જો(result)
    પાછા result;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  જો(flags & (CURLWS_TEXT | CURLWS_દ્વિઅંકી)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  જો(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PING frame is too big");
    પાછા CURLE_TOO_LARGE;
  }
  જો(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PONG frame is too big");
    પાછા CURLE_TOO_LARGE;
  }
  જો(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given CLOSE frame is too big");
    પાછા CURLE_TOO_LARGE;
  }

  head[0] = enc->firstbyte = firstb;
  જો(payload_len > 65535) {
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
  નહીંતર જો(payload_len >= 126) {
    head[1] = 126 | WSBIT_MASK;
    head[2] = (uint8_t)((payload_len >> 8) & 0xff);
    head[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  નહીંતર {
    head[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, data, "sending");

  /* 4 bytes random */

  result = Curl_rand(data, (uint8_t *)&enc->mask, કદ(enc->mask));
  જો(result)
    પાછા result;

#ifdef DEBUGBUILD
  જો(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->mask, 0, કદ(enc->mask));
#endif

  /* add 4 bytes mask */
  memcpy(&head[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  result = Curl_bufq_write(out, head, hlen, &nwritten);
  જો(result)
    પાછા result;
  જો(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    પાછા CURLE_SEND_ERROR;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ws_enc_write_head(સંરચના Curl_easy *data,
                                  સંરચના websocket *ws,
                                  સંરચના ws_encoder *enc,
                                  unsigned પૂર્ણાંક flags,
                                  curl_off_t payload_len,
                                  સંરચના bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  જો(ws->pending.type) {
    CURLcode result = ws_enc_add_pending(data, ws);
    જો(result)
      પાછા result;
  }
  પાછા ws_enc_add_frame(data, enc, flags, payload_len, out);
}

સ્થિર CURLcode ws_enc_write_payload(સંરચના ws_encoder *enc,
                                     સંરચના Curl_easy *data,
                                     અચલ uint8_t *buf, size_t buflen,
                                     સંરચના bufq *out, size_t *pnwritten)
{
  CURLcode result;
  size_t i, len, n, remain;

  *pnwritten = 0;
  જો(Curl_bufq_is_full(out))
    પાછા CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  જો(remain < len)
    len = remain;

  માટે(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    result = Curl_bufq_write(out, &c, 1, &n);
    જો(result) {
      જો((result != CURLE_AGAIN) || !i)
        પાછા result;
      તોડો;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, data, "buffered");
  પાછા CURLE_OK;
}

સ્થિર CURLcode ws_enc_add_pending(સંરચના Curl_easy *data,
                                   સંરચના websocket *ws)
{
  CURLcode result;
  size_t n;

  જો(!ws->pending.type) /* no pending frame here */
    પાછા CURLE_OK;
  જો(ws->enc.payload_remain) /* in the middle of another frame */
    પાછા CURLE_AGAIN;

  result = ws_enc_add_frame(data, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  જો(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding head: %d",
                (પૂર્ણાંક)result);
    જાઓ out;
  }
  result = ws_enc_write_payload(&ws->enc, data, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  જો(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding payload: %d",
                (પૂર્ણાંક)result);
    જાઓ out;
  }
  જો(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(data, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, કદ(ws->pending));

out:
  પાછા result;
}

સ્થિર CURLcode ws_enc_send(સંરચના Curl_easy *data,
                            સંરચના websocket *ws,
                            અચલ uint8_t *buffer,
                            size_t buflen,
                            curl_off_t fragsize,
                            unsigned પૂર્ણાંક flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!data->set.ws_raw_mode);
  *pnsent = 0;

  જો(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    જો(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(data, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
    જો((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(data, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  નહીંતર {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    જો(result)
      પાછા result;

    result = ws_enc_write_head(data, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    જો(result) {
      CURL_TRC_WS(data, "curl_ws_send(), error writing frame head %d",
                  (પૂર્ણાંક)result);
      પાછા result;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  જ્યારે(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    જો(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      result = ws_enc_write_payload(&ws->enc, data,
                                    buffer + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      જો(result && (result != CURLE_AGAIN))
        પાછા result;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      જો(!ws->sendbuf_payload) {
        પાછા CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    જો(!result && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      buffer += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    નહીંતર જો(result == CURLE_AGAIN) {
      જો(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        પાછા CURLE_OK;
      }
      નહીંતર {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(data, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        પાછા CURLE_AGAIN;
      }
    }
    નહીંતર
      પાછા result;  /* real error sending the data */
  }
  પાછા CURLE_OK;
}

સંરચના cr_ws_ctx {
  સંરચના Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

સ્થિર CURLcode cr_ws_init(સંરચના Curl_easy *data, સંરચના Curl_creader *reader)
{
  (શૂન્ય)data;
  (શૂન્ય)reader;
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cr_ws_close(સંરચના Curl_easy *data, સંરચના Curl_creader *reader)
{
  (શૂન્ય)data;
  (શૂન્ય)reader;
}

સ્થિર CURLcode cr_ws_read(સંરચના Curl_easy *data,
                           સંરચના Curl_creader *reader,
                           અક્ષર *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  સંરચના cr_ws_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, n;
  સંરચના websocket *ws;
  bool eos;

  *pnread = 0;
  જો(ctx->eos) {
    *peos = TRUE;
    પાછા CURLE_OK;
  }

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  જો(!ws) {
    failf(data, "[WS] not a websocket transfer");
    પાછા CURLE_FAILED_INIT;
  }

  જો(Curl_bufq_is_empty(&ws->sendbuf)) {
    જો(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      પાછા CURLE_OK;
    }

    જો(ws->enc.payload_remain) {
      CURL_TRC_WS(data, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    જો(result)
      પાછા result;
    ctx->read_eos = eos;

    જો(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(data, reader->next);
    }
    નહીંતર જો(!nread) {
      /* nothing to convert, return this right away */
      જો(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      જાઓ out;
    }

    જો(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      result = ws_enc_write_head(data, ws, &ws->enc, CURLWS_દ્વિઅંકી, nread,
                                 &ws->sendbuf);
      જો(result)
        જાઓ out;
    }

    result = ws_enc_write_payload(&ws->enc, data, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    જો(result)
      જાઓ out;
    CURL_TRC_READ(data, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  જો(!result && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (પૂર્ણાંક)result, *pnread, *peos);
  પાછા result;
}

સ્થિર curl_off_t cr_ws_total_length(સંરચના Curl_easy *data,
                                     સંરચના Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (શૂન્ય)data;
  (શૂન્ય)reader;
  પાછા -1;
}

સ્થિર અચલ સંરચના Curl_crtype ws_cr_encode = {
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
  કદ(સંરચના cr_ws_ctx)
};

સંરચના wsfield {
  અચલ અક્ષર *name;
  અચલ અક્ષર *val;
};

CURLcode Curl_ws_request(સંરચના Curl_easy *data, સંરચના dynbuf *req)
{
  unsigned પૂર્ણાંક i;
  CURLcode result = CURLE_OK;
  uint8_t rand[16];
  અક્ષર *randstr;
  size_t randlen;
  અક્ષર keyval[40];
  સંરચના SingleRequest *k = &data->req;
  સંરચના wsfield heads[] = {
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
      "Sec-WebSocket-Key", ખાલી,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  result = Curl_rand(data, rand, કદ(rand));
  જો(result)
    પાછા result;
  result = curlx_base64_encode(rand, કદ(rand), &randstr, &randlen);
  જો(result)
    પાછા result;
  DEBUGASSERT(randlen < કદ(keyval));
  જો(randlen >= કદ(keyval)) {
    curlx_free(randstr);
    પાછા CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, કદ(keyval), randstr, randlen);
  curlx_free(randstr);
  માટે(i = 0; !result && (i < CURL_ARRAYSIZE(heads)); i++) {
    જો(!Curl_checkheaders(data, heads[i].name, strlen(heads[i].name))) {
      result = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  data->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  data->conn->bits.upgrade_in_progress = TRUE;
  પાછા result;
}

સ્થિર શૂન્ય ws_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના websocket *ws = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(સંરચના Curl_easy *data,
                        અચલ અક્ષર *mem, size_t nread)
{
  સંરચના SingleRequest *k = &data->req;
  સંરચના websocket *ws;
  સંરચના Curl_cwriter *ws_dec_writer = ખાલી;
  સંરચના Curl_creader *ws_enc_reader = ખાલી;
  CURLcode result;

  DEBUGASSERT(data->conn);
  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  જો(!ws) {
    size_t chunk_size = WS_CHUNK_SIZE;
    ws = curlx_calloc(1, કદ(*ws));
    જો(!ws)
      પાછા CURLE_OUT_OF_MEMORY;
#ifdef DEBUGBUILD
    {
      અચલ અક્ષર *p = getenv("CURL_WS_CHUNK_SIZE");
      જો(p) {
        curl_off_t l;
        જો(!curlx_str_સંખ્યા(&p, &l, 1 * 1024 * 1024))
          chunk_size = (size_t)l;
      }
    }
#endif
    CURL_TRC_WS(data, "WS, using chunk size %zu", chunk_size);
    Curl_bufq_init2(&ws->recvbuf, chunk_size, WS_CHUNK_ગણતરી,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_size, WS_CHUNK_ગણતરી,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    result = Curl_conn_meta_set(data->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    જો(result)
      પાછા result;
  }
  નહીંતર {
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
  જો(result)
    જાઓ out;
  result = Curl_cwriter_add(data, ws_dec_writer);
  જો(result)
    જાઓ out;
  ws_dec_writer = ખાલી; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  જો(data->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    જો(nread) {
      result = Curl_cf_recvbuf_add(data, data->conn, FIRSTSOCKET,
                                   (અચલ uint8_t *)mem, nread);
      જો(result)
        જાઓ out;
    }
    CURL_REQ_CLEAR_RECV(data); /* read no more content */
  }
  નહીંતર { /* !connect_only */
    જો(data->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(data, "UPLOAD set, add ws-encode reader");
      result = Curl_creader_set_fread(data, -1);
      જો(result)
        જાઓ out;

      જો(!data->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        result = Curl_creader_create(&ws_enc_reader, data, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        જો(result)
          જાઓ out;
        result = Curl_creader_add(data, ws_enc_reader);
        જો(result)
          જાઓ out;
        ws_enc_reader = ખાલી; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      data->req.eos_read = FALSE;
      data->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(data);
    }

    /* Then pass any additional data to the writers */
    જો(nread) {
      result = Curl_client_write(data, CLIENTWRITE_BODY, mem, nread);
      જો(result)
        જાઓ out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  જો(ws_dec_writer)
    Curl_cwriter_free(data, ws_dec_writer);
  જો(ws_enc_reader)
    Curl_creader_free(data, ws_enc_reader);
  જો(result)
    CURL_TRC_WS(data, "Curl_ws_accept() failed -> %d", (પૂર્ણાંક)result);
  નહીંતર
    CURL_TRC_WS(data, "websocket established, %s mode",
                data->set.connect_only ? "connect-only" : "callback");
  પાછા result;
}

સંરચના ws_collect {
  સંરચના Curl_easy *data;
  સંરચના websocket *ws;
  uint8_t *buffer;
  size_t buflen;
  size_t bufidx;
  પૂર્ણાંક frame_age;
  પૂર્ણાંક frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

સ્થિર CURLcode ws_client_collect(અચલ uint8_t *buf, size_t buflen,
                                  પૂર્ણાંક frame_age, પૂર્ણાંક frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  શૂન્ય *userp,
                                  size_t *pnwritten)
{
  સંરચના ws_collect *ctx = userp;
  સંરચના Curl_easy *data = ctx->data;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  જો(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  જો(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  જો(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(ctx->data, ctx->ws, buf, buflen, CURLWS_PONG);
    જો(result)
      પાછા result;
    *pnwritten = buflen;
  }
  નહીંતર {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    જો(!write_len) {
      જો(!buflen)  /* 0 length write, we accept that */
        પાછા CURLE_OK;
      પાછા CURLE_AGAIN;  /* no more space */
    }
    memcpy(ctx->buffer + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  પાછા result;
}

સ્થિર CURLcode nw_in_recv(શૂન્ય *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  સંરચના Curl_easy *data = reader_ctx;
  પાછા Curl_easy_recv(data, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, શૂન્ય *buffer,
                      size_t buflen, size_t *recv,
                      અચલ સંરચના curl_ws_frame **metap)
{
  સંરચના Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  જો(CURL_EAPI_ENTER(&guard, curl, ws_recv, &result)) {
    સંરચના Curl_easy *data = curl;
    સંરચના connectdata *conn;
    સંરચના websocket *ws;
    સંરચના ws_collect ctx;

    જો(!recv || !metap || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      જાઓ out;
    }

    *recv = 0;
    *metap = ખાલી;

    conn = data->conn;
    જો(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      જો(!data->set.connect_only) {
        failf(data, "[WS] CONNECT_ONLY is required");
        result = CURLE_UNSUPPORTED_PROTOCOL;
        જાઓ out;
      }

      Curl_getconnectinfo(data, &conn);
      જો(!conn) {
        failf(data, "[WS] connection not found");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        જાઓ out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    જો(!ws) {
      failf(data, "[WS] connection is not setup for websocket");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      જાઓ out;
    }

    memset(&ctx, 0, કદ(ctx));
    ctx.data = data;
    ctx.ws = ws;
    ctx.buffer = buffer;
    ctx.buflen = buflen;

    જ્યારે(1) {
      /* receive more when our buffer is empty */
      જો(Curl_bufq_is_empty(&ws->recvbuf)) {
        size_t n;
        result = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, data, &n);
        જો(result)
          જાઓ out;
        નહીંતર જો(n == 0) {
          /* connection closed */
          infof(data, "[WS] connection expectedly closed?");
          result = CURLE_GOT_NOTHING;
          જાઓ out;
        }
        CURL_TRC_WS(data, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      result = ws_dec_pass(&ws->dec, data, &ws->recvbuf,
                           ws_client_collect, &ctx);
      જો(result == CURLE_AGAIN) {
        જો(!ctx.written) {
          ws_dec_info(&ws->dec, data, "need more input");
          ચાલુ;  /* nothing written, try more input */
        }
        તોડો;
      }
      નહીંતર જો(result) {
        જાઓ out;
      }
      નહીંતર જો(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        તોડો;
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
    જો(!data->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(data, ws);
      જો(!r2)
        (શૂન્ય)ws_flush(data, ws, Curl_api_is_in_callback(data));
    }
    result = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}

સ્થિર CURLcode ws_flush(સંરચના Curl_easy *data, સંરચના websocket *ws,
                         bool blocking)
{
  CURLcode result;

  /* If there is space, add any pending control frame */
  જો(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_size) {
    result = ws_enc_add_pending(data, ws);
    જો(result && (result != CURLE_AGAIN))
      પાછા result;
  }

  જો(!Curl_bufq_is_empty(&ws->sendbuf)) {
    અચલ uint8_t *out;
    size_t outlen, n;
#ifdef DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_next = FALSE;
    size_t chunk_egain = 0;
    અચલ અક્ષર *p = getenv("CURL_WS_CHUNK_EAGAIN");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#endif

    જ્યારે(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#ifdef DEBUGBUILD
      જો(eagain_next)
        પાછા CURLE_AGAIN;
      જો(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_next = TRUE;
      }
#endif
      જો(blocking) {
        result = ws_send_raw_blocking(data, ws, (અચલ અક્ષર *)out, outlen);
        n = result ? 0 : outlen;
      }
      નહીંતર જો(data->set.connect_only || Curl_api_is_in_callback(data))
        result = Curl_senddata(data, out, outlen, &n);
      નહીંતર {
        result = Curl_xfer_send(data, out, outlen, FALSE, &n);
        જો(!result && !n && outlen)
          result = CURLE_AGAIN;
      }

      જો(result == CURLE_AGAIN) {
        CURL_TRC_WS(data, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        પાછા result;
      }
      નહીંતર જો(result) {
        failf(data, "[WS] flush, write error %d", (પૂર્ણાંક)result);
        પાછા result;
      }
      નહીંતર {
        CURL_TRC_WS(data, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ws_send_raw_blocking(સંરચના Curl_easy *data,
                                     સંરચના websocket *ws,
                                     અચલ અક્ષર *buffer, size_t buflen)
{
  CURLcode result = CURLE_OK;
  size_t nwritten;

  જો(!data)
    પાછા result;

  (શૂન્ય)ws;
  જ્યારે(buflen) {
    result = Curl_xfer_send(data, buffer, buflen, FALSE, &nwritten);
    જો(result)
      પાછા result;
    DEBUGASSERT(nwritten <= buflen);
    buffer += nwritten;
    buflen -= nwritten;
    જો(buflen) {
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      timediff_t ડાબું_ms;
      પૂર્ણાંક ev;

      CURL_TRC_WS(data, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      ડાબું_ms = Curl_timeleft_ms(data);
      જો(ડાબું_ms < 0) {
        failf(data, "[WS] Timeout waiting for socket becoming writable");
        પાછા CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      જો(sock == CURL_SOCKET_BAD)
        પાછા CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, ડાબું_ms ? ડાબું_ms : 500);
      જો(ev < 0) {
        failf(data, "[WS] Error while waiting for socket becoming writable");
        પાછા CURLE_SEND_ERROR;
      }
    }
  }
  પાછા result;
}

સ્થિર CURLcode ws_send_raw(સંરચના Curl_easy *data, અચલ શૂન્ય *buffer,
                            size_t buflen, size_t *pnwritten)
{
  સંરચના websocket *ws;
  CURLcode result;

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  જો(!ws) {
    failf(data, "[WS] Not a websocket transfer");
    પાછા CURLE_SEND_ERROR;
  }
  જો(!buflen)
    પાછા CURLE_OK;

  જો(Curl_api_is_in_callback(data)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    result = ws_flush(data, ws, TRUE);
    જો(result)
      પાછા result;
    result = ws_send_raw_blocking(data, ws, buffer, buflen);
    જો(!result)
      *pnwritten = buflen;
  }
  નહીંતર {
    /* We need any pending data to be sent or EAGAIN this call. */
    result = ws_flush(data, ws, FALSE);
    જો(result)
      પાછા result;
    result = Curl_senddata(data, buffer, buflen, pnwritten);
  }

  CURL_TRC_WS(data, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (પૂર્ણાંક)result, *pnwritten);
  પાછા result;
}

CURLcode curl_ws_send(CURL *curl, અચલ શૂન્ય *buffer_arg,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned પૂર્ણાંક flags)
{
  સંરચના Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  જો(CURL_EAPI_ENTER(&guard, curl, ws_send, &result)) {
    સંરચના websocket *ws;
    અચલ uint8_t *buffer = buffer_arg;
    સંરચના Curl_easy *data = curl;
    size_t ndummy;
    size_t *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, data->set.ws_raw_mode);

    *pnsent = 0;

    જો(!buffer && buflen) {
      failf(data, "[WS] buffer is NULL when buflen is not");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      જાઓ out;
    }

    જો(!data->conn && data->set.connect_only) {
      result = Curl_connect_only_attach(data);
      જો(result)
        જાઓ out;
    }
    જો(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    જો(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }

    જો(data->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      result = ws_flush(data, ws, FALSE);
      જો(result)
        જાઓ out;

      જો(!buffer) {
        failf(data, "[WS] buffer is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        જાઓ out;
      }
      જો(!sent) {
        failf(data, "[WS] sent is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        જાઓ out;
      }
      જો(fragsize || flags) {
        failf(data, "[WS] fragsize and flags must be zero in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        જાઓ out;
      }
      result = ws_send_raw(data, buffer, buflen, pnsent);
      જાઓ out;
    }

    /* Not RAW mode, we do the frame encoding */
    result = ws_enc_send(data, ws, buffer, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, data->set.ws_raw_mode, (પૂર્ણાંક)result,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}

સ્થિર CURLcode ws_setup_conn(સંરચના Curl_easy *data,
                              સંરચના connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  data->state.http_neg.accept_09 = FALSE;
  data->state.http_neg.only_10 = FALSE;
  data->state.http_neg.wanted = CURL_HTTP_V1x;
  data->state.http_neg.allowed = CURL_HTTP_V1x;
  પાછા Curl_http_setup_conn(data, conn);
}

અચલ સંરચના curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  સંરચના Curl_easy *data = curl;
  જો(GOOD_EASY_HANDLE(data) && Curl_api_is_in_callback(data) &&
     data->conn && !data->set.ws_raw_mode) {
    સંરચના websocket *ws;
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    જો(ws)
      પાછા &ws->recvframe;
  }
  પાછા ખાલી;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned પૂર્ણાંક flags,
                                         curl_off_t frame_len)
{
  સંરચના Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  જો(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &result)) {
    સંરચના Curl_easy *data = curl;
    સંરચના websocket *ws;

    જો(data->set.ws_raw_mode) {
      failf(data, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      result = CURLE_FAILED_INIT;
      જાઓ out;
    }

    CURL_TRC_WS(data, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    જો(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    જો(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }

    જો(ws->enc.payload_remain) {
      failf(data, "[WS] previous frame not finished");
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }

    result = ws_enc_write_head(data, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    જો(result)
      CURL_TRC_WS(data, "curl_start_frame(), error adding frame head %d",
                  (પૂર્ણાંક)result);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}

અચલ સંરચના Curl_protocol Curl_protocol_ws = {
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

#નહીંતર

CURLcode curl_ws_recv(CURL *curl, શૂન્ય *buffer, size_t buflen,
                      size_t *recv,
                      અચલ સંરચના curl_ws_frame **metap)
{
  (શૂન્ય)curl;
  (શૂન્ય)buffer;
  (શૂન્ય)buflen;
  (શૂન્ય)recv;
  (શૂન્ય)metap;
  પાછા CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, અચલ શૂન્ય *buffer,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned પૂર્ણાંક flags)
{
  (શૂન્ય)curl;
  (શૂન્ય)buffer;
  (શૂન્ય)buflen;
  (શૂન્ય)sent;
  (શૂન્ય)fragsize;
  (શૂન્ય)flags;
  પાછા CURLE_NOT_BUILT_IN;
}

અચલ સંરચના curl_ws_frame *curl_ws_meta(CURL *data)
{
  (શૂન્ય)data;
  પાછા ખાલી;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned પૂર્ણાંક flags,
                                         curl_off_t frame_len)
{
  (શૂન્ય)curl;
  (શૂન્ય)flags;
  (શૂન્ય)frame_len;
  પાછા CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
