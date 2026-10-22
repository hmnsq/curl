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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"

#ifndef CURL_DISABLE_MQTT

#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "mqtt.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"

/* first byte is command.
   second byte is for flags. */
#define MQTT_செய்தி_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_செய்தி_PUBLISH    0x30
#define MQTT_செய்தி_SUBSCRIBE  0x82
#define MQTT_செய்தி_SUBACK     0x90
#define MQTT_செய்தி_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_செய்தி_PINGRESP   0xD0

#define MQTT_CONNACK_LEN  2
#define MQTT_SUBACK_LEN   3
#define MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#define CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

எண்ணிக்கை mqttstate {
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

கட்டமைப்பு mqtt_conn {
  எண்ணிக்கை mqttstate state;
  எண்ணிக்கை mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned முழு packetid;
};

/* protocol-specific transfer-related data */
கட்டமைப்பு MQTT {
  கட்டமைப்பு dynbuf sendbuf;
  /* when receiving */
  கட்டமைப்பு dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned எழுத்து pkt_hd[4]; /* for decoding the arriving packet length */
  கட்டமைப்பு curltime lastTime; /* last time we sent or received data */
  unsigned எழுத்து firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

நிலையான வெறுமை mqtt_easy_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு MQTT *mq = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

நிலையான வெறுமை mqtt_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  curlx_free(entry);
}

நிலையான CURLcode mqtt_setup_conn(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  கட்டமைப்பு mqtt_conn *mqtt;
  கட்டமைப்பு MQTT *mq;

  mqtt = curlx_calloc(1, அளவு(*mqtt));
  எனில்(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, அளவு(கட்டமைப்பு MQTT));
  எனில்(!mq)
    திரும்பு CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  எனில்(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode mqtt_send(கட்டமைப்பு Curl_easy *data,
                          மாறிலி எழுத்து *buf, size_t len)
{
  size_t n;
  CURLcode result;
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  எனில்(!mq)
    திரும்பு CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  எனில்(result)
    திரும்பு result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  எனில்(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  திரும்பு result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
நிலையான CURLcode mqtt_flush(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  எனில்(!mq)
    திரும்பு CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  எனில்(!len)
    திரும்பு CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  எனில்(result)
    திரும்பு result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  திரும்பு curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
நிலையான CURLcode mqtt_pollset(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  எனில்(mq && curlx_dyn_len(&mq->sendbuf))
    திரும்பு Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  திரும்பு Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

நிலையான முழு mqtt_encode_len(எழுத்து *buf, size_t len)
{
  முழு i;

  க்கு(i = 0; (len > 0) && (i < 4); i++) {
    unsigned எழுத்து encoded;
    encoded = len % 0x80;
    len /= 0x80;
    எனில்(len)
      encoded |= 0x80;
    buf[i] = (எழுத்து)encoded;
  }

  திரும்பு i;
}

/* add the passwd to the CONNECT packet */
நிலையான முழு add_passwd(மாறிலி எழுத்து *passwd, மாறிலி size_t plen,
                      எழுத்து *pkt, மாறிலி size_t start, முழு remain_pos)
{
  /* magic number that need to be set properly */
  மாறிலி size_t conn_flags_pos = remain_pos + 8;
  எனில்(plen > 0xffff)
    திரும்பு 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (எழுத்து)((plen >> 8) & 0xFF);
  pkt[start + 1] = (எழுத்து)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  திரும்பு 0;
}

/* add user to the CONNECT packet */
நிலையான முழு add_user(மாறிலி எழுத்து *username, மாறிலி size_t ulen,
                    unsigned எழுத்து *pkt, மாறிலி size_t start, முழு remain_pos)
{
  /* magic number that need to be set properly */
  மாறிலி size_t conn_flags_pos = remain_pos + 8;
  எனில்(ulen > 0xffff)
    திரும்பு 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned எழுத்து)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned எழுத்து)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  திரும்பு 0;
}

/* add client ID to the CONNECT packet */
நிலையான முழு add_client_id(மாறிலி எழுத்து *client_id, மாறிலி size_t client_id_len,
                         எழுத்து *pkt, மாறிலி size_t start)
{
  எனில்(client_id_len != MQTT_CLIENTID_LEN)
    திரும்பு 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  திரும்பு 0;
}

/* Set initial values of CONNECT packet */
நிலையான முழு init_connpack(எழுத்து *packet, எழுத்து *remain, முழு remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_செய்தி_CONNECT;
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
  திரும்பு remain_pos + 10;
}

நிலையான CURLcode mqtt_connect(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  முழு pos = 0;
  முழு rc = 0;
  /* remain length */
  முழு remain_pos = 0;
  எழுத்து remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  எழுத்து client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  மாறிலி size_t clen = CURL_CSTRLEN("curl");
  எழுத்து *packet = வெற்று;

  /* extracting username from request */
  கட்டமைப்பு Curl_creds *creds = data->state.creds;
  மாறிலி size_t ulen = creds ? strlen(creds->user) : 0;
  மாறிலி size_t plen = creds ? strlen(creds->passwd) : 0;
  மாறிலி size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  எனில்(packetlen > 0xFFFFFFF)
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  எனில்(!packet)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned எழுத்து *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  எனில்(rc) {
    failf(data, "Client ID length mismatched: [%zu]", strlen(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    செல் end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  எனில்(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned எழுத்து *)packet, start_user, remain_pos);
    எனில்(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      செல் end;
    }
  }

  /* if passwd was provided, add it to the packet */
  எனில்(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    எனில்(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      செல் end;
    }
  }

  எனில்(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  எனில்(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  திரும்பு result;
}

நிலையான CURLcode mqtt_disconnect(கட்டமைப்பு Curl_easy *data)
{
  திரும்பு mqtt_send(data, "\xe0\x00", 2);
}

நிலையான CURLcode mqtt_recv_atleast(கட்டமைப்பு Curl_easy *data, size_t nbytes)
{
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  எனில்(!mq)
    திரும்பு CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  எனில்(rlen < nbytes) {
    unsigned எழுத்து readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < அளவு(readbuf));
    result = Curl_xfer_recv(data, (எழுத்து *)readbuf, nbytes - rlen, &nread);
    எனில்(result)
      திரும்பு result;
    எனில்(!nread) /* EOF */
       திரும்பு CURLE_RECV_ERROR;
    எனில்(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      திரும்பு CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  திரும்பு (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

நிலையான வெறுமை mqtt_recv_consume(கட்டமைப்பு Curl_easy *data, size_t nbytes)
{
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  எனில்(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    எனில்(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    இல்லையெனில்
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

நிலையான CURLcode mqtt_verify_connack(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  மாறிலி எழுத்து *ptr;

  DEBUGASSERT(mq);
  எனில்(!mq)
    திரும்பு CURLE_FAILED_INIT;
  எனில்(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  எனில்(result)
    திரும்பு result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  எனில்(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned எழுத்து)ptr[0], (unsigned எழுத்து)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode mqtt_get_topic(கட்டமைப்பு Curl_easy *data,
                               எழுத்து **topic, size_t *topiclen)
{
  மாறிலி எழுத்து *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  எனில்(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    எனில்(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  இல்லையெனில்
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  திரும்பு result;
}

நிலையான CURLcode mqtt_subscribe(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  எழுத்து *topic = வெற்று;
  size_t topiclen;
  unsigned எழுத்து *packet = வெற்று;
  size_t packetlen;
  எழுத்து encodedsize[4];
  size_t n;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  எனில்(!mqtt)
    திரும்பு CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  எனில்(result)
    செல் fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((எழுத்து *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  எனில்(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    செல் fail;
  }

  packet[0] = MQTT_செய்தி_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (மாறிலி எழுத்து *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  திரும்பு result;
}

/*
 * Called when the first byte was already read.
 */
நிலையான CURLcode mqtt_verify_suback(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  மாறிலி எழுத்து *ptr;

  எனில்(!mqtt || !mq)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  எனில்(result)
    செல் fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  எனில்(((unsigned எழுத்து)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned எழுத்து)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    செல் fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  திரும்பு result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

நிலையான CURLcode mqtt_publish(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result;
  எழுத்து *payload = data->set.postfields;
  size_t payloadlen;
  எழுத்து *topic = வெற்று;
  size_t topiclen;
  unsigned எழுத்து *pkt = வெற்று;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  எழுத்து encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  எனில்(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
  எனில்(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    எனில்(postfieldsize > 0) /* off_t does not fit into size_t */
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  எனில்(result)
    செல் fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  எனில்((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    செல் fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  எனில்(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    செல் fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  எனில்(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    செல் fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_செய்தி_PUBLISH;
  memcpy(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  memcpy(&pkt[i], topic, topiclen);
  i += topiclen;
  memcpy(&pkt[i], payload, payloadlen);
  i += payloadlen;
  result = mqtt_send(data, (மாறிலி எழுத்து *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  திரும்பு result;
}

/* return FALSE on success, TRUE on error */
நிலையான bool mqtt_decode_len(size_t *lenp, மாறிலி unsigned எழுத்து *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned எழுத்து encoded = 128;

  க்கு(i = 0; (i < buflen) && (encoded & 128); i++) {
    எனில்(i == 4)
      திரும்பு TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  எனில்(encoded & 128)
    /* truncated size */
    திரும்பு TRUE;

  *lenp = len;
  திரும்பு FALSE;
}

#எனில் defined(DEBUGBUILD) && defined(CURLVERBOSE)
நிலையான மாறிலி எழுத்து * மாறிலி statenames[] = {
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
நிலையான வெறுமை mqstate(கட்டமைப்பு Curl_easy *data,
                    எண்ணிக்கை mqttstate state,
                    எண்ணிக்கை mqttstate nextstate) /* used if state == FIRST */
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  எனில்(!mqtt)
    திரும்பு;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  எனில்(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

நிலையான CURLcode mqtt_read_publish(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  கட்டமைப்பு mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned எழுத்து packet;

  DEBUGASSERT(mqtt);
  எனில்(!mqtt || !mq)
    திரும்பு CURLE_FAILED_INIT;

  தேர்வு(mqtt->state) {
MQTT_SUBACK_COMING:
  நிலை MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    எனில்(result)
      நிறுத்து;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    நிறுத்து;

  நிலை MQTT_SUBACK:
  நிலை MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    எனில்(packet == MQTT_செய்தி_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    இல்லையெனில் எனில்(packet == MQTT_செய்தி_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      செல் MQTT_SUBACK_COMING;
    }
    இல்லையெனில் எனில்(packet == MQTT_செய்தி_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      செல் end;
    }
    இல்லையெனில் {
      result = CURLE_WEIRD_SERVER_REPLY;
      செல் end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    எனில்(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      செல் end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  நிலை MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    எழுத்து buffer[4 * 1024];
    size_t rest = mq->npacket;
    எனில்(rest > அளவு(buffer))
      rest = அளவு(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    எனில்(result) {
      எனில்(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      செல் end;
    }
    எனில்(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      செல் end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    எனில்(result)
      செல் end;

    mq->npacket -= nread;
    எனில்(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    நிறுத்து;
  }
  இயல்பு:
    DEBUGASSERT(வெற்று); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    செல் end;
  }
end:
  திரும்பு result;
}

நிலையான CURLcode mqtt_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  எனில்(!mq)
    திரும்பு CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  எனில்(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (முழு)result);
    திரும்பு result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode mqtt_done(கட்டமைப்பு Curl_easy *data,
                          CURLcode status, bool premature)
{
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (வெறுமை)status;
  (வெறுமை)premature;
  எனில்(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  திரும்பு CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
நிலையான CURLcode mqtt_ping(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  எனில்(!mqtt || !mq)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    கட்டமைப்பு curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    எனில்(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned எழுத்து packet[2] = { 0xC0, 0x00 };
      size_t packetlen = அளவு(packet);

      result = mqtt_send(data, (எழுத்து *)packet, packetlen);
      எனில்(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  திரும்பு result;
}

நிலையான CURLcode mqtt_doing(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned எழுத்து recvbyte;
  கட்டமைப்பு mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  எனில்(!mqtt || !mq)
    திரும்பு CURLE_FAILED_INIT;

  *done = FALSE;

  எனில்(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    எனில்(result || curlx_dyn_len(&mq->sendbuf))
      திரும்பு result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  எனில்(result || curlx_dyn_len(&mq->sendbuf))
    திரும்பு result;

  infof(data, "mqtt_doing: state [%d]", (முழு)mqtt->state);
  தேர்வு(mqtt->state) {
  நிலை MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (எழுத்து *)&mq->firstbyte, 1, &nread);
    எனில்(result)
      நிறுத்து;
    இல்லையெனில் எனில்(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      நிறுத்து;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (மாறிலி எழுத்து *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  நிலை MQTT_REMAINING_LENGTH:
    செய் {
      result = Curl_xfer_recv(data, (எழுத்து *)&recvbyte, 1, &nread);
      எனில்(result || !nread)
        நிறுத்து;
      Curl_debug(data, CURLINFO_HEADER_IN, (மாறிலி எழுத்து *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } போது((recvbyte & 0x80) && (mq->npacket < 4));
    எனில்(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    எனில்(result)
      நிறுத்து;
    எனில்(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      நிறுத்து;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      மாறிலி unsigned எழுத்து type = mq->firstbyte & 0xF0;
      மாறிலி unsigned எழுத்து reserved = mq->firstbyte & 0x0F;
      எனில்((type == MQTT_செய்தி_DISCONNECT || type == MQTT_செய்தி_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_செய்தி_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        நிறுத்து;
      }
    }
    எனில்(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      நிறுத்து;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    எனில்(mq->firstbyte == MQTT_செய்தி_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    எனில்(mq->firstbyte == MQTT_செய்தி_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    நிறுத்து;
  நிலை MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    எனில்(result)
      நிறுத்து;

    எனில்(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      எனில்(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      நிறுத்து;
    }

    result = mqtt_publish(data);
    எனில்(result)
      நிறுத்து;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  நிலை MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    எனில்(curlx_dyn_len(&mq->sendbuf))
      நிறுத்து;
    result = mqtt_disconnect(data);
    எனில்(result)
      நிறுத்து;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  நிலை MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    எனில்(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    நிறுத்து;

  நிலை MQTT_SUBACK:
  நிலை MQTT_PUBWAIT:
  நிலை MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    நிறுத்து;

  இயல்பு:
    failf(data, "State not handled yet");
    *done = TRUE;
    நிறுத்து;
  }

  எனில்(result == CURLE_AGAIN)
    result = CURLE_OK;
  திரும்பு result;
}

#ifdef USE_SSL

நிலையான CURLcode mqtts_connecting(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  எனில்(result)
    connclose(conn);
  திரும்பு result;
}

/*
 * MQTTS protocol.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_mqtts = {
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
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_mqtt = {
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
