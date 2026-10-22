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
#شامل "curl_setup.h"
#شامل "urldata.h"

#ifndef CURL_DISABLE_MQTT

#شامل "transfer.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "mqtt.h"
#شامل "select.h"
#شامل "url.h"
#شامل "escape.h"
#شامل "rand.h"
#شامل "cfilters.h"
#شامل "connect.h"

/* first byte is command.
   second byte is for flags. */
#define MQTT_پیام_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_پیام_PUBLISH    0x30
#define MQTT_پیام_SUBSCRIBE  0x82
#define MQTT_پیام_SUBACK     0x90
#define MQTT_پیام_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_پیام_PINGRESP   0xD0

#define MQTT_CONNACK_LEN  2
#define MQTT_SUBACK_LEN   3
#define MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#define CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

شمارش mqttstate {
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

ساختار mqtt_conn {
  شمارش mqttstate state;
  شمارش mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned صحیح packetid;
};

/* protocol-specific transfer-related data */
ساختار MQTT {
  ساختار dynbuf sendbuf;
  /* when receiving */
  ساختار dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned نویسه pkt_hd[4]; /* for decoding the arriving packet length */
  ساختار curltime lastTime; /* last time we sent or received data */
  unsigned نویسه firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

ایستا تهی mqtt_easy_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار MQTT *mq = entry;
  (تهی)کلید;
  (تهی)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

ایستا تهی mqtt_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  (تهی)کلید;
  (تهی)klen;
  curlx_free(entry);
}

ایستا CURLcode mqtt_setup_conn(ساختار Curl_easy *data,
                                ساختار connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  ساختار mqtt_conn *mqtt;
  ساختار MQTT *mq;

  mqtt = curlx_calloc(1, اندازه(*mqtt));
  اگر(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, اندازه(ساختار MQTT));
  اگر(!mq)
    بازگشت CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  اگر(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;
  بازگشت CURLE_OK;
}

ایستا CURLcode mqtt_send(ساختار Curl_easy *data,
                          ثابت نویسه *buf, size_t len)
{
  size_t n;
  CURLcode result;
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  اگر(!mq)
    بازگشت CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  اگر(result)
    بازگشت result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  اگر(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  بازگشت result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
ایستا CURLcode mqtt_flush(ساختار Curl_easy *data)
{
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  اگر(!mq)
    بازگشت CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  اگر(!len)
    بازگشت CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  اگر(result)
    بازگشت result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  بازگشت curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
ایستا CURLcode mqtt_pollset(ساختار Curl_easy *data,
                             ساختار easy_pollset *ps)
{
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  اگر(mq && curlx_dyn_len(&mq->sendbuf))
    بازگشت Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  بازگشت Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

ایستا صحیح mqtt_encode_len(نویسه *buf, size_t len)
{
  صحیح i;

  برای(i = 0; (len > 0) && (i < 4); i++) {
    unsigned نویسه encoded;
    encoded = len % 0x80;
    len /= 0x80;
    اگر(len)
      encoded |= 0x80;
    buf[i] = (نویسه)encoded;
  }

  بازگشت i;
}

/* add the passwd to the CONNECT packet */
ایستا صحیح add_passwd(ثابت نویسه *passwd, ثابت size_t plen,
                      نویسه *pkt, ثابت size_t start, صحیح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  اگر(plen > 0xffff)
    بازگشت 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (نویسه)((plen >> 8) & 0xFF);
  pkt[start + 1] = (نویسه)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  بازگشت 0;
}

/* add user to the CONNECT packet */
ایستا صحیح add_user(ثابت نویسه *username, ثابت size_t ulen,
                    unsigned نویسه *pkt, ثابت size_t start, صحیح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  اگر(ulen > 0xffff)
    بازگشت 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned نویسه)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned نویسه)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  بازگشت 0;
}

/* add client ID to the CONNECT packet */
ایستا صحیح add_client_id(ثابت نویسه *client_id, ثابت size_t client_id_len,
                         نویسه *pkt, ثابت size_t start)
{
  اگر(client_id_len != MQTT_CLIENTID_LEN)
    بازگشت 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  بازگشت 0;
}

/* Set initial values of CONNECT packet */
ایستا صحیح init_connpack(نویسه *packet, نویسه *remain, صحیح remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_پیام_CONNECT;
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
  بازگشت remain_pos + 10;
}

ایستا CURLcode mqtt_connect(ساختار Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  صحیح pos = 0;
  صحیح rc = 0;
  /* remain length */
  صحیح remain_pos = 0;
  نویسه remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  نویسه client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ثابت size_t clen = CURL_CSTRLEN("curl");
  نویسه *packet = پوچ;

  /* extracting username from request */
  ساختار Curl_creds *creds = data->state.creds;
  ثابت size_t ulen = creds ? strlen(creds->user) : 0;
  ثابت size_t plen = creds ? strlen(creds->passwd) : 0;
  ثابت size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  اگر(packetlen > 0xFFFFFFF)
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  اگر(!packet)
    بازگشت CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned نویسه *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  اگر(rc) {
    failf(data, "Client ID length mismatched: [%zu]", strlen(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    برو end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  اگر(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned نویسه *)packet, start_user, remain_pos);
    اگر(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      برو end;
    }
  }

  /* if passwd was provided, add it to the packet */
  اگر(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    اگر(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      برو end;
    }
  }

  اگر(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  اگر(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  بازگشت result;
}

ایستا CURLcode mqtt_disconnect(ساختار Curl_easy *data)
{
  بازگشت mqtt_send(data, "\xe0\x00", 2);
}

ایستا CURLcode mqtt_recv_atleast(ساختار Curl_easy *data, size_t nbytes)
{
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  اگر(!mq)
    بازگشت CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  اگر(rlen < nbytes) {
    unsigned نویسه readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < اندازه(readbuf));
    result = Curl_xfer_recv(data, (نویسه *)readbuf, nbytes - rlen, &nread);
    اگر(result)
      بازگشت result;
    اگر(!nread) /* EOF */
       بازگشت CURLE_RECV_ERROR;
    اگر(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      بازگشت CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  بازگشت (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

ایستا تهی mqtt_recv_consume(ساختار Curl_easy *data, size_t nbytes)
{
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  اگر(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    اگر(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    وگرنه
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

ایستا CURLcode mqtt_verify_connack(ساختار Curl_easy *data)
{
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  ثابت نویسه *ptr;

  DEBUGASSERT(mq);
  اگر(!mq)
    بازگشت CURLE_FAILED_INIT;
  اگر(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  اگر(result)
    بازگشت result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  اگر(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned نویسه)ptr[0], (unsigned نویسه)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  بازگشت CURLE_OK;
}

ایستا CURLcode mqtt_get_topic(ساختار Curl_easy *data,
                               نویسه **topic, size_t *topiclen)
{
  ثابت نویسه *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  اگر(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    اگر(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  وگرنه
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  بازگشت result;
}

ایستا CURLcode mqtt_subscribe(ساختار Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  نویسه *topic = پوچ;
  size_t topiclen;
  unsigned نویسه *packet = پوچ;
  size_t packetlen;
  نویسه encodedsize[4];
  size_t n;
  ساختار connectdata *conn = data->conn;
  ساختار mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  اگر(!mqtt)
    بازگشت CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  اگر(result)
    برو fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((نویسه *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  اگر(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    برو fail;
  }

  packet[0] = MQTT_پیام_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (ثابت نویسه *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  بازگشت result;
}

/*
 * Called when the first byte was already read.
 */
ایستا CURLcode mqtt_verify_suback(ساختار Curl_easy *data)
{
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  ساختار connectdata *conn = data->conn;
  ساختار mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  ثابت نویسه *ptr;

  اگر(!mqtt || !mq)
    بازگشت CURLE_FAILED_INIT;

  اگر(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  اگر(result)
    برو fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  اگر(((unsigned نویسه)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned نویسه)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    برو fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  بازگشت result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

ایستا CURLcode mqtt_publish(ساختار Curl_easy *data)
{
  CURLcode result;
  نویسه *payload = data->set.postfields;
  size_t payloadlen;
  نویسه *topic = پوچ;
  size_t topiclen;
  unsigned نویسه *pkt = پوچ;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  نویسه encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  اگر(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }
  اگر(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    اگر(postfieldsize > 0) /* off_t does not fit into size_t */
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  اگر(result)
    برو fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  اگر((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    برو fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  اگر(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    برو fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  اگر(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    برو fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_پیام_PUBLISH;
  memcpy(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  memcpy(&pkt[i], topic, topiclen);
  i += topiclen;
  memcpy(&pkt[i], payload, payloadlen);
  i += payloadlen;
  result = mqtt_send(data, (ثابت نویسه *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  بازگشت result;
}

/* return FALSE on success, TRUE on error */
ایستا bool mqtt_decode_len(size_t *lenp, ثابت unsigned نویسه *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned نویسه encoded = 128;

  برای(i = 0; (i < buflen) && (encoded & 128); i++) {
    اگر(i == 4)
      بازگشت TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  اگر(encoded & 128)
    /* truncated size */
    بازگشت TRUE;

  *lenp = len;
  بازگشت FALSE;
}

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
ایستا ثابت نویسه * ثابت statenames[] = {
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
ایستا تهی mqstate(ساختار Curl_easy *data,
                    شمارش mqttstate state,
                    شمارش mqttstate nextstate) /* used if state == FIRST */
{
  ساختار connectdata *conn = data->conn;
  ساختار mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  اگر(!mqtt)
    بازگشت;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  اگر(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

ایستا CURLcode mqtt_read_publish(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  ساختار mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned نویسه packet;

  DEBUGASSERT(mqtt);
  اگر(!mqtt || !mq)
    بازگشت CURLE_FAILED_INIT;

  انتخاب(mqtt->state) {
MQTT_SUBACK_COMING:
  حالت MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    اگر(result)
      بشکن;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    بشکن;

  حالت MQTT_SUBACK:
  حالت MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    اگر(packet == MQTT_پیام_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    وگرنه اگر(packet == MQTT_پیام_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      برو MQTT_SUBACK_COMING;
    }
    وگرنه اگر(packet == MQTT_پیام_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      برو end;
    }
    وگرنه {
      result = CURLE_WEIRD_SERVER_REPLY;
      برو end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    اگر(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      برو end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  حالت MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    نویسه buffer[4 * 1024];
    size_t rest = mq->npacket;
    اگر(rest > اندازه(buffer))
      rest = اندازه(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    اگر(result) {
      اگر(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      برو end;
    }
    اگر(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      برو end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    اگر(result)
      برو end;

    mq->npacket -= nread;
    اگر(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    بشکن;
  }
  پیشفرض:
    DEBUGASSERT(پوچ); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    برو end;
  }
end:
  بازگشت result;
}

ایستا CURLcode mqtt_do(ساختار Curl_easy *data, bool *done)
{
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  اگر(!mq)
    بازگشت CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  اگر(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (صحیح)result);
    بازگشت result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  بازگشت CURLE_OK;
}

ایستا CURLcode mqtt_done(ساختار Curl_easy *data,
                          CURLcode status, bool premature)
{
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (تهی)status;
  (تهی)premature;
  اگر(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  بازگشت CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
ایستا CURLcode mqtt_ping(ساختار Curl_easy *data)
{
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  اگر(!mqtt || !mq)
    بازگشت CURLE_FAILED_INIT;

  اگر(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    ساختار curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    اگر(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned نویسه packet[2] = { 0xC0, 0x00 };
      size_t packetlen = اندازه(packet);

      result = mqtt_send(data, (نویسه *)packet, packetlen);
      اگر(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  بازگشت result;
}

ایستا CURLcode mqtt_doing(ساختار Curl_easy *data, bool *done)
{
  ساختار MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned نویسه recvbyte;
  ساختار mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  اگر(!mqtt || !mq)
    بازگشت CURLE_FAILED_INIT;

  *done = FALSE;

  اگر(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    اگر(result || curlx_dyn_len(&mq->sendbuf))
      بازگشت result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  اگر(result || curlx_dyn_len(&mq->sendbuf))
    بازگشت result;

  infof(data, "mqtt_doing: state [%d]", (صحیح)mqtt->state);
  انتخاب(mqtt->state) {
  حالت MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (نویسه *)&mq->firstbyte, 1, &nread);
    اگر(result)
      بشکن;
    وگرنه اگر(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      بشکن;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (ثابت نویسه *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  حالت MQTT_REMAINING_LENGTH:
    انجامبده {
      result = Curl_xfer_recv(data, (نویسه *)&recvbyte, 1, &nread);
      اگر(result || !nread)
        بشکن;
      Curl_debug(data, CURLINFO_HEADER_IN, (ثابت نویسه *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } وقتی((recvbyte & 0x80) && (mq->npacket < 4));
    اگر(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    اگر(result)
      بشکن;
    اگر(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      بشکن;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ثابت unsigned نویسه type = mq->firstbyte & 0xF0;
      ثابت unsigned نویسه reserved = mq->firstbyte & 0x0F;
      اگر((type == MQTT_پیام_DISCONNECT || type == MQTT_پیام_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_پیام_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        بشکن;
      }
    }
    اگر(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      بشکن;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    اگر(mq->firstbyte == MQTT_پیام_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    اگر(mq->firstbyte == MQTT_پیام_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    بشکن;
  حالت MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    اگر(result)
      بشکن;

    اگر(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      اگر(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      بشکن;
    }

    result = mqtt_publish(data);
    اگر(result)
      بشکن;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالت MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    اگر(curlx_dyn_len(&mq->sendbuf))
      بشکن;
    result = mqtt_disconnect(data);
    اگر(result)
      بشکن;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالت MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    اگر(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    بشکن;

  حالت MQTT_SUBACK:
  حالت MQTT_PUBWAIT:
  حالت MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    بشکن;

  پیشفرض:
    failf(data, "State not handled yet");
    *done = TRUE;
    بشکن;
  }

  اگر(result == CURLE_AGAIN)
    result = CURLE_OK;
  بازگشت result;
}

#ifdef USE_SSL

ایستا CURLcode mqtts_connecting(ساختار Curl_easy *data, bool *done)
{
  ساختار connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  اگر(result)
    connclose(conn);
  بازگشت result;
}

/*
 * MQTTS protocol.
 */
ثابت ساختار Curl_protocol Curl_protocol_mqtts = {
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
ثابت ساختار Curl_protocol Curl_protocol_mqtt = {
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
