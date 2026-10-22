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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "ws.h"

#ifndef CURL_DISABLE_WEBSOCKETS

#অন্তর্ভাব "url.h"
#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "cf-recvbuf.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "easyif.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curlx/strcopy.h"

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
#define WS_CHUNK_আকার  65535
#define WS_CHUNK_গুনতি 2

/* a client-side WS frame decoder, parsing frame headers and
 * payload, keeping track of current position and stats */
গণনা ws_dec_state {
  WS_DEC_INIT,
  WS_DEC_শীর্ষ,
  WS_DEC_PAYLOAD
};

কাঠামো ws_decoder {
  পূর্ণসংখ্যা frame_age;        /* zero */
  পূর্ণসংখ্যা frame_flags;      /* See the CURLWS_* defines */
  curl_off_t payload_offset;   /* the offset parsing is at */
  curl_off_t payload_len;
  uint8_t শীর্ষ[10];
  পূর্ণসংখ্যা শীর্ষ_len, শীর্ষ_যোগ;
  গণনা ws_dec_state state;
  পূর্ণসংখ্যা cont_flags;
};

/* a client-side WS frame encoder, generating frame headers and
 * converting payloads, tracking remaining data in current frame */
কাঠামো ws_encoder {
  curl_off_t payload_len;  /* payload length of current frame */
  curl_off_t payload_remain;  /* remaining payload of current */
  unsigned পূর্ণসংখ্যা xori; /* xor index */
  uint8_t mask[4]; /* 32-bit mask for this connection */
  uint8_t firstbyte; /* first byte of frame we encode */
  BIT(contfragment); /* set TRUE if the previous fragment sent was not final */
};

/* Control frames are allowed up to 125 characters, rfc6455, ch. 5.5 */
#define WS_MAX_CNTRL_LEN    125

কাঠামো ws_cntrl_frame {
  unsigned পূর্ণসংখ্যা type;
  size_t payload_len;
  uint8_t payload[WS_MAX_CNTRL_LEN];
};

/* A websocket connection with en- and decoder that treat frames
 * and keep track of boundaries. */
কাঠামো websocket {
  কাঠামো Curl_easy *তথ্য; /* used for write callback handling */
  কাঠামো ws_decoder dec;  /* decode of ws frames */
  কাঠামো ws_encoder enc;  /* encode of ws frames */
  কাঠামো bufq recvbuf;    /* raw data from the server */
  কাঠামো bufq sendbuf;    /* raw data to be sent to the server */
  কাঠামো curl_ws_frame recvframe;  /* the current WS FRAME received */
  কাঠামো ws_cntrl_frame pending; /* a control frame pending to be sent */
  size_t sendbuf_payload; /* number of payload bytes in sendbuf */
  BIT(using_reader);      /* sending WS data is handled via a client reader */
};

#ifdef CURLVERBOSE
স্থির ধ্রুবক অক্ষর *ws_frame_name_of_op(uint8_t firstbyte)
{
  নির্বাচন(firstbyte & WSBIT_OPCODE_MASK) {
  ক্ষেত্র WSBIT_OPCODE_CONT:
    ফেরত "CONT";
  ক্ষেত্র WSBIT_OPCODE_TEXT:
    ফেরত "TEXT";
  ক্ষেত্র WSBIT_OPCODE_BIN:
    ফেরত "BIN";
  ক্ষেত্র WSBIT_OPCODE_CLOSE:
    ফেরত "CLOSE";
  ক্ষেত্র WSBIT_OPCODE_PING:
    ফেরত "PING";
  ক্ষেত্র WSBIT_OPCODE_PONG:
    ফেরত "PONG";
  পূর্বনির্ধারিত:
    ফেরত "???";
  }
}
#endif

স্থির পূর্ণসংখ্যা ws_frame_firstbyte2flags(কাঠামো Curl_easy *তথ্য,
                                    uint8_t firstbyte, পূর্ণসংখ্যা cont_flags)
{
  নির্বাচন(firstbyte) {
  /* 0x00 - intermediate TEXT/BINARY fragment */
  ক্ষেত্র WSBIT_OPCODE_CONT:
    যদি(!(cont_flags & CURLWS_CONT)) {
      failf(তথ্য, "[WS] no ongoing fragmented message to resume");
      ফেরত 0;
    }
    ফেরত cont_flags | CURLWS_CONT;
  /* 0x80 - final TEXT/BIN fragment */
  ক্ষেত্র (WSBIT_OPCODE_CONT | WSBIT_FIN):
    যদি(!(cont_flags & CURLWS_CONT)) {
      failf(তথ্য, "[WS] no ongoing fragmented message to resume");
      ফেরত 0;
    }
    ফেরত cont_flags & ~CURLWS_CONT;
  /* 0x01 - first TEXT fragment */
  ক্ষেত্র WSBIT_OPCODE_TEXT:
    যদি(cont_flags & CURLWS_CONT) {
      failf(তথ্য, "[WS] fragmented message interrupted by new TEXT msg");
      ফেরত 0;
    }
    ফেরত CURLWS_TEXT | CURLWS_CONT;
  /* 0x81 - unfragmented TEXT msg */
  ক্ষেত্র (WSBIT_OPCODE_TEXT | WSBIT_FIN):
    যদি(cont_flags & CURLWS_CONT) {
      failf(তথ্য, "[WS] fragmented message interrupted by new TEXT msg");
      ফেরত 0;
    }
    ফেরত CURLWS_TEXT;
  /* 0x02 - first BINARY fragment */
  ক্ষেত্র WSBIT_OPCODE_BIN:
    যদি(cont_flags & CURLWS_CONT) {
      failf(তথ্য, "[WS] fragmented message interrupted by new BINARY msg");
      ফেরত 0;
    }
    ফেরত CURLWS_দ্বিমিক | CURLWS_CONT;
  /* 0x82 - unfragmented BINARY msg */
  ক্ষেত্র (WSBIT_OPCODE_BIN | WSBIT_FIN):
    যদি(cont_flags & CURLWS_CONT) {
      failf(তথ্য, "[WS] fragmented message interrupted by new BINARY msg");
      ফেরত 0;
    }
    ফেরত CURLWS_দ্বিমিক;
  /* 0x08 - first CLOSE fragment */
  ক্ষেত্র WSBIT_OPCODE_CLOSE:
    failf(তথ্য, "[WS] invalid fragmented CLOSE frame");
    ফেরত 0;
  /* 0x88 - unfragmented CLOSE */
  ক্ষেত্র (WSBIT_OPCODE_CLOSE | WSBIT_FIN):
    ফেরত CURLWS_CLOSE;
  /* 0x09 - first PING fragment */
  ক্ষেত্র WSBIT_OPCODE_PING:
    failf(তথ্য, "[WS] invalid fragmented PING frame");
    ফেরত 0;
  /* 0x89 - unfragmented PING */
  ক্ষেত্র (WSBIT_OPCODE_PING | WSBIT_FIN):
    ফেরত CURLWS_PING;
  /* 0x0a - first PONG fragment */
  ক্ষেত্র WSBIT_OPCODE_PONG:
    failf(তথ্য, "[WS] invalid fragmented PONG frame");
    ফেরত 0;
  /* 0x8a - unfragmented PONG */
  ক্ষেত্র (WSBIT_OPCODE_PONG | WSBIT_FIN):
    ফেরত CURLWS_PONG;
  /* invalid first byte */
  পূর্বনির্ধারিত:
    যদি(firstbyte & WSBIT_RSV_MASK)
      /* any of the reserved bits 0x40/0x20/0x10 are set */
      failf(তথ্য, "[WS] invalid reserved bits: %02x", firstbyte);
    অন্যথা
      /* any of the reserved opcodes 0x3-0x7 or 0xb-0xf is used */
      failf(তথ্য, "[WS] invalid opcode: %02x", firstbyte);
    ফেরত 0;
  }
}

স্থির CURLcode ws_frame_flags2firstbyte(কাঠামো Curl_easy *তথ্য,
                                         unsigned পূর্ণসংখ্যা flags,
                                         bool contfragment,
                                         uint8_t *pfirstbyte)
{
  *pfirstbyte = 0;
  নির্বাচন(flags & ~CURLWS_OFFSET) {
  ক্ষেত্র 0:
    যদি(contfragment) {
      CURL_TRC_WS(তথ্য, "no flags given; interpreting as continuation "
                  "fragment for compatibility");
      *pfirstbyte = (WSBIT_OPCODE_CONT | WSBIT_FIN);
      ফেরত CURLE_OK;
    }
    failf(তথ্য, "[WS] no flags given");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ক্ষেত্র CURLWS_CONT:
    যদি(contfragment) {
      infof(তথ্য, "[WS] setting CURLWS_CONT flag without message type is "
                  "supported for compatibility but highly discouraged");
      *pfirstbyte = WSBIT_OPCODE_CONT;
      ফেরত CURLE_OK;
    }
    failf(তথ্য, "[WS] No ongoing fragmented message to continue");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ক্ষেত্র CURLWS_TEXT:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_TEXT | WSBIT_FIN);
    ফেরত CURLE_OK;
  ক্ষেত্র (CURLWS_TEXT | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_TEXT;
    ফেরত CURLE_OK;
  ক্ষেত্র CURLWS_দ্বিমিক:
    *pfirstbyte = contfragment ? (WSBIT_OPCODE_CONT | WSBIT_FIN)
                               : (WSBIT_OPCODE_BIN | WSBIT_FIN);
    ফেরত CURLE_OK;
  ক্ষেত্র (CURLWS_দ্বিমিক | CURLWS_CONT):
    *pfirstbyte = contfragment ? WSBIT_OPCODE_CONT : WSBIT_OPCODE_BIN;
    ফেরত CURLE_OK;
  ক্ষেত্র CURLWS_CLOSE:
    *pfirstbyte = WSBIT_OPCODE_CLOSE | WSBIT_FIN;
    ফেরত CURLE_OK;
  ক্ষেত্র (CURLWS_CLOSE | CURLWS_CONT):
    failf(তথ্য, "[WS] CLOSE frame must not be fragmented");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ক্ষেত্র CURLWS_PING:
    *pfirstbyte = WSBIT_OPCODE_PING | WSBIT_FIN;
    ফেরত CURLE_OK;
  ক্ষেত্র (CURLWS_PING | CURLWS_CONT):
    failf(তথ্য, "[WS] PING frame must not be fragmented");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ক্ষেত্র CURLWS_PONG:
    *pfirstbyte = WSBIT_OPCODE_PONG | WSBIT_FIN;
    ফেরত CURLE_OK;
  ক্ষেত্র (CURLWS_PONG | CURLWS_CONT):
    failf(তথ্য, "[WS] PONG frame must not be fragmented");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  পূর্বনির্ধারিত:
    failf(তথ্য, "[WS] unknown flags: %x", flags);
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

স্থির শূন্য ws_dec_info(কাঠামো ws_decoder *dec, কাঠামো Curl_easy *তথ্য,
                        ধ্রুবক অক্ষর *বার্তা)
{
  NOVERBOSE((শূন্য)বার্তা);
  নির্বাচন(dec->শীর্ষ_len) {
  ক্ষেত্র 0:
    বিরতি;
  ক্ষেত্র 1:
    CURL_TRC_WS(তথ্য, "decoded %s [%s%s]", বার্তা,
                ws_frame_name_of_op(dec->শীর্ষ[0]),
                (dec->শীর্ষ[0] & WSBIT_FIN) ? "" : " NON-FINAL");
    বিরতি;
  পূর্বনির্ধারিত:
    যদি(dec->শীর্ষ_len < dec->শীর্ষ_যোগ) {
      CURL_TRC_WS(তথ্য, "decoded %s [%s%s](%d/%d)", বার্তা,
                  ws_frame_name_of_op(dec->শীর্ষ[0]),
                  (dec->শীর্ষ[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->শীর্ষ_len, dec->শীর্ষ_যোগ);
    }
    অন্যথা {
      CURL_TRC_WS(তথ্য, "decoded %s [%s%s payload=%"
                  FMT_OFF_T "/%" FMT_OFF_T "]",
                  বার্তা, ws_frame_name_of_op(dec->শীর্ষ[0]),
                  (dec->শীর্ষ[0] & WSBIT_FIN) ? "" : " NON-FINAL",
                  dec->payload_offset, dec->payload_len);
    }
    বিরতি;
  }
}

স্থির CURLcode ws_send_raw_blocking(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো websocket *ws,
                                     ধ্রুবক অক্ষর *বাফার, size_t buflen);

প্রকারলেখ CURLcode ws_write_payload(ধ্রুবক uint8_t *buf, size_t buflen,
                                  পূর্ণসংখ্যা frame_age, পূর্ণসংখ্যা frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  শূন্য *userp,
                                  size_t *pnwritten);

স্থির শূন্য ws_dec_পরবর্তী_frame(কাঠামো ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->শীর্ষ_len = dec->শীর্ষ_যোগ = 0;
  dec->state = WS_DEC_INIT;
  /* dec->cont_flags must be carried over to next frame */
}

স্থির শূন্য ws_dec_reset(কাঠামো ws_decoder *dec)
{
  dec->frame_age = 0;
  dec->frame_flags = 0;
  dec->payload_offset = 0;
  dec->payload_len = 0;
  dec->শীর্ষ_len = dec->শীর্ষ_যোগ = 0;
  dec->state = WS_DEC_INIT;
  dec->cont_flags = 0;
}

স্থির শূন্য ws_dec_init(কাঠামো ws_decoder *dec)
{
  ws_dec_reset(dec);
}

স্থির CURLcode ws_dec_read_শীর্ষ(কাঠামো ws_decoder *dec,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো bufq *inraw)
{
  ধ্রুবক uint8_t *inbuf;
  size_t inlen;

  যখন(Curl_bufq_peek(inraw, &inbuf, &inlen)) {
    যদি(dec->শীর্ষ_len == 0) {
      dec->শীর্ষ[0] = *inbuf;
      Curl_bufq_skip(inraw, 1);

      dec->frame_flags = ws_frame_firstbyte2flags(তথ্য, dec->শীর্ষ[0],
                                                  dec->cont_flags);
      যদি(!dec->frame_flags) {
        ws_dec_reset(dec);
        ফেরত CURLE_RECV_ERROR;
      }

      /* fragmentation only applies to data frames (text/binary);
       * control frames (close/ping/pong) do not affect the CONT status */
      যদি(dec->frame_flags & (CURLWS_TEXT | CURLWS_দ্বিমিক)) {
        dec->cont_flags = dec->frame_flags;
      }

      dec->শীর্ষ_len = 1;
#যদি 0
      ws_dec_info(dec, তথ্য, "seeing opcode");
#endif
      চালু;
    }
    অন্যথা যদি(dec->শীর্ষ_len == 1) {
      dec->শীর্ষ[1] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      dec->শীর্ষ_len = 2;

      যদি(dec->শীর্ষ[1] & WSBIT_MASK) {
        /* A client MUST close a connection if it detects a masked frame. */
        failf(তথ্য, "[WS] masked input frame");
        ws_dec_reset(dec);
        ফেরত CURLE_RECV_ERROR;
      }
      যদি(dec->frame_flags & CURLWS_PING && dec->শীর্ষ[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PING frames is 125 bytes.
           Accepting overlong pings would mean sending equivalent pongs! */
        failf(তথ্য, "[WS] received PING frame is too big");
        ws_dec_reset(dec);
        ফেরত CURLE_RECV_ERROR;
      }
      যদি(dec->frame_flags & CURLWS_PONG && dec->শীর্ষ[1] > WS_MAX_CNTRL_LEN) {
        /* The maximum valid size of PONG frames is 125 bytes. */
        failf(তথ্য, "[WS] received PONG frame is too big");
        ws_dec_reset(dec);
        ফেরত CURLE_RECV_ERROR;
      }
      যদি(dec->frame_flags & CURLWS_CLOSE && dec->শীর্ষ[1] > WS_MAX_CNTRL_LEN) {
        failf(তথ্য, "[WS] received CLOSE frame is too big");
        ws_dec_reset(dec);
        ফেরত CURLE_RECV_ERROR;
      }

      /* How long is the frame head? */
      যদি(dec->শীর্ষ[1] == 126) {
        dec->শীর্ষ_যোগ = 4;
        চালু;
      }
      অন্যথা যদি(dec->শীর্ষ[1] == 127) {
        dec->শীর্ষ_যোগ = 10;
        চালু;
      }
      অন্যথা {
        dec->শীর্ষ_যোগ = 2;
      }
    }

    যদি(dec->শীর্ষ_len < dec->শীর্ষ_যোগ) {
      dec->শীর্ষ[dec->শীর্ষ_len] = *inbuf;
      Curl_bufq_skip(inraw, 1);
      ++dec->শীর্ষ_len;
      যদি(dec->শীর্ষ_len < dec->শীর্ষ_যোগ) {
#যদি 0
        ws_dec_info(dec, তথ্য, "decoding head");
#endif
        চালু;
      }
    }
    /* got the complete frame head */
    DEBUGASSERT(dec->শীর্ষ_len == dec->শীর্ষ_যোগ);
    নির্বাচন(dec->শীর্ষ_যোগ) {
    ক্ষেত্র 2:
      dec->payload_len = dec->শীর্ষ[1];
      বিরতি;
    ক্ষেত্র 4:
      dec->payload_len = (dec->শীর্ষ[2] << 8) | dec->শীর্ষ[3];
      বিরতি;
    ক্ষেত্র 10:
      যদি(dec->শীর্ষ[2] > 127) {
        failf(তথ্য, "[WS] frame length longer than 63 bits not supported");
        ফেরত CURLE_RECV_ERROR;
      }
      dec->payload_len =
        (curl_off_t)dec->শীর্ষ[2] << 56 |
        (curl_off_t)dec->শীর্ষ[3] << 48 |
        (curl_off_t)dec->শীর্ষ[4] << 40 |
        (curl_off_t)dec->শীর্ষ[5] << 32 |
        (curl_off_t)dec->শীর্ষ[6] << 24 |
        (curl_off_t)dec->শীর্ষ[7] << 16 |
        (curl_off_t)dec->শীর্ষ[8] <<  8 |
        dec->শীর্ষ[9];
      বিরতি;
    পূর্বনির্ধারিত:
      /* this should never happen */
      DEBUGASSERT(0);
      failf(তথ্য, "[WS] unexpected frame header length");
      ফেরত CURLE_RECV_ERROR;
    }

    dec->frame_age = 0;
    dec->payload_offset = 0;
    ws_dec_info(dec, তথ্য, "head");
    ফেরত CURLE_OK;
  }
  ফেরত CURLE_AGAIN;
}

স্থির CURLcode ws_dec_pass_payload(কাঠামো ws_decoder *dec,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো bufq *inraw,
                                    ws_write_payload *write_cb,
                                    শূন্য *write_ctx)
{
  ধ্রুবক uint8_t *inbuf;
  size_t inlen;
  size_t nwritten;
  CURLcode ফলাফল;
  size_t remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                     0, SIZE_MAX);

  যখন(remain && Curl_bufq_peek(inraw, &inbuf, &inlen) &&
        !Curl_cwriter_is_paused(তথ্য)) {
    যদি(inlen > remain)
      inlen = remain;
    ফলাফল = write_cb(inbuf, inlen, dec->frame_age, dec->frame_flags,
                      dec->payload_offset, dec->payload_len,
                      write_ctx, &nwritten);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    Curl_bufq_skip(inraw, nwritten);
    dec->payload_offset += nwritten;
    remain = curlx_sotouz_range(dec->payload_len - dec->payload_offset,
                                0, SIZE_MAX);
    CURL_TRC_WS(তথ্য, "passed %zu bytes payload, %zu remain",
                nwritten, remain);
  }

  ফেরত remain ? CURLE_AGAIN : CURLE_OK;
}

স্থির CURLcode ws_dec_pass(কাঠামো ws_decoder *dec,
                            কাঠামো Curl_easy *তথ্য,
                            কাঠামো bufq *inraw,
                            ws_write_payload *write_cb,
                            শূন্য *write_ctx)
{
  CURLcode ফলাফল;

  যদি(Curl_bufq_is_empty(inraw))
    ফেরত CURLE_AGAIN;

  নির্বাচন(dec->state) {
  ক্ষেত্র WS_DEC_INIT:
    ws_dec_পরবর্তী_frame(dec);
    dec->state = WS_DEC_শীর্ষ;
    FALLTHROUGH();
  ক্ষেত্র WS_DEC_শীর্ষ:
    ফলাফল = ws_dec_read_শীর্ষ(dec, তথ্য, inraw);
    যদি(ফলাফল) {
      যদি(ফলাফল != CURLE_AGAIN) {
        failf(তথ্য, "[WS] decode frame error %d", (পূর্ণসংখ্যা)ফলাফল);
        বিরতি;  /* real error */
      }
      /* incomplete ws frame head */
      DEBUGASSERT(Curl_bufq_is_empty(inraw));
      বিরতি;
    }
    /* head parsing done */
    dec->state = WS_DEC_PAYLOAD;
    যদি(dec->payload_len == 0) {
      size_t nwritten;
      ধ্রুবক uint8_t tmp = '\0';
      /* special case of a 0 length frame, need to write once */
      ফলাফল = write_cb(&tmp, 0, dec->frame_age, dec->frame_flags,
                        0, 0, write_ctx, &nwritten);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      dec->state = WS_DEC_INIT;
      বিরতি;
    }
    FALLTHROUGH();
  ক্ষেত্র WS_DEC_PAYLOAD:
    ফলাফল = ws_dec_pass_payload(dec, তথ্য, inraw, write_cb, write_ctx);
    ws_dec_info(dec, তথ্য, "passing");
    যদি(ফলাফল)
      ফেরত ফলাফল;
    /* payload parsing done */
    dec->state = WS_DEC_INIT;
    বিরতি;
  পূর্বনির্ধারিত:
    /* we covered all enums above, but some code analyzers are wimps */
    ফলাফল = CURLE_FAILED_INIT;
  }
  ফেরত ফলাফল;
}

স্থির শূন্য update_meta(কাঠামো websocket *ws,
                        পূর্ণসংখ্যা frame_age, পূর্ণসংখ্যা frame_flags,
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
কাঠামো ws_cw_ctx {
  কাঠামো Curl_cwriter super;
  কাঠামো bufq buf;
};

স্থির CURLcode ws_cw_init(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_cwriter *writer)
{
  কাঠামো ws_cw_ctx *ctx = writer->ctx;
  (শূন্য)তথ্য;
  Curl_bufq_init2(&ctx->buf, WS_CHUNK_আকার, 1, BUFQ_OPT_SOFT_LIMIT);
  ফেরত CURLE_OK;
}

স্থির শূন্য ws_cw_close(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_cwriter *writer)
{
  কাঠামো ws_cw_ctx *ctx = writer->ctx;
  (শূন্য)তথ্য;
  Curl_bufq_free(&ctx->buf);
}

কাঠামো ws_cw_dec_ctx {
  কাঠামো Curl_easy *তথ্য;
  কাঠামো websocket *ws;
  কাঠামো Curl_cwriter *পরবর্তী_writer;
  পূর্ণসংখ্যা cw_type;
};

স্থির CURLcode ws_flush(কাঠামো Curl_easy *তথ্য, কাঠামো websocket *ws,
                         bool blocking);
স্থির CURLcode ws_enc_add_pending(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো websocket *ws);

স্থির CURLcode ws_enc_add_cntrl(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো websocket *ws,
                                 ধ্রুবক uint8_t *payload,
                                 size_t plen,
                                 unsigned পূর্ণসংখ্যা frame_type)
{
  (শূন্য)তথ্য;
  DEBUGASSERT(plen <= WS_MAX_CNTRL_LEN);
  যদি(plen > WS_MAX_CNTRL_LEN)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  /* Overwrite any pending frame with the new one, we keep
   * only one. */
  ws->pending.type = frame_type;
  ws->pending.payload_len = plen;
  memcpy(ws->pending.payload, payload, plen);
  ফেরত CURLE_OK;
}

স্থির curl_off_t ws_payload_remain(curl_off_t payload_যোগ,
                                    curl_off_t payload_offset,
                                    size_t payload_buffered)
{
  curl_off_t buffered, remain = payload_যোগ - payload_offset;
  যদি((payload_যোগ < 0) || (payload_offset < 0) || (remain < 0))
    ফেরত -1;
  buffered = curlx_uztoso(payload_buffered);
  যদি(remain < buffered)
    ফেরত -1;
  ফেরত remain - buffered;
}

স্থির CURLcode ws_cw_dec_পরবর্তী(ধ্রুবক uint8_t *buf, size_t buflen,
                               পূর্ণসংখ্যা frame_age, পূর্ণসংখ্যা frame_flags,
                               curl_off_t payload_offset,
                               curl_off_t payload_len,
                               শূন্য *user_তথ্য,
                               size_t *pnwritten)
{
  কাঠামো ws_cw_dec_ctx *ctx = user_তথ্য;
  কাঠামো Curl_easy *তথ্য = ctx->তথ্য;
  কাঠামো websocket *ws = ctx->ws;
  bool auto_pong = !তথ্য->set.ws_raw_mode && !তথ্য->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode ফলাফল;

  (শূন্য)frame_age;
  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  যদি(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  যদি(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(তথ্য, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    ফলাফল = ws_enc_add_cntrl(তথ্য, ws, buf, buflen, CURLWS_PONG);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  অন্যথা যদি(buflen || !remain) {
    /* forward the decoded frame to the next client writer. */
    update_meta(ws, frame_age, frame_flags, payload_offset,
                payload_len, buflen);

    CURL_TRC_WRITE(তথ্য, "[WS] pass %zu decoded bytes", buflen);
    ফলাফল = Curl_cwriter_write(তথ্য, ctx->পরবর্তী_writer,
                                (ctx->cw_type | CLIENTWRITE_0LEN),
                                (ধ্রুবক অক্ষর *)buf, buflen);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  *pnwritten = buflen;
  ফেরত CURLE_OK;
}

স্থির CURLcode ws_cw_write(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                            ধ্রুবক অক্ষর *buf, size_t nbytes)
{
  কাঠামো ws_cw_ctx *ctx = writer->ctx;
  কাঠামো websocket *ws;
  CURLcode ফলাফল = CURLE_OK;

  CURL_TRC_WRITE(তথ্য, "[WS] write(len=%zu, type=%d)", nbytes, type);
  যদি(!(type & CLIENTWRITE_BODY) || তথ্য->set.ws_raw_mode)
    ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, nbytes);

  ws = Curl_conn_meta_get(তথ্য->conn, CURL_META_PROTO_WS_CONN);
  যদি(!ws) {
    failf(তথ্য, "[WS] not a websocket transfer");
    ফেরত CURLE_FAILED_INIT;
  }

  যদি(nbytes) {
    size_t nwritten;
    ফলাফল = Curl_bufq_write(&ctx->buf, (ধ্রুবক uint8_t *)buf,
                             nbytes, &nwritten);
    যদি(ফলাফল) {
      infof(তথ্য, "[WS] error adding data to buffer %d", (পূর্ণসংখ্যা)ফলাফল);
      ফেরত ফলাফল;
    }
  }

  ফলাফল = Curl_cwriter_flush(তথ্য, writer->পরবর্তী);
  যদি(ফলাফল)
    যাও out;

  যখন(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(তথ্য)) {
    কাঠামো ws_cw_dec_ctx pass_ctx;
    pass_ctx.তথ্য = তথ্য;
    pass_ctx.ws = ws;
    pass_ctx.পরবর্তী_writer = writer->পরবর্তী;
    pass_ctx.cw_type = type;
    ফলাফল = ws_dec_pass(&ws->dec, তথ্য, &ctx->buf,
                         ws_cw_dec_পরবর্তী, &pass_ctx);
    যদি(ফলাফল == CURLE_AGAIN) {
      /* insufficient amount of data, keep it for later.
       * we pretend to have written all since we have a copy */
      ফলাফল = CURLE_OK;
      যাও out;
    }
    অন্যথা যদি(ফলাফল) {
      failf(তথ্য, "[WS] decode payload error %d", (পূর্ণসংখ্যা)ফলাফল);
      Curl_bufq_reset(&ctx->buf);
      যাও out;
    }
  }

out:
  যদি(!ফলাফল && !ws->using_reader) {
    ফলাফল = ws_flush(তথ্য, ws, Curl_api_is_in_callback(তথ্য));
    যদি(ফলাফল == CURLE_AGAIN)
      ফলাফল = CURLE_OK;
  }

  যদি(!ফলাফল && (type & CLIENTWRITE_EOS) &&
     (!Curl_bufq_is_empty(&ctx->buf) || (ws->dec.state != WS_DEC_INIT))) {
    failf(তথ্য, "[WS] decode ended with incomplete frame");
    ফলাফল = CURLE_RECV_ERROR;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ws_cw_flush(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_cwriter *writer)
{
  CURLcode ফলাফল = CURLE_OK;

  CURL_TRC_WRITE(তথ্য, "[ws] flush");
  যদি(!তথ্য->set.ws_raw_mode) {
    কাঠামো ws_cw_ctx *ctx = writer->ctx;
    কাঠামো websocket *ws;

    /* Frames should be written one by one, else the meta data does
     * not fit. Flush the next writer first, so it does not aggregate
     * our flushed data with anything it might have buffered. */
    ফলাফল = Curl_cwriter_flush(তথ্য, writer->পরবর্তী);
    যদি(ফলাফল)
      যাও out;

    ws = Curl_conn_meta_get(তথ্য->conn, CURL_META_PROTO_WS_CONN);
    যদি(!ws) {
      failf(তথ্য, "[WS] not a websocket transfer");
      ফেরত CURLE_FAILED_INIT;
    }

    যখন(!Curl_bufq_is_empty(&ctx->buf) && !Curl_cwriter_is_paused(তথ্য)) {
      কাঠামো ws_cw_dec_ctx pass_ctx;
      pass_ctx.তথ্য = তথ্য;
      pass_ctx.ws = ws;
      pass_ctx.পরবর্তী_writer = writer->পরবর্তী;
      pass_ctx.cw_type = CLIENTWRITE_BODY;
      ফলাফল = ws_dec_pass(&ws->dec, তথ্য, &ctx->buf,
                           ws_cw_dec_পরবর্তী, &pass_ctx);
      যদি(ফলাফল == CURLE_AGAIN) {
        /* insufficient amount of data, keep it for later.
         * we pretend to have written all since we have a copy */
        ফলাফল = CURLE_OK;
        যাও out;
      }
      অন্যথা যদি(ফলাফল) {
        failf(তথ্য, "[WS] decode payload error %d", (পূর্ণসংখ্যা)ফলাফল);
        Curl_bufq_reset(&ctx->buf);
        যাও out;
      }
    }
  }

out:
  যদি(!ফলাফল)
    ফলাফল = Curl_cwriter_flush(তথ্য, writer->পরবর্তী);
  ফেরত ফলাফল;
}

/* WebSocket payload decoding client writer. */
স্থির ধ্রুবক কাঠামো Curl_cwtype ws_cw_decode = {
  "ws-decode",
  ফাঁকা,
  0,
  ws_cw_init,
  ws_cw_write,
  ws_cw_flush,
  ws_cw_close,
  আকার(কাঠামো ws_cw_ctx)
};

স্থির শূন্য ws_enc_info(কাঠামো ws_encoder *enc, কাঠামো Curl_easy *তথ্য,
                        ধ্রুবক অক্ষর *বার্তা)
{
  NOVERBOSE((শূন্য)enc);
  NOVERBOSE((শূন্য)বার্তা);
  CURL_TRC_WS(তথ্য, "WS-ENC: %s [%s%s payload=%"
              FMT_OFF_T "/%" FMT_OFF_T "]",
              বার্তা, ws_frame_name_of_op(enc->firstbyte),
              (enc->firstbyte & WSBIT_FIN) ? "" : " NON-FIN",
              enc->payload_len - enc->payload_remain, enc->payload_len);
}

স্থির শূন্য ws_enc_reset(কাঠামো ws_encoder *enc)
{
  enc->payload_remain = 0;
  enc->xori = 0;
  enc->contfragment = FALSE;
}

স্থির শূন্য ws_enc_init(কাঠামো ws_encoder *enc)
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

স্থির CURLcode ws_enc_add_frame(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ws_encoder *enc,
                                 unsigned পূর্ণসংখ্যা flags,
                                 curl_off_t payload_len,
                                 কাঠামো bufq *out)
{
  uint8_t firstb = 0;
  uint8_t শীর্ষ[14];
  CURLcode ফলাফল;
  size_t hlen, nwritten;

  যদি(payload_len < 0) {
    failf(তথ্য, "[WS] starting new frame with negative payload length %"
                FMT_OFF_T, payload_len);
    ফেরত CURLE_SEND_ERROR;
  }

  যদি(enc->payload_remain > 0) {
    /* trying to write a new frame before the previous one is finished */
    failf(তথ্য, "[WS] starting new frame with %" FMT_OFF_T " bytes "
                "from last one remaining to be sent", enc->payload_remain);
    ফেরত CURLE_SEND_ERROR;
  }

  ফলাফল = ws_frame_flags2firstbyte(তথ্য, flags, (bool)enc->contfragment,
                                    &firstb);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* fragmentation only applies to data frames (text/binary);
   * control frames (close/ping/pong) do not affect the CONT status */
  যদি(flags & (CURLWS_TEXT | CURLWS_দ্বিমিক)) {
    enc->contfragment = (curl_bit)((flags & CURLWS_CONT) ? TRUE : FALSE);
  }

  যদি(flags & CURLWS_PING && payload_len > WS_MAX_CNTRL_LEN) {
    failf(তথ্য, "[WS] given PING frame is too big");
    ফেরত CURLE_TOO_LARGE;
  }
  যদি(flags & CURLWS_PONG && payload_len > WS_MAX_CNTRL_LEN) {
    failf(তথ্য, "[WS] given PONG frame is too big");
    ফেরত CURLE_TOO_LARGE;
  }
  যদি(flags & CURLWS_CLOSE && payload_len > WS_MAX_CNTRL_LEN) {
    failf(তথ্য, "[WS] given CLOSE frame is too big");
    ফেরত CURLE_TOO_LARGE;
  }

  শীর্ষ[0] = enc->firstbyte = firstb;
  যদি(payload_len > 65535) {
    শীর্ষ[1] = 127 | WSBIT_MASK;
    শীর্ষ[2] = (uint8_t)((payload_len >> 56) & 0xff);
    শীর্ষ[3] = (uint8_t)((payload_len >> 48) & 0xff);
    শীর্ষ[4] = (uint8_t)((payload_len >> 40) & 0xff);
    শীর্ষ[5] = (uint8_t)((payload_len >> 32) & 0xff);
    শীর্ষ[6] = (uint8_t)((payload_len >> 24) & 0xff);
    শীর্ষ[7] = (uint8_t)((payload_len >> 16) & 0xff);
    শীর্ষ[8] = (uint8_t)((payload_len >> 8) & 0xff);
    শীর্ষ[9] = (uint8_t)(payload_len & 0xff);
    hlen = 10;
  }
  অন্যথা যদি(payload_len >= 126) {
    শীর্ষ[1] = 126 | WSBIT_MASK;
    শীর্ষ[2] = (uint8_t)((payload_len >> 8) & 0xff);
    শীর্ষ[3] = (uint8_t)(payload_len & 0xff);
    hlen = 4;
  }
  অন্যথা {
    শীর্ষ[1] = (uint8_t)payload_len | WSBIT_MASK;
    hlen = 2;
  }

  enc->payload_remain = enc->payload_len = payload_len;
  ws_enc_info(enc, তথ্য, "sending");

  /* 4 bytes random */

  ফলাফল = Curl_rand(তথ্য, (uint8_t *)&enc->mask, আকার(enc->mask));
  যদি(ফলাফল)
    ফেরত ফলাফল;

#ifdef DEBUGBUILD
  যদি(getenv("CURL_WS_FORCE_ZERO_MASK"))
    /* force the bit mask to 0x00000000, effectively disabling masking */
    memset(&enc->mask, 0, আকার(enc->mask));
#endif

  /* add 4 bytes mask */
  memcpy(&শীর্ষ[hlen], &enc->mask, 4);
  hlen += 4;
  /* reset for payload to come */
  enc->xori = 0;

  ফলাফল = Curl_bufq_write(out, শীর্ষ, hlen, &nwritten);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  যদি(nwritten != hlen) {
    /* We use a bufq with SOFT_LIMIT, writing should always succeed */
    DEBUGASSERT(0);
    ফেরত CURLE_SEND_ERROR;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ws_enc_write_শীর্ষ(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো websocket *ws,
                                  কাঠামো ws_encoder *enc,
                                  unsigned পূর্ণসংখ্যা flags,
                                  curl_off_t payload_len,
                                  কাঠামো bufq *out)
{
  /* starting a new frame, we want a clean sendbuf.
   * Any pending control frame we can add now as part of the flush. */
  যদি(ws->pending.type) {
    CURLcode ফলাফল = ws_enc_add_pending(তথ্য, ws);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  ফেরত ws_enc_add_frame(তথ্য, enc, flags, payload_len, out);
}

স্থির CURLcode ws_enc_write_payload(কাঠামো ws_encoder *enc,
                                     কাঠামো Curl_easy *তথ্য,
                                     ধ্রুবক uint8_t *buf, size_t buflen,
                                     কাঠামো bufq *out, size_t *pnwritten)
{
  CURLcode ফলাফল;
  size_t i, len, n, remain;

  *pnwritten = 0;
  যদি(Curl_bufq_is_full(out))
    ফেরত CURLE_AGAIN;

  /* not the most performant way to do this */
  len = buflen;
  remain = curlx_sotouz_range(enc->payload_remain, 0, SIZE_MAX);
  যদি(remain < len)
    len = remain;

  জন্য(i = 0; i < len; ++i) {
    uint8_t c = buf[i] ^ enc->mask[enc->xori];
    ফলাফল = Curl_bufq_write(out, &c, 1, &n);
    যদি(ফলাফল) {
      যদি((ফলাফল != CURLE_AGAIN) || !i)
        ফেরত ফলাফল;
      বিরতি;
    }
    enc->xori++;
    enc->xori &= 3;
  }
  *pnwritten = i;
  enc->payload_remain -= (curl_off_t)i;
  ws_enc_info(enc, তথ্য, "buffered");
  ফেরত CURLE_OK;
}

স্থির CURLcode ws_enc_add_pending(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো websocket *ws)
{
  CURLcode ফলাফল;
  size_t n;

  যদি(!ws->pending.type) /* no pending frame here */
    ফেরত CURLE_OK;
  যদি(ws->enc.payload_remain) /* in the middle of another frame */
    ফেরত CURLE_AGAIN;

  ফলাফল = ws_enc_add_frame(তথ্য, &ws->enc, ws->pending.type,
                            (curl_off_t)ws->pending.payload_len,
                            &ws->sendbuf);
  যদি(ফলাফল) {
    CURL_TRC_WS(তথ্য, "ws_enc_cntrl(), error adding head: %d",
                (পূর্ণসংখ্যা)ফলাফল);
    যাও out;
  }
  ফলাফল = ws_enc_write_payload(&ws->enc, তথ্য, ws->pending.payload,
                                ws->pending.payload_len,
                                &ws->sendbuf, &n);
  যদি(ফলাফল) {
    CURL_TRC_WS(তথ্য, "ws_enc_cntrl(), error adding payload: %d",
                (পূর্ণসংখ্যা)ফলাফল);
    যাও out;
  }
  যদি(n != ws->pending.payload_len) {
    DEBUGASSERT(0); /* buffer should always be able to take all */
    CURL_TRC_WS(তথ্য, "ws_enc_cntrl(), error added only %zu/%zu payload,",
                n, ws->pending.payload_len);
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }
  /* the frame should be complete now */
  DEBUGASSERT(!ws->enc.payload_remain);
  memset(&ws->pending, 0, আকার(ws->pending));

out:
  ফেরত ফলাফল;
}

স্থির CURLcode ws_enc_send(কাঠামো Curl_easy *তথ্য,
                            কাঠামো websocket *ws,
                            ধ্রুবক uint8_t *বাফার,
                            size_t buflen,
                            curl_off_t fragsize,
                            unsigned পূর্ণসংখ্যা flags,
                            size_t *pnsent)
{
  size_t n;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(!তথ্য->set.ws_raw_mode);
  *pnsent = 0;

  যদি(ws->enc.payload_remain || !Curl_bufq_is_empty(&ws->sendbuf)) {
    /* a frame is ongoing with payload buffered or more payload
     * that needs to be encoded into the buffer */
    যদি(buflen < ws->sendbuf_payload) {
      /* We have been called with LESS buffer data than before. This
       * is not how it is supposed too work. */
      failf(তথ্য, "[WS] curl_ws_send() called with smaller 'buflen' than "
            "bytes already buffered in previous call, %zu vs %zu",
            buflen, ws->sendbuf_payload);
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }
    যদি((curl_off_t)buflen >
       (ws->enc.payload_remain + (curl_off_t)ws->sendbuf_payload)) {
      /* too large buflen beyond payload length of frame */
      failf(তথ্য, "[WS] unaligned frame size (sending %zu instead of "
            "%" FMT_OFF_T ")", buflen,
            (curl_off_t)(ws->enc.payload_remain + ws->sendbuf_payload));
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  অন্যথা {
    ফলাফল = ws_flush(তথ্য, ws, Curl_api_is_in_callback(তথ্য));
    যদি(ফলাফল)
      ফেরত ফলাফল;

    ফলাফল = ws_enc_write_শীর্ষ(তথ্য, ws, &ws->enc, flags,
                               (flags & CURLWS_OFFSET) ?
                               fragsize : (curl_off_t)buflen,
                               &ws->sendbuf);
    যদি(ফলাফল) {
      CURL_TRC_WS(তথ্য, "curl_ws_send(), error writing frame head %d",
                  (পূর্ণসংখ্যা)ফলাফল);
      ফেরত ফলাফল;
    }
  }

  /* While there is either sendbuf to flush OR more payload to encode... */
  যখন(!Curl_bufq_is_empty(&ws->sendbuf) || (buflen > ws->sendbuf_payload)) {
    /* Try to add more payload to sendbuf */
    যদি(buflen > ws->sendbuf_payload) {
      size_t পূর্ব_len = Curl_bufq_len(&ws->sendbuf);
      ফলাফল = ws_enc_write_payload(&ws->enc, তথ্য,
                                    বাফার + ws->sendbuf_payload,
                                    buflen - ws->sendbuf_payload,
                                    &ws->sendbuf, &n);
      যদি(ফলাফল && (ফলাফল != CURLE_AGAIN))
        ফেরত ফলাফল;
      ws->sendbuf_payload += Curl_bufq_len(&ws->sendbuf) - পূর্ব_len;
      যদি(!ws->sendbuf_payload) {
        ফেরত CURLE_AGAIN;
      }
    }

    /* flush, blocking when in callback */
    ফলাফল = ws_flush(তথ্য, ws, Curl_api_is_in_callback(তথ্য));
    যদি(!ফলাফল && ws->sendbuf_payload > 0) {
      *pnsent += ws->sendbuf_payload;
      বাফার += ws->sendbuf_payload;
      buflen -= ws->sendbuf_payload;
      ws->sendbuf_payload = 0;
    }
    অন্যথা যদি(ফলাফল == CURLE_AGAIN) {
      যদি(ws->sendbuf_payload > Curl_bufq_len(&ws->sendbuf)) {
        /* blocked, part of payload bytes remain, report length
         * that we managed to send. */
        size_t flushed = (ws->sendbuf_payload - Curl_bufq_len(&ws->sendbuf));
        *pnsent += flushed;
        ws->sendbuf_payload -= flushed;
        ফেরত CURLE_OK;
      }
      অন্যথা {
        /* blocked before sending headers or 1st payload byte. We cannot report
         * OK on 0-length send (caller counts only payload) and EAGAIN */
        CURL_TRC_WS(তথ্য, "EAGAIN flushing sendbuf, payload_encoded: %zu/%zu",
                    ws->sendbuf_payload, buflen);
        DEBUGASSERT(*pnsent == 0);
        ফেরত CURLE_AGAIN;
      }
    }
    অন্যথা
      ফেরত ফলাফল;  /* real error sending the data */
  }
  ফেরত CURLE_OK;
}

কাঠামো cr_ws_ctx {
  কাঠামো Curl_creader super;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

স্থির CURLcode cr_ws_init(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_creader *reader)
{
  (শূন্য)তথ্য;
  (শূন্য)reader;
  ফেরত CURLE_OK;
}

স্থির শূন্য cr_ws_close(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_creader *reader)
{
  (শূন্য)তথ্য;
  (শূন্য)reader;
}

স্থির CURLcode cr_ws_read(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_creader *reader,
                           অক্ষর *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  কাঠামো cr_ws_ctx *ctx = reader->ctx;
  CURLcode ফলাফল = CURLE_OK;
  size_t nread, n;
  কাঠামো websocket *ws;
  bool eos;

  *pnread = 0;
  যদি(ctx->eos) {
    *peos = TRUE;
    ফেরত CURLE_OK;
  }

  ws = Curl_conn_meta_get(তথ্য->conn, CURL_META_PROTO_WS_CONN);
  যদি(!ws) {
    failf(তথ্য, "[WS] not a websocket transfer");
    ফেরত CURLE_FAILED_INIT;
  }

  যদি(Curl_bufq_is_empty(&ws->sendbuf)) {
    যদি(ctx->read_eos) {
      ctx->eos = TRUE;
      *peos = TRUE;
      ফেরত CURLE_OK;
    }

    যদি(ws->enc.payload_remain) {
      CURL_TRC_WS(তথ্য, "current frame, %" FMT_OFF_T " remaining",
                  ws->enc.payload_remain);
      blen = curlx_sotouz_range(ws->enc.payload_remain, 0, blen);
    }

    ফলাফল = Curl_creader_read(তথ্য, reader->পরবর্তী, buf, blen, &nread, &eos);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    ctx->read_eos = eos;

    যদি(!Curl_bufq_is_empty(&ws->sendbuf)) {
      /* client_read started a new frame, we disregard any eos reported */
      ctx->read_eos = FALSE;
      Curl_creader_clear_eos(তথ্য, reader->পরবর্তী);
    }
    অন্যথা যদি(!nread) {
      /* nothing to convert, return this right away */
      যদি(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      যাও out;
    }

    যদি(!ws->enc.payload_remain && Curl_bufq_is_empty(&ws->sendbuf)) {
      /* encode the data as a new BINARY frame */
      ফলাফল = ws_enc_write_শীর্ষ(তথ্য, ws, &ws->enc, CURLWS_দ্বিমিক, nread,
                                 &ws->sendbuf);
      যদি(ফলাফল)
        যাও out;
    }

    ফলাফল = ws_enc_write_payload(&ws->enc, তথ্য, (uint8_t *)buf,
                                  nread, &ws->sendbuf, &n);
    যদি(ফলাফল)
      যাও out;
    CURL_TRC_READ(তথ্য, "cr_ws_read, added %zu payload, len=%zu", nread, n);
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ws->sendbuf));
  *peos = FALSE;
  ফলাফল = Curl_bufq_cread(&ws->sendbuf, buf, blen, pnread);
  যদি(!ফলাফল && ctx->read_eos && Curl_bufq_is_empty(&ws->sendbuf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(তথ্য, "cr_ws_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (পূর্ণসংখ্যা)ফলাফল, *pnread, *peos);
  ফেরত ফলাফল;
}

স্থির curl_off_t cr_ws_যোগ_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (শূন্য)তথ্য;
  (শূন্য)reader;
  ফেরত -1;
}

স্থির ধ্রুবক কাঠামো Curl_crtype ws_cr_encode = {
  "ws-encode",
  cr_ws_init,
  cr_ws_read,
  cr_ws_close,
  Curl_creader_def_needs_rewind,
  cr_ws_যোগ_দৈর্ঘ্য,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  আকার(কাঠামো cr_ws_ctx)
};

কাঠামো wsfield {
  ধ্রুবক অক্ষর *name;
  ধ্রুবক অক্ষর *val;
};

CURLcode Curl_ws_request(কাঠামো Curl_easy *তথ্য, কাঠামো dynbuf *req)
{
  unsigned পূর্ণসংখ্যা i;
  CURLcode ফলাফল = CURLE_OK;
  uint8_t rand[16];
  অক্ষর *randstr;
  size_t randlen;
  অক্ষর keyval[40];
  কাঠামো SingleRequest *k = &তথ্য->req;
  কাঠামো wsfield heads[] = {
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
      "Sec-WebSocket-Key", ফাঁকা,
    }
  };
  heads[2].val = &keyval[0];

  /* 16 bytes random */
  ফলাফল = Curl_rand(তথ্য, rand, আকার(rand));
  যদি(ফলাফল)
    ফেরত ফলাফল;
  ফলাফল = curlx_base64_encode(rand, আকার(rand), &randstr, &randlen);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  DEBUGASSERT(randlen < আকার(keyval));
  যদি(randlen >= আকার(keyval)) {
    curlx_free(randstr);
    ফেরত CURLE_FAILED_INIT;
  }
  curlx_strcopy(keyval, আকার(keyval), randstr, randlen);
  curlx_free(randstr);
  জন্য(i = 0; !ফলাফল && (i < CURL_ARRAYSIZE(heads)); i++) {
    যদি(!Curl_checkheaders(তথ্য, heads[i].name, strlen(heads[i].name))) {
      ফলাফল = curlx_dyn_addf(req, "%s: %s\r\n", heads[i].name, heads[i].val);
    }
  }
  তথ্য->state.http_hd_upgrade = TRUE;
  k->upgr101 = UPGR101_WS;
  তথ্য->conn->bits.upgrade_in_progress = TRUE;
  ফেরত ফলাফল;
}

স্থির শূন্য ws_conn_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো websocket *ws = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  Curl_bufq_free(&ws->recvbuf);
  Curl_bufq_free(&ws->sendbuf);
  curlx_free(ws);
}

/*
 * 'nread' is number of bytes of websocket data already in the buffer at
 * 'mem'.
 */
CURLcode Curl_ws_accept(কাঠামো Curl_easy *তথ্য,
                        ধ্রুবক অক্ষর *mem, size_t nread)
{
  কাঠামো SingleRequest *k = &তথ্য->req;
  কাঠামো websocket *ws;
  কাঠামো Curl_cwriter *ws_dec_writer = ফাঁকা;
  কাঠামো Curl_creader *ws_enc_reader = ফাঁকা;
  CURLcode ফলাফল;

  DEBUGASSERT(তথ্য->conn);
  ws = Curl_conn_meta_get(তথ্য->conn, CURL_META_PROTO_WS_CONN);
  যদি(!ws) {
    size_t chunk_আকার = WS_CHUNK_আকার;
    ws = curlx_calloc(1, আকার(*ws));
    যদি(!ws)
      ফেরত CURLE_OUT_OF_MEMORY;
#ifdef DEBUGBUILD
    {
      ধ্রুবক অক্ষর *p = getenv("CURL_WS_CHUNK_SIZE");
      যদি(p) {
        curl_off_t l;
        যদি(!curlx_str_সংখ্যা(&p, &l, 1 * 1024 * 1024))
          chunk_আকার = (size_t)l;
      }
    }
#endif
    CURL_TRC_WS(তথ্য, "WS, using chunk size %zu", chunk_আকার);
    Curl_bufq_init2(&ws->recvbuf, chunk_আকার, WS_CHUNK_গুনতি,
                    BUFQ_OPT_SOFT_LIMIT);
    Curl_bufq_init2(&ws->sendbuf, chunk_আকার, WS_CHUNK_গুনতি,
                    BUFQ_OPT_SOFT_LIMIT);
    ws_dec_init(&ws->dec);
    ws_enc_init(&ws->enc);
    ফলাফল = Curl_conn_meta_set(তথ্য->conn, CURL_META_PROTO_WS_CONN,
                                ws, ws_conn_dtor);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  অন্যথা {
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

  infof(তথ্য, "[WS] Received 101, switch to WebSocket");

  /* Install our client writer that decodes WS frames payload */
  ফলাফল = Curl_cwriter_create(&ws_dec_writer, তথ্য, &ws_cw_decode,
                               CURL_CW_CONTENT_DECODE);
  যদি(ফলাফল)
    যাও out;
  ফলাফল = Curl_cwriter_add(তথ্য, ws_dec_writer);
  যদি(ফলাফল)
    যাও out;
  ws_dec_writer = ফাঁকা; /* owned by transfer now */

  k->header = FALSE; /* we will not get more response headers */

  যদি(তথ্য->set.connect_only) {
    /* In CONNECT_ONLY setup, the payloads from `mem` need to be received
     * when using `curl_ws_recv/curl_easy_recv` later on, after this transfer
     * is already marked as DONE.
     * Since `curl_easy_recv()` is also supposed to work, we need
     * to buffer the data at connection level. See #22107 */
    যদি(nread) {
      ফলাফল = Curl_cf_recvbuf_add(তথ্য, তথ্য->conn, FIRSTSOCKET,
                                   (ধ্রুবক uint8_t *)mem, nread);
      যদি(ফলাফল)
        যাও out;
    }
    CURL_REQ_CLEAR_RECV(তথ্য); /* read no more content */
  }
  অন্যথা { /* !connect_only */
    যদি(তথ্য->set.method == HTTPREQ_PUT) {
      CURL_TRC_WS(তথ্য, "UPLOAD set, add ws-encode reader");
      ফলাফল = Curl_creader_set_fread(তথ্য, -1);
      যদি(ফলাফল)
        যাও out;

      যদি(!তথ্য->set.ws_raw_mode) {
        /* Add our client reader encoding WS BINARY frames */
        ফলাফল = Curl_creader_create(&ws_enc_reader, তথ্য, &ws_cr_encode,
                                     CURL_CR_CONTENT_ENCODE);
        যদি(ফলাফল)
          যাও out;
        ফলাফল = Curl_creader_add(তথ্য, ws_enc_reader);
        যদি(ফলাফল)
          যাও out;
        ws_enc_reader = ফাঁকা; /* owned by transfer now */
        ws->using_reader = TRUE;
      }

      /* start over with sending */
      তথ্য->req.eos_read = FALSE;
      তথ্য->req.upload_done = FALSE;
      CURL_REQ_SET_SEND(তথ্য);
    }

    /* Then pass any additional data to the writers */
    যদি(nread) {
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, mem, nread);
      যদি(ফলাফল)
        যাও out;
    }
  }

  k->upgr101 = UPGR101_RECEIVED;
  k->header = FALSE; /* we will not get more responses */

out:
  যদি(ws_dec_writer)
    Curl_cwriter_free(তথ্য, ws_dec_writer);
  যদি(ws_enc_reader)
    Curl_creader_free(তথ্য, ws_enc_reader);
  যদি(ফলাফল)
    CURL_TRC_WS(তথ্য, "Curl_ws_accept() failed -> %d", (পূর্ণসংখ্যা)ফলাফল);
  অন্যথা
    CURL_TRC_WS(তথ্য, "websocket established, %s mode",
                তথ্য->set.connect_only ? "connect-only" : "callback");
  ফেরত ফলাফল;
}

কাঠামো ws_collect {
  কাঠামো Curl_easy *তথ্য;
  কাঠামো websocket *ws;
  uint8_t *বাফার;
  size_t buflen;
  size_t bufidx;
  পূর্ণসংখ্যা frame_age;
  পূর্ণসংখ্যা frame_flags;
  curl_off_t payload_offset;
  curl_off_t payload_len;
  bool written;
};

স্থির CURLcode ws_client_collect(ধ্রুবক uint8_t *buf, size_t buflen,
                                  পূর্ণসংখ্যা frame_age, পূর্ণসংখ্যা frame_flags,
                                  curl_off_t payload_offset,
                                  curl_off_t payload_len,
                                  শূন্য *userp,
                                  size_t *pnwritten)
{
  কাঠামো ws_collect *ctx = userp;
  কাঠামো Curl_easy *তথ্য = ctx->তথ্য;
  bool auto_pong = !তথ্য->set.ws_raw_mode && !তথ্য->set.ws_no_auto_pong;
  curl_off_t remain;
  CURLcode ফলাফল = CURLE_OK;

  *pnwritten = 0;
  remain = ws_payload_remain(payload_len, payload_offset, buflen);
  যদি(remain < 0) {
    DEBUGASSERT(0); /* parameter mismatch */
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  যদি(!ctx->bufidx) {
    /* first write */
    ctx->frame_age = frame_age;
    ctx->frame_flags = frame_flags;
    ctx->payload_offset = payload_offset;
    ctx->payload_len = payload_len;
  }

  যদি(auto_pong && (frame_flags & CURLWS_PING) && !remain) {
    /* auto-respond to PINGs, only works for single-frame payloads atm */
    CURL_TRC_WS(তথ্য, "auto PONG to [PING payload=%" FMT_OFF_T
                "/%" FMT_OFF_T "]", payload_offset, payload_len);
    /* send back the exact same content as a PONG */
    ফলাফল = ws_enc_add_cntrl(ctx->তথ্য, ctx->ws, buf, buflen, CURLWS_PONG);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    *pnwritten = buflen;
  }
  অন্যথা {
    size_t write_len;

    ctx->written = TRUE;
    DEBUGASSERT(ctx->buflen >= ctx->bufidx);
    write_len = CURLMIN(buflen, ctx->buflen - ctx->bufidx);
    যদি(!write_len) {
      যদি(!buflen)  /* 0 length write, we accept that */
        ফেরত CURLE_OK;
      ফেরত CURLE_AGAIN;  /* no more space */
    }
    memcpy(ctx->বাফার + ctx->bufidx, buf, write_len);
    ctx->bufidx += write_len;
    *pnwritten = write_len;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode nw_in_recv(শূন্য *reader_ctx,
                           uint8_t *buf, size_t buflen,
                           size_t *pnread)
{
  কাঠামো Curl_easy *তথ্য = reader_ctx;
  ফেরত Curl_easy_recv(তথ্য, buf, buflen, pnread);
}

CURLcode curl_ws_recv(CURL *curl, শূন্য *বাফার,
                      size_t buflen, size_t *recv,
                      ধ্রুবক কাঠামো curl_ws_frame **metap)
{
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল = CURLE_OK;

  যদি(CURL_EAPI_ENTER(&guard, curl, ws_recv, &ফলাফল)) {
    কাঠামো Curl_easy *তথ্য = curl;
    কাঠামো connectdata *conn;
    কাঠামো websocket *ws;
    কাঠামো ws_collect ctx;

    যদি(!recv || !metap || (buflen && !বাফার)) {
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }

    *recv = 0;
    *metap = ফাঁকা;

    conn = তথ্য->conn;
    যদি(!conn) {
      /* Unhappy hack with lifetimes of transfers and connection */
      যদি(!তথ্য->set.connect_only) {
        failf(তথ্য, "[WS] CONNECT_ONLY is required");
        ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;
        যাও out;
      }

      Curl_getconnectinfo(তথ্য, &conn);
      যদি(!conn) {
        failf(তথ্য, "[WS] connection not found");
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
        যাও out;
      }
    }
    ws = Curl_conn_meta_get(conn, CURL_META_PROTO_WS_CONN);
    যদি(!ws) {
      failf(তথ্য, "[WS] connection is not setup for websocket");
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }

    memset(&ctx, 0, আকার(ctx));
    ctx.তথ্য = তথ্য;
    ctx.ws = ws;
    ctx.বাফার = বাফার;
    ctx.buflen = buflen;

    যখন(1) {
      /* receive more when our buffer is empty */
      যদি(Curl_bufq_is_empty(&ws->recvbuf)) {
        size_t n;
        ফলাফল = Curl_bufq_slurp(&ws->recvbuf, nw_in_recv, তথ্য, &n);
        যদি(ফলাফল)
          যাও out;
        অন্যথা যদি(n == 0) {
          /* connection closed */
          infof(তথ্য, "[WS] connection expectedly closed?");
          ফলাফল = CURLE_GOT_NOTHING;
          যাও out;
        }
        CURL_TRC_WS(তথ্য, "curl_ws_recv, added %zu bytes from network",
                    Curl_bufq_len(&ws->recvbuf));
      }

      ফলাফল = ws_dec_pass(&ws->dec, তথ্য, &ws->recvbuf,
                           ws_client_collect, &ctx);
      যদি(ফলাফল == CURLE_AGAIN) {
        যদি(!ctx.written) {
          ws_dec_info(&ws->dec, তথ্য, "need more input");
          চালু;  /* nothing written, try more input */
        }
        বিরতি;
      }
      অন্যথা যদি(ফলাফল) {
        যাও out;
      }
      অন্যথা যদি(ctx.written) {
        /* The decoded frame is passed back to our caller.
         * There are frames like PING were we auto-respond to and
         * that we do not return. For these `ctx.written` is not set. */
        বিরতি;
      }
    }

    /* update frame information to be passed back */
    update_meta(ws, ctx.frame_age, ctx.frame_flags, ctx.payload_offset,
                ctx.payload_len, ctx.bufidx);
    *metap = &ws->recvframe;
    *recv = ws->recvframe.len;
    CURL_TRC_WS(তথ্য, "curl_ws_recv(len=%zu) -> %zu bytes (frame at %"
                FMT_OFF_T ", %" FMT_OFF_T " left)",
                buflen, *recv, ws->recvframe.offset,
                ws->recvframe.bytesleft);
    /* all's well, try to send any pending control. we do not know
     * when the application will call `curl_ws_send()` again. */
    যদি(!তথ্য->set.ws_raw_mode && !ws->using_reader && ws->pending.type) {
      CURLcode r2 = ws_enc_add_pending(তথ্য, ws);
      যদি(!r2)
        (শূন্য)ws_flush(তথ্য, ws, Curl_api_is_in_callback(তথ্য));
    }
    ফলাফল = CURLE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}

স্থির CURLcode ws_flush(কাঠামো Curl_easy *তথ্য, কাঠামো websocket *ws,
                         bool blocking)
{
  CURLcode ফলাফল;

  /* If there is space, add any pending control frame */
  যদি(Curl_bufq_len(&ws->sendbuf) < ws->sendbuf.chunk_আকার) {
    ফলাফল = ws_enc_add_pending(তথ্য, ws);
    যদি(ফলাফল && (ফলাফল != CURLE_AGAIN))
      ফেরত ফলাফল;
  }

  যদি(!Curl_bufq_is_empty(&ws->sendbuf)) {
    ধ্রুবক uint8_t *out;
    size_t outlen, n;
#ifdef DEBUGBUILD
    /* Simulate a blocking send after this chunk has been sent */
    bool eagain_পরবর্তী = FALSE;
    size_t chunk_egain = 0;
    ধ্রুবক অক্ষর *p = getenv("CURL_WS_CHUNK_EAGAIN");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, 1 * 1024 * 1024))
        chunk_egain = (size_t)l;
    }
#endif

    যখন(Curl_bufq_peek(&ws->sendbuf, &out, &outlen)) {
#ifdef DEBUGBUILD
      যদি(eagain_পরবর্তী)
        ফেরত CURLE_AGAIN;
      যদি(chunk_egain && (outlen > chunk_egain)) {
        outlen = chunk_egain;
        eagain_পরবর্তী = TRUE;
      }
#endif
      যদি(blocking) {
        ফলাফল = ws_send_raw_blocking(তথ্য, ws, (ধ্রুবক অক্ষর *)out, outlen);
        n = ফলাফল ? 0 : outlen;
      }
      অন্যথা যদি(তথ্য->set.connect_only || Curl_api_is_in_callback(তথ্য))
        ফলাফল = Curl_senddata(তথ্য, out, outlen, &n);
      অন্যথা {
        ফলাফল = Curl_xfer_send(তথ্য, out, outlen, FALSE, &n);
        যদি(!ফলাফল && !n && outlen)
          ফলাফল = CURLE_AGAIN;
      }

      যদি(ফলাফল == CURLE_AGAIN) {
        CURL_TRC_WS(তথ্য, "flush EAGAIN, %zu bytes remain in buffer",
                    Curl_bufq_len(&ws->sendbuf));
        ফেরত ফলাফল;
      }
      অন্যথা যদি(ফলাফল) {
        failf(তথ্য, "[WS] flush, write error %d", (পূর্ণসংখ্যা)ফলাফল);
        ফেরত ফলাফল;
      }
      অন্যথা {
        CURL_TRC_WS(তথ্য, "flushed %zu bytes", n);
        Curl_bufq_skip(&ws->sendbuf, n);
      }
    }
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ws_send_raw_blocking(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো websocket *ws,
                                     ধ্রুবক অক্ষর *বাফার, size_t buflen)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t nwritten;

  যদি(!তথ্য)
    ফেরত ফলাফল;

  (শূন্য)ws;
  যখন(buflen) {
    ফলাফল = Curl_xfer_send(তথ্য, বাফার, buflen, FALSE, &nwritten);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    DEBUGASSERT(nwritten <= buflen);
    বাফার += nwritten;
    buflen -= nwritten;
    যদি(buflen) {
      curl_socket_t sock = তথ্য->conn->sock[FIRSTSOCKET];
      timediff_t বাম_ms;
      পূর্ণসংখ্যা ev;

      CURL_TRC_WS(তথ্য, "ws_send_raw_blocking() partial, %zu left to send",
                  buflen);
      বাম_ms = Curl_timeleft_ms(তথ্য);
      যদি(বাম_ms < 0) {
        failf(তথ্য, "[WS] Timeout waiting for socket becoming writable");
        ফেরত CURLE_SEND_ERROR;
      }

      /* POLLOUT socket */
      যদি(sock == CURL_SOCKET_BAD)
        ফেরত CURLE_SEND_ERROR;
      ev = SOCKET_WRITABLE(sock, বাম_ms ? বাম_ms : 500);
      যদি(ev < 0) {
        failf(তথ্য, "[WS] Error while waiting for socket becoming writable");
        ফেরত CURLE_SEND_ERROR;
      }
    }
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ws_send_raw(কাঠামো Curl_easy *তথ্য, ধ্রুবক শূন্য *বাফার,
                            size_t buflen, size_t *pnwritten)
{
  কাঠামো websocket *ws;
  CURLcode ফলাফল;

  ws = Curl_conn_meta_get(তথ্য->conn, CURL_META_PROTO_WS_CONN);
  যদি(!ws) {
    failf(তথ্য, "[WS] Not a websocket transfer");
    ফেরত CURLE_SEND_ERROR;
  }
  যদি(!buflen)
    ফেরত CURLE_OK;

  যদি(Curl_api_is_in_callback(তথ্য)) {
    /* When invoked from inside callbacks, we do a blocking send as the
     * callback will probably not implement partial writes that may then
     * mess up the ws framing subsequently.
     * We need any pending data to be flushed before sending. */
    ফলাফল = ws_flush(তথ্য, ws, TRUE);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    ফলাফল = ws_send_raw_blocking(তথ্য, ws, বাফার, buflen);
    যদি(!ফলাফল)
      *pnwritten = buflen;
  }
  অন্যথা {
    /* We need any pending data to be sent or EAGAIN this call. */
    ফলাফল = ws_flush(তথ্য, ws, FALSE);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    ফলাফল = Curl_senddata(তথ্য, বাফার, buflen, pnwritten);
  }

  CURL_TRC_WS(তথ্য, "ws_send_raw(len=%zu) -> %d, %zu",
              buflen, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
  ফেরত ফলাফল;
}

CURLcode curl_ws_send(CURL *curl, ধ্রুবক শূন্য *বাফার_arg,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned পূর্ণসংখ্যা flags)
{
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল = CURLE_OK;

  যদি(CURL_EAPI_ENTER(&guard, curl, ws_send, &ফলাফল)) {
    কাঠামো websocket *ws;
    ধ্রুবক uint8_t *বাফার = বাফার_arg;
    কাঠামো Curl_easy *তথ্য = curl;
    size_t ndummy;
    size_t *pnsent = sent ? sent : &ndummy;

    CURL_TRC_WS(তথ্য, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x), raw=%d",
                buflen, fragsize, flags, তথ্য->set.ws_raw_mode);

    *pnsent = 0;

    যদি(!বাফার && buflen) {
      failf(তথ্য, "[WS] buffer is NULL when buflen is not");
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }

    যদি(!তথ্য->conn && তথ্য->set.connect_only) {
      ফলাফল = Curl_connect_only_attach(তথ্য);
      যদি(ফলাফল)
        যাও out;
    }
    যদি(!তথ্য->conn) {
      failf(তথ্য, "[WS] No associated connection");
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
    ws = Curl_conn_meta_get(তথ্য->conn, CURL_META_PROTO_WS_CONN);
    যদি(!ws) {
      failf(তথ্য, "[WS] Not a websocket transfer");
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }

    যদি(তথ্য->set.ws_raw_mode) {
      /* In raw mode, we write directly to the connection */
      /* try flushing any content still waiting to be sent. */
      ফলাফল = ws_flush(তথ্য, ws, FALSE);
      যদি(ফলাফল)
        যাও out;

      যদি(!বাফার) {
        failf(তথ্য, "[WS] buffer is NULL in raw mode");
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
        যাও out;
      }
      যদি(!sent) {
        failf(তথ্য, "[WS] sent is NULL in raw mode");
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
        যাও out;
      }
      যদি(fragsize || flags) {
        failf(তথ্য, "[WS] fragsize and flags must be zero in raw mode");
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
        যাও out;
      }
      ফলাফল = ws_send_raw(তথ্য, বাফার, buflen, pnsent);
      যাও out;
    }

    /* Not RAW mode, we do the frame encoding */
    ফলাফল = ws_enc_send(তথ্য, ws, বাফার, buflen, fragsize, flags, pnsent);
    CURL_TRC_WS(তথ্য, "curl_ws_send(len=%zu, fragsize=%" FMT_OFF_T
                ", flags=%x, raw=%d) -> %d, %zu",
                buflen, fragsize, flags, তথ্য->set.ws_raw_mode, (পূর্ণসংখ্যা)ফলাফল,
                *pnsent);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}

স্থির CURLcode ws_setup_conn(কাঠামো Curl_easy *তথ্য,
                              কাঠামো connectdata *conn)
{
  /* WebSocket is 1.1 only (for now) */
  তথ্য->state.http_neg.accept_09 = FALSE;
  তথ্য->state.http_neg.only_10 = FALSE;
  তথ্য->state.http_neg.wanted = CURL_HTTP_V1x;
  তথ্য->state.http_neg.allowed = CURL_HTTP_V1x;
  ফেরত Curl_http_setup_conn(তথ্য, conn);
}

ধ্রুবক কাঠামো curl_ws_frame *curl_ws_meta(CURL *curl)
{
  /* we only return something for websocket, called from within the callback
     when not using raw mode */
  কাঠামো Curl_easy *তথ্য = curl;
  যদি(GOOD_EASY_HANDLE(তথ্য) && Curl_api_is_in_callback(তথ্য) &&
     তথ্য->conn && !তথ্য->set.ws_raw_mode) {
    কাঠামো websocket *ws;
    ws = Curl_conn_meta_get(তথ্য->conn, CURL_META_PROTO_WS_CONN);
    যদি(ws)
      ফেরত &ws->recvframe;
  }
  ফেরত ফাঁকা;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned পূর্ণসংখ্যা flags,
                                         curl_off_t frame_len)
{
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল = CURLE_OK;

  যদি(CURL_EAPI_ENTER(&guard, curl, ws_start_frame, &ফলাফল)) {
    কাঠামো Curl_easy *তথ্য = curl;
    কাঠামো websocket *ws;

    যদি(তথ্য->set.ws_raw_mode) {
      failf(তথ্য, "cannot curl_ws_start_frame() with CURLWS_RAW_MODE enabled");
      ফলাফল = CURLE_FAILED_INIT;
      যাও out;
    }

    CURL_TRC_WS(তথ্য, "curl_ws_start_frame(flags=%x, frame_len=%" FMT_OFF_T,
                flags, frame_len);

    যদি(!তথ্য->conn) {
      failf(তথ্য, "[WS] No associated connection");
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
    ws = Curl_conn_meta_get(তথ্য->conn, CURL_META_PROTO_WS_CONN);
    যদি(!ws) {
      failf(তথ্য, "[WS] Not a websocket transfer");
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }

    যদি(ws->enc.payload_remain) {
      failf(তথ্য, "[WS] previous frame not finished");
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }

    ফলাফল = ws_enc_write_শীর্ষ(তথ্য, ws, &ws->enc, flags, frame_len,
                               &ws->sendbuf);
    যদি(ফলাফল)
      CURL_TRC_WS(তথ্য, "curl_start_frame(), error adding frame head %d",
                  (পূর্ণসংখ্যা)ফলাফল);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}

ধ্রুবক কাঠামো Curl_protocol Curl_protocol_ws = {
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

#অন্যথা

CURLcode curl_ws_recv(CURL *curl, শূন্য *বাফার, size_t buflen,
                      size_t *recv,
                      ধ্রুবক কাঠামো curl_ws_frame **metap)
{
  (শূন্য)curl;
  (শূন্য)বাফার;
  (শূন্য)buflen;
  (শূন্য)recv;
  (শূন্য)metap;
  ফেরত CURLE_NOT_BUILT_IN;
}

CURLcode curl_ws_send(CURL *curl, ধ্রুবক শূন্য *বাফার,
                      size_t buflen, size_t *sent,
                      curl_off_t fragsize,
                      unsigned পূর্ণসংখ্যা flags)
{
  (শূন্য)curl;
  (শূন্য)বাফার;
  (শূন্য)buflen;
  (শূন্য)sent;
  (শূন্য)fragsize;
  (শূন্য)flags;
  ফেরত CURLE_NOT_BUILT_IN;
}

ধ্রুবক কাঠামো curl_ws_frame *curl_ws_meta(CURL *তথ্য)
{
  (শূন্য)তথ্য;
  ফেরত ফাঁকা;
}

CURL_EXTERN CURLcode curl_ws_start_frame(CURL *curl,
                                         unsigned পূর্ণসংখ্যা flags,
                                         curl_off_t frame_len)
{
  (শূন্য)curl;
  (শূন্য)flags;
  (শূন্য)frame_len;
  ফেরত CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
