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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "urldata.h"

#ifndef CURL_DISABLE_MQTT

#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "mqtt.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "escape.h"
#অন্তর্ভাৱ "rand.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"

/* first byte is command.
   second byte is for flags. */
#define MQTT_বাৰ্তা_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_বাৰ্তা_PUBLISH    0x30
#define MQTT_বাৰ্তা_SUBSCRIBE  0x82
#define MQTT_বাৰ্তা_SUBACK     0x90
#define MQTT_বাৰ্তা_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_বাৰ্তা_PINGRESP   0xD0

#define MQTT_CONNACK_LEN  2
#define MQTT_SUBACK_LEN   3
#define MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#define CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

গণনা mqttstate {
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

সংরচনা mqtt_conn {
  গণনা mqttstate state;
  গণনা mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned পূর্ণাঙ্ক packetid;
};

/* protocol-specific transfer-related data */
সংরচনা MQTT {
  সংরচনা dynbuf sendbuf;
  /* when receiving */
  সংরচনা dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned ৱর্ণ pkt_hd[4]; /* for decoding the arriving packet length */
  সংরচনা curltime lastTime; /* last time we sent or received data */
  unsigned ৱর্ণ firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

স্থির শূন্য mqtt_easy_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা MQTT *mq = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

স্থির শূন্য mqtt_conn_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_free(entry);
}

স্থির CURLcode mqtt_setup_conn(সংরচনা Curl_easy *data,
                                সংরচনা connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  সংরচনা mqtt_conn *mqtt;
  সংরচনা MQTT *mq;

  mqtt = curlx_calloc(1, পরিমাণ(*mqtt));
  যদি(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, পরিমাণ(সংরচনা MQTT));
  যদি(!mq)
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  যদি(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode mqtt_send(সংরচনা Curl_easy *data,
                          ধ্রুৱ ৱর্ণ *buf, size_t len)
{
  size_t n;
  CURLcode result;
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  যদি(!mq)
    ঘূৰাই CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  যদি(result)
    ঘূৰাই result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  যদি(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  ঘূৰাই result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
স্থির CURLcode mqtt_flush(সংরচনা Curl_easy *data)
{
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  যদি(!mq)
    ঘূৰাই CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  যদি(!len)
    ঘূৰাই CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  যদি(result)
    ঘূৰাই result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  ঘূৰাই curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
স্থির CURLcode mqtt_pollset(সংরচনা Curl_easy *data,
                             সংরচনা easy_pollset *ps)
{
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  যদি(mq && curlx_dyn_len(&mq->sendbuf))
    ঘূৰাই Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  ঘূৰাই Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

স্থির পূর্ণাঙ্ক mqtt_encode_len(ৱর্ণ *buf, size_t len)
{
  পূর্ণাঙ্ক i;

  বাবে(i = 0; (len > 0) && (i < 4); i++) {
    unsigned ৱর্ণ encoded;
    encoded = len % 0x80;
    len /= 0x80;
    যদি(len)
      encoded |= 0x80;
    buf[i] = (ৱর্ণ)encoded;
  }

  ঘূৰাই i;
}

/* add the passwd to the CONNECT packet */
স্থির পূর্ণাঙ্ক add_passwd(ধ্রুৱ ৱর্ণ *passwd, ধ্রুৱ size_t plen,
                      ৱর্ণ *pkt, ধ্রুৱ size_t start, পূর্ণাঙ্ক remain_pos)
{
  /* magic number that need to be set properly */
  ধ্রুৱ size_t conn_flags_pos = remain_pos + 8;
  যদি(plen > 0xffff)
    ঘূৰাই 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (ৱর্ণ)((plen >> 8) & 0xFF);
  pkt[start + 1] = (ৱর্ণ)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  ঘূৰাই 0;
}

/* add user to the CONNECT packet */
স্থির পূর্ণাঙ্ক add_user(ধ্রুৱ ৱর্ণ *username, ধ্রুৱ size_t ulen,
                    unsigned ৱর্ণ *pkt, ধ্রুৱ size_t start, পূর্ণাঙ্ক remain_pos)
{
  /* magic number that need to be set properly */
  ধ্রুৱ size_t conn_flags_pos = remain_pos + 8;
  যদি(ulen > 0xffff)
    ঘূৰাই 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned ৱর্ণ)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned ৱর্ণ)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  ঘূৰাই 0;
}

/* add client ID to the CONNECT packet */
স্থির পূর্ণাঙ্ক add_client_id(ধ্রুৱ ৱর্ণ *client_id, ধ্রুৱ size_t client_id_len,
                         ৱর্ণ *pkt, ধ্রুৱ size_t start)
{
  যদি(client_id_len != MQTT_CLIENTID_LEN)
    ঘূৰাই 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  ঘূৰাই 0;
}

/* Set initial values of CONNECT packet */
স্থির পূর্ণাঙ্ক init_connpack(ৱর্ণ *packet, ৱর্ণ *remain, পূর্ণাঙ্ক remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_বাৰ্তা_CONNECT;
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
  ঘূৰাই remain_pos + 10;
}

স্থির CURLcode mqtt_connect(সংরচনা Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক pos = 0;
  পূর্ণাঙ্ক rc = 0;
  /* remain length */
  পূর্ণাঙ্ক remain_pos = 0;
  ৱর্ণ remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  ৱর্ণ client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ধ্রুৱ size_t clen = CURL_CSTRLEN("curl");
  ৱর্ণ *packet = রিক্ত;

  /* extracting username from request */
  সংরচনা Curl_creds *creds = data->state.creds;
  ধ্রুৱ size_t ulen = creds ? strlen(creds->user) : 0;
  ধ্রুৱ size_t plen = creds ? strlen(creds->passwd) : 0;
  ধ্রুৱ size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  যদি(packetlen > 0xFFFFFFF)
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  যদি(!packet)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned ৱর্ণ *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  যদি(rc) {
    failf(data, "Client ID length mismatched: [%zu]", strlen(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    গচ্ছ end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  যদি(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned ৱর্ণ *)packet, start_user, remain_pos);
    যদি(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      গচ্ছ end;
    }
  }

  /* if passwd was provided, add it to the packet */
  যদি(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    যদি(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      গচ্ছ end;
    }
  }

  যদি(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  যদি(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  ঘূৰাই result;
}

স্থির CURLcode mqtt_disconnect(সংরচনা Curl_easy *data)
{
  ঘূৰাই mqtt_send(data, "\xe0\x00", 2);
}

স্থির CURLcode mqtt_recv_atleast(সংরচনা Curl_easy *data, size_t nbytes)
{
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  যদি(!mq)
    ঘূৰাই CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  যদি(rlen < nbytes) {
    unsigned ৱর্ণ readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < পরিমাণ(readbuf));
    result = Curl_xfer_recv(data, (ৱর্ণ *)readbuf, nbytes - rlen, &nread);
    যদি(result)
      ঘূৰাই result;
    যদি(!nread) /* EOF */
       ঘূৰাই CURLE_RECV_ERROR;
    যদি(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  ঘূৰাই (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

স্থির শূন্য mqtt_recv_consume(সংরচনা Curl_easy *data, size_t nbytes)
{
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  যদি(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    যদি(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    অন্যথা
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

স্থির CURLcode mqtt_verify_connack(সংরচনা Curl_easy *data)
{
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  ধ্রুৱ ৱর্ণ *ptr;

  DEBUGASSERT(mq);
  যদি(!mq)
    ঘূৰাই CURLE_FAILED_INIT;
  যদি(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  যদি(result)
    ঘূৰাই result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  যদি(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned ৱর্ণ)ptr[0], (unsigned ৱর্ণ)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode mqtt_get_topic(সংরচনা Curl_easy *data,
                               ৱর্ণ **topic, size_t *topiclen)
{
  ধ্রুৱ ৱর্ণ *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  যদি(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    যদি(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  অন্যথা
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  ঘূৰাই result;
}

স্থির CURLcode mqtt_subscribe(সংরচনা Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ৱর্ণ *topic = রিক্ত;
  size_t topiclen;
  unsigned ৱর্ণ *packet = রিক্ত;
  size_t packetlen;
  ৱর্ণ encodedsize[4];
  size_t n;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  যদি(!mqtt)
    ঘূৰাই CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  যদি(result)
    গচ্ছ fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((ৱর্ণ *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  যদি(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ fail;
  }

  packet[0] = MQTT_বাৰ্তা_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (ধ্রুৱ ৱর্ণ *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  ঘূৰাই result;
}

/*
 * Called when the first byte was already read.
 */
স্থির CURLcode mqtt_verify_suback(সংরচনা Curl_easy *data)
{
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  সংরচনা connectdata *conn = data->conn;
  সংরচনা mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  ধ্রুৱ ৱর্ণ *ptr;

  যদি(!mqtt || !mq)
    ঘূৰাই CURLE_FAILED_INIT;

  যদি(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  যদি(result)
    গচ্ছ fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  যদি(((unsigned ৱর্ণ)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned ৱর্ণ)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    গচ্ছ fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  ঘূৰাই result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

স্থির CURLcode mqtt_publish(সংরচনা Curl_easy *data)
{
  CURLcode result;
  ৱর্ণ *payload = data->set.postfields;
  size_t payloadlen;
  ৱর্ণ *topic = রিক্ত;
  size_t topiclen;
  unsigned ৱর্ণ *pkt = রিক্ত;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  ৱর্ণ encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  যদি(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }
  যদি(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    যদি(postfieldsize > 0) /* off_t does not fit into size_t */
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  যদি(result)
    গচ্ছ fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  যদি((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    গচ্ছ fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  যদি(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    গচ্ছ fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  যদি(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_বাৰ্তা_PUBLISH;
  memcpy(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  memcpy(&pkt[i], topic, topiclen);
  i += topiclen;
  memcpy(&pkt[i], payload, payloadlen);
  i += payloadlen;
  result = mqtt_send(data, (ধ্রুৱ ৱর্ণ *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  ঘূৰাই result;
}

/* return FALSE on success, TRUE on error */
স্থির bool mqtt_decode_len(size_t *lenp, ধ্রুৱ unsigned ৱর্ণ *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned ৱর্ণ encoded = 128;

  বাবে(i = 0; (i < buflen) && (encoded & 128); i++) {
    যদি(i == 4)
      ঘূৰাই TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  যদি(encoded & 128)
    /* truncated size */
    ঘূৰাই TRUE;

  *lenp = len;
  ঘূৰাই FALSE;
}

#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ statenames[] = {
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
স্থির শূন্য mqstate(সংরচনা Curl_easy *data,
                    গণনা mqttstate state,
                    গণনা mqttstate nextstate) /* used if state == FIRST */
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  যদি(!mqtt)
    ঘূৰাই;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  যদি(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

স্থির CURLcode mqtt_read_publish(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  সংরচনা mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned ৱর্ণ packet;

  DEBUGASSERT(mqtt);
  যদি(!mqtt || !mq)
    ঘূৰাই CURLE_FAILED_INIT;

  ৱিকল্প(mqtt->state) {
MQTT_SUBACK_COMING:
  পক্ষ MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    যদি(result)
      ভঙ্গ;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    ভঙ্গ;

  পক্ষ MQTT_SUBACK:
  পক্ষ MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    যদি(packet == MQTT_বাৰ্তা_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    অন্যথা যদি(packet == MQTT_বাৰ্তা_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      গচ্ছ MQTT_SUBACK_COMING;
    }
    অন্যথা যদি(packet == MQTT_বাৰ্তা_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      গচ্ছ end;
    }
    অন্যথা {
      result = CURLE_WEIRD_SERVER_REPLY;
      গচ্ছ end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    যদি(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      গচ্ছ end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  পক্ষ MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    ৱর্ণ buffer[4 * 1024];
    size_t rest = mq->npacket;
    যদি(rest > পরিমাণ(buffer))
      rest = পরিমাণ(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    যদি(result) {
      যদি(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      গচ্ছ end;
    }
    যদি(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      গচ্ছ end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    যদি(result)
      গচ্ছ end;

    mq->npacket -= nread;
    যদি(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    ভঙ্গ;
  }
  স্ৱভাৱ:
    DEBUGASSERT(রিক্ত); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    গচ্ছ end;
  }
end:
  ঘূৰাই result;
}

স্থির CURLcode mqtt_do(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  যদি(!mq)
    ঘূৰাই CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  যদি(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (পূর্ণাঙ্ক)result);
    ঘূৰাই result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode mqtt_done(সংরচনা Curl_easy *data,
                          CURLcode status, bool premature)
{
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (শূন্য)status;
  (শূন্য)premature;
  যদি(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  ঘূৰাই CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
স্থির CURLcode mqtt_ping(সংরচনা Curl_easy *data)
{
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  যদি(!mqtt || !mq)
    ঘূৰাই CURLE_FAILED_INIT;

  যদি(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    সংরচনা curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    যদি(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned ৱর্ণ packet[2] = { 0xC0, 0x00 };
      size_t packetlen = পরিমাণ(packet);

      result = mqtt_send(data, (ৱর্ণ *)packet, packetlen);
      যদি(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  ঘূৰাই result;
}

স্থির CURLcode mqtt_doing(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned ৱর্ণ recvbyte;
  সংরচনা mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  যদি(!mqtt || !mq)
    ঘূৰাই CURLE_FAILED_INIT;

  *done = FALSE;

  যদি(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    যদি(result || curlx_dyn_len(&mq->sendbuf))
      ঘূৰাই result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  যদি(result || curlx_dyn_len(&mq->sendbuf))
    ঘূৰাই result;

  infof(data, "mqtt_doing: state [%d]", (পূর্ণাঙ্ক)mqtt->state);
  ৱিকল্প(mqtt->state) {
  পক্ষ MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (ৱর্ণ *)&mq->firstbyte, 1, &nread);
    যদি(result)
      ভঙ্গ;
    অন্যথা যদি(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      ভঙ্গ;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (ধ্রুৱ ৱর্ণ *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  পক্ষ MQTT_REMAINING_LENGTH:
    কৰা {
      result = Curl_xfer_recv(data, (ৱর্ণ *)&recvbyte, 1, &nread);
      যদি(result || !nread)
        ভঙ্গ;
      Curl_debug(data, CURLINFO_HEADER_IN, (ধ্রুৱ ৱর্ণ *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } যেতিয়ালৈ((recvbyte & 0x80) && (mq->npacket < 4));
    যদি(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    যদি(result)
      ভঙ্গ;
    যদি(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      ভঙ্গ;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ধ্রুৱ unsigned ৱর্ণ type = mq->firstbyte & 0xF0;
      ধ্রুৱ unsigned ৱর্ণ reserved = mq->firstbyte & 0x0F;
      যদি((type == MQTT_বাৰ্তা_DISCONNECT || type == MQTT_বাৰ্তা_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_বাৰ্তা_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        ভঙ্গ;
      }
    }
    যদি(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      ভঙ্গ;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    যদি(mq->firstbyte == MQTT_বাৰ্তা_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    যদি(mq->firstbyte == MQTT_বাৰ্তা_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    ভঙ্গ;
  পক্ষ MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    যদি(result)
      ভঙ্গ;

    যদি(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      যদি(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      ভঙ্গ;
    }

    result = mqtt_publish(data);
    যদি(result)
      ভঙ্গ;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  পক্ষ MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    যদি(curlx_dyn_len(&mq->sendbuf))
      ভঙ্গ;
    result = mqtt_disconnect(data);
    যদি(result)
      ভঙ্গ;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  পক্ষ MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    যদি(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    ভঙ্গ;

  পক্ষ MQTT_SUBACK:
  পক্ষ MQTT_PUBWAIT:
  পক্ষ MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    ভঙ্গ;

  স্ৱভাৱ:
    failf(data, "State not handled yet");
    *done = TRUE;
    ভঙ্গ;
  }

  যদি(result == CURLE_AGAIN)
    result = CURLE_OK;
  ঘূৰাই result;
}

#ifdef USE_SSL

স্থির CURLcode mqtts_connecting(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  যদি(result)
    connclose(conn);
  ঘূৰাই result;
}

/*
 * MQTTS protocol.
 */
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_mqtts = {
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
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_mqtt = {
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
