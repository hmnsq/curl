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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "ws.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS

#అన్తర్భావ "url.h"
#అన్తర్భావ "bufq.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "cf-recvbuf.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "easyif.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curlx/strcopy.h"

/* RFC 6455 Section 5.2

    0 1 2 3 4 5 6 7
   +-+-+-+-+-------+
   |F|R|R|R| opcode|
   |I|S|S|S|  (4)  |
   |N|V|V|V|       |
   | |1|2|3|       |
 */
#నిర్వచించు WSBIT_FIN          0x80
#నిర్వచించు WSBIT_RSV1         0x40
#నిర్వచించు WSBIT_RSV2         0x20
#నిర్వచించు WSBIT_RSV3         0x10
#నిర్వచించు WSBIT_RSV_ముసుగు     (WSBIT_RSV1 | WSBIT_RSV2 | WSBIT_RSV3)
#నిర్వచించు WSBIT_OPCODE_CONT  0x0
#నిర్వచించు WSBIT_OPCODE_పాఠ్యం  0x1
#నిర్వచించు WSBIT_OPCODE_BIN   0x2
#నిర్వచించు WSBIT_OPCODE_CLOSE 0x8
#నిర్వచించు WSBIT_OPCODE_PING  0x9
#నిర్వచించు WSBIT_OPCODE_PONG  0xa
#నిర్వచితమైతే CURLVERBOSE
#నిర్వచించు WSBIT_OPCODE_ముసుగు  0xf
#అయితే_ముగింపు

#నిర్వచించు WSBIT_ముసుగు 0x80

/* buffer dimensioning */
#నిర్వచించు WS_CHUNK_కొలత  65535
#నిర్వచించు WS_CHUNK_లెక్క 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
గణన ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_తల,
  WS_DEC_PAYLOAD
};

నిర్మాణం ws_decoder {
  పూర్ణాంకం చట్రం_age;        /* zero */
  పూర్ణాంకం చట్రం_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_స్థానభ్రంశం;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t తల[10];
  పూర్ణాంకం తల_len, తల_మొత్తం;
  గణన ws_dec_state state;
  పూర్ణాంకం cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
నిర్మాణం ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  గుర్తులేని పూర్ణాంకం xori; /* xor index */
  uint8_t ముసుగు[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  బిట్టు(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#నిర్వచించు WS_MAX_CNTRL_LEN    125

నిర్మాణం ws_cntrl_చట్రం {
  గుర్తులేని పూర్ణాంకం type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
నిర్మాణం websocket {
  నిర్మాణం Curl_easy *దత్తాంశం; /* used for write callback handling */
  నిర్మాణం ws_decoder dec;  /* decode of ws frames */
  నిర్మాణం ws_encoder enc;  /* encode of ws frames */
  నిర్మాణం bufq recvbuf;    /* raw data from the server */
  నిర్మాణం bufq sendbuf;    /* raw data to be sent to the server */
  నిర్మాణం curl_ws_చట్రం recvframe;  /* the current WS FRAME received */
  నిర్మాణం ws_cntrl_చట్రం pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  బిట్టు(using_reader);      /* sending WS data is handled via a client reader */
};

#నిర్వచితమైతే CURLVERBOSE
స్థిర స్థిరరాశి అక్షరం *ws_చట్రం_name_యొక్క_op(uint8_t firstbyte)
{
  ఎంపిక(firstbyte & WSBIT_OPCODE_ముసుగు) {
  స్థితి WSBIT_OPCODE_CONT:
    తిరిగి "CONT";
  స్థితి WSBIT_OPCODE_పాఠ్యం:
    తిరిగి "TEXT";
  స్థితి WSBIT_OPCODE_BIN:
    తిరిగి "BIN";
  స్థితి WSBIT_OPCODE_CLOSE:
    తిరిగి "CLOSE";
  స్థితి WSBIT_OPCODE_PING:
    తిరిగి "PING";
  స్థితి WSBIT_OPCODE_PONG:
    తిరిగి "PONG";
  అప్రమేయ:
    తిరిగి "???";
  }
}
#అయితే_ముగింపు

స్థిర పూర్ణాంకం ws_చట్రం_firstbyte2flags(నిర్మాణం Curl_easy *దత్తాంశం,
                                    uint8_t firstbyte, పూర్ణాంకం cont_flags)
{
  ఎంపిక(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  స్థితి WSBIT_OPCODE_CONT:
    ఒకవేళ(!(cont_flags & CURLWS_CONT)) {
      failf(దత్తాంశం, "[WS] no ongoing fragmented message to resume");
      తిరిగి 0;
    }
    తిరిగి cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  స్థితి (WSBIT_OPCODE_CONT | WSBIT_FIN):
    ఒకవేళ(!(cont_flags & CURLWS_CONT)) {
      failf(దత్తాంశం, "[WS] no ongoing fragmented message to resume");
      తిరిగి 0;
    }
    తిరిగి cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  స్థితి WSBIT_OPCODE_పాఠ్యం:
    ఒకవేళ(cont_flags & CURLWS_CONT) {
      failf(దత్తాంశం, "[WS] fragmented message interrupted by new TEXT msg");
      తిరిగి 0;
    }
    తిరిగి CURLWS_పాఠ్యం | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  స్థితి (WSBIT_OPCODE_పాఠ్యం | WSBIT_FIN):
    ఒకవేళ(cont_flags & CURLWS_CONT) {
      failf(దత్తాంశం, "[WS] fragmented message interrupted by new TEXT msg");
      తిరిగి 0;
    }
    తిరిగి CURLWS_పాఠ్యం;
  /* 0x02 - first BINARY fragment */
  స్థితి WSBIT_OPCODE_BIN:
    ఒకవేళ(cont_flags & CURLWS_CONT) {
      failf(దత్తాంశం, "[WS] fragmented message interrupted by new BINARY msg");
      తిరిగి 0;
    }
    తిరిగి CURLWS_ద్విమాన | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  స్థితి (WSBIT_OPCODE_BIN | WSBIT_FIN):
    ఒకవేళ(cont_flags & CURLWS_CONT) {
      failf(దత్తాంశం, "[WS] fragmented message interrupted by new BINARY msg");
      తిరిగి 0;
    }
    తిరిగి CURLWS_ద్విమాన;
  /* 0x08 - first CLOSE fragment */
  స్థితి WSBIT_OPCODE_CLOSE:
    failf(దత్తాంశం, "[WS] invalid fragmented CLOSE frame");
    తిరిగి 0;
  /* 0x88 - unfragmented CLOSE */
  స్థితి (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    తిరిగి CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  స్థితి WSBIT_OPCODE_PING:
    failf(దత్తాంశం, "[WS] invalid fragmented PING frame");
    తిరిగి 0;
  /* 0x89 - unfragmented PING */
  స్థితి (WSBIT_OPCODE_PING | WSBIT_FIN):
    తిరిగి CURLWS_PING;
  /* 0x0a - first PONG fragment */
  స్థితి WSBIT_OPCODE_PONG:
    failf(దత్తాంశం, "[WS] invalid fragmented PONG frame");
    తిరిగి 0;
  /* 0x8a - unfragmented PONG */
  స్థితి (WSBIT_OPCODE_PONG | WSBIT_FIN):
    తిరిగి CURLWS_PONG;
  /* invalid first byte */
  అప్రమేయ:
    ఒకవేళ(firstbyte & WSBIT_RSV_ముసుగు)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(దత్తాంశం, "[WS] invalid reserved bits: %02x", firstbyte);
    లేకపోతే
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(దత్తాంశం, "[WS] invalid opcode: %02x", firstbyte);
    తిరిగి 0;
  }
}

స్థిర CURLcode ws_చట్రం_flags2firstbyte(నిర్మాణం Curl_easy *దత్తాంశం,
                                         గుర్తులేని పూర్ణాంకం flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  ఎంపిక(flags & ~CURLWS_స్థానభ్రంశం) {
  స్థితి 0:
    ఒకవేళ(contfragment) {
      CURL_TRC_WS(దత్తాంశం, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      తిరిగి CURLE_OK;
    }
    failf(దత్తాంశం, "[WS] no flags given");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  స్థితి CURLWS_CONT:
    ఒకవేళ(contfragment) {
      infof(దత్తాంశం, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      తిరిగి CURLE_OK;
    }
    failf(దత్తాంశం, "[WS] No ongoing fragmented message to continue");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  స్థితి CURLWS_పాఠ్యం:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_పాఠ్యం | WSBIT_FIN);
    తిరిగి CURLE_OK;
  స్థితి (CURLWS_పాఠ్యం | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_పాఠ్యం;
    తిరిగి CURLE_OK;
  స్థితి CURLWS_ద్విమాన:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    తిరిగి CURLE_OK;
  స్థితి (CURLWS_ద్విమాన | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    తిరిగి CURLE_OK;
  స్థితి CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    తిరిగి CURLE_OK;
  స్థితి (CURLWS_CLOSE | CURLWS_CONT):
    failf(దత్తాంశం, "[WS] CLOSE frame must not be fragmented");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  స్థితి CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    తిరిగి CURLE_OK;
  స్థితి (CURLWS_PING | CURLWS_CONT):
    failf(దత్తాంశం, "[WS] PING frame must not be fragmented");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  స్థితి CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    తిరిగి CURLE_OK;
  స్థితి (CURLWS_PONG | CURLWS_CONT):
    failf(దత్తాంశం, "[WS] PONG frame must not be fragmented");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  అప్రమేయ:
    failf(దత్తాంశం, "[WS] unknown flags: %x", flags);
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
}

స్థిర శూన్య ws_dec_info(నిర్మాణం ws_decoder *dec, నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి అక్షరం *సందేశం)
{
  NOVERBOSE((శూన్య)సందేశం);
  ఎంపిక(dec->తల_len) {
  స్థితి 0:
    ఆపు;
  స్థితి 1:
    CURL_TRC_WS(దత్తాంశం, "decoded %s [%s%s]", సందేశం,
                ws_చట్రం_name_యొక్క_op(dec->తల[0]),
                (dec->తల[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    ఆపు;
  అప్రమేయ:
    ఒకవేళ(dec->తల_len < dec->తల_మొత్తం) {
      CURL_TRC_WS(దత్తాంశం, "decoded %s [%s%s](%d/%d)", సందేశం,
                  ws_చట్రం_name_యొక్క_op(dec->తల[0]),
                  (dec->తల[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->తల_len, dec->తల_మొత్తం);
    }
    లేకపోతే {
      CURL_TRC_WS(దత్తాంశం, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  సందేశం, ws_చట్రం_name_యొక్క_op(dec->తల[0]),
                  (dec->తల[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_స్థానభ్రంశం, dec->payload_len);
    }
    ఆపు;
  }
}

స్థిర CURLcode ws_send_raw_blocking(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం websocket *ws,
                                     స్థిరరాశి అక్షరం *బఫరు, size_t buflen);

రకనిర్వచనం CURLcode ws_write_payload(స్థిరరాశి uint8_t *buf, size_t buflen,
                                  పూర్ణాంకం చట్రం_age, పూర్ణాంకం చట్రం_flags,
                                  curl_off_t payload_స్థానభ్రంశం,
                                  curl_off_t payload_len,
                                  శూన్య *userp,
                                  size_t *pnwritten);

స్థిర శూన్య ws_dec_తదుపరి_చట్రం(నిర్మాణం ws_decoder *dec)
{
  dec->చట్రం_age = 0;
  dec->చట్రం_flags = 0;
  dec->payload_స్థానభ్రంశం = 0;
  dec->payload_len = 0;
  dec->తల_len = dec->తల_మొత్తం = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

స్థిర శూన్య ws_dec_reset(నిర్మాణం ws_decoder *dec)
{
  dec->చట్రం_age = 0;
  dec->చట్రం_flags = 0;
  dec->payload_స్థానభ్రంశం = 0;
  dec->payload_len = 0;
  dec->తల_len = dec->తల_మొత్తం = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

స్థిర శూన్య ws_dec_init(నిర్మాణం ws_decoder *dec)
{
  ws_dec_reset(dec);
}

స్థిర CURLcode ws_dec_read_తల(నిర్మాణం ws_decoder *dec,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం bufq *inraw)
{
  స్థిరరాశి uint8_t *inbuf;
  size_t inlen;

  అయితే(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    ఒకవేళ(dec->తల_len == 0) {
      dec->తల[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->చట్రం_flags = ws_చట్రం_firstbyte2flags(దత్తాంశం, dec->తల[0],
                                                  dec->cont_flags);
      ఒకవేళ(!dec->చట్రం_flags) {
        ws_dec_reset(dec);
        తిరిగి CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      ఒకవేళ(dec->చట్రం_flags & (CURLWS_పాఠ్యం | CURLWS_ద్విమాన)) {
        dec->cont_flags = dec->చట్రం_flags;
      }

      dec->తల_len = 1;
#ఒకవేళ 0
      ws_dec_info(dec, దత్తాంశం, "seeing opcode");
#అయితే_ముగింపు
      కొనసాగు;
    }
    లేకపోతే ఒకవేళ(dec->తల_len == 1) {
      dec->తల[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->తల_len = 2;

      ఒకవేళ(dec->తల[1] & WSBIT_ముసుగు) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(దత్తాంశం, "[WS] masked input frame");
        ws_dec_reset(dec);
        తిరిగి CURLE_RECV_ERROR;
      }
      ఒకవేళ(dec->చట్రం_flags & CURLWS_PING && dec->తల[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(దత్తాంశం, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        తిరిగి CURLE_RECV_ERROR;
      }
      ఒకవేళ(dec->చట్రం_flags & CURLWS_PONG && dec->తల[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(దత్తాంశం, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        తిరిగి CURLE_RECV_ERROR;
      }
      ఒకవేళ(dec->చట్రం_flags & CURLWS_CLOSE && dec->తల[1] > WS_MAX_CNTRL_LEN) {
        failf(దత్తాంశం, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        తిరిగి CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      ఒకవేళ(dec->తల[1] == 126) {
        dec->తల_మొత్తం = 4;
        కొనసాగు;
      }
      లేకపోతే ఒకవేళ(dec->తల[1] == 127) {
        dec->తల_మొత్తం = 10;
        కొనసాగు;
      }
      లేకపోతే {
        dec->తల_మొత్తం = 2;
      }
    }

    ఒకవేళ(dec->తల_len < dec->తల_మొత్తం) {
      dec->తల[dec->తల_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->తల_len;
      ఒకవేళ(dec->తల_len < dec->తల_మొత్తం) {
#ఒకవేళ 0
        ws_dec_info(dec, దత్తాంశం, "decoding head");
#అయితే_ముగింపు
        కొనసాగు;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->తల_len == dec->తల_మొత్తం);
    ఎంపిక(dec->తల_మొత్తం) {
    స్థితి 2:
      dec->payload_len = dec->తల[1];
      ఆపు;
    స్థితి 4:
      dec->payload_len = (dec->తల[2] << 8) | dec->తల[3];
      ఆపు;
    స్థితి 10:
      ఒకవేళ(dec->తల[2] > 127) {
        failf(దత్తాంశం, "[WS] frame length longer than 63 bits not supported");
        తిరిగి CURLE_RECV_ERROR;
      }
      dec->payload_len =
        (curl_off_t)dec->తల[2] << 56 |
        (curl_off_t)dec->తల[3] << 48 |
        (curl_off_t)dec->తల[4] << 40 |
        (curl_off_t)dec->తల[5] << 32 |
        (curl_off_t)dec->తల[6] << 24 |
        (curl_off_t)dec->తల[7] << 16 |
        (curl_off_t)dec->తల[8] <<  8 |
        dec->తల[9];
      ఆపు;
    అప్రమేయ:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(దత్తాంశం, "[WS] unexpected frame header length");
      తిరిగి CURLE_RECV_ERROR;
    }

    dec->చట్రం_age = 0;
    dec->payload_స్థానభ్రంశం = 0;
    ws_dec_info(dec, దత్తాంశం, "head");
    తిరిగి CURLE_OK;
  }
  తిరిగి CURLE_AGAIN;
}

స్థిర CURLcode ws_dec_pass_payload(నిర్మాణం ws_decoder *dec,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం bufq *inraw,
                                    ws_write_payload *write_cb,
                                    శూన్య *write_ctx)
{
  స్థిరరాశి uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode ఫలితం;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_స్థానభ్రంశం,
                                     0, SIZE_MAX);

  అయితే(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(దత్తాంశం)) {
    ఒకవేళ(inlen > remain)
      inlen = remain;
    ఫలితం = write_cb(inbuf, inlen, dec->చట్రం_age, dec->చట్రం_flags,
                      dec->payload_స్థానభ్రంశం, dec->payload_len,
                      write_ctx, &nwritten);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_స్థానభ్రంశం += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_స్థానభ్రంశం,
                                0, SIZE_MAX);
    CURL_TRC_WS(దత్తాంశం, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  తిరిగి remain ? CURLE_AGAIN : CURLE_OK;
}

స్థిర CURLcode ws_dec_pass(నిర్మాణం ws_decoder *dec,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం bufq *inraw,
                            ws_write_payload *write_cb,
                            శూన్య *write_ctx)
{
  CURLcode ఫలితం;

  ఒకవేళ(Curl_bufq_is_ఖాళీలు(inraw))
    తిరిగి CURLE_AGAIN;

  ఎంపిక(dec->state) {
  స్థితి WS_DEC_INIT:
    ws_dec_తదుపరి_చట్రం(dec);
    dec->state = WS_DEC_తల;
    FALLTHROUGH();
  స్థితి WS_DEC_తల:
    ఫలితం = ws_dec_read_తల(dec, దత్తాంశం, inraw);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం != CURLE_AGAIN) {
        failf(దత్తాంశం, "[WS] decode frame error %d", (పూర్ణాంకం)ఫలితం);
        ఆపు;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_ఖాళీలు(inraw));
      ఆపు;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    ఒకవేళ(dec->payload_len == 0) {
      size_t nwritten;
      స్థిరరాశి uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      ఫలితం = write_cb(&tmp, 0, dec->చట్రం_age, dec->చట్రం_flags,
                        0, 0, write_ctx, &nwritten);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      dec->state = WS_DEC_INIT;
      ఆపు;
    }
    FALLTHROUGH();
  స్థితి WS_DEC_PAYLOAD:
    ఫలితం = ws_dec_pass_payload(dec, దత్తాంశం, inraw, write_cb, write_ctx);
    ws_dec_info(dec, దత్తాంశం, "passing");
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    ఆపు;
  అప్రమేయ:
    /* we covered all enums above, but some code analyzers are wimps */
    ఫలితం = CURLE_FAILED_INIT;
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య update_meta(నిర్మాణం websocket *ws,
                        పూర్ణాంకం చట్రం_age, పూర్ణాంకం చట్రం_flags,
                        curl_off_t payload_స్థానభ్రంశం,
                        curl_off_t payload_len,
                        size_t cur_len)
{
  curl_off_t bytesleft = (payload_len - payload_స్థానభ్రంశం - cur_len);

  ws->recvframe.age = చట్రం_age;
  ws->recvframe.flags = చట్రం_flags;
  ws->recvframe.స్థానభ్రంశం = payload_స్థానభ్రంశం;
  ws->recvframe.len = cur_len;
  ws->recvframe.bytesleft = bytesleft;
}

/* WebSocket decoding client writer */
నిర్మాణం ws_cw_ctx {
  నిర్మాణం Curl_cwriter super;
  నిర్మాణం bufq buf;
};

స్థిర CURLcode ws_cw_init(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం ws_cw_ctx *ctx = writer->ctx;
  (శూన్య)దత్తాంశం;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_కొలత, 1, BUFQ_OPT_SOFT_హద్దు);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య ws_cw_close(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం ws_cw_ctx *ctx = writer->ctx;
  (శూన్య)దత్తాంశం;
  Curl_bufq_free(&ctx->buf);
}

నిర్మాణం ws_cw_dec_ctx {
  నిర్మాణం Curl_easy *దత్తాంశం;
  నిర్మాణం websocket *ws;
  నిర్మాణం Curl_cwriter *తదుపరి_writer;
  పూర్ణాంకం cw_type;
};

స్థిర CURLcode ws_flush(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం websocket *ws,
                         bool blocking);
స్థిర CURLcode ws_enc_కూడు_pending(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం websocket *ws);

స్థిర CURLcode ws_enc_కూడు_cntrl(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం websocket *ws,
                                 స్థిరరాశి uint8_t *payload,
                                 size_t plen,
                                 గుర్తులేని పూర్ణాంకం చట్రం_type)
{
  (శూన్య)దత్తాంశం;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  ఒకవేళ(plen > WS_MAX_CNTRL_LEN)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = చట్రం_type;
  ws->pending.payload_len = plen;
  జ్ఞాపకం_నకలు(ws->pending.payload, payload, plen);
  తిరిగి CURLE_OK;
}

స్థిర curl_off_t ws_payload_remain(curl_off_t payload_మొత్తం,
                                    curl_off_t payload_స్థానభ్రంశం,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_మొత్తం - payload_స్థానభ్రంశం;
  ఒకవేళ((payload_మొత్తం < 0) || (payload_స్థానభ్రంశం < 0) || (remain < 0))
    తిరిగి -1;
  buffered = curlx_uztoso(payload_buffered);
  ఒకవేళ(remain < buffered)
    తిరిగి -1;
  తిరిగి remain - buffered;
}

స్థిర CURLcode ws_cw_dec_తదుపరి(స్థిరరాశి uint8_t *buf, size_t buflen,
                               పూర్ణాంకం చట్రం_age, పూర్ణాంకం చట్రం_flags,
                               curl_off_t payload_స్థానభ్రంశం,
                               curl_off_t payload_len,
                               శూన్య *user_దత్తాంశం,
                               size_t *pnwritten)
{
  నిర్మాణం ws_cw_dec_ctx *ctx = user_దత్తాంశం;
  నిర్మాణం Curl_easy *దత్తాంశం = ctx->దత్తాంశం;
  నిర్మాణం websocket *ws = ctx->ws;
  bool auto_pong = !దత్తాంశం->సమితి.ws_raw_బహుళకం && !దత్తాంశం->సమితి.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode ఫలితం;

  (శూన్య)చట్రం_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_స్థానభ్రంశం, buflen);
  ఒకవేళ(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  ఒకవేళ(auto_pong && (చట్రం_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(దత్తాంశం, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_స్థానభ్రంశం, payload_len);
    /* send back the exact same content as a PONG */
    ఫలితం = ws_enc_కూడు_cntrl(దత్తాంశం, ws, buf, buflen, CURLWS_PONG);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  లేకపోతే ఒకవేళ(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, చట్రం_age, చట్రం_flags, payload_స్థానభ్రంశం,
                payload_len, buflen);

    CURL_TRC_WRITE(దత్తాంశం, "[WS] pass %zu decoded bytes", buflen);
    ఫలితం = Curl_cwriter_write(దత్తాంశం, ctx->తదుపరి_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (స్థిరరాశి అక్షరం *)buf, buflen);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  *pnwritten = buflen;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ws_cw_write(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                            స్థిరరాశి అక్షరం *buf, size_t nbytes)
{
  నిర్మాణం ws_cw_ctx *ctx = writer->ctx;
  నిర్మాణం websocket *ws;
  CURLcode ఫలితం = CURLE_OK;

  CURL_TRC_WRITE(దత్తాంశం, "[WS] write(len=%zu, type=%d)", nbytes, type);
  ఒకవేళ(!(type & CLIENTWRITE_BODY) || దత్తాంశం->సమితి.ws_raw_బహుళకం)
    తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, nbytes);

  ws = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_PROTO_WS_CONN);
  ఒకవేళ(!ws) {
    failf(దత్తాంశం, "[WS] not a websocket transfer");
    తిరిగి CURLE_FAILED_INIT;
  }

  ఒకవేళ(nbytes) {
    size_t nwritten;
    ఫలితం = Curl_bufq_write(&ctx->buf, (స్థిరరాశి uint8_t *)buf,
                             nbytes, &nwritten);
    ఒకవేళ(ఫలితం) {
      infof(దత్తాంశం, "[WS] error adding data to buffer %d", (పూర్ణాంకం)ఫలితం);
      తిరిగి ఫలితం;
    }
  }

  ఫలితం = Curl_cwriter_flush(దత్తాంశం, writer->తదుపరి);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  అయితే(!Curl_bufq_is_ఖాళీలు(&ctx->buf) && !Curl_cwriter_is_paused(దత్తాంశం)) {
    నిర్మాణం ws_cw_dec_ctx pass_ctx;
    pass_ctx.దత్తాంశం = దత్తాంశం;
    pass_ctx.ws = ws;
    pass_ctx.తదుపరి_writer = writer->తదుపరి;
    pass_ctx.cw_type = type;
    ఫలితం = ws_dec_pass(&ws->dec, దత్తాంశం, &ctx->buf,
                         ws_cw_dec_తదుపరి, &pass_ctx);
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      ఫలితం = CURLE_OK;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "[WS] decode payload error %d", (పూర్ణాంకం)ఫలితం);
      Curl_bufq_reset(&ctx->buf);
      వెళ్ళు బహిర్గతం;
    }
  }

బహిర్గతం:
  ఒకవేళ(!ఫలితం && !ws->using_reader) {
    ఫలితం = ws_flush(దత్తాంశం, ws, Curl_api_is_లోపల_callback(దత్తాంశం));
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      ఫలితం = CURLE_OK;
  }

  ఒకవేళ(!ఫలితం && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_ఖాళీలు(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(దత్తాంశం, "[WS] decode ended with incomplete frame");
    ఫలితం = CURLE_RECV_ERROR;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ws_cw_flush(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_cwriter *writer)
{
  CURLcode ఫలితం = CURLE_OK;

  CURL_TRC_WRITE(దత్తాంశం, "[ws] flush");
  ఒకవేళ(!దత్తాంశం->సమితి.ws_raw_బహుళకం) {
    నిర్మాణం ws_cw_ctx *ctx = writer->ctx;
    నిర్మాణం websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    ఫలితం = Curl_cwriter_flush(దత్తాంశం, writer->తదుపరి);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    ws = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_PROTO_WS_CONN);
    ఒకవేళ(!ws) {
      failf(దత్తాంశం, "[WS] not a websocket transfer");
      తిరిగి CURLE_FAILED_INIT;
    }

    అయితే(!Curl_bufq_is_ఖాళీలు(&ctx->buf) && !Curl_cwriter_is_paused(దత్తాంశం)) {
      నిర్మాణం ws_cw_dec_ctx pass_ctx;
      pass_ctx.దత్తాంశం = దత్తాంశం;
      pass_ctx.ws = ws;
      pass_ctx.తదుపరి_writer = writer->తదుపరి;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      ఫలితం = ws_dec_pass(&ws->dec, దత్తాంశం, &ctx->buf,
                           ws_cw_dec_తదుపరి, &pass_ctx);
      ఒకవేళ(ఫలితం == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        ఫలితం = CURLE_OK;
        వెళ్ళు బహిర్గతం;
      }
      లేకపోతే ఒకవేళ(ఫలితం) {
        failf(దత్తాంశం, "[WS] decode payload error %d", (పూర్ణాంకం)ఫలితం);
        Curl_bufq_reset(&ctx->buf);
        వెళ్ళు బహిర్గతం;
      }
    }
  }

బహిర్గతం:
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_cwriter_flush(దత్తాంశం, writer->తదుపరి);
  తిరిగి ఫలితం;
}

/* WebSocket payload decoding client writer. */
స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype ws_cw_decode = {
  "ws-decode",
  ఖాళీ,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  పరిమాణం(నిర్మాణం ws_cw_ctx)
};

స్థిర శూన్య ws_enc_info(నిర్మాణం ws_encoder *enc, నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి అక్షరం *సందేశం)
{
  NOVERBOSE((శూన్య)enc);
  NOVERBOSE((శూన్య)సందేశం);
  CURL_TRC_WS(దత్తాంశం, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              సందేశం, ws_చట్రం_name_యొక్క_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

స్థిర శూన్య ws_enc_reset(నిర్మాణం ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

స్థిర శూన్య ws_enc_init(నిర్మాణం ws_encoder *enc)
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

స్థిర CURLcode ws_enc_కూడు_చట్రం(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ws_encoder *enc,
                                 గుర్తులేని పూర్ణాంకం flags,
                                 curl_off_t payload_len,
                                 నిర్మాణం bufq *బహిర్గతం)
{
  uint8_t firstb = 0;
  uint8_t తల[14];
  CURLcode ఫలితం;
  size_t hlen, nwritten;

  ఒకవేళ(payload_len < 0) {
    failf(దత్తాంశం, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    తిరిగి CURLE_SEND_ERROR;
  }

  ఒకవేళ(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(దత్తాంశం, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    తిరిగి CURLE_SEND_ERROR;
  }

  ఫలితం = ws_చట్రం_flags2firstbyte(దత్తాంశం, flags, (bool)enc->contfragment,
                                    &firstb);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  ఒకవేళ(flags & (CURLWS_పాఠ్యం | CURLWS_ద్విమాన)) {
    enc->contfragment = (curl_బిట్టు)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  ఒకవేళ(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(దత్తాంశం, "[WS] given PING frame is too big");
    తిరిగి CURLE_TOO_LARGE;
  }
  ఒకవేళ(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(దత్తాంశం, "[WS] given PONG frame is too big");
    తిరిగి CURLE_TOO_LARGE;
  }
  ఒకవేళ(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(దత్తాంశం, "[WS] given CLOSE frame is too big");
    తిరిగి CURLE_TOO_LARGE;
  }

  తల[0] = enc->firstbyte = firstb;
  ఒకవేళ(payload_len > 65535) {
    తల[1] = 127 | WSBIT_ముసుగు;
    తల[2] = (uint8_t)((payload_len >> 56) & 0xff);
    తల[3] = (uint8_t)((payload_len >> 48) & 0xff);
    తల[4] = (uint8_t)((payload_len >> 40) & 0xff);
    తల[5] = (uint8_t)((payload_len >> 32) & 0xff);
    తల[6] = (uint8_t)((payload_len >> 24) & 0xff);
    తల[7] = (uint8_t)((payload_len >> 16) & 0xff);
    తల[8] = (uint8_t)((payload_len >> 8) & 0xff);
    తల[9] = (uint8_t)(payload_len & 0xff);
    hlen = 10;
  }
  లేకపోతే ఒకవేళ(payload_len >= 126) {
    తల[1] = 126 | WSBIT_ముసుగు;
    తల[2] = (uint8_t)((payload_len >> 8) & 0xff);
    తల[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  లేకపోతే {
    తల[1] = (uint8_t)payload_len | WSBIT_ముసుగు;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, దత్తాంశం, "sending");

  /* 4 bytes random */

  ఫలితం = Curl_rand(దత్తాంశం, (uint8_t *)&enc->ముసుగు, పరిమాణం(enc->ముసుగు));
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->ముసుగు, 0, పరిమాణం(enc->ముసుగు));
#అయితే_ముగింపు

  /* add 4 bytes mask */
  జ్ఞాపకం_నకలు(&తల[hlen], &enc->ముసుగు, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  ఫలితం = Curl_bufq_write(బహిర్గతం, తల, hlen, &nwritten);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఒకవేళ(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    తిరిగి CURLE_SEND_ERROR;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ws_enc_write_తల(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం websocket *ws,
                                  నిర్మాణం ws_encoder *enc,
                                  గుర్తులేని పూర్ణాంకం flags,
                                  curl_off_t payload_len,
                                  నిర్మాణం bufq *బహిర్గతం)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  ఒకవేళ(ws->pending.type) {
    CURLcode ఫలితం = ws_enc_కూడు_pending(దత్తాంశం, ws);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  తిరిగి ws_enc_కూడు_చట్రం(దత్తాంశం, enc, flags, payload_len, బహిర్గతం);
}

స్థిర CURLcode ws_enc_write_payload(నిర్మాణం ws_encoder *enc,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     స్థిరరాశి uint8_t *buf, size_t buflen,
                                     నిర్మాణం bufq *బహిర్గతం, size_t *pnwritten)
{
  CURLcode ఫలితం;
  size_t i, len, n, remain;

  *pnwritten = 0;
  ఒకవేళ(Curl_bufq_is_నిండిన(బహిర్గతం))
    తిరిగి CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  ఒకవేళ(remain < len)
    len = remain;

  కొరకు(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->ముసుగు[enc->xori];
    ఫలితం = Curl_bufq_write(బహిర్గతం, &c, 1, &n);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ((ఫలితం != CURLE_AGAIN) || !i)
        తిరిగి ఫలితం;
      ఆపు;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, దత్తాంశం, "buffered");
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ws_enc_కూడు_pending(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం websocket *ws)
{
  CURLcode ఫలితం;
  size_t n;

  ఒకవేళ(!ws->pending.type) /* no pending frame here */
    తిరిగి CURLE_OK;
  ఒకవేళ(ws->enc.payload_remain) /* in the middle of another frame */
    తిరిగి CURLE_AGAIN;

  ఫలితం = ws_enc_కూడు_చట్రం(దత్తాంశం, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  ఒకవేళ(ఫలితం) {
    CURL_TRC_WS(దత్తాంశం, "ws_enc_cntrl(), error adding head: %d",
                (పూర్ణాంకం)ఫలితం);
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = ws_enc_write_payload(&ws->enc, దత్తాంశం, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  ఒకవేళ(ఫలితం) {
    CURL_TRC_WS(దత్తాంశం, "ws_enc_cntrl(), error adding payload: %d",
                (పూర్ణాంకం)ఫలితం);
    వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(దత్తాంశం, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, పరిమాణం(ws->pending));

బహిర్గతం:
  తిరిగి ఫలితం;
}

స్థిర CURLcode ws_enc_send(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం websocket *ws,
                            స్థిరరాశి uint8_t *బఫరు,
                            size_t buflen,
                            curl_off_t fragsize,
                            గుర్తులేని పూర్ణాంకం flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(!దత్తాంశం->సమితి.ws_raw_బహుళకం);
  *pnsent = 0;

  ఒకవేళ(ws->enc.payload_remain || !Curl_bufq_is_ఖాళీలు(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    ఒకవేళ(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(దత్తాంశం, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
    ఒకవేళ((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(దత్తాంశం, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
  }
  లేకపోతే {
    ఫలితం = ws_flush(దత్తాంశం, ws, Curl_api_is_లోపల_callback(దత్తాంశం));
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఫలితం = ws_enc_write_తల(దత్తాంశం, ws, &ws->enc, flags,
                               (flags & CURLWS_స్థానభ్రంశం) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    ఒకవేళ(ఫలితం) {
      CURL_TRC_WS(దత్తాంశం, "curl_ws_send(), error writing frame head %d",
                  (పూర్ణాంకం)ఫలితం);
      తిరిగి ఫలితం;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  అయితే(!Curl_bufq_is_ఖాళీలు(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    ఒకవేళ(buflen > ws->sendbuf_payload) {
      size_t prev_len = Curl_bufq_len(&ws->sendbuf);
      ఫలితం = ws_enc_write_payload(&ws->enc, దత్తాంశం,
                                    బఫరు + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN))
        తిరిగి ఫలితం;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - prev_len;
      ఒకవేళ(!ws->sendbuf_payload) {
        తిరిగి CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    ఫలితం = ws_flush(దత్తాంశం, ws, Curl_api_is_లోపల_callback(దత్తాంశం));
    ఒకవేళ(!ఫలితం && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      బఫరు += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    లేకపోతే ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      ఒకవేళ(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        తిరిగి CURLE_OK;
      }
      లేకపోతే {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(దత్తాంశం, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        తిరిగి CURLE_AGAIN;
      }
    }
    లేకపోతే
      తిరిగి ఫలితం;  /* real error sending the data */
  }
  తిరిగి CURLE_OK;
}

నిర్మాణం cr_ws_ctx {
  నిర్మాణం Curl_creader super;
  బిట్టు(read_eos);  /* we read an EOS from the next reader */
  బిట్టు(eos);       /* we have returned an EOS */
};

స్థిర CURLcode cr_ws_init(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_creader *reader)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cr_ws_close(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_creader *reader)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
}

స్థిర CURLcode cr_ws_read(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_creader *reader,
                           అక్షరం *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  నిర్మాణం cr_ws_ctx *ctx = reader->ctx;
  CURLcode ఫలితం = CURLE_OK;
  size_t nread, n;
  నిర్మాణం websocket *ws;
  bool eos;

  *pnread = 0;
  ఒకవేళ(ctx->eos) {
    *peos = TRUE;
    తిరిగి CURLE_OK;
  }

  ws = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_PROTO_WS_CONN);
  ఒకవేళ(!ws) {
    failf(దత్తాంశం, "[WS] not a websocket transfer");
    తిరిగి CURLE_FAILED_INIT;
  }

  ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ws->sendbuf)) {
    ఒకవేళ(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      తిరిగి CURLE_OK;
    }

    ఒకవేళ(ws->enc.payload_remain) {
      CURL_TRC_WS(దత్తాంశం, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    ఫలితం = Curl_creader_read(దత్తాంశం, reader->తదుపరి, buf, blen, &nread, &eos);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ctx->read_eos = eos;

    ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(దత్తాంశం, reader->తదుపరి);
    }
    లేకపోతే ఒకవేళ(!nread) {
      /* nothing to convert, return this right away */
      ఒకవేళ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(!ws->enc.payload_remain && Curl_bufq_is_ఖాళీలు(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      ఫలితం = ws_enc_write_తల(దత్తాంశం, ws, &ws->enc, CURLWS_ద్విమాన, nread,
                                 &ws->sendbuf);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }

    ఫలితం = ws_enc_write_payload(&ws->enc, దత్తాంశం, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    CURL_TRC_READ(దత్తాంశం, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_ఖాళీలు(&ws->sendbuf));
  *peos = FALSE;
  ఫలితం = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  ఒకవేళ(!ఫలితం && ctx->read_eos && Curl_bufq_is_ఖాళీలు(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

బహిర్గతం:
  CURL_TRC_READ(దత్తాంశం, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (పూర్ణాంకం)ఫలితం, *pnread, *peos);
  తిరిగి ఫలితం;
}

స్థిర curl_off_t cr_ws_మొత్తం_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  తిరిగి -1;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_మొత్తం_నిడివి,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_పూర్తి,
  పరిమాణం(నిర్మాణం cr_ws_ctx)
};

నిర్మాణం wsfield {
  స్థిరరాశి అక్షరం *name;
  స్థిరరాశి అక్షరం *val;
};

CURLcode Curl_ws_request(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం dynbuf *req)
{
  గుర్తులేని పూర్ణాంకం i;
  CURLcode ఫలితం = CURLE_OK;
  uint8_t rand[16];
  అక్షరం *randstr;
  size_t randlen;
  అక్షరం keyval[40];
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  నిర్మాణం wsfield heads[] = {
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
      "Sec-WebSocket-Key", ఖాళీ,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  ఫలితం = Curl_rand(దత్తాంశం, rand, పరిమాణం(rand));
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఫలితం = curlx_base64_encode(rand, పరిమాణం(rand), &randstr, &randlen);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  DEBUGASSERT(randlen < పరిమాణం(keyval));
  ఒకవేళ(randlen >= పరిమాణం(keyval)) {
    curlx_free(randstr);
    తిరిగి CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, పరిమాణం(keyval), randstr, randlen);
  curlx_free(randstr);
  కొరకు(i = 0; !ఫలితం && (i < CURL_ARRAYSIZE(heads)); i++) {
    ఒకవేళ(!Curl_checkheaders(దత్తాంశం, heads[i].name, అక్షరశ్రేణి_పొడవు(heads[i].name))) {
      ఫలితం = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  దత్తాంశం->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  దత్తాంశం->conn->బిట్లు.upgrade_లోపల_పురోగతి = TRUE;
  తిరిగి ఫలితం;
}

స్థిర శూన్య ws_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం websocket *ws = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి అక్షరం *mem, size_t nread)
{
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  నిర్మాణం websocket *ws;
  నిర్మాణం Curl_cwriter *ws_dec_writer = ఖాళీ;
  నిర్మాణం Curl_creader *ws_enc_reader = ఖాళీ;
  CURLcode ఫలితం;

  DEBUGASSERT(దత్తాంశం->conn);
  ws = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_PROTO_WS_CONN);
  ఒకవేళ(!ws) {
    size_t chunk_కొలత = WS_CHUNK_కొలత;
    ws = curlx_calloc(1, పరిమాణం(*ws));
    ఒకవేళ(!ws)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
#నిర్వచితమైతే DEBUGBUILD
    {
      స్థిరరాశి అక్షరం *p = getenv("CURL_WS_CHUNK_SIZE");
      ఒకవేళ(p) {
        curl_off_t l;
        ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, 1 * 1024 * 1024))
          chunk_కొలత = (size_t)l;
      }
    }
#అయితే_ముగింపు
    CURL_TRC_WS(దత్తాంశం, "WS, using chunk size %zu", chunk_కొలత);
    Curl_bufq_init2(&ws->recvbuf, chunk_కొలత, WS_CHUNK_లెక్క,
                    BUFQ_OPT_SOFT_హద్దు);
    Curl_bufq_init2(&ws->sendbuf, chunk_కొలత, WS_CHUNK_లెక్క,
                    BUFQ_OPT_SOFT_హద్దు);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    ఫలితం = Curl_conn_meta_సమితి(దత్తాంశం->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  లేకపోతే {
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

  infof(దత్తాంశం, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  ఫలితం = Curl_cwriter_create(&ws_dec_writer, దత్తాంశం, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఫలితం = Curl_cwriter_కూడు(దత్తాంశం, ws_dec_writer);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ws_dec_writer = ఖాళీ; /* owned by transfer now */

  k->శీర్షిక = FALSE; /* we will not get more response headers */

  ఒకవేళ(దత్తాంశం->సమితి.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    ఒకవేళ(nread) {
      ఫలితం = Curl_cf_recvbuf_కూడు(దత్తాంశం, దత్తాంశం->conn, FIRSTSOCKET,
                                   (స్థిరరాశి uint8_t *)mem, nread);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
    CURL_REQ_CLEAR_RECV(దత్తాంశం); /* read no more content */
  }
  లేకపోతే { /* !connect_only */
    ఒకవేళ(దత్తాంశం->సమితి.పద్ధతి == HTTPREQ_PUT) {
      CURL_TRC_WS(దత్తాంశం, "UPLOAD set, add ws-encode reader");
      ఫలితం = Curl_creader_సమితి_fread(దత్తాంశం, -1);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;

      ఒకవేళ(!దత్తాంశం->సమితి.ws_raw_బహుళకం) {
        /* Add our client reader encoding WS BINARY frames */
        ఫలితం = Curl_creader_create(&ws_enc_reader, దత్తాంశం, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
        ఫలితం = Curl_creader_కూడు(దత్తాంశం, ws_enc_reader);
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
        ws_enc_reader = ఖాళీ; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      దత్తాంశం->req.eos_read = FALSE;
      దత్తాంశం->req.upload_పూర్తి = FALSE;
      CURL_REQ_సమితి_SEND(దత్తాంశం);
    }

    /* Then pass any additional data to the writers */
    ఒకవేళ(nread) {
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, mem, nread);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->శీర్షిక = FALSE; /* we will not get more responses */

బహిర్గతం:
  ఒకవేళ(ws_dec_writer)
    Curl_cwriter_free(దత్తాంశం, ws_dec_writer);
  ఒకవేళ(ws_enc_reader)
    Curl_creader_free(దత్తాంశం, ws_enc_reader);
  ఒకవేళ(ఫలితం)
    CURL_TRC_WS(దత్తాంశం, "Curl_ws_accept() failed -> %d", (పూర్ణాంకం)ఫలితం);
  లేకపోతే
    CURL_TRC_WS(దత్తాంశం, "websocket established, %s mode",
                దత్తాంశం->సమితి.connect_only ? "connect-only" : "callback");
  తిరిగి ఫలితం;
}

నిర్మాణం ws_collect {
  నిర్మాణం Curl_easy *దత్తాంశం;
  నిర్మాణం websocket *ws;
  uint8_t *బఫరు;
  size_t buflen;
  size_t bufidx;
  పూర్ణాంకం చట్రం_age;
  పూర్ణాంకం చట్రం_flags;
  curl_off_t payload_స్థానభ్రంశం;
  curl_off_t payload_len;
  bool written;
};

స్థిర CURLcode ws_client_collect(స్థిరరాశి uint8_t *buf, size_t buflen,
                                  పూర్ణాంకం చట్రం_age, పూర్ణాంకం చట్రం_flags,
                                  curl_off_t payload_స్థానభ్రంశం,
                                  curl_off_t payload_len,
                                  శూన్య *userp,
                                  size_t *pnwritten)
{
  నిర్మాణం ws_collect *ctx = userp;
  నిర్మాణం Curl_easy *దత్తాంశం = ctx->దత్తాంశం;
  bool auto_pong = !దత్తాంశం->సమితి.ws_raw_బహుళకం && !దత్తాంశం->సమితి.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode ఫలితం = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_స్థానభ్రంశం, buflen);
  ఒకవేళ(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  ఒకవేళ(!ctx->bufidx) {
    /* first write */
    ctx->చట్రం_age = చట్రం_age;
    ctx->చట్రం_flags = చట్రం_flags;
    ctx->payload_స్థానభ్రంశం = payload_స్థానభ్రంశం;
    ctx->payload_len = payload_len;
  }

  ఒకవేళ(auto_pong && (చట్రం_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(దత్తాంశం, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_స్థానభ్రంశం, payload_len);
    /* send back the exact same content as a PONG */
    ఫలితం = ws_enc_కూడు_cntrl(ctx->దత్తాంశం, ctx->ws, buf, buflen, CURLWS_PONG);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    *pnwritten = buflen;
  }
  లేకపోతే {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    ఒకవేళ(!write_len) {
      ఒకవేళ(!buflen)  /* 0 length write, we accept that */
        తిరిగి CURLE_OK;
      తిరిగి CURLE_AGAIN;  /* no more space */
    }
    జ్ఞాపకం_నకలు(ctx->బఫరు + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode nw_లోపల_recv(శూన్య *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  నిర్మాణం Curl_easy *దత్తాంశం = reader_ctx;
  తిరిగి Curl_easy_recv(దత్తాంశం, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, శూన్య *బఫరు,
                      size_t buflen, size_t *recv,
                      స్థిరరాశి నిర్మాణం curl_ws_చట్రం **metap)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, ws_recv, &ఫలితం)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    నిర్మాణం connectdata *conn;
    నిర్మాణం websocket *ws;
    నిర్మాణం ws_collect ctx;

    ఒకవేళ(!recv || !metap || (buflen && !బఫరు)) {
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }

    *recv = 0;
    *metap = ఖాళీ;

    conn = దత్తాంశం->conn;
    ఒకవేళ(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      ఒకవేళ(!దత్తాంశం->సమితి.connect_only) {
        failf(దత్తాంశం, "[WS] CONNECT_ONLY is required");
        ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;
        వెళ్ళు బహిర్గతం;
      }

      Curl_getconnectinfo(దత్తాంశం, &conn);
      ఒకవేళ(!conn) {
        failf(దత్తాంశం, "[WS] connection not found");
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
        వెళ్ళు బహిర్గతం;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    ఒకవేళ(!ws) {
      failf(దత్తాంశం, "[WS] connection is not setup for websocket");
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }

    memset(&ctx, 0, పరిమాణం(ctx));
    ctx.దత్తాంశం = దత్తాంశం;
    ctx.ws = ws;
    ctx.బఫరు = బఫరు;
    ctx.buflen = buflen;

    అయితే(1) {
      /* receive more when our buffer is empty */
      ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ws->recvbuf)) {
        size_t n;
        ఫలితం = Curl_bufq_slurp(&ws->recvbuf, nw_లోపల_recv, దత్తాంశం, &n);
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
        లేకపోతే ఒకవేళ(n == 0) {
          /* connection closed */
          infof(దత్తాంశం, "[WS] connection expectedly closed?");
          ఫలితం = CURLE_GOT_NOTHING;
          వెళ్ళు బహిర్గతం;
        }
        CURL_TRC_WS(దత్తాంశం, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      ఫలితం = ws_dec_pass(&ws->dec, దత్తాంశం, &ws->recvbuf,
                           ws_client_collect, &ctx);
      ఒకవేళ(ఫలితం == CURLE_AGAIN) {
        ఒకవేళ(!ctx.written) {
          ws_dec_info(&ws->dec, దత్తాంశం, "need more input");
          కొనసాగు;  /* nothing written, try more input */
        }
        ఆపు;
      }
      లేకపోతే ఒకవేళ(ఫలితం) {
        వెళ్ళు బహిర్గతం;
      }
      లేకపోతే ఒకవేళ(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        ఆపు;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.చట్రం_age, ctx.చట్రం_flags, ctx.payload_స్థానభ్రంశం,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(దత్తాంశం, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.స్థానభ్రంశం,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    ఒకవేళ(!దత్తాంశం->సమితి.ws_raw_బహుళకం && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_కూడు_pending(దత్తాంశం, ws);
      ఒకవేళ(!r2)
        (శూన్య)ws_flush(దత్తాంశం, ws, Curl_api_is_లోపల_callback(దత్తాంశం));
    }
    ఫలితం = CURLE_OK;
  }
బహిర్గతం:
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ws_flush(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం websocket *ws,
                         bool blocking)
{
  CURLcode ఫలితం;

  /* If there is space, add any pending control frame */
  ఒకవేళ(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_కొలత) {
    ఫలితం = ws_enc_కూడు_pending(దత్తాంశం, ws);
    ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN))
      తిరిగి ఫలితం;
  }

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ws->sendbuf)) {
    స్థిరరాశి uint8_t *బహిర్గతం;
    size_t outlen, n;
#నిర్వచితమైతే DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_తదుపరి = FALSE;
    size_t chunk_egain = 0;
    స్థిరరాశి అక్షరం *p = getenv("CURL_WS_CHUNK_EAGAIN");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#అయితే_ముగింపు

    అయితే(Curl_bufq_peek(&ws->sendbuf, &బహిర్గతం, &outlen)) {
#నిర్వచితమైతే DEBUGBUILD
      ఒకవేళ(eagain_తదుపరి)
        తిరిగి CURLE_AGAIN;
      ఒకవేళ(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_తదుపరి = TRUE;
      }
#అయితే_ముగింపు
      ఒకవేళ(blocking) {
        ఫలితం = ws_send_raw_blocking(దత్తాంశం, ws, (స్థిరరాశి అక్షరం *)బహిర్గతం, outlen);
        n = ఫలితం ? 0 : outlen;
      }
      లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.connect_only || Curl_api_is_లోపల_callback(దత్తాంశం))
        ఫలితం = Curl_senddata(దత్తాంశం, బహిర్గతం, outlen, &n);
      లేకపోతే {
        ఫలితం = Curl_xfer_send(దత్తాంశం, బహిర్గతం, outlen, FALSE, &n);
        ఒకవేళ(!ఫలితం && !n && outlen)
          ఫలితం = CURLE_AGAIN;
      }

      ఒకవేళ(ఫలితం == CURLE_AGAIN) {
        CURL_TRC_WS(దత్తాంశం, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        తిరిగి ఫలితం;
      }
      లేకపోతే ఒకవేళ(ఫలితం) {
        failf(దత్తాంశం, "[WS] flush, write error %d", (పూర్ణాంకం)ఫలితం);
        తిరిగి ఫలితం;
      }
      లేకపోతే {
        CURL_TRC_WS(దత్తాంశం, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ws_send_raw_blocking(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం websocket *ws,
                                     స్థిరరాశి అక్షరం *బఫరు, size_t buflen)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t nwritten;

  ఒకవేళ(!దత్తాంశం)
    తిరిగి ఫలితం;

  (శూన్య)ws;
  అయితే(buflen) {
    ఫలితం = Curl_xfer_send(దత్తాంశం, బఫరు, buflen, FALSE, &nwritten);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    DEBUGASSERT(nwritten <= buflen);
    బఫరు += nwritten;
    buflen -= nwritten;
    ఒకవేళ(buflen) {
      curl_socket_t sock = దత్తాంశం->conn->sock[FIRSTSOCKET];
      timediff_t ఎడమ_ms;
      పూర్ణాంకం ev;

      CURL_TRC_WS(దత్తాంశం, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      ఎడమ_ms = Curl_timeleft_ms(దత్తాంశం);
      ఒకవేళ(ఎడమ_ms < 0) {
        failf(దత్తాంశం, "[WS] Timeout waiting for socket becoming writable");
        తిరిగి CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      ఒకవేళ(sock == CURL_SOCKET_BAD)
        తిరిగి CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, ఎడమ_ms ? ఎడమ_ms : 500);
      ఒకవేళ(ev < 0) {
        failf(దత్తాంశం, "[WS] Error while waiting for socket becoming writable");
        తిరిగి CURLE_SEND_ERROR;
      }
    }
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ws_send_raw(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి శూన్య *బఫరు,
                            size_t buflen, size_t *pnwritten)
{
  నిర్మాణం websocket *ws;
  CURLcode ఫలితం;

  ws = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_PROTO_WS_CONN);
  ఒకవేళ(!ws) {
    failf(దత్తాంశం, "[WS] Not a websocket transfer");
    తిరిగి CURLE_SEND_ERROR;
  }
  ఒకవేళ(!buflen)
    తిరిగి CURLE_OK;

  ఒకవేళ(Curl_api_is_లోపల_callback(దత్తాంశం)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    ఫలితం = ws_flush(దత్తాంశం, ws, TRUE);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఫలితం = ws_send_raw_blocking(దత్తాంశం, ws, బఫరు, buflen);
    ఒకవేళ(!ఫలితం)
      *pnwritten = buflen;
  }
  లేకపోతే {
    /* We need any pending data to be sent or EAGAIN this call. */
    ఫలితం = ws_flush(దత్తాంశం, ws, FALSE);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఫలితం = Curl_senddata(దత్తాంశం, బఫరు, buflen, pnwritten);
  }

  CURL_TRC_WS(దత్తాంశం, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (పూర్ణాంకం)ఫలితం, *pnwritten);
  తిరిగి ఫలితం;
}

CURLcode curl_ws_send(CURL *curl, స్థిరరాశి శూన్య *బఫరు_arg,
                      size_t buflen, size_t *పంపిన,
                      curl_off_t fragsize,
                      గుర్తులేని పూర్ణాంకం flags)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, ws_send, &ఫలితం)) {
    నిర్మాణం websocket *ws;
    స్థిరరాశి uint8_t *బఫరు = బఫరు_arg;
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    size_t ndummy;
    size_t *pnsent = పంపిన ? పంపిన : &ndummy;

    CURL_TRC_WS(దత్తాంశం, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, దత్తాంశం->సమితి.ws_raw_బహుళకం);

    *pnsent = 0;

    ఒకవేళ(!బఫరు && buflen) {
      failf(దత్తాంశం, "[WS] buffer is NULL when buflen is not");
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(!దత్తాంశం->conn && దత్తాంశం->సమితి.connect_only) {
      ఫలితం = Curl_connect_only_attach(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ(!దత్తాంశం->conn) {
      failf(దత్తాంశం, "[WS] No associated connection");
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    ws = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_PROTO_WS_CONN);
    ఒకవేళ(!ws) {
      failf(దత్తాంశం, "[WS] Not a websocket transfer");
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(దత్తాంశం->సమితి.ws_raw_బహుళకం) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      ఫలితం = ws_flush(దత్తాంశం, ws, FALSE);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;

      ఒకవేళ(!బఫరు) {
        failf(దత్తాంశం, "[WS] buffer is NULL in raw mode");
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
        వెళ్ళు బహిర్గతం;
      }
      ఒకవేళ(!పంపిన) {
        failf(దత్తాంశం, "[WS] sent is NULL in raw mode");
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
        వెళ్ళు బహిర్గతం;
      }
      ఒకవేళ(fragsize || flags) {
        failf(దత్తాంశం, "[WS] fragsize and flags must be zero in raw mode");
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
        వెళ్ళు బహిర్గతం;
      }
      ఫలితం = ws_send_raw(దత్తాంశం, బఫరు, buflen, pnsent);
      వెళ్ళు బహిర్గతం;
    }

    /* Not RAW mode, we do the frame encoding */
    ఫలితం = ws_enc_send(దత్తాంశం, ws, బఫరు, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(దత్తాంశం, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, దత్తాంశం->సమితి.ws_raw_బహుళకం, (పూర్ణాంకం)ఫలితం,
                *pnsent);
  }
బహిర్గతం:
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ws_అమరిక_conn(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  దత్తాంశం->state.http_neg.accept_09 = FALSE;
  దత్తాంశం->state.http_neg.only_10 = FALSE;
  దత్తాంశం->state.http_neg.wanted = CURL_HTTP_V1x;
  దత్తాంశం->state.http_neg.allowed = CURL_HTTP_V1x;
  తిరిగి Curl_http_అమరిక_conn(దత్తాంశం, conn);
}

స్థిరరాశి నిర్మాణం curl_ws_చట్రం *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  నిర్మాణం Curl_easy *దత్తాంశం = curl;
  ఒకవేళ(GOOD_EASY_HANDLE(దత్తాంశం) && Curl_api_is_లోపల_callback(దత్తాంశం) &&
     దత్తాంశం->conn && !దత్తాంశం->సమితి.ws_raw_బహుళకం) {
    నిర్మాణం websocket *ws;
    ws = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_PROTO_WS_CONN);
    ఒకవేళ(ws)
      తిరిగి &ws->recvframe;
  }
  తిరిగి ఖాళీ;
}

CURL_EXTERN CURLcode curl_ws_ప్రారంభం_చట్రం(CURL *curl,
                                         గుర్తులేని పూర్ణాంకం flags,
                                         curl_off_t చట్రం_len)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, ws_ప్రారంభం_చట్రం, &ఫలితం)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    నిర్మాణం websocket *ws;

    ఒకవేళ(దత్తాంశం->సమితి.ws_raw_బహుళకం) {
      failf(దత్తాంశం, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      ఫలితం = CURLE_FAILED_INIT;
      వెళ్ళు బహిర్గతం;
    }

    CURL_TRC_WS(దత్తాంశం, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, చట్రం_len);

    ఒకవేళ(!దత్తాంశం->conn) {
      failf(దత్తాంశం, "[WS] No associated connection");
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    ws = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_PROTO_WS_CONN);
    ఒకవేళ(!ws) {
      failf(దత్తాంశం, "[WS] Not a websocket transfer");
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(ws->enc.payload_remain) {
      failf(దత్తాంశం, "[WS] previous frame not finished");
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    ఫలితం = ws_enc_write_తల(దత్తాంశం, ws, &ws->enc, flags, చట్రం_len,
                               &ws->sendbuf);
    ఒకవేళ(ఫలితం)
      CURL_TRC_WS(దత్తాంశం, "curl_start_frame(), error adding frame head %d",
                  (పూర్ణాంకం)ఫలితం);
  }
బహిర్గతం:
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}

స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_ws = {
  ws_అమరిక_conn,                        /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_పూర్తి,                       /* done */
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

#లేకపోతే

CURLcode curl_ws_recv(CURL *curl, శూన్య *బఫరు, size_t buflen,
                      size_t *recv,
                      స్థిరరాశి నిర్మాణం curl_ws_చట్రం **metap)
{
  (శూన్య)curl;
  (శూన్య)బఫరు;
  (శూన్య)buflen;
  (శూన్య)recv;
  (శూన్య)metap;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

CURLcode curl_ws_send(CURL *curl, స్థిరరాశి శూన్య *బఫరు,
                      size_t buflen, size_t *పంపిన,
                      curl_off_t fragsize,
                      గుర్తులేని పూర్ణాంకం flags)
{
  (శూన్య)curl;
  (శూన్య)బఫరు;
  (శూన్య)buflen;
  (శూన్య)పంపిన;
  (శూన్య)fragsize;
  (శూన్య)flags;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

స్థిరరాశి నిర్మాణం curl_ws_చట్రం *curl_ws_meta(CURL *దత్తాంశం)
{
  (శూన్య)దత్తాంశం;
  తిరిగి ఖాళీ;
}

CURL_EXTERN CURLcode curl_ws_ప్రారంభం_చట్రం(CURL *curl,
                                         గుర్తులేని పూర్ణాంకం flags,
                                         curl_off_t చట్రం_len)
{
  (శూన్య)curl;
  (శూన్య)flags;
  (శూన్య)చట్రం_len;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

#అయితే_ముగింపు /* !CURL_DISABLE_WEBSOCKETS */
