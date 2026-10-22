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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "ws.h"

#ifndef CURL_DISABLE_WEBSOCKETS

#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "cf-recvbuf.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "easyif.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

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
#define WS_CHUNK_ଗଣନା 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
ଗଣନା ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_HEAD,
  WS_DEC_PAYLOAD
};

ସଂରଚନା ws_decoder {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_age;        /* zero */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t head[10];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା head_len, head_total;
  ଗଣନା ws_dec_state state;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
ସଂରଚନା ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#define WS_MAX_CNTRL_LEN    125

ସଂରଚନା ws_cntrl_frame {
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
ସଂରଚନା websocket {
  ସଂରଚନା Curl_easy *data; /* used for write callback handling */
  ସଂରଚନା ws_decoder dec;  /* decode of ws frames */
  ସଂରଚନା ws_encoder enc;  /* encode of ws frames */
  ସଂରଚନା bufq recvbuf;    /* raw data from the server */
  ସଂରଚନା bufq sendbuf;    /* raw data to be sent to the server */
  ସଂରଚନା curl_ws_frame recvframe;  /* the current WS FRAME received */
  ସଂରଚନା ws_cntrl_frame pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#ifdef CURLVERBOSE
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ws_frame_name_of_op(uint8_t firstbyte)
{
  ବାଛ(firstbyte & WSBIT_OPCODE_MASK) {
  ସ୍ଥିତି WSBIT_OPCODE_CONT:
    ଫେରନ୍ତୁ "CONT";
  ସ୍ଥିତି WSBIT_OPCODE_TEXT:
    ଫେରନ୍ତୁ "TEXT";
  ସ୍ଥିତି WSBIT_OPCODE_BIN:
    ଫେରନ୍ତୁ "BIN";
  ସ୍ଥିତି WSBIT_OPCODE_CLOSE:
    ଫେରନ୍ତୁ "CLOSE";
  ସ୍ଥିତି WSBIT_OPCODE_PING:
    ଫେରନ୍ତୁ "PING";
  ସ୍ଥିତି WSBIT_OPCODE_PONG:
    ଫେରନ୍ତୁ "PONG";
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ "???";
  }
}
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ws_frame_firstbyte2flags(ସଂରଚନା Curl_easy *data,
                                    uint8_t firstbyte, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cont_flags)
{
  ବାଛ(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  ସ୍ଥିତି WSBIT_OPCODE_CONT:
    ଯଦି(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  ସ୍ଥିତି (WSBIT_OPCODE_CONT | WSBIT_FIN):
    ଯଦି(!(cont_flags & CURLWS_CONT)) {
      failf(data, "[WS] no ongoing fragmented message to resume");
      ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  ସ୍ଥିତି WSBIT_OPCODE_TEXT:
    ଯଦି(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  ସ୍ଥିତି (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    ଯଦି(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new TEXT msg");
      ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  ସ୍ଥିତି WSBIT_OPCODE_BIN:
    ଯଦି(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ CURLWS_ଦ୍ୱିମିକ | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  ସ୍ଥିତି (WSBIT_OPCODE_BIN | WSBIT_FIN):
    ଯଦି(cont_flags & CURLWS_CONT) {
      failf(data, "[WS] fragmented message interrupted by new BINARY msg");
      ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ CURLWS_ଦ୍ୱିମିକ;
  /* 0x08 - first CLOSE fragment */
  ସ୍ଥିତି WSBIT_OPCODE_CLOSE:
    failf(data, "[WS] invalid fragmented CLOSE frame");
    ଫେରନ୍ତୁ 0;
  /* 0x88 - unfragmented CLOSE */
  ସ୍ଥିତି (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    ଫେରନ୍ତୁ CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  ସ୍ଥିତି WSBIT_OPCODE_PING:
    failf(data, "[WS] invalid fragmented PING frame");
    ଫେରନ୍ତୁ 0;
  /* 0x89 - unfragmented PING */
  ସ୍ଥିତି (WSBIT_OPCODE_PING | WSBIT_FIN):
    ଫେରନ୍ତୁ CURLWS_PING;
  /* 0x0a - first PONG fragment */
  ସ୍ଥିତି WSBIT_OPCODE_PONG:
    failf(data, "[WS] invalid fragmented PONG frame");
    ଫେରନ୍ତୁ 0;
  /* 0x8a - unfragmented PONG */
  ସ୍ଥିତି (WSBIT_OPCODE_PONG | WSBIT_FIN):
    ଫେରନ୍ତୁ CURLWS_PONG;
  /* invalid first byte */
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(data, "[WS] invalid reserved bits: %02x", firstbyte);
    ଅନ୍ୟଥା
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(data, "[WS] invalid opcode: %02x", firstbyte);
    ଫେରନ୍ତୁ 0;
  }
}

ସ୍ଥିର CURLcode ws_frame_flags2firstbyte(ସଂରଚନା Curl_easy *data,
                                         unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  ବାଛ(flags & ~CURLWS_OFFSET) {
  ସ୍ଥିତି 0:
    ଯଦି(contfragment) {
      CURL_TRC_WS(data, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      ଫେରନ୍ତୁ CURLE_OK;
    }
    failf(data, "[WS] no flags given");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ସ୍ଥିତି CURLWS_CONT:
    ଯଦି(contfragment) {
      infof(data, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    failf(data, "[WS] No ongoing fragmented message to continue");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ସ୍ଥିତି CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURLWS_ଦ୍ୱିମିକ:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି (CURLWS_ଦ୍ୱିମିକ | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି (CURLWS_CLOSE | CURLWS_CONT):
    failf(data, "[WS] CLOSE frame must not be fragmented");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ସ୍ଥିତି CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି (CURLWS_PING | CURLWS_CONT):
    failf(data, "[WS] PING frame must not be fragmented");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ସ୍ଥିତି CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି (CURLWS_PONG | CURLWS_CONT):
    failf(data, "[WS] PONG frame must not be fragmented");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "[WS] unknown flags: %x", flags);
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ ws_dec_info(ସଂରଚନା ws_decoder *dec, ସଂରଚନା Curl_easy *data,
                        ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ)
{
  NOVERBOSE((ଶୂନ୍ୟ)ସନ୍ଦେଶ);
  ବାଛ(dec->head_len) {
  ସ୍ଥିତି 0:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 1:
    CURL_TRC_WS(data, "decoded %s [%s%s]", ସନ୍ଦେଶ,
                ws_frame_name_of_op(dec->head[0]),
                (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି(dec->head_len < dec->head_total) {
      CURL_TRC_WS(data, "decoded %s [%s%s](%d/%d)", ସନ୍ଦେଶ,
                  ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->head_len, dec->head_total);
    }
    ଅନ୍ୟଥା {
      CURL_TRC_WS(data, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  ସନ୍ଦେଶ, ws_frame_name_of_op(dec->head[0]),
                  (dec->head[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    ଭାଙ୍ଗ;
  }
}

ସ୍ଥିର CURLcode ws_send_raw_blocking(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା websocket *ws,
                                     ଅଚଳ ଅକ୍ଷର *buffer, size_t buflen);

ପ୍ରକାରବ୍ୟାଖ୍ୟା CURLcode ws_write_payload(ଅଚଳ uint8_t *buf, size_t buflen,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_age, ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  ଶୂନ୍ୟ *userp,
                                  size_t *pnwritten);

ସ୍ଥିର ଶୂନ୍ୟ ws_dec_next_frame(ସଂରଚନା ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

ସ୍ଥିର ଶୂନ୍ୟ ws_dec_reset(ସଂରଚନା ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->head_len = dec->head_total = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ ws_dec_init(ସଂରଚନା ws_decoder *dec)
{
  ws_dec_reset(dec);
}

ସ୍ଥିର CURLcode ws_dec_read_head(ସଂରଚନା ws_decoder *dec,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା bufq *inraw)
{
  ଅଚଳ uint8_t *inbuf;
  size_t inlen;

  ଯେତେବେଳେ(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    ଯଦି(dec->head_len == 0) {
      dec->head[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(data, dec->head[0],
                                                  dec->cont_flags);
      ଯଦି(!dec->frame_flags) {
        ws_dec_reset(dec);
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      ଯଦି(dec->frame_flags & (CURLWS_TEXT | CURLWS_ଦ୍ୱିମିକ)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->head_len = 1;
#ଯଦି 0
      ws_dec_info(dec, data, "seeing opcode");
#endif
      ଜାରି;
    }
    ଅନ୍ୟଥା ଯଦି(dec->head_len == 1) {
      dec->head[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->head_len = 2;

      ଯଦି(dec->head[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(data, "[WS] masked input frame");
        ws_dec_reset(dec);
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
      ଯଦି(dec->frame_flags & CURLWS_PING && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(data, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
      ଯଦି(dec->frame_flags & CURLWS_PONG && dec->head[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(data, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
      ଯଦି(dec->frame_flags & CURLWS_CLOSE && dec->head[1] > WS_MAX_CNTRL_LEN) {
        failf(data, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      ଯଦି(dec->head[1] == 126) {
        dec->head_total = 4;
        ଜାରି;
      }
      ଅନ୍ୟଥା ଯଦି(dec->head[1] == 127) {
        dec->head_total = 10;
        ଜାରି;
      }
      ଅନ୍ୟଥା {
        dec->head_total = 2;
      }
    }

    ଯଦି(dec->head_len < dec->head_total) {
      dec->head[dec->head_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->head_len;
      ଯଦି(dec->head_len < dec->head_total) {
#ଯଦି 0
        ws_dec_info(dec, data, "decoding head");
#endif
        ଜାରି;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->head_len == dec->head_total);
    ବାଛ(dec->head_total) {
    ସ୍ଥିତି 2:
      dec->payload_len = dec->head[1];
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 4:
      dec->payload_len = (dec->head[2] << 8) | dec->head[3];
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 10:
      ଯଦି(dec->head[2] > 127) {
        failf(data, "[WS] frame length longer than 63 bits not supported");
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
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
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(data, "[WS] unexpected frame header length");
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, data, "head");
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଫେରନ୍ତୁ CURLE_AGAIN;
}

ସ୍ଥିର CURLcode ws_dec_pass_payload(ସଂରଚନା ws_decoder *dec,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା bufq *inraw,
                                    ws_write_payload *write_cb,
                                    ଶୂନ୍ୟ *write_ctx)
{
  ଅଚଳ uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode result;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  ଯେତେବେଳେ(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(data)) {
    ଯଦି(inlen > remain)
      inlen = remain;
    result = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(data, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  ଫେରନ୍ତୁ remain ? CURLE_AGAIN : CURLE_OK;
}

ସ୍ଥିର CURLcode ws_dec_pass(ସଂରଚନା ws_decoder *dec,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା bufq *inraw,
                            ws_write_payload *write_cb,
                            ଶୂନ୍ୟ *write_ctx)
{
  CURLcode result;

  ଯଦି(Curl_bufq_is_empty(inraw))
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ବାଛ(dec->state) {
  ସ୍ଥିତି WS_DEC_INIT:
    ws_dec_next_frame(dec);
    dec->state = WS_DEC_HEAD;
    FALLTHROUGH();
  ସ୍ଥିତି WS_DEC_HEAD:
    result = ws_dec_read_head(dec, data, inraw);
    ଯଦି(result) {
      ଯଦି(result != CURLE_AGAIN) {
        failf(data, "[WS] decode frame error %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଭାଙ୍ଗ;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      ଭାଙ୍ଗ;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    ଯଦି(dec->payload_len == 0) {
      size_t nwritten;
      ଅଚଳ uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      result = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      dec->state = WS_DEC_INIT;
      ଭାଙ୍ଗ;
    }
    FALLTHROUGH();
  ସ୍ଥିତି WS_DEC_PAYLOAD:
    result = ws_dec_pass_payload(dec, data, inraw, write_cb, write_ctx);
    ws_dec_info(dec, data, "passing");
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* we covered all enums above, but some code analyzers are wimps */
    result = CURLE_FAILED_INIT;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ update_meta(ସଂରଚନା websocket *ws,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_age, ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_flags,
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
ସଂରଚନା ws_cw_ctx {
  ସଂରଚନା Curl_cwriter super;
  ସଂରଚନା bufq buf;
};

ସ୍ଥିର CURLcode ws_cw_init(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା ws_cw_ctx *ctx = writer->ctx;
  (ଶୂନ୍ୟ)data;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_SIZE, 1, BUFQ_OPT_SOFT_LIMIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ ws_cw_close(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା ws_cw_ctx *ctx = writer->ctx;
  (ଶୂନ୍ୟ)data;
  Curl_bufq_free(&ctx->buf);
}

ସଂରଚନା ws_cw_dec_ctx {
  ସଂରଚନା Curl_easy *data;
  ସଂରଚନା websocket *ws;
  ସଂରଚନା Curl_cwriter *next_writer;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cw_type;
};

ସ୍ଥିର CURLcode ws_flush(ସଂରଚନା Curl_easy *data, ସଂରଚନା websocket *ws,
                         bool blocking);
ସ୍ଥିର CURLcode ws_enc_add_pending(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା websocket *ws);

ସ୍ଥିର CURLcode ws_enc_add_cntrl(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା websocket *ws,
                                 ଅଚଳ uint8_t *payload,
                                 size_t plen,
                                 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_type)
{
  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  ଯଦି(plen > WS_MAX_CNTRL_LEN)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  memcpy(ws->pending.payload, payload, plen);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର curl_off_t ws_payload_remain(curl_off_t payload_total,
                                    curl_off_t payload_offset,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_total - payload_offset;
  ଯଦି((payload_total < 0) || (payload_offset < 0) || (remain < 0))
    ଫେରନ୍ତୁ -1;
  buffered = curlx_uztoso(payload_buffered);
  ଯଦି(remain < buffered)
    ଫେରନ୍ତୁ -1;
  ଫେରନ୍ତୁ remain - buffered;
}

ସ୍ଥିର CURLcode ws_cw_dec_next(ଅଚଳ uint8_t *buf, size_t buflen,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_age, ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               ଶୂନ୍ୟ *user_data,
                               size_t *pnwritten)
{
  ସଂରଚନା ws_cw_dec_ctx *ctx = user_data;
  ସଂରଚନା Curl_easy *data = ctx->data;
  ସଂରଚନା websocket *ws = ctx->ws;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result;

  (ଶୂନ୍ୟ)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  ଯଦି(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ଯଦି(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(data, ws, buf, buflen, CURLWS_PONG);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା ଯଦି(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(data, "[WS] pass %zu decoded bytes", buflen);
    result = Curl_cwriter_write(data, ctx->next_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ଅଚଳ ଅକ୍ଷର *)buf, buflen);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  *pnwritten = buflen;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ws_cw_write(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                            ଅଚଳ ଅକ୍ଷର *buf, size_t nbytes)
{
  ସଂରଚନା ws_cw_ctx *ctx = writer->ctx;
  ସଂରଚନା websocket *ws;
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[WS] write(len=%zu, type=%d)", nbytes, type);
  ଯଦି(!(type & CLIENTWRITE_BODY) || data->set.ws_raw_mode)
    ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ଯଦି(!ws) {
    failf(data, "[WS] not a websocket transfer");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  ଯଦି(nbytes) {
    size_t nwritten;
    result = Curl_bufq_write(&ctx->buf, (ଅଚଳ uint8_t *)buf,
                             nbytes, &nwritten);
    ଯଦି(result) {
      infof(data, "[WS] error adding data to buffer %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଫେରନ୍ତୁ result;
    }
  }

  result = Curl_cwriter_flush(data, writer->next);
  ଯଦି(result)
    ଯାଅ out;

  ଯେତେବେଳେ(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
    ସଂରଚନା ws_cw_dec_ctx pass_ctx;
    pass_ctx.data = data;
    pass_ctx.ws = ws;
    pass_ctx.next_writer = writer->next;
    pass_ctx.cw_type = type;
    result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                         ws_cw_dec_next, &pass_ctx);
    ଯଦି(result == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      result = CURLE_OK;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(result) {
      failf(data, "[WS] decode payload error %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      Curl_bufq_reset(&ctx->buf);
      ଯାଅ out;
    }
  }

out:
  ଯଦି(!result && !ws->using_reader) {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    ଯଦି(result == CURLE_AGAIN)
      result = CURLE_OK;
  }

  ଯଦି(!result && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(data, "[WS] decode ended with incomplete frame");
    result = CURLE_RECV_ERROR;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ws_cw_flush(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_cwriter *writer)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_WRITE(data, "[ws] flush");
  ଯଦି(!data->set.ws_raw_mode) {
    ସଂରଚନା ws_cw_ctx *ctx = writer->ctx;
    ସଂରଚନା websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    result = Curl_cwriter_flush(data, writer->next);
    ଯଦି(result)
      ଯାଅ out;

    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ଯଦି(!ws) {
      failf(data, "[WS] not a websocket transfer");
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    }

    ଯେତେବେଳେ(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(data)) {
      ସଂରଚନା ws_cw_dec_ctx pass_ctx;
      pass_ctx.data = data;
      pass_ctx.ws = ws;
      pass_ctx.next_writer = writer->next;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      result = ws_dec_pass(&ws->dec, data, &ctx->buf,
                           ws_cw_dec_next, &pass_ctx);
      ଯଦି(result == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        result = CURLE_OK;
        ଯାଅ out;
      }
      ଅନ୍ୟଥା ଯଦି(result) {
        failf(data, "[WS] decode payload error %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        Curl_bufq_reset(&ctx->buf);
        ଯାଅ out;
      }
    }
  }

out:
  ଯଦି(!result)
    result = Curl_cwriter_flush(data, writer->next);
  ଫେରନ୍ତୁ result;
}

/* WebSocket payload decoding client writer. */
ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype ws_cw_decode = {
  "ws-decode",
  ଖାଲି,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  ଆକାର(ସଂରଚନା ws_cw_ctx)
};

ସ୍ଥିର ଶୂନ୍ୟ ws_enc_info(ସଂରଚନା ws_encoder *enc, ସଂରଚନା Curl_easy *data,
                        ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ)
{
  NOVERBOSE((ଶୂନ୍ୟ)enc);
  NOVERBOSE((ଶୂନ୍ୟ)ସନ୍ଦେଶ);
  CURL_TRC_WS(data, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              ସନ୍ଦେଶ, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

ସ୍ଥିର ଶୂନ୍ୟ ws_enc_reset(ସଂରଚନା ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ ws_enc_init(ସଂରଚନା ws_encoder *enc)
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

ସ୍ଥିର CURLcode ws_enc_add_frame(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ws_encoder *enc,
                                 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                                 curl_off_t payload_len,
                                 ସଂରଚନା bufq *out)
{
  uint8_t firstb = 0;
  uint8_t head[14];
  CURLcode result;
  size_t hlen, nwritten;

  ଯଦି(payload_len < 0) {
    failf(data, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }

  ଯଦି(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(data, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }

  result = ws_frame_flags2firstbyte(data, flags, (bool)enc->contfragment,
                                    &firstb);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  ଯଦି(flags & (CURLWS_TEXT | CURLWS_ଦ୍ୱିମିକ)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  ଯଦି(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PING frame is too big");
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  }
  ଯଦି(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given PONG frame is too big");
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  }
  ଯଦି(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(data, "[WS] given CLOSE frame is too big");
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  }

  head[0] = enc->firstbyte = firstb;
  ଯଦି(payload_len > 65535) {
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
  ଅନ୍ୟଥା ଯଦି(payload_len >= 126) {
    head[1] = 126 | WSBIT_MASK;
    head[2] = (uint8_t)((payload_len >> 8) & 0xff);
    head[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  ଅନ୍ୟଥା {
    head[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, data, "sending");

  /* 4 bytes random */

  result = Curl_rand(data, (uint8_t *)&enc->mask, ଆକାର(enc->mask));
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

#ifdef DEBUGBUILD
  ଯଦି(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->mask, 0, ଆକାର(enc->mask));
#endif

  /* add 4 bytes mask */
  memcpy(&head[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  result = Curl_bufq_write(out, head, hlen, &nwritten);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଯଦି(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ws_enc_write_head(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା websocket *ws,
                                  ସଂରଚନା ws_encoder *enc,
                                  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                                  curl_off_t payload_len,
                                  ସଂରଚନା bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  ଯଦି(ws->pending.type) {
    CURLcode result = ws_enc_add_pending(data, ws);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ ws_enc_add_frame(data, enc, flags, payload_len, out);
}

ସ୍ଥିର CURLcode ws_enc_write_payload(ସଂରଚନା ws_encoder *enc,
                                     ସଂରଚନା Curl_easy *data,
                                     ଅଚଳ uint8_t *buf, size_t buflen,
                                     ସଂରଚନା bufq *out, size_t *pnwritten)
{
  CURLcode result;
  size_t i, len, n, remain;

  *pnwritten = 0;
  ଯଦି(Curl_bufq_is_full(out))
    ଫେରନ୍ତୁ CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  ଯଦି(remain < len)
    len = remain;

  ପାଇଁ(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    result = Curl_bufq_write(out, &c, 1, &n);
    ଯଦି(result) {
      ଯଦି((result != CURLE_AGAIN) || !i)
        ଫେରନ୍ତୁ result;
      ଭାଙ୍ଗ;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, data, "buffered");
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ws_enc_add_pending(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା websocket *ws)
{
  CURLcode result;
  size_t n;

  ଯଦି(!ws->pending.type) /* no pending frame here */
    ଫେରନ୍ତୁ CURLE_OK;
  ଯଦି(ws->enc.payload_remain) /* in the middle of another frame */
    ଫେରନ୍ତୁ CURLE_AGAIN;

  result = ws_enc_add_frame(data, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  ଯଦି(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding head: %d",
                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯାଅ out;
  }
  result = ws_enc_write_payload(&ws->enc, data, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  ଯଦି(result) {
    CURL_TRC_WS(data, "ws_enc_cntrl(), error adding payload: %d",
                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯାଅ out;
  }
  ଯଦି(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(data, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    result = CURLE_SEND_ERROR;
    ଯାଅ out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, ଆକାର(ws->pending));

out:
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ws_enc_send(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା websocket *ws,
                            ଅଚଳ uint8_t *buffer,
                            size_t buflen,
                            curl_off_t fragsize,
                            unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!data->set.ws_raw_mode);
  *pnsent = 0;

  ଯଦି(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    ଯଦି(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(data, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ଯଦି((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(data, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  ଅନ୍ୟଥା {
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    result = ws_enc_write_head(data, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    ଯଦି(result) {
      CURL_TRC_WS(data, "curl_ws_send(), error writing frame head %d",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଫେରନ୍ତୁ result;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  ଯେତେବେଳେ(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    ଯଦି(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      result = ws_enc_write_payload(&ws->enc, data,
                                    buffer + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      ଯଦି(result && (result != CURLE_AGAIN))
        ଫେରନ୍ତୁ result;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      ଯଦି(!ws->sendbuf_payload) {
        ଫେରନ୍ତୁ CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    result = ws_flush(data, ws, Curl_api_is_in_callback(data));
    ଯଦି(!result && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      buffer += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    ଅନ୍ୟଥା ଯଦି(result == CURLE_AGAIN) {
      ଯଦି(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଅନ୍ୟଥା {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(data, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        ଫେରନ୍ତୁ CURLE_AGAIN;
      }
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ result;  /* real error sending the data */
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସଂରଚନା cr_ws_ctx {
  ସଂରଚନା Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

ସ୍ଥିର CURLcode cr_ws_init(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_creader *reader)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cr_ws_close(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_creader *reader)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
}

ସ୍ଥିର CURLcode cr_ws_read(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_creader *reader,
                           ଅକ୍ଷର *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ସଂରଚନା cr_ws_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, n;
  ସଂରଚନା websocket *ws;
  bool eos;

  *pnread = 0;
  ଯଦି(ctx->eos) {
    *peos = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ଯଦି(!ws) {
    failf(data, "[WS] not a websocket transfer");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  ଯଦି(Curl_bufq_is_empty(&ws->sendbuf)) {
    ଯଦି(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }

    ଯଦି(ws->enc.payload_remain) {
      CURL_TRC_WS(data, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ctx->read_eos = eos;

    ଯଦି(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(data, reader->next);
    }
    ଅନ୍ୟଥା ଯଦି(!nread) {
      /* nothing to convert, return this right away */
      ଯଦି(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      ଯାଅ out;
    }

    ଯଦି(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      result = ws_enc_write_head(data, ws, &ws->enc, CURLWS_ଦ୍ୱିମିକ, nread,
                                 &ws->sendbuf);
      ଯଦି(result)
        ଯାଅ out;
    }

    result = ws_enc_write_payload(&ws->enc, data, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    ଯଦି(result)
      ଯାଅ out;
    CURL_TRC_READ(data, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  ଯଦି(!result && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread, *peos);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର curl_off_t cr_ws_total_length(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_crtype ws_cr_encode = {
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
  ଆକାର(ସଂରଚନା cr_ws_ctx)
};

ସଂରଚନା wsfield {
  ଅଚଳ ଅକ୍ଷର *name;
  ଅଚଳ ଅକ୍ଷର *val;
};

CURLcode Curl_ws_request(ସଂରଚନା Curl_easy *data, ସଂରଚନା dynbuf *req)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  CURLcode result = CURLE_OK;
  uint8_t rand[16];
  ଅକ୍ଷର *randstr;
  size_t randlen;
  ଅକ୍ଷର keyval[40];
  ସଂରଚନା SingleRequest *k = &data->req;
  ସଂରଚନା wsfield heads[] = {
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
      "Sec-WebSocket-Key", ଖାଲି,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  result = Curl_rand(data, rand, ଆକାର(rand));
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  result = curlx_base64_encode(rand, ଆକାର(rand), &randstr, &randlen);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  DEBUGASSERT(randlen < ଆକାର(keyval));
  ଯଦି(randlen >= ଆକାର(keyval)) {
    curlx_free(randstr);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, ଆକାର(keyval), randstr, randlen);
  curlx_free(randstr);
  ପାଇଁ(i = 0; !result && (i < CURL_ARRAYSIZE(heads)); i++) {
    ଯଦି(!Curl_checkheaders(data, heads[i].name, strlen(heads[i].name))) {
      result = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  data->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  data->conn->bits.upgrade_in_progress = TRUE;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ ws_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା websocket *ws = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(ସଂରଚନା Curl_easy *data,
                        ଅଚଳ ଅକ୍ଷର *mem, size_t nread)
{
  ସଂରଚନା SingleRequest *k = &data->req;
  ସଂରଚନା websocket *ws;
  ସଂରଚନା Curl_cwriter *ws_dec_writer = ଖାଲି;
  ସଂରଚନା Curl_creader *ws_enc_reader = ଖାଲି;
  CURLcode result;

  DEBUGASSERT(data->conn);
  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ଯଦି(!ws) {
    size_t chunk_size = WS_CHUNK_SIZE;
    ws = curlx_calloc(1, ଆକାର(*ws));
    ଯଦି(!ws)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
#ifdef DEBUGBUILD
    {
      ଅଚଳ ଅକ୍ଷର *p = getenv("CURL_WS_CHUNK_SIZE");
      ଯଦି(p) {
        curl_off_t l;
        ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, 1 * 1024 * 1024))
          chunk_size = (size_t)l;
      }
    }
#endif
    CURL_TRC_WS(data, "WS, using chunk size %zu", chunk_size);
    Curl_bufq_init2(&ws->recvbuf, chunk_size, WS_CHUNK_ଗଣନା,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_size, WS_CHUNK_ଗଣନା,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    result = Curl_conn_meta_set(data->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା {
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
  ଯଦି(result)
    ଯାଅ out;
  result = Curl_cwriter_add(data, ws_dec_writer);
  ଯଦି(result)
    ଯାଅ out;
  ws_dec_writer = ଖାଲି; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  ଯଦି(data->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    ଯଦି(nread) {
      result = Curl_cf_recvbuf_add(data, data->conn, FIRSTSOCKET,
                                   (ଅଚଳ uint8_t *)mem, nread);
      ଯଦି(result)
        ଯାଅ out;
    }
    CURL_REQ_CLEAR_RECV(data); /* read no more content */
  }
  ଅନ୍ୟଥା { /* !connect_only */
    ଯଦି(data->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(data, "UPLOAD set, add ws-encode reader");
      result = Curl_creader_set_fread(data, -1);
      ଯଦି(result)
        ଯାଅ out;

      ଯଦି(!data->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        result = Curl_creader_create(&ws_enc_reader, data, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        ଯଦି(result)
          ଯାଅ out;
        result = Curl_creader_add(data, ws_enc_reader);
        ଯଦି(result)
          ଯାଅ out;
        ws_enc_reader = ଖାଲି; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      data->req.eos_read = FALSE;
      data->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(data);
    }

    /* Then pass any additional data to the writers */
    ଯଦି(nread) {
      result = Curl_client_write(data, CLIENTWRITE_BODY, mem, nread);
      ଯଦି(result)
        ଯାଅ out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  ଯଦି(ws_dec_writer)
    Curl_cwriter_free(data, ws_dec_writer);
  ଯଦି(ws_enc_reader)
    Curl_creader_free(data, ws_enc_reader);
  ଯଦି(result)
    CURL_TRC_WS(data, "Curl_ws_accept() failed -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଅନ୍ୟଥା
    CURL_TRC_WS(data, "websocket established, %s mode",
                data->set.connect_only ? "connect-only" : "callback");
  ଫେରନ୍ତୁ result;
}

ସଂରଚନା ws_collect {
  ସଂରଚନା Curl_easy *data;
  ସଂରଚନା websocket *ws;
  uint8_t *buffer;
  size_t buflen;
  size_t bufidx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_age;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

ସ୍ଥିର CURLcode ws_client_collect(ଅଚଳ uint8_t *buf, size_t buflen,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_age, ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  ଶୂନ୍ୟ *userp,
                                  size_t *pnwritten)
{
  ସଂରଚନା ws_collect *ctx = userp;
  ସଂରଚନା Curl_easy *data = ctx->data;
  bool auto_pong = !data->set.ws_raw_mode && !data->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  ଯଦି(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ଯଦି(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  ଯଦି(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(data, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    result = ws_enc_add_cntrl(ctx->data, ctx->ws, buf, buflen, CURLWS_PONG);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    *pnwritten = buflen;
  }
  ଅନ୍ୟଥା {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    ଯଦି(!write_len) {
      ଯଦି(!buflen)  /* 0 length write, we accept that */
        ଫେରନ୍ତୁ CURLE_OK;
      ଫେରନ୍ତୁ CURLE_AGAIN;  /* no more space */
    }
    memcpy(ctx->buffer + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode nw_in_recv(ଶୂନ୍ୟ *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  ସଂରଚନା Curl_easy *data = reader_ctx;
  ଫେରନ୍ତୁ Curl_easy_recv(data, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, ଶୂନ୍ୟ *buffer,
                      size_t buflen, size_t *recv,
                      ଅଚଳ ସଂରଚନା curl_ws_frame **metap)
{
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, ws_recv, &result)) {
    ସଂରଚନା Curl_easy *data = curl;
    ସଂରଚନା connectdata *conn;
    ସଂରଚନା websocket *ws;
    ସଂରଚନା ws_collect ctx;

    ଯଦି(!recv || !metap || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ଯାଅ out;
    }

    *recv = 0;
    *metap = ଖାଲି;

    conn = data->conn;
    ଯଦି(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      ଯଦି(!data->set.connect_only) {
        failf(data, "[WS] CONNECT_ONLY is required");
        result = CURLE_UNSUPPORTED_PROTOCOL;
        ଯାଅ out;
      }

      Curl_getconnectinfo(data, &conn);
      ଯଦି(!conn) {
        failf(data, "[WS] connection not found");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ଯାଅ out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    ଯଦି(!ws) {
      failf(data, "[WS] connection is not setup for websocket");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ଯାଅ out;
    }

    memset(&ctx, 0, ଆକାର(ctx));
    ctx.data = data;
    ctx.ws = ws;
    ctx.buffer = buffer;
    ctx.buflen = buflen;

    ଯେତେବେଳେ(1) {
      /* receive more when our buffer is empty */
      ଯଦି(Curl_bufq_is_empty(&ws->recvbuf)) {
        size_t n;
        result = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, data, &n);
        ଯଦି(result)
          ଯାଅ out;
        ଅନ୍ୟଥା ଯଦି(n == 0) {
          /* connection closed */
          infof(data, "[WS] connection expectedly closed?");
          result = CURLE_GOT_NOTHING;
          ଯାଅ out;
        }
        CURL_TRC_WS(data, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      result = ws_dec_pass(&ws->dec, data, &ws->recvbuf,
                           ws_client_collect, &ctx);
      ଯଦି(result == CURLE_AGAIN) {
        ଯଦି(!ctx.written) {
          ws_dec_info(&ws->dec, data, "need more input");
          ଜାରି;  /* nothing written, try more input */
        }
        ଭାଙ୍ଗ;
      }
      ଅନ୍ୟଥା ଯଦି(result) {
        ଯାଅ out;
      }
      ଅନ୍ୟଥା ଯଦି(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        ଭାଙ୍ଗ;
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
    ଯଦି(!data->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(data, ws);
      ଯଦି(!r2)
        (ଶୂନ୍ୟ)ws_flush(data, ws, Curl_api_is_in_callback(data));
    }
    result = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ws_flush(ସଂରଚନା Curl_easy *data, ସଂରଚନା websocket *ws,
                         bool blocking)
{
  CURLcode result;

  /* If there is space, add any pending control frame */
  ଯଦି(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_size) {
    result = ws_enc_add_pending(data, ws);
    ଯଦି(result && (result != CURLE_AGAIN))
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(!Curl_bufq_is_empty(&ws->sendbuf)) {
    ଅଚଳ uint8_t *out;
    size_t outlen, n;
#ifdef DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_next = FALSE;
    size_t chunk_egain = 0;
    ଅଚଳ ଅକ୍ଷର *p = getenv("CURL_WS_CHUNK_EAGAIN");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#endif

    ଯେତେବେଳେ(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#ifdef DEBUGBUILD
      ଯଦି(eagain_next)
        ଫେରନ୍ତୁ CURLE_AGAIN;
      ଯଦି(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_next = TRUE;
      }
#endif
      ଯଦି(blocking) {
        result = ws_send_raw_blocking(data, ws, (ଅଚଳ ଅକ୍ଷର *)out, outlen);
        n = result ? 0 : outlen;
      }
      ଅନ୍ୟଥା ଯଦି(data->set.connect_only || Curl_api_is_in_callback(data))
        result = Curl_senddata(data, out, outlen, &n);
      ଅନ୍ୟଥା {
        result = Curl_xfer_send(data, out, outlen, FALSE, &n);
        ଯଦି(!result && !n && outlen)
          result = CURLE_AGAIN;
      }

      ଯଦି(result == CURLE_AGAIN) {
        CURL_TRC_WS(data, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        ଫେରନ୍ତୁ result;
      }
      ଅନ୍ୟଥା ଯଦି(result) {
        failf(data, "[WS] flush, write error %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଫେରନ୍ତୁ result;
      }
      ଅନ୍ୟଥା {
        CURL_TRC_WS(data, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ws_send_raw_blocking(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା websocket *ws,
                                     ଅଚଳ ଅକ୍ଷର *buffer, size_t buflen)
{
  CURLcode result = CURLE_OK;
  size_t nwritten;

  ଯଦି(!data)
    ଫେରନ୍ତୁ result;

  (ଶୂନ୍ୟ)ws;
  ଯେତେବେଳେ(buflen) {
    result = Curl_xfer_send(data, buffer, buflen, FALSE, &nwritten);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    DEBUGASSERT(nwritten <= buflen);
    buffer += nwritten;
    buflen -= nwritten;
    ଯଦି(buflen) {
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      timediff_t ବାମ_ms;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ev;

      CURL_TRC_WS(data, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      ବାମ_ms = Curl_timeleft_ms(data);
      ଯଦି(ବାମ_ms < 0) {
        failf(data, "[WS] Timeout waiting for socket becoming writable");
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      ଯଦି(sock == CURL_SOCKET_BAD)
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, ବାମ_ms ? ବାମ_ms : 500);
      ଯଦି(ev < 0) {
        failf(data, "[WS] Error while waiting for socket becoming writable");
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      }
    }
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ws_send_raw(ସଂରଚନା Curl_easy *data, ଅଚଳ ଶୂନ୍ୟ *buffer,
                            size_t buflen, size_t *pnwritten)
{
  ସଂରଚନା websocket *ws;
  CURLcode result;

  ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
  ଯଦି(!ws) {
    failf(data, "[WS] Not a websocket transfer");
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }
  ଯଦି(!buflen)
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(Curl_api_is_in_callback(data)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    result = ws_flush(data, ws, TRUE);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    result = ws_send_raw_blocking(data, ws, buffer, buflen);
    ଯଦି(!result)
      *pnwritten = buflen;
  }
  ଅନ୍ୟଥା {
    /* We need any pending data to be sent or EAGAIN this call. */
    result = ws_flush(data, ws, FALSE);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    result = Curl_senddata(data, buffer, buflen, pnwritten);
  }

  CURL_TRC_WS(data, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten);
  ଫେରନ୍ତୁ result;
}

CURLcode curl_ws_send(CURL *curl, ଅଚଳ ଶୂନ୍ୟ *buffer_arg,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, ws_send, &result)) {
    ସଂରଚନା websocket *ws;
    ଅଚଳ uint8_t *buffer = buffer_arg;
    ସଂରଚନା Curl_easy *data = curl;
    size_t ndummy;
    size_t *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, data->set.ws_raw_mode);

    *pnsent = 0;

    ଯଦି(!buffer && buflen) {
      failf(data, "[WS] buffer is NULL when buflen is not");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ଯାଅ out;
    }

    ଯଦି(!data->conn && data->set.connect_only) {
      result = Curl_connect_only_attach(data);
      ଯଦି(result)
        ଯାଅ out;
    }
    ଯଦି(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ଯଦି(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }

    ଯଦି(data->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      result = ws_flush(data, ws, FALSE);
      ଯଦି(result)
        ଯାଅ out;

      ଯଦି(!buffer) {
        failf(data, "[WS] buffer is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ଯାଅ out;
      }
      ଯଦି(!sent) {
        failf(data, "[WS] sent is NULL in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ଯାଅ out;
      }
      ଯଦି(fragsize || flags) {
        failf(data, "[WS] fragsize and flags must be zero in raw mode");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ଯାଅ out;
      }
      result = ws_send_raw(data, buffer, buflen, pnsent);
      ଯାଅ out;
    }

    /* Not RAW mode, we do the frame encoding */
    result = ws_enc_send(data, ws, buffer, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(data, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, data->set.ws_raw_mode, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ws_setup_conn(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  data->state.http_neg.accept_09 = FALSE;
  data->state.http_neg.only_10 = FALSE;
  data->state.http_neg.wanted = CURL_HTTP_V1x;
  data->state.http_neg.allowed = CURL_HTTP_V1x;
  ଫେରନ୍ତୁ Curl_http_setup_conn(data, conn);
}

ଅଚଳ ସଂରଚନା curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  ସଂରଚନା Curl_easy *data = curl;
  ଯଦି(GOOD_EASY_HANDLE(data) && Curl_api_is_in_callback(data) &&
     data->conn && !data->set.ws_raw_mode) {
    ସଂରଚନା websocket *ws;
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ଯଦି(ws)
      ଫେରନ୍ତୁ &ws->recvframe;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                                         curl_off_t frame_len)
{
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &result)) {
    ସଂରଚନା Curl_easy *data = curl;
    ସଂରଚନା websocket *ws;

    ଯଦି(data->set.ws_raw_mode) {
      failf(data, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      result = CURLE_FAILED_INIT;
      ଯାଅ out;
    }

    CURL_TRC_WS(data, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    ଯଦି(!data->conn) {
      failf(data, "[WS] No associated connection");
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
    ws = Curl_conn_meta_get(data->conn, CURL_META_PROTO_WS_CONN);
    ଯଦି(!ws) {
      failf(data, "[WS] Not a websocket transfer");
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }

    ଯଦି(ws->enc.payload_remain) {
      failf(data, "[WS] previous frame not finished");
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }

    result = ws_enc_write_head(data, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    ଯଦି(result)
      CURL_TRC_WS(data, "curl_start_frame(), error adding frame head %d",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}

ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_ws = {
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

#ଅନ୍ୟଥା

CURLcode curl_ws_recv(CURL *curl, ଶୂନ୍ୟ *buffer, size_t buflen,
                      size_t *recv,
                      ଅଚଳ ସଂରଚନା curl_ws_frame **metap)
{
  (ଶୂନ୍ୟ)curl;
  (ଶୂନ୍ୟ)buffer;
  (ଶୂନ୍ୟ)buflen;
  (ଶୂନ୍ୟ)recv;
  (ଶୂନ୍ୟ)metap;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, ଅଚଳ ଶୂନ୍ୟ *buffer,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  (ଶୂନ୍ୟ)curl;
  (ଶୂନ୍ୟ)buffer;
  (ଶୂନ୍ୟ)buflen;
  (ଶୂନ୍ୟ)sent;
  (ଶୂନ୍ୟ)fragsize;
  (ଶୂନ୍ୟ)flags;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

ଅଚଳ ସଂରଚନା curl_ws_frame *curl_ws_meta(CURL *data)
{
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ ଖାଲି;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                                         curl_off_t frame_len)
{
  (ଶୂନ୍ୟ)curl;
  (ଶୂନ୍ୟ)flags;
  (ଶୂନ୍ୟ)frame_len;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
