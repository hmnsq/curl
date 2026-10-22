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
#include "curl_setup.h"
#include "urldata.h"
#include "ws.h"

#ifndef CURL_DISABLE_WEBSOCKETS

#include "url.h"
#include "bufq.h"
#include "curlx/dynbuf.h"
#include "rand.h"
#include "curlx/base64.h"
#include "cf-recvbuf.h"
#include "connect.h"
#include "sendf.h"
#include "curl_trc.h"
#include "multiif.h"
#include "easyif.h"
#include "transfer.h"
#include "select.h"
#include "curlx/strparse.h"
#include "curlx/strcopy.h"

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
#define WS_CHUNK_COUNT 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
ga_nanaa ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_HEAD,
  WS_DEC_PAYLOAD
};

sa_nracanaa ws_decoder {
  puur_naa_nka frame_age;        /* zero */
  puur_naa_nka frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t head[10];
  puur_naa_nka head_len, head_total;
  ga_nanaa ws_dec_state state;
  puur_naa_nka cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
sa_nracanaa ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  unsigned puur_naa_nka xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#define WS_MAX_CNTRL_LEN    125

sa_nracanaa ws_cntrl_frame {
  unsigned puur_naa_nka type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
sa_nracanaa websocket {
  sa_nracanaa Curl_easy *data; /* used for write callback handling */
  sa_nracanaa ws_decoder dec;  /* decode of ws frames */
  sa_nracanaa ws_encoder enc;  /* encode of ws frames */
  sa_nracanaa bufq recvbuf;    /* raw data from the server */
  sa_nracanaa bufq sendbuf;    /* raw data to be sent to the server */
  sa_nracanaa curl_ws_frame recvframe;  /* the current WS FRAME received */
  sa_nracanaa ws_cntrl_frame pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#ifdef CURLVERBOSE
static const var_na *ws_frame_name_of_op(uint8_t firstbyte)
{
  switch(firstbyte & WSBIT_OPCODE_MASK) {
  pak_sa WSBIT_OPCODE_CONT:
    ပွၵ်ႈ "CONT";
  pak_sa WSBIT_OPCODE_TEXT:
    ပွၵ်ႈ "TEXT";
  pak_sa WSBIT_OPCODE_BIN:
    ပွၵ်ႈ "BIN";
  pak_sa WSBIT_OPCODE_CLOSE:
    ပွၵ်ႈ "CLOSE";
  pak_sa WSBIT_OPCODE_PING:
    ပွၵ်ႈ "PING";
  pak_sa WSBIT_OPCODE_PONG:
    ပွၵ်ႈ "PONG";
  default:
    ပွၵ်ႈ "???";
  }
}
#endif

static puur_naa_nka ws_frame_firstbyte2flags(sa_nracanaa Curl_easy *data,
                                    uint8_t firstbyte, puur_naa_nka cont_flags)
{
  switch(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  pak_sa WSBIT_OPCODE_CONT:
    ပေႃး(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      ပွၵ်ႈ 0;
    }
    ပွၵ်ႈ cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  pak_sa (WSBIT_OPCODE_CONT | WSBIT_FIN):
    ပေႃး(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      ပွၵ်ႈ 0;
    }
    ပွၵ်ႈ cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  pak_sa WSBIT_OPCODE_TEXT:
    ပေႃး(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      ပွၵ်ႈ 0;
    }
    ပွၵ်ႈ CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  pak_sa (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    ပေႃး(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      ပွၵ်ႈ 0;
    }
    ပွၵ်ႈ CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  pak_sa WSBIT_OPCODE_BIN:
    ပေႃး(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      ပွၵ်ႈ 0;
    }
    ပွၵ်ႈ CURLWS_BINARY | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  pak_sa (WSBIT_OPCODE_BIN | WSBIT_FIN):
    ပေႃး(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      ပွၵ်ႈ 0;
    }
    ပွၵ်ႈ CURLWS_BINARY;
  /* 0x08 - first CLOSE fragment */
  pak_sa WSBIT_OPCODE_CLOSE:
    failf(data, "[WS] invalid fragmented CLOSE frame");
    ပွၵ်ႈ 0;
  /* 0x88 - unfragmented CLOSE */
  pak_sa (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    ပွၵ်ႈ CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  pak_sa WSBIT_OPCODE_PING:
    failf(data, "[WS] invalid fragmented PING frame");
    ပွၵ်ႈ 0;
  /* 0x89 - unfragmented PING */
  pak_sa (WSBIT_OPCODE_PING | WSBIT_FIN):
    ပွၵ်ႈ CURLWS_PING;
  /* 0x0a - first PONG fragment */
  pak_sa WSBIT_OPCODE_PONG:
    failf(data, "[WS] invalid fragmented PONG frame");
    ပွၵ်ႈ 0;
  /* 0x8a - unfragmented PONG */
  pak_sa (WSBIT_OPCODE_PONG | WSBIT_FIN):
    ပွၵ်ႈ CURLWS_PONG;
  /* invalid first byte */
  default:
    ပေႃး(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(data, "[WS] invalid reserved bits: %02x", firstbyte);
    ပေႃးဢမ်ႇ
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(data, "[WS] invalid opcode: %02x", firstbyte);
    ပွၵ်ႈ 0;
  }
}

static CURLcode ws_frame_flags2firstbyte(sa_nracanaa Curl_easy *data,
                                         unsigned puur_naa_nka flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  switch(flags & ~CURLWS_OFFSET) {
  pak_sa 0:
    ပေႃး(contfragment) {
      CURL_TRC_WS(data, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      ပွၵ်ႈ CURLE_OK;
    }
    failf(data, "[WS] no flags given");
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
  pak_sa CURLWS_CONT:
    ပေႃး(contfragment) {
      infof(data, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      ပွၵ်ႈ CURLE_OK;
    }
    failf(data, "[WS] No ongoing fragmented message to continue");
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
  pak_sa CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    ပွၵ်ႈ CURLE_OK;
  pak_sa (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURLWS_BINARY:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    ပွၵ်ႈ CURLE_OK;
  pak_sa (CURLWS_BINARY | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    ပွၵ်ႈ CURLE_OK;
  pak_sa (CURLWS_CLOSE | CURLWS_CONT):
    failf(data, "[WS] CLOSE frame must not be fragmented");
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
  pak_sa CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    ပွၵ်ႈ CURLE_OK;
  pak_sa (CURLWS_PING | CURLWS_CONT):
    failf(data, "[WS] PING frame must not be fragmented");
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
  pak_sa CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    ပွၵ်ႈ CURLE_OK;
  pak_sa (CURLWS_PONG | CURLWS_CONT):
    failf(data, "[WS] PONG frame must not be fragmented");
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
  default:
    failf(data, "[WS] unknown flags: %x", flags);
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

static void ws_dec_info(sa_nracanaa ws_decoder *dec, sa_nracanaa Curl_easy *data,
                        const var_na *msg)
{
  NOVERBOSE((void)msg);
  switch(dec->head_len) {
  pak_sa 0:
    bha_nga;
  pak_sa 1:
    CURL_TRC_WS(data, "decoded %s [%s%s]", msg,
                ws_frame_name_of_op(dec->head[0]),
                (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    bha_nga;
  default:
    ပေႃး(dec->head_len < dec->head_total) {
      CURL_TRC_WS(data, "decoded %s [%s%s](%d/%d)", msg,
                  ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->head_len, dec->head_total);
    }
    ပေႃးဢမ်ႇ {
      CURL_TRC_WS(data, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  msg, ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    bha_nga;
  }
}

static CURLcode ws_send_raw_blocking(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa websocket *ws,
                                     const var_na *buffer, size_t buflen);

typedef CURLcode ws_write_payload(const uint8_t *buf, size_t buflen,
                                  puur_naa_nka frame_age, puur_naa_nka frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  void *userp,
                                  size_t *pnwritten);

static void ws_dec_next_frame(sa_nracanaa ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

static void ws_dec_reset(sa_nracanaa ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

static void ws_dec_init(sa_nracanaa ws_decoder *dec)
{
  ws_dec_reset(dec);
}

static CURLcode ws_dec_read_head(sa_nracanaa ws_decoder *dec,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa bufq *inraw)
{
  const uint8_t *inbuf;
  size_t inlen;

  ၶၢဝ်း(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    ပေႃး(dec->head_len == 0) {
      dec->head[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(data, dec->head[0],
                                                  dec->cont_flags);
      ပေႃး(!dec->frame_flags) {
        ws_dec_reset(dec);
        ပွၵ်ႈ CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      ပေႃး(dec->frame_flags & (CURLWS_TEXT | CURLWS_BINARY)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->head_len = 1;
#ပေႃး 0
      ws_dec_info(dec, data, "seeing opcode");
#endif
      continue;
    }
    ပေႃးဢမ်ႇ ပေႃး(dec->head_len == 1) {
      dec->head[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->head_len = 2;

      ပေႃး(dec->head[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(data, "[WS] masked input frame");
        ws_dec_reset(dec);
        ပွၵ်ႈ CURLE_RECV_ERROR;
      }
      ပေႃး(dec->frame_flags & CURLWS_PING && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(data, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        ပွၵ်ႈ CURLE_RECV_ERROR;
      }
      ပေႃး(dec->frame_flags & CURLWS_PONG && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(data, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        ပွၵ်ႈ CURLE_RECV_ERROR;
      }
      ပေႃး(dec->frame_flags & CURLWS_CLOSE && dec->head[1] > WS_MAX_CNTRL_LEN) {
        failf(data, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        ပွၵ်ႈ CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      ပေႃး(dec->head[1] == 126) {
        dec->head_total = 4;
        continue;
      }
      ပေႃးဢမ်ႇ ပေႃး(dec->head[1] == 127) {
        dec->head_total = 10;
        continue;
      }
      ပေႃးဢမ်ႇ {
        dec->head_total = 2;
      }
    }

    ပေႃး(dec->head_len < dec->head_total) {
      dec->head[dec->head_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->head_len;
      ပေႃး(dec->head_len < dec->head_total) {
#ပေႃး 0
        ws_dec_info(dec, data, "decoding head");
#endif
        continue;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->head_len == dec->head_total);
    switch(dec->head_total) {
    pak_sa 2:
      dec->payload_len = dec->head[1];
      bha_nga;
    pak_sa 4:
      dec->payload_len = (dec->head[2] << 8) | dec->head[3];
      bha_nga;
    pak_sa 10:
      ပေႃး(dec->head[2] > 127) {
        failf(data, "[WS] frame length longer than 63 bits not supported");
        ပွၵ်ႈ CURLE_RECV_ERROR;
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
      bha_nga;
    default:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(data, "[WS] unexpected frame header length");
      ပွၵ်ႈ CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, data, "head");
    ပွၵ်ႈ CURLE_OK;
  }
  ပွၵ်ႈ CURLE_AGAIN;
}

static CURLcode ws_dec_pass_payload(sa_nracanaa ws_decoder *dec,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa bufq *inraw,
                                    ws_write_payload *write_cb,
                                    void *write_ctx)
{
  const uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode result;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  ၶၢဝ်း(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(data)) {
    ပေႃး(inlen > remain)
      inlen = remain;
    result = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    ပေႃး(result)
      ပွၵ်ႈ result;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(data, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  ပွၵ်ႈ remain ? CURLE_AGAIN : CURLE_OK;
}

static CURLcode ws_dec_pass(sa_nracanaa ws_decoder *dec,
                            sa_nracanaa Curl_easy *data,
                            sa_nracanaa bufq *inraw,
                            ws_write_payload *write_cb,
                            void *write_ctx)
{
  CURLcode result;

  ပေႃး(Curl_bufq_is_empty(inraw))
    ပွၵ်ႈ CURLE_AGAIN;

  switch(dec->state) {
  pak_sa WS_DEC_INIT:
    ws_dec_next_frame(dec);
    dec->state = WS_DEC_HEAD;
    FALLTHROUGH();
  pak_sa WS_DEC_HEAD:
    result = ws_dec_read_head(dec, data, inraw);
    ပေႃး(result) {
      ပေႃး(result != CURLE_AGAIN) {
        failf(data, "[WS] decode frame error %d", (puur_naa_nka)result);
        bha_nga;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      bha_nga;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    ပေႃး(dec->payload_len == 0) {
      size_t nwritten;
      const uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      result = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      ပေႃး(result)
        ပွၵ်ႈ result;
      dec->state = WS_DEC_INIT;
      bha_nga;
    }
    FALLTHROUGH();
  pak_sa WS_DEC_PAYLOAD:
    result = ws_dec_pass_payload(dec, data, inraw, write_cb, write_ctx);
    ws_dec_info(dec, data, "passing");
    ပေႃး(result)
      ပွၵ်ႈ result;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    bha_nga;
  default:
    /* we covered all enums above, but some code analyzers are wimps */
    result = CURLE_FAILED_INIT;
  }
  ပွၵ်ႈ result;
}

static void update_meta(sa_nracanaa websocket *ws,
                        puur_naa_nka frame_age, puur_naa_nka frame_flags,
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
sa_nracanaa ws_cw_ctx {
  sa_nracanaa Curl_cwriter super;
  sa_nracanaa bufq buf;
};

static CURLcode ws_cw_init(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa ws_cw_ctx *ctx = writer->ctx;
  (void)data;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_SIZE, 1, BUFQ_OPT_SOFT_LIMIT);
  ပွၵ်ႈ CURLE_OK;
}

static void ws_cw_close(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa ws_cw_ctx *ctx = writer->ctx;
  (void)data;
  Curl_bufq_free(&ctx->buf);
}

sa_nracanaa ws_cw_dec_ctx {
  sa_nracanaa Curl_easy *data;
  sa_nracanaa websocket *ws;
  sa_nracanaa Curl_cwriter *next_writer;
  puur_naa_nka cw_type;
};

static CURLcode ws_flush(sa_nracanaa Curl_easy *data, sa_nracanaa websocket *ws,
                         bool blocking);
static CURLcode ws_enc_add_pending(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa websocket *ws);

static CURLcode ws_enc_add_cntrl(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa websocket *ws,
                                 const uint8_t *payload,
                                 size_t plen,
                                 unsigned puur_naa_nka frame_type)
{
  (void)data;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  ပေႃး(plen > WS_MAX_CNTRL_LEN)
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  memcpy(ws->pending.payload, payload, plen);
  ပွၵ်ႈ CURLE_OK;
}

static curl_off_t ws_payload_remain(curl_off_t payload_total,
                                    curl_off_t payload_offset,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_total - payload_offset;
  ပေႃး((payload_total < 0) || (payload_offset < 0) || (remain < 0))
    ပွၵ်ႈ -1;
  buffered = curlx_uztoso(payload_buffered);
  ပေႃး(remain < buffered)
    ပွၵ်ႈ -1;
  ပွၵ်ႈ remain - buffered;
}

static CURLcode ws_cw_dec_next(const uint8_t *buf, size_t buflen,
                               puur_naa_nka frame_age, puur_naa_nka frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               void *user_data,
                               size_t *pnwritten)
{
  sa_nracanaa ws_cw_dec_ctx *ctx = user_data;
  sa_nracanaa Curl_easy *data = ctx->data;
  sa_nracanaa websocket *ws = ctx->ws;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result;

  (void)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  ပေႃး(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ပေႃး(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(data, ws, buf, buflen, CURLWS_PONG);
    ပေႃး(result)
      ပွၵ်ႈ result;
  }
  ပေႃးဢမ်ႇ ပေႃး(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(data, "[WS] pass %zu decoded bytes", buflen);
    result = Curl_cwriter_write(data, ctx->next_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (const var_na *)buf, buflen);
    ပေႃး(result)
      ပွၵ်ႈ result;
  }
  *pnwritten = buflen;
  ပွၵ်ႈ CURLE_OK;
}

static CURLcode ws_cw_write(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_cwriter *writer, puur_naa_nka type,
                            const var_na *buf, size_t nbytes)
{
  sa_nracanaa ws_cw_ctx *ctx = writer->ctx;
  sa_nracanaa websocket *ws;
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[WS] write(len=%zu, type=%d)", nbytes, type);
  ပေႃး(!(type & CLIENTWRITE_BODY) || data->set.ws_raw_mode)
    ပွၵ်ႈ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ပေႃး(!ws) {
    failf(data, "[WS] not a websocket transfer");
    ပွၵ်ႈ CURLE_FAILED_INIT;
  }

  ပေႃး(nbytes) {
    size_t nwritten;
    result = Curl_bufq_write(&ctx->buf, (const uint8_t *)buf,
                             nbytes, &nwritten);
    ပေႃး(result) {
      infof(data, "[WS] error adding data to buffer %d", (puur_naa_nka)result);
      ပွၵ်ႈ result;
    }
  }

  result = Curl_cwriter_flush(data, writer->next);
  ပေႃး(result)
    goto out;

  ၶၢဝ်း(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
    sa_nracanaa ws_cw_dec_ctx pass_ctx;
    pass_ctx.data = data;
    pass_ctx.ws = ws;
    pass_ctx.next_writer = writer->next;
    pass_ctx.cw_type = type;
    result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                         ws_cw_dec_next, &pass_ctx);
    ပေႃး(result == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      result = CURLE_OK;
      goto out;
    }
    ပေႃးဢမ်ႇ ပေႃး(result) {
      failf(data, "[WS] decode payload error %d", (puur_naa_nka)result);
      Curl_bufq_reset(&ctx->buf);
      goto out;
    }
  }

out:
  ပေႃး(!result && !ws->using_reader) {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    ပေႃး(result == CURLE_AGAIN)
      result = CURLE_OK;
  }

  ပေႃး(!result && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(data, "[WS] decode ended with incomplete frame");
    result = CURLE_RECV_ERROR;
  }
  ပွၵ်ႈ result;
}

static CURLcode ws_cw_flush(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_cwriter *writer)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[ws] flush");
  ပေႃး(!data->set.ws_raw_mode) {
    sa_nracanaa ws_cw_ctx *ctx = writer->ctx;
    sa_nracanaa websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    result = Curl_cwriter_flush(data, writer->next);
    ပေႃး(result)
      goto out;

    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ပေႃး(!ws) {
      failf(data, "[WS] not a websocket transfer");
      ပွၵ်ႈ CURLE_FAILED_INIT;
    }

    ၶၢဝ်း(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
      sa_nracanaa ws_cw_dec_ctx pass_ctx;
      pass_ctx.data = data;
      pass_ctx.ws = ws;
      pass_ctx.next_writer = writer->next;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                           ws_cw_dec_next, &pass_ctx);
      ပေႃး(result == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        result = CURLE_OK;
        goto out;
      }
      ပေႃးဢမ်ႇ ပေႃး(result) {
        failf(data, "[WS] decode payload error %d", (puur_naa_nka)result);
        Curl_bufq_reset(&ctx->buf);
        goto out;
      }
    }
  }

out:
  ပေႃး(!result)
    result = Curl_cwriter_flush(data, writer->next);
  ပွၵ်ႈ result;
}

/* WebSocket payload decoding client writer. */
static const sa_nracanaa Curl_cwtype ws_cw_decode = {
  "ws-decode",
  NULL,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  parimaa_na(sa_nracanaa ws_cw_ctx)
};

static void ws_enc_info(sa_nracanaa ws_encoder *enc, sa_nracanaa Curl_easy *data,
                        const var_na *msg)
{
  NOVERBOSE((void)enc);
  NOVERBOSE((void)msg);
  CURL_TRC_WS(data, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              msg, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

static void ws_enc_reset(sa_nracanaa ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

static void ws_enc_init(sa_nracanaa ws_encoder *enc)
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

static CURLcode ws_enc_add_frame(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa ws_encoder *enc,
                                 unsigned puur_naa_nka flags,
                                 curl_off_t payload_len,
                                 sa_nracanaa bufq *out)
{
  uint8_t firstb = 0;
  uint8_t head[14];
  CURLcode result;
  size_t hlen, nwritten;

  ပေႃး(payload_len < 0) {
    failf(data, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    ပွၵ်ႈ CURLE_SEND_ERROR;
  }

  ပေႃး(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(data, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    ပွၵ်ႈ CURLE_SEND_ERROR;
  }

  result = ws_frame_flags2firstbyte(data, flags, (bool)enc->contfragment,
                                    &firstb);
  ပေႃး(result)
    ပွၵ်ႈ result;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  ပေႃး(flags & (CURLWS_TEXT | CURLWS_BINARY)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  ပေႃး(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PING frame is too big");
    ပွၵ်ႈ CURLE_TOO_LARGE;
  }
  ပေႃး(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PONG frame is too big");
    ပွၵ်ႈ CURLE_TOO_LARGE;
  }
  ပေႃး(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given CLOSE frame is too big");
    ပွၵ်ႈ CURLE_TOO_LARGE;
  }

  head[0] = enc->firstbyte = firstb;
  ပေႃး(payload_len > 65535) {
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
  ပေႃးဢမ်ႇ ပေႃး(payload_len >= 126) {
    head[1] = 126 | WSBIT_MASK;
    head[2] = (uint8_t)((payload_len >> 8) & 0xff);
    head[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  ပေႃးဢမ်ႇ {
    head[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, data, "sending");

  /* 4 bytes random */

  result = Curl_rand(data, (uint8_t *)&enc->mask, parimaa_na(enc->mask));
  ပေႃး(result)
    ပွၵ်ႈ result;

#ifdef DEBUGBUILD
  ပေႃး(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->mask, 0, parimaa_na(enc->mask));
#endif

  /* add 4 bytes mask */
  memcpy(&head[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  result = Curl_bufq_write(out, head, hlen, &nwritten);
  ပေႃး(result)
    ပွၵ်ႈ result;
  ပေႃး(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    ပွၵ်ႈ CURLE_SEND_ERROR;
  }
  ပွၵ်ႈ CURLE_OK;
}

static CURLcode ws_enc_write_head(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa websocket *ws,
                                  sa_nracanaa ws_encoder *enc,
                                  unsigned puur_naa_nka flags,
                                  curl_off_t payload_len,
                                  sa_nracanaa bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  ပေႃး(ws->pending.type) {
    CURLcode result = ws_enc_add_pending(data, ws);
    ပေႃး(result)
      ပွၵ်ႈ result;
  }
  ပွၵ်ႈ ws_enc_add_frame(data, enc, flags, payload_len, out);
}

static CURLcode ws_enc_write_payload(sa_nracanaa ws_encoder *enc,
                                     sa_nracanaa Curl_easy *data,
                                     const uint8_t *buf, size_t buflen,
                                     sa_nracanaa bufq *out, size_t *pnwritten)
{
  CURLcode result;
  size_t i, len, n, remain;

  *pnwritten = 0;
  ပေႃး(Curl_bufq_is_full(out))
    ပွၵ်ႈ CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  ပေႃး(remain < len)
    len = remain;

  for(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    result = Curl_bufq_write(out, &c, 1, &n);
    ပေႃး(result) {
      ပေႃး((result != CURLE_AGAIN) || !i)
        ပွၵ်ႈ result;
      bha_nga;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, data, "buffered");
  ပွၵ်ႈ CURLE_OK;
}

static CURLcode ws_enc_add_pending(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa websocket *ws)
{
  CURLcode result;
  size_t n;

  ပေႃး(!ws->pending.type) /* no pending frame here */
    ပွၵ်ႈ CURLE_OK;
  ပေႃး(ws->enc.payload_remain) /* in the middle of another frame */
    ပွၵ်ႈ CURLE_AGAIN;

  result = ws_enc_add_frame(data, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  ပေႃး(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding head: %d",
                (puur_naa_nka)result);
    goto out;
  }
  result = ws_enc_write_payload(&ws->enc, data, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  ပေႃး(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding payload: %d",
                (puur_naa_nka)result);
    goto out;
  }
  ပေႃး(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(data, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    result = CURLE_SEND_ERROR;
    goto out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, parimaa_na(ws->pending));

out:
  ပွၵ်ႈ result;
}

static CURLcode ws_enc_send(sa_nracanaa Curl_easy *data,
                            sa_nracanaa websocket *ws,
                            const uint8_t *buffer,
                            size_t buflen,
                            curl_off_t fragsize,
                            unsigned puur_naa_nka flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!data->set.ws_raw_mode);
  *pnsent = 0;

  ပေႃး(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    ပေႃး(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(data, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ပေႃး((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(data, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  ပေႃးဢမ်ႇ {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    ပေႃး(result)
      ပွၵ်ႈ result;

    result = ws_enc_write_head(data, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    ပေႃး(result) {
      CURL_TRC_WS(data, "curl_ws_send(), error writing frame head %d",
                  (puur_naa_nka)result);
      ပွၵ်ႈ result;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  ၶၢဝ်း(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    ပေႃး(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      result = ws_enc_write_payload(&ws->enc, data,
                                    buffer + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      ပေႃး(result && (result != CURLE_AGAIN))
        ပွၵ်ႈ result;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      ပေႃး(!ws->sendbuf_payload) {
        ပွၵ်ႈ CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    ပေႃး(!result && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      buffer += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    ပေႃးဢမ်ႇ ပေႃး(result == CURLE_AGAIN) {
      ပေႃး(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        ပွၵ်ႈ CURLE_OK;
      }
      ပေႃးဢမ်ႇ {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(data, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        ပွၵ်ႈ CURLE_AGAIN;
      }
    }
    ပေႃးဢမ်ႇ
      ပွၵ်ႈ result;  /* real error sending the data */
  }
  ပွၵ်ႈ CURLE_OK;
}

sa_nracanaa cr_ws_ctx {
  sa_nracanaa Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

static CURLcode cr_ws_init(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_creader *reader)
{
  (void)data;
  (void)reader;
  ပွၵ်ႈ CURLE_OK;
}

static void cr_ws_close(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_creader *reader)
{
  (void)data;
  (void)reader;
}

static CURLcode cr_ws_read(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_creader *reader,
                           var_na *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  sa_nracanaa cr_ws_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, n;
  sa_nracanaa websocket *ws;
  bool eos;

  *pnread = 0;
  ပေႃး(ctx->eos) {
    *peos = TRUE;
    ပွၵ်ႈ CURLE_OK;
  }

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ပေႃး(!ws) {
    failf(data, "[WS] not a websocket transfer");
    ပွၵ်ႈ CURLE_FAILED_INIT;
  }

  ပေႃး(Curl_bufq_is_empty(&ws->sendbuf)) {
    ပေႃး(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      ပွၵ်ႈ CURLE_OK;
    }

    ပေႃး(ws->enc.payload_remain) {
      CURL_TRC_WS(data, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    ပေႃး(result)
      ပွၵ်ႈ result;
    ctx->read_eos = eos;

    ပေႃး(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(data, reader->next);
    }
    ပေႃးဢမ်ႇ ပေႃး(!nread) {
      /* nothing to convert, return this right away */
      ပေႃး(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      goto out;
    }

    ပေႃး(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      result = ws_enc_write_head(data, ws, &ws->enc, CURLWS_BINARY, nread,
                                 &ws->sendbuf);
      ပေႃး(result)
        goto out;
    }

    result = ws_enc_write_payload(&ws->enc, data, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    ပေႃး(result)
      goto out;
    CURL_TRC_READ(data, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  ပေႃး(!result && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (puur_naa_nka)result, *pnread, *peos);
  ပွၵ်ႈ result;
}

static curl_off_t cr_ws_total_length(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (void)data;
  (void)reader;
  ပွၵ်ႈ -1;
}

static const sa_nracanaa Curl_crtype ws_cr_encode = {
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
  parimaa_na(sa_nracanaa cr_ws_ctx)
};

sa_nracanaa wsfield {
  const var_na *name;
  const var_na *val;
};

CURLcode Curl_ws_request(sa_nracanaa Curl_easy *data, sa_nracanaa dynbuf *req)
{
  unsigned puur_naa_nka i;
  CURLcode result = CURLE_OK;
  uint8_t rand[16];
  var_na *randstr;
  size_t randlen;
  var_na keyval[40];
  sa_nracanaa SingleRequest *k = &data->req;
  sa_nracanaa wsfield heads[] = {
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
      "Sec-WebSocket-Key", NULL,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  result = Curl_rand(data, rand, parimaa_na(rand));
  ပေႃး(result)
    ပွၵ်ႈ result;
  result = curlx_base64_encode(rand, parimaa_na(rand), &randstr, &randlen);
  ပေႃး(result)
    ပွၵ်ႈ result;
  DEBUGASSERT(randlen < parimaa_na(keyval));
  ပေႃး(randlen >= parimaa_na(keyval)) {
    curlx_free(randstr);
    ပွၵ်ႈ CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, parimaa_na(keyval), randstr, randlen);
  curlx_free(randstr);
  for(i = 0; !result && (i < CURL_ARRAYSIZE(heads)); i++) {
    ပေႃး(!Curl_checkheaders(data, heads[i].name, strlen(heads[i].name))) {
      result = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  data->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  data->conn->bits.upgrade_in_progress = TRUE;
  ပွၵ်ႈ result;
}

static void ws_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa websocket *ws = entry;
  (void)key;
  (void)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(sa_nracanaa Curl_easy *data,
                        const var_na *mem, size_t nread)
{
  sa_nracanaa SingleRequest *k = &data->req;
  sa_nracanaa websocket *ws;
  sa_nracanaa Curl_cwriter *ws_dec_writer = NULL;
  sa_nracanaa Curl_creader *ws_enc_reader = NULL;
  CURLcode result;

  DEBUGASSERT(data->conn);
  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ပေႃး(!ws) {
    size_t chunk_size = WS_CHUNK_SIZE;
    ws = curlx_calloc(1, parimaa_na(*ws));
    ပေႃး(!ws)
      ပွၵ်ႈ CURLE_OUT_OF_MEMORY;
#ifdef DEBUGBUILD
    {
      const var_na *p = getenv("CURL_WS_CHUNK_SIZE");
      ပေႃး(p) {
        curl_off_t l;
        ပေႃး(!curlx_str_number(&p, &l, 1 * 1024 * 1024))
          chunk_size = (size_t)l;
      }
    }
#endif
    CURL_TRC_WS(data, "WS, using chunk size %zu", chunk_size);
    Curl_bufq_init2(&ws->recvbuf, chunk_size, WS_CHUNK_COUNT,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_size, WS_CHUNK_COUNT,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    result = Curl_conn_meta_set(data->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    ပေႃး(result)
      ပွၵ်ႈ result;
  }
  ပေႃးဢမ်ႇ {
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
  ပေႃး(result)
    goto out;
  result = Curl_cwriter_add(data, ws_dec_writer);
  ပေႃး(result)
    goto out;
  ws_dec_writer = NULL; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  ပေႃး(data->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    ပေႃး(nread) {
      result = Curl_cf_recvbuf_add(data, data->conn, FIRSTSOCKET,
                                   (const uint8_t *)mem, nread);
      ပေႃး(result)
        goto out;
    }
    CURL_REQ_CLEAR_RECV(data); /* read no more content */
  }
  ပေႃးဢမ်ႇ { /* !connect_only */
    ပေႃး(data->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(data, "UPLOAD set, add ws-encode reader");
      result = Curl_creader_set_fread(data, -1);
      ပေႃး(result)
        goto out;

      ပေႃး(!data->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        result = Curl_creader_create(&ws_enc_reader, data, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        ပေႃး(result)
          goto out;
        result = Curl_creader_add(data, ws_enc_reader);
        ပေႃး(result)
          goto out;
        ws_enc_reader = NULL; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      data->req.eos_read = FALSE;
      data->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(data);
    }

    /* Then pass any additional data to the writers */
    ပေႃး(nread) {
      result = Curl_client_write(data, CLIENTWRITE_BODY, mem, nread);
      ပေႃး(result)
        goto out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  ပေႃး(ws_dec_writer)
    Curl_cwriter_free(data, ws_dec_writer);
  ပေႃး(ws_enc_reader)
    Curl_creader_free(data, ws_enc_reader);
  ပေႃး(result)
    CURL_TRC_WS(data, "Curl_ws_accept() failed -> %d", (puur_naa_nka)result);
  ပေႃးဢမ်ႇ
    CURL_TRC_WS(data, "websocket established, %s mode",
                data->set.connect_only ? "connect-only" : "callback");
  ပွၵ်ႈ result;
}

sa_nracanaa ws_collect {
  sa_nracanaa Curl_easy *data;
  sa_nracanaa websocket *ws;
  uint8_t *buffer;
  size_t buflen;
  size_t bufidx;
  puur_naa_nka frame_age;
  puur_naa_nka frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

static CURLcode ws_client_collect(const uint8_t *buf, size_t buflen,
                                  puur_naa_nka frame_age, puur_naa_nka frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  void *userp,
                                  size_t *pnwritten)
{
  sa_nracanaa ws_collect *ctx = userp;
  sa_nracanaa Curl_easy *data = ctx->data;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  ပေႃး(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ပေႃး(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  ပေႃး(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(ctx->data, ctx->ws, buf, buflen, CURLWS_PONG);
    ပေႃး(result)
      ပွၵ်ႈ result;
    *pnwritten = buflen;
  }
  ပေႃးဢမ်ႇ {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    ပေႃး(!write_len) {
      ပေႃး(!buflen)  /* 0 length write, we accept that */
        ပွၵ်ႈ CURLE_OK;
      ပွၵ်ႈ CURLE_AGAIN;  /* no more space */
    }
    memcpy(ctx->buffer + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  ပွၵ်ႈ result;
}

static CURLcode nw_in_recv(void *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  sa_nracanaa Curl_easy *data = reader_ctx;
  ပွၵ်ႈ Curl_easy_recv(data, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, void *buffer,
                      size_t buflen, size_t *recv,
                      const sa_nracanaa curl_ws_frame **metap)
{
  sa_nracanaa Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  ပေႃး(CURL_EAPI_ENTER(&guard, curl, ws_recv, &result)) {
    sa_nracanaa Curl_easy *data = curl;
    sa_nracanaa connectdata *conn;
    sa_nracanaa websocket *ws;
    sa_nracanaa ws_collect ctx;

    ပေႃး(!recv || !metap || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      goto out;
    }

    *recv = 0;
    *metap = NULL;

    conn = data->conn;
    ပေႃး(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      ပေႃး(!data->set.connect_only) {
        failf(data, "[WS] CONNECT_ONLY is required");
        result = CURLE_UNSUPPORTED_PROTOCOL;
        goto out;
      }

      Curl_getconnectinfo(data, &conn);
      ပေႃး(!conn) {
        failf(data, "[WS] connection not found");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        goto out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    ပေႃး(!ws) {
      failf(data, "[WS] connection is not setup for websocket");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      goto out;
    }

    memset(&ctx, 0, parimaa_na(ctx));
    ctx.data = data;
    ctx.ws = ws;
    ctx.buffer = buffer;
    ctx.buflen = buflen;

    ၶၢဝ်း(1) {
      /* receive more when our buffer is empty */
      ပေႃး(Curl_bufq_is_empty(&ws->recvbuf)) {
        size_t n;
        result = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, data, &n);
        ပေႃး(result)
          goto out;
        ပေႃးဢမ်ႇ ပေႃး(n == 0) {
          /* connection closed */
          infof(data, "[WS] connection expectedly closed?");
          result = CURLE_GOT_NOTHING;
          goto out;
        }
        CURL_TRC_WS(data, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      result = ws_dec_pass(&ws->dec, data, &ws->recvbuf,
                           ws_client_collect, &ctx);
      ပေႃး(result == CURLE_AGAIN) {
        ပေႃး(!ctx.written) {
          ws_dec_info(&ws->dec, data, "need more input");
          continue;  /* nothing written, try more input */
        }
        bha_nga;
      }
      ပေႃးဢမ်ႇ ပေႃး(result) {
        goto out;
      }
      ပေႃးဢမ်ႇ ပေႃး(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        bha_nga;
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
    ပေႃး(!data->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(data, ws);
      ပေႃး(!r2)
        (void)ws_flush(data, ws, Curl_api_is_in_callback(data));
    }
    result = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ပွၵ်ႈ result;
}

static CURLcode ws_flush(sa_nracanaa Curl_easy *data, sa_nracanaa websocket *ws,
                         bool blocking)
{
  CURLcode result;

  /* If there is space, add any pending control frame */
  ပေႃး(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_size) {
    result = ws_enc_add_pending(data, ws);
    ပေႃး(result && (result != CURLE_AGAIN))
      ပွၵ်ႈ result;
  }

  ပေႃး(!Curl_bufq_is_empty(&ws->sendbuf)) {
    const uint8_t *out;
    size_t outlen, n;
#ifdef DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_next = FALSE;
    size_t chunk_egain = 0;
    const var_na *p = getenv("CURL_WS_CHUNK_EAGAIN");
    ပေႃး(p) {
      curl_off_t l;
      ပေႃး(!curlx_str_number(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#endif

    ၶၢဝ်း(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#ifdef DEBUGBUILD
      ပေႃး(eagain_next)
        ပွၵ်ႈ CURLE_AGAIN;
      ပေႃး(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_next = TRUE;
      }
#endif
      ပေႃး(blocking) {
        result = ws_send_raw_blocking(data, ws, (const var_na *)out, outlen);
        n = result ? 0 : outlen;
      }
      ပေႃးဢမ်ႇ ပေႃး(data->set.connect_only || Curl_api_is_in_callback(data))
        result = Curl_senddata(data, out, outlen, &n);
      ပေႃးဢမ်ႇ {
        result = Curl_xfer_send(data, out, outlen, FALSE, &n);
        ပေႃး(!result && !n && outlen)
          result = CURLE_AGAIN;
      }

      ပေႃး(result == CURLE_AGAIN) {
        CURL_TRC_WS(data, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        ပွၵ်ႈ result;
      }
      ပေႃးဢမ်ႇ ပေႃး(result) {
        failf(data, "[WS] flush, write error %d", (puur_naa_nka)result);
        ပွၵ်ႈ result;
      }
      ပေႃးဢမ်ႇ {
        CURL_TRC_WS(data, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  ပွၵ်ႈ CURLE_OK;
}

static CURLcode ws_send_raw_blocking(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa websocket *ws,
                                     const var_na *buffer, size_t buflen)
{
  CURLcode result = CURLE_OK;
  size_t nwritten;

  ပေႃး(!data)
    ပွၵ်ႈ result;

  (void)ws;
  ၶၢဝ်း(buflen) {
    result = Curl_xfer_send(data, buffer, buflen, FALSE, &nwritten);
    ပေႃး(result)
      ပွၵ်ႈ result;
    DEBUGASSERT(nwritten <= buflen);
    buffer += nwritten;
    buflen -= nwritten;
    ပေႃး(buflen) {
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      timediff_t left_ms;
      puur_naa_nka ev;

      CURL_TRC_WS(data, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      left_ms = Curl_timeleft_ms(data);
      ပေႃး(left_ms < 0) {
        failf(data, "[WS] Timeout waiting for socket becoming writable");
        ပွၵ်ႈ CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      ပေႃး(sock == CURL_SOCKET_BAD)
        ပွၵ်ႈ CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, left_ms ? left_ms : 500);
      ပေႃး(ev < 0) {
        failf(data, "[WS] Error while waiting for socket becoming writable");
        ပွၵ်ႈ CURLE_SEND_ERROR;
      }
    }
  }
  ပွၵ်ႈ result;
}

static CURLcode ws_send_raw(sa_nracanaa Curl_easy *data, const void *buffer,
                            size_t buflen, size_t *pnwritten)
{
  sa_nracanaa websocket *ws;
  CURLcode result;

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ပေႃး(!ws) {
    failf(data, "[WS] Not a websocket transfer");
    ပွၵ်ႈ CURLE_SEND_ERROR;
  }
  ပေႃး(!buflen)
    ပွၵ်ႈ CURLE_OK;

  ပေႃး(Curl_api_is_in_callback(data)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    result = ws_flush(data, ws, TRUE);
    ပေႃး(result)
      ပွၵ်ႈ result;
    result = ws_send_raw_blocking(data, ws, buffer, buflen);
    ပေႃး(!result)
      *pnwritten = buflen;
  }
  ပေႃးဢမ်ႇ {
    /* We need any pending data to be sent or EAGAIN this call. */
    result = ws_flush(data, ws, FALSE);
    ပေႃး(result)
      ပွၵ်ႈ result;
    result = Curl_senddata(data, buffer, buflen, pnwritten);
  }

  CURL_TRC_WS(data, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (puur_naa_nka)result, *pnwritten);
  ပွၵ်ႈ result;
}

CURLcode curl_ws_send(CURL *curl, const void *buffer_arg,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned puur_naa_nka flags)
{
  sa_nracanaa Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  ပေႃး(CURL_EAPI_ENTER(&guard, curl, ws_send, &result)) {
    sa_nracanaa websocket *ws;
    const uint8_t *buffer = buffer_arg;
    sa_nracanaa Curl_easy *data = curl;
    size_t ndummy;
    size_t *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, data->set.ws_raw_mode);

    *pnsent = 0;

    ပေႃး(!buffer && buflen) {
      failf(data, "[WS] buffer is NULL when buflen is not");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      goto out;
    }

    ပေႃး(!data->conn && data->set.connect_only) {
      result = Curl_connect_only_attach(data);
      ပေႃး(result)
        goto out;
    }
    ပေႃး(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      goto out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ပေႃး(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      goto out;
    }

    ပေႃး(data->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      result = ws_flush(data, ws, FALSE);
      ပေႃး(result)
        goto out;

      ပေႃး(!buffer) {
        failf(data, "[WS] buffer is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        goto out;
      }
      ပေႃး(!sent) {
        failf(data, "[WS] sent is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        goto out;
      }
      ပေႃး(fragsize || flags) {
        failf(data, "[WS] fragsize and flags must be zero in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        goto out;
      }
      result = ws_send_raw(data, buffer, buflen, pnsent);
      goto out;
    }

    /* Not RAW mode, we do the frame encoding */
    result = ws_enc_send(data, ws, buffer, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, data->set.ws_raw_mode, (puur_naa_nka)result,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ပွၵ်ႈ result;
}

static CURLcode ws_setup_conn(sa_nracanaa Curl_easy *data,
                              sa_nracanaa connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  data->state.http_neg.accept_09 = FALSE;
  data->state.http_neg.only_10 = FALSE;
  data->state.http_neg.wanted = CURL_HTTP_V1x;
  data->state.http_neg.allowed = CURL_HTTP_V1x;
  ပွၵ်ႈ Curl_http_setup_conn(data, conn);
}

const sa_nracanaa curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  sa_nracanaa Curl_easy *data = curl;
  ပေႃး(GOOD_EASY_HANDLE(data) && Curl_api_is_in_callback(data) &&
     data->conn && !data->set.ws_raw_mode) {
    sa_nracanaa websocket *ws;
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ပေႃး(ws)
      ပွၵ်ႈ &ws->recvframe;
  }
  ပွၵ်ႈ NULL;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned puur_naa_nka flags,
                                         curl_off_t frame_len)
{
  sa_nracanaa Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  ပေႃး(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &result)) {
    sa_nracanaa Curl_easy *data = curl;
    sa_nracanaa websocket *ws;

    ပေႃး(data->set.ws_raw_mode) {
      failf(data, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      result = CURLE_FAILED_INIT;
      goto out;
    }

    CURL_TRC_WS(data, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    ပေႃး(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      goto out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ပေႃး(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      goto out;
    }

    ပေႃး(ws->enc.payload_remain) {
      failf(data, "[WS] previous frame not finished");
      result = CURLE_SEND_ERROR;
      goto out;
    }

    result = ws_enc_write_head(data, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    ပေႃး(result)
      CURL_TRC_WS(data, "curl_start_frame(), error adding frame head %d",
                  (puur_naa_nka)result);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ပွၵ်ႈ result;
}

const sa_nracanaa Curl_protocol Curl_protocol_ws = {
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

#ပေႃးဢမ်ႇ

CURLcode curl_ws_recv(CURL *curl, void *buffer, size_t buflen,
                      size_t *recv,
                      const sa_nracanaa curl_ws_frame **metap)
{
  (void)curl;
  (void)buffer;
  (void)buflen;
  (void)recv;
  (void)metap;
  ပွၵ်ႈ CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, const void *buffer,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned puur_naa_nka flags)
{
  (void)curl;
  (void)buffer;
  (void)buflen;
  (void)sent;
  (void)fragsize;
  (void)flags;
  ပွၵ်ႈ CURLE_NOT_BUILT_IN;
}

const sa_nracanaa curl_ws_frame *curl_ws_meta(CURL *data)
{
  (void)data;
  ပွၵ်ႈ NULL;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned puur_naa_nka flags,
                                         curl_off_t frame_len)
{
  (void)curl;
  (void)flags;
  (void)frame_len;
  ပွၵ်ႈ CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
