/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 * Copyright (C) Björn Stenberg, <bjorn@haxx.se>
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

#ifndef CURL_DISABLE_MQTT

#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "mqtt.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"

/* first byte is command.
   second byte is for flags. */
#define MQTT_ಸಂದೇಶ_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_ಸಂದೇಶ_PUBLISH    0x30
#define MQTT_ಸಂದೇಶ_SUBSCRIBE  0x82
#define MQTT_ಸಂದೇಶ_SUBACK     0x90
#define MQTT_ಸಂದೇಶ_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_ಸಂದೇಶ_PINGRESP   0xD0

#define MQTT_CONNACK_LEN  2
#define MQTT_SUBACK_LEN   3
#define MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#define CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

ಗಣನಾ mqttstate {
  MQTT_FIRST,             /* 0 */
  MQTT_REMAINING_LENGTH,  /* 1 */
  MQTT_CONNACK,           /* 2 */
  MQTT_SUBACK,            /* 3 */
  MQTT_SUBACK_COMING,     /* 4 - the SUBACK remainder */
  MQTT_PUBWAIT,    /* 5 - wait for publish */
  MQTT_PUB_REMAIN,  /* 6 - wait for the remainder of the publish */
  MQTT_POST_DRAIN,        /* 7 - finish sending PUBLISH */
  MQTT_DISCONNECT_DRAIN,  /* 8 - finish sending DISCONNECT */

  MQTT_NOSTATE /* 9 - never used an actual state */
};

ಸಂರಚನಾ mqtt_conn {
  ಗಣನಾ mqttstate state;
  ಗಣನಾ mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned ಪೂರ್ಣಾಙ್ಕ packetid;
};

/* protocol-specific transfer-related data */
ಸಂರಚನಾ MQTT {
  ಸಂರಚನಾ dynbuf sendbuf;
  /* when receiving */
  ಸಂರಚನಾ dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned ವರ್ಣ pkt_hd[4]; /* for decoding the arriving packet length */
  ಸಂರಚನಾ curltime lastTime; /* last time we sent or received data */
  unsigned ವರ್ಣ firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

ಸ್ಥಿರ ಶೂನ್ಯ mqtt_easy_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ MQTT *mq = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

ಸ್ಥಿರ ಶೂನ್ಯ mqtt_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_free(entry);
}

ಸ್ಥಿರ CURLcode mqtt_setup_conn(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  ಸಂರಚನಾ mqtt_conn *mqtt;
  ಸಂರಚನಾ MQTT *mq;

  mqtt = curlx_calloc(1, ಪರಿಮಾಣ(*mqtt));
  ಯದಿ(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ MQTT));
  ಯದಿ(!mq)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  ಯದಿ(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mqtt_send(ಸಂರಚನಾ Curl_easy *data,
                          ಧ್ರುವ ವರ್ಣ *buf, size_t len)
{
  size_t n;
  CURLcode result;
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  ಯದಿ(!mq)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  ಯದಿ(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
ಸ್ಥಿರ CURLcode mqtt_flush(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  ಯದಿ(!mq)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  ಯದಿ(!len)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
ಸ್ಥಿರ CURLcode mqtt_pollset(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  ಯದಿ(mq && curlx_dyn_len(&mq->sendbuf))
    ಪ್ರತ್ಯಾಗಮ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  ಪ್ರತ್ಯಾಗಮ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mqtt_encode_len(ವರ್ಣ *buf, size_t len)
{
  ಪೂರ್ಣಾಙ್ಕ i;

  ಪ್ರತಿ(i = 0; (len > 0) && (i < 4); i++) {
    unsigned ವರ್ಣ encoded;
    encoded = len % 0x80;
    len /= 0x80;
    ಯದಿ(len)
      encoded |= 0x80;
    buf[i] = (ವರ್ಣ)encoded;
  }

  ಪ್ರತ್ಯಾಗಮ i;
}

/* add the passwd to the CONNECT packet */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ add_passwd(ಧ್ರುವ ವರ್ಣ *passwd, ಧ್ರುವ size_t plen,
                      ವರ್ಣ *pkt, ಧ್ರುವ size_t start, ಪೂರ್ಣಾಙ್ಕ remain_pos)
{
  /* magic number that need to be set properly */
  ಧ್ರುವ size_t conn_flags_pos = remain_pos + 8;
  ಯದಿ(plen > 0xffff)
    ಪ್ರತ್ಯಾಗಮ 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (ವರ್ಣ)((plen >> 8) & 0xFF);
  pkt[start + 1] = (ವರ್ಣ)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* add user to the CONNECT packet */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ add_user(ಧ್ರುವ ವರ್ಣ *username, ಧ್ರುವ size_t ulen,
                    unsigned ವರ್ಣ *pkt, ಧ್ರುವ size_t start, ಪೂರ್ಣಾಙ್ಕ remain_pos)
{
  /* magic number that need to be set properly */
  ಧ್ರುವ size_t conn_flags_pos = remain_pos + 8;
  ಯದಿ(ulen > 0xffff)
    ಪ್ರತ್ಯಾಗಮ 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned ವರ್ಣ)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned ವರ್ಣ)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* add client ID to the CONNECT packet */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ add_client_id(ಧ್ರುವ ವರ್ಣ *client_id, ಧ್ರುವ size_t client_id_len,
                         ವರ್ಣ *pkt, ಧ್ರುವ size_t start)
{
  ಯದಿ(client_id_len != MQTT_CLIENTID_LEN)
    ಪ್ರತ್ಯಾಗಮ 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* Set initial values of CONNECT packet */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ init_connpack(ವರ್ಣ *packet, ವರ್ಣ *remain, ಪೂರ್ಣಾಙ್ಕ remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_ಸಂದೇಶ_CONNECT;
  /* remaining length field */
  memcpy(&packet[1], remain, remain_pos);
  /* Fixed header ends */

  /* Variable header starts */
  /* protocol length */
  packet[remain_pos + 1] = 0x00;
  packet[remain_pos + 2] = 0x04;
  /* protocol name */
  packet[remain_pos + 3] = 'M';
  packet[remain_pos + 4] = 'Q';
  packet[remain_pos + 5] = 'T';
  packet[remain_pos + 6] = 'T';
  /* protocol level */
  packet[remain_pos + 7] = 0x04;
  /* CONNECT flag: CleanSession */
  packet[remain_pos + 8] = 0x02;
  /* keep-alive 0 = disabled */
  packet[remain_pos + 9] = 0x00;
  packet[remain_pos + 10] = 0x3c;
  /* end of variable header */
  ಪ್ರತ್ಯಾಗಮ remain_pos + 10;
}

ಸ್ಥಿರ CURLcode mqtt_connect(ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ pos = 0;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  /* remain length */
  ಪೂರ್ಣಾಙ್ಕ remain_pos = 0;
  ವರ್ಣ remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  ವರ್ಣ client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ಧ್ರುವ size_t clen = CURL_CSTRLEN("curl");
  ವರ್ಣ *packet = ರಿಕ್ತ;

  /* extracting username from request */
  ಸಂರಚನಾ Curl_creds *creds = data->state.creds;
  ಧ್ರುವ size_t ulen = creds ? strlen(creds->user) : 0;
  ಧ್ರುವ size_t plen = creds ? strlen(creds->passwd) : 0;
  ಧ್ರುವ size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  ಯದಿ(packetlen > 0xFFFFFFF)
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  ಯದಿ(!packet)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned ವರ್ಣ *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  ಯದಿ(rc) {
    failf(data, "Client ID length mismatched: [%zu]", strlen(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    ಗಚ್ಛ end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  ಯದಿ(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned ವರ್ಣ *)packet, start_user, remain_pos);
    ಯದಿ(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      ಗಚ್ಛ end;
    }
  }

  /* if passwd was provided, add it to the packet */
  ಯದಿ(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    ಯದಿ(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      ಗಚ್ಛ end;
    }
  }

  ಯದಿ(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  ಯದಿ(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mqtt_disconnect(ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ mqtt_send(data, "\xe0\x00", 2);
}

ಸ್ಥಿರ CURLcode mqtt_recv_atleast(ಸಂರಚನಾ Curl_easy *data, size_t nbytes)
{
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  ಯದಿ(!mq)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  ಯದಿ(rlen < nbytes) {
    unsigned ವರ್ಣ readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < ಪರಿಮಾಣ(readbuf));
    result = Curl_xfer_recv(data, (ವರ್ಣ *)readbuf, nbytes - rlen, &nread);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಯದಿ(!nread) /* EOF */
       ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    ಯದಿ(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  ಪ್ರತ್ಯಾಗಮ (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

ಸ್ಥಿರ ಶೂನ್ಯ mqtt_recv_consume(ಸಂರಚನಾ Curl_easy *data, size_t nbytes)
{
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  ಯದಿ(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    ಯದಿ(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    ಅನ್ಯಥಾ
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

ಸ್ಥಿರ CURLcode mqtt_verify_connack(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  ಧ್ರುವ ವರ್ಣ *ptr;

  DEBUGASSERT(mq);
  ಯದಿ(!mq)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  ಯದಿ(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned ವರ್ಣ)ptr[0], (unsigned ವರ್ಣ)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mqtt_get_topic(ಸಂರಚನಾ Curl_easy *data,
                               ವರ್ಣ **topic, size_t *topiclen)
{
  ಧ್ರುವ ವರ್ಣ *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  ಯದಿ(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    ಯದಿ(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  ಅನ್ಯಥಾ
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mqtt_subscribe(ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ವರ್ಣ *topic = ರಿಕ್ತ;
  size_t topiclen;
  unsigned ವರ್ಣ *packet = ರಿಕ್ತ;
  size_t packetlen;
  ವರ್ಣ encodedsize[4];
  size_t n;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ಯದಿ(!mqtt)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((ವರ್ಣ *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  ಯದಿ(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ fail;
  }

  packet[0] = MQTT_ಸಂದೇಶ_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (ಧ್ರುವ ವರ್ಣ *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Called when the first byte was already read.
 */
ಸ್ಥಿರ CURLcode mqtt_verify_suback(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  ಧ್ರುವ ವರ್ಣ *ptr;

  ಯದಿ(!mqtt || !mq)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  ಯದಿ(((unsigned ವರ್ಣ)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned ವರ್ಣ)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    ಗಚ್ಛ fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  ಪ್ರತ್ಯಾಗಮ result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

ಸ್ಥಿರ CURLcode mqtt_publish(ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result;
  ವರ್ಣ *payload = data->set.postfields;
  size_t payloadlen;
  ವರ್ಣ *topic = ರಿಕ್ತ;
  size_t topiclen;
  unsigned ವರ್ಣ *pkt = ರಿಕ್ತ;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  ವರ್ಣ encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  ಯದಿ(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ಯದಿ(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    ಯದಿ(postfieldsize > 0) /* off_t does not fit into size_t */
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  ಯದಿ((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    ಗಚ್ಛ fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  ಯದಿ(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    ಗಚ್ಛ fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  ಯದಿ(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_ಸಂದೇಶ_PUBLISH;
  memcpy(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  memcpy(&pkt[i], topic, topiclen);
  i += topiclen;
  memcpy(&pkt[i], payload, payloadlen);
  i += payloadlen;
  result = mqtt_send(data, (ಧ್ರುವ ವರ್ಣ *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* return FALSE on success, TRUE on error */
ಸ್ಥಿರ bool mqtt_decode_len(size_t *lenp, ಧ್ರುವ unsigned ವರ್ಣ *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned ವರ್ಣ encoded = 128;

  ಪ್ರತಿ(i = 0; (i < buflen) && (encoded & 128); i++) {
    ಯದಿ(i == 4)
      ಪ್ರತ್ಯಾಗಮ TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  ಯದಿ(encoded & 128)
    /* truncated size */
    ಪ್ರತ್ಯಾಗಮ TRUE;

  *lenp = len;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ಯದಿ defined(DEBUGBUILD) && defined(CURLVERBOSE)
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ statenames[] = {
  "MQTT_FIRST",
  "MQTT_REMAINING_LENGTH",
  "MQTT_CONNACK",
  "MQTT_SUBACK",
  "MQTT_SUBACK_COMING",
  "MQTT_PUBWAIT",
  "MQTT_PUB_REMAIN",
  "MQTT_POST_DRAIN",
  "MQTT_DISCONNECT_DRAIN",

  "NOT A STATE"
};
#endif

/* The only way to change state */
ಸ್ಥಿರ ಶೂನ್ಯ mqstate(ಸಂರಚನಾ Curl_easy *data,
                    ಗಣನಾ mqttstate state,
                    ಗಣನಾ mqttstate nextstate) /* used if state == FIRST */
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  ಯದಿ(!mqtt)
    ಪ್ರತ್ಯಾಗಮ;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  ಯದಿ(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

ಸ್ಥಿರ CURLcode mqtt_read_publish(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  ಸಂರಚನಾ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned ವರ್ಣ packet;

  DEBUGASSERT(mqtt);
  ಯದಿ(!mqtt || !mq)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ವಿಕಲ್ಪ(mqtt->state) {
MQTT_SUBACK_COMING:
  ಪಕ್ಷ MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    ಯದಿ(result)
      ಭಙ್ಗ;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    ಭಙ್ಗ;

  ಪಕ್ಷ MQTT_SUBACK:
  ಪಕ್ಷ MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    ಯದಿ(packet == MQTT_ಸಂದೇಶ_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    ಅನ್ಯಥಾ ಯದಿ(packet == MQTT_ಸಂದೇಶ_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      ಗಚ್ಛ MQTT_SUBACK_COMING;
    }
    ಅನ್ಯಥಾ ಯದಿ(packet == MQTT_ಸಂದೇಶ_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      ಗಚ್ಛ end;
    }
    ಅನ್ಯಥಾ {
      result = CURLE_WEIRD_SERVER_REPLY;
      ಗಚ್ಛ end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    ಯದಿ(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      ಗಚ್ಛ end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  ಪಕ್ಷ MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    ವರ್ಣ buffer[4 * 1024];
    size_t rest = mq->npacket;
    ಯದಿ(rest > ಪರಿಮಾಣ(buffer))
      rest = ಪರಿಮಾಣ(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    ಯದಿ(result) {
      ಯದಿ(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      ಗಚ್ಛ end;
    }
    ಯದಿ(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      ಗಚ್ಛ end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    ಯದಿ(result)
      ಗಚ್ಛ end;

    mq->npacket -= nread;
    ಯದಿ(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    ಭಙ್ಗ;
  }
  ಸ್ವಭಾವ:
    DEBUGASSERT(ರಿಕ್ತ); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    ಗಚ್ಛ end;
  }
end:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mqtt_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  ಯದಿ(!mq)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  ಯದಿ(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (ಪೂರ್ಣಾಙ್ಕ)result);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mqtt_done(ಸಂರಚನಾ Curl_easy *data,
                          CURLcode status, bool premature)
{
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (ಶೂನ್ಯ)status;
  (ಶೂನ್ಯ)premature;
  ಯದಿ(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
ಸ್ಥಿರ CURLcode mqtt_ping(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ಯದಿ(!mqtt || !mq)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    ಸಂರಚನಾ curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    ಯದಿ(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned ವರ್ಣ packet[2] = { 0xC0, 0x00 };
      size_t packetlen = ಪರಿಮಾಣ(packet);

      result = mqtt_send(data, (ವರ್ಣ *)packet, packetlen);
      ಯದಿ(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mqtt_doing(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned ವರ್ಣ recvbyte;
  ಸಂರಚನಾ mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  ಯದಿ(!mqtt || !mq)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  *done = FALSE;

  ಯದಿ(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    ಯದಿ(result || curlx_dyn_len(&mq->sendbuf))
      ಪ್ರತ್ಯಾಗಮ result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  ಯದಿ(result || curlx_dyn_len(&mq->sendbuf))
    ಪ್ರತ್ಯಾಗಮ result;

  infof(data, "mqtt_doing: state [%d]", (ಪೂರ್ಣಾಙ್ಕ)mqtt->state);
  ವಿಕಲ್ಪ(mqtt->state) {
  ಪಕ್ಷ MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (ವರ್ಣ *)&mq->firstbyte, 1, &nread);
    ಯದಿ(result)
      ಭಙ್ಗ;
    ಅನ್ಯಥಾ ಯದಿ(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      ಭಙ್ಗ;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (ಧ್ರುವ ವರ್ಣ *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  ಪಕ್ಷ MQTT_REMAINING_LENGTH:
    ಕುರು {
      result = Curl_xfer_recv(data, (ವರ್ಣ *)&recvbyte, 1, &nread);
      ಯದಿ(result || !nread)
        ಭಙ್ಗ;
      Curl_debug(data, CURLINFO_HEADER_IN, (ಧ್ರುವ ವರ್ಣ *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } ಯಾವತ್((recvbyte & 0x80) && (mq->npacket < 4));
    ಯದಿ(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    ಯದಿ(result)
      ಭಙ್ಗ;
    ಯದಿ(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      ಭಙ್ಗ;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ಧ್ರುವ unsigned ವರ್ಣ type = mq->firstbyte & 0xF0;
      ಧ್ರುವ unsigned ವರ್ಣ reserved = mq->firstbyte & 0x0F;
      ಯದಿ((type == MQTT_ಸಂದೇಶ_DISCONNECT || type == MQTT_ಸಂದೇಶ_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_ಸಂದೇಶ_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        ಭಙ್ಗ;
      }
    }
    ಯದಿ(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      ಭಙ್ಗ;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    ಯದಿ(mq->firstbyte == MQTT_ಸಂದೇಶ_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    ಯದಿ(mq->firstbyte == MQTT_ಸಂದೇಶ_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    ಯದಿ(result)
      ಭಙ್ಗ;

    ಯದಿ(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      ಯದಿ(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      ಭಙ್ಗ;
    }

    result = mqtt_publish(data);
    ಯದಿ(result)
      ಭಙ್ಗ;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  ಪಕ್ಷ MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    ಯದಿ(curlx_dyn_len(&mq->sendbuf))
      ಭಙ್ಗ;
    result = mqtt_disconnect(data);
    ಯದಿ(result)
      ಭಙ್ಗ;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  ಪಕ್ಷ MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    ಯದಿ(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    ಭಙ್ಗ;

  ಪಕ್ಷ MQTT_SUBACK:
  ಪಕ್ಷ MQTT_PUBWAIT:
  ಪಕ್ಷ MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    failf(data, "State not handled yet");
    *done = TRUE;
    ಭಙ್ಗ;
  }

  ಯದಿ(result == CURLE_AGAIN)
    result = CURLE_OK;
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef USE_SSL

ಸ್ಥಿರ CURLcode mqtts_connecting(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  ಯದಿ(result)
    connclose(conn);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * MQTTS protocol.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_mqtts = {
  mqtt_setup_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_done,                          /* done */
  ZERO_NULL,                          /* do_more */
  ZERO_NULL,                          /* connect_it */
  mqtts_connecting,                   /* connecting */
  mqtt_doing,                         /* doing */
  ZERO_NULL,                          /* proto_pollset */
  mqtt_pollset,                       /* doing_pollset */
  ZERO_NULL,                          /* domore_pollset */
  ZERO_NULL,                          /* perform_pollset */
  ZERO_NULL,                          /* disconnect */
  ZERO_NULL,                          /* write_resp */
  ZERO_NULL,                          /* write_resp_hd */
  ZERO_NULL,                          /* connection_is_dead */
  ZERO_NULL,                          /* attach connection */
  ZERO_NULL,                          /* follow */
};

#endif

/*
 * MQTT protocol.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_mqtt = {
  mqtt_setup_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_done,                          /* done */
  ZERO_NULL,                          /* do_more */
  ZERO_NULL,                          /* connect_it */
  ZERO_NULL,                          /* connecting */
  mqtt_doing,                         /* doing */
  ZERO_NULL,                          /* proto_pollset */
  mqtt_pollset,                       /* doing_pollset */
  ZERO_NULL,                          /* domore_pollset */
  ZERO_NULL,                          /* perform_pollset */
  ZERO_NULL,                          /* disconnect */
  ZERO_NULL,                          /* write_resp */
  ZERO_NULL,                          /* write_resp_hd */
  ZERO_NULL,                          /* connection_is_dead */
  ZERO_NULL,                          /* attach connection */
  ZERO_NULL,                          /* follow */
};

#endif /* CURL_DISABLE_MQTT */
