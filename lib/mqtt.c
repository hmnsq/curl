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
#ضمن "curl_setup.h"
#ضمن "urldata.h"

#ifndef CURL_DISABLE_MQTT

#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "mqtt.h"
#ضمن "select.h"
#ضمن "url.h"
#ضمن "escape.h"
#ضمن "rand.h"
#ضمن "cfilters.h"
#ضمن "connect.h"

/* first byte is command.
   second byte is for flags. */
#define MQTT_MSG_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_MSG_PUBLISH    0x30
#define MQTT_MSG_SUBSCRIBE  0x82
#define MQTT_MSG_SUBACK     0x90
#define MQTT_MSG_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_MSG_PINGRESP   0xD0

#define MQTT_CONNACK_LEN  2
#define MQTT_SUBACK_LEN   3
#define MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#define CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

تعداد mqttstate {
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

بنية mqtt_conn {
  تعداد mqttstate state;
  تعداد mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned صحيح packetid;
};

/* protocol-specific transfer-related data */
بنية MQTT {
  بنية dynbuf sendbuf;
  /* when receiving */
  بنية dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned حرف pkt_hd[4]; /* for decoding the arriving packet length */
  بنية curltime lastTime; /* last time we sent or received data */
  unsigned حرف firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

ساكن فراغ mqtt_easy_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية MQTT *mq = entry;
  (فراغ)key;
  (فراغ)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

ساكن فراغ mqtt_conn_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  (فراغ)key;
  (فراغ)klen;
  curlx_free(entry);
}

ساكن CURLcode mqtt_setup_conn(بنية Curl_easy *data,
                                بنية connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  بنية mqtt_conn *mqtt;
  بنية MQTT *mq;

  mqtt = curlx_calloc(1, حجم(*mqtt));
  אם(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    החזר CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, حجم(بنية MQTT));
  אם(!mq)
    החזר CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  אם(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    החזר CURLE_OUT_OF_MEMORY;
  החזר CURLE_OK;
}

ساكن CURLcode mqtt_send(بنية Curl_easy *data,
                          ثابت حرف *buf, size_t len)
{
  size_t n;
  CURLcode result;
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  אם(!mq)
    החזר CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  אם(result)
    החזר result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  אם(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  החזר result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
ساكن CURLcode mqtt_flush(بنية Curl_easy *data)
{
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  אם(!mq)
    החזר CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  אם(!len)
    החזר CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  אם(result)
    החזר result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  החזר curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
ساكن CURLcode mqtt_pollset(بنية Curl_easy *data,
                             بنية easy_pollset *ps)
{
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  אם(mq && curlx_dyn_len(&mq->sendbuf))
    החזר Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  החזר Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

ساكن صحيح mqtt_encode_len(حرف *buf, size_t len)
{
  صحيح i;

  עבור(i = 0; (len > 0) && (i < 4); i++) {
    unsigned حرف encoded;
    encoded = len % 0x80;
    len /= 0x80;
    אם(len)
      encoded |= 0x80;
    buf[i] = (حرف)encoded;
  }

  החזר i;
}

/* add the passwd to the CONNECT packet */
ساكن صحيح add_passwd(ثابت حرف *passwd, ثابت size_t plen,
                      حرف *pkt, ثابت size_t start, صحيح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  אם(plen > 0xffff)
    החזר 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (حرف)((plen >> 8) & 0xFF);
  pkt[start + 1] = (حرف)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  החזר 0;
}

/* add user to the CONNECT packet */
ساكن صحيح add_user(ثابت حرف *username, ثابت size_t ulen,
                    unsigned حرف *pkt, ثابت size_t start, صحيح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  אם(ulen > 0xffff)
    החזר 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned حرف)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned حرف)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  החזר 0;
}

/* add client ID to the CONNECT packet */
ساكن صحيح add_client_id(ثابت حرف *client_id, ثابت size_t client_id_len,
                         حرف *pkt, ثابت size_t start)
{
  אם(client_id_len != MQTT_CLIENTID_LEN)
    החזר 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  החזר 0;
}

/* Set initial values of CONNECT packet */
ساكن صحيح init_connpack(حرف *packet, حرف *remain, صحيح remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_MSG_CONNECT;
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
  החזר remain_pos + 10;
}

ساكن CURLcode mqtt_connect(بنية Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  صحيح pos = 0;
  صحيح rc = 0;
  /* remain length */
  صحيح remain_pos = 0;
  حرف remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  حرف client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ثابت size_t clen = CURL_CSTRLEN("curl");
  حرف *packet = عدم;

  /* extracting username from request */
  بنية Curl_creds *creds = data->state.creds;
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
  אם(packetlen > 0xFFFFFFF)
    החזר CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  אם(!packet)
    החזר CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned حرف *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  אם(rc) {
    failf(data, "Client ID length mismatched: [%zu]", strlen(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    اذهب end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  אם(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned حرف *)packet, start_user, remain_pos);
    אם(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      اذهب end;
    }
  }

  /* if passwd was provided, add it to the packet */
  אם(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    אם(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      اذهب end;
    }
  }

  אם(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  אם(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  החזר result;
}

ساكن CURLcode mqtt_disconnect(بنية Curl_easy *data)
{
  החזר mqtt_send(data, "\xe0\x00", 2);
}

ساكن CURLcode mqtt_recv_atleast(بنية Curl_easy *data, size_t nbytes)
{
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  אם(!mq)
    החזר CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  אם(rlen < nbytes) {
    unsigned حرف readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < حجم(readbuf));
    result = Curl_xfer_recv(data, (حرف *)readbuf, nbytes - rlen, &nread);
    אם(result)
      החזר result;
    אם(!nread) /* EOF */
       החזר CURLE_RECV_ERROR;
    אם(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      החזר CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  החזר (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

ساكن فراغ mqtt_recv_consume(بنية Curl_easy *data, size_t nbytes)
{
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  אם(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    אם(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    אחרת
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

ساكن CURLcode mqtt_verify_connack(بنية Curl_easy *data)
{
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  ثابت حرف *ptr;

  DEBUGASSERT(mq);
  אם(!mq)
    החזר CURLE_FAILED_INIT;
  אם(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    החזר CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  אם(result)
    החזר result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  אם(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned حرف)ptr[0], (unsigned حرف)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    החזר CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  החזר CURLE_OK;
}

ساكن CURLcode mqtt_get_topic(بنية Curl_easy *data,
                               حرف **topic, size_t *topiclen)
{
  ثابت حرف *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  אם(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    אם(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  אחרת
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  החזר result;
}

ساكن CURLcode mqtt_subscribe(بنية Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  حرف *topic = عدم;
  size_t topiclen;
  unsigned حرف *packet = عدم;
  size_t packetlen;
  حرف encodedsize[4];
  size_t n;
  بنية connectdata *conn = data->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  אם(!mqtt)
    החזר CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  אם(result)
    اذهب fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((حرف *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  אם(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail;
  }

  packet[0] = MQTT_MSG_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (ثابت حرف *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  החזר result;
}

/*
 * Called when the first byte was already read.
 */
ساكن CURLcode mqtt_verify_suback(بنية Curl_easy *data)
{
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  بنية connectdata *conn = data->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  ثابت حرف *ptr;

  אם(!mqtt || !mq)
    החזר CURLE_FAILED_INIT;

  אם(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    החזר CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  אם(result)
    اذهب fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  אם(((unsigned حرف)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned حرف)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    اذهب fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  החזר result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

ساكن CURLcode mqtt_publish(بنية Curl_easy *data)
{
  CURLcode result;
  حرف *payload = data->set.postfields;
  size_t payloadlen;
  حرف *topic = عدم;
  size_t topiclen;
  unsigned حرف *pkt = عدم;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  حرف encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  אם(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }
  אם(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    אם(postfieldsize > 0) /* off_t does not fit into size_t */
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  אם(result)
    اذهب fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  אם((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    اذهب fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  אם(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    اذهب fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  אם(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_MSG_PUBLISH;
  memcpy(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  memcpy(&pkt[i], topic, topiclen);
  i += topiclen;
  memcpy(&pkt[i], payload, payloadlen);
  i += payloadlen;
  result = mqtt_send(data, (ثابت حرف *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  החזר result;
}

/* return FALSE on success, TRUE on error */
ساكن bool mqtt_decode_len(size_t *lenp, ثابت unsigned حرف *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned حرف encoded = 128;

  עבור(i = 0; (i < buflen) && (encoded & 128); i++) {
    אם(i == 4)
      החזר TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  אם(encoded & 128)
    /* truncated size */
    החזר TRUE;

  *lenp = len;
  החזר FALSE;
}

#אם defined(DEBUGBUILD) && defined(CURLVERBOSE)
ساكن ثابت حرف * ثابت statenames[] = {
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
ساكن فراغ mqstate(بنية Curl_easy *data,
                    تعداد mqttstate state,
                    تعداد mqttstate nextstate) /* used if state == FIRST */
{
  بنية connectdata *conn = data->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  אם(!mqtt)
    החזר;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  אם(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

ساكن CURLcode mqtt_read_publish(بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned حرف packet;

  DEBUGASSERT(mqtt);
  אם(!mqtt || !mq)
    החזר CURLE_FAILED_INIT;

  بدل(mqtt->state) {
MQTT_SUBACK_COMING:
  حالة MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    אם(result)
      اكسر;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    اكسر;

  حالة MQTT_SUBACK:
  حالة MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    אם(packet == MQTT_MSG_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    אחרת אם(packet == MQTT_MSG_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      اذهب MQTT_SUBACK_COMING;
    }
    אחרת אם(packet == MQTT_MSG_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      اذهب end;
    }
    אחרת {
      result = CURLE_WEIRD_SERVER_REPLY;
      اذهب end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    אם(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      اذهب end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  حالة MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    حرف buffer[4 * 1024];
    size_t rest = mq->npacket;
    אם(rest > حجم(buffer))
      rest = حجم(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    אם(result) {
      אם(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      اذهب end;
    }
    אם(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      اذهب end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    אם(result)
      اذهب end;

    mq->npacket -= nread;
    אם(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    اكسر;
  }
  افتراضي:
    DEBUGASSERT(عدم); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    اذهب end;
  }
end:
  החזר result;
}

ساكن CURLcode mqtt_do(بنية Curl_easy *data, bool *done)
{
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  אם(!mq)
    החזר CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  אם(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (صحيح)result);
    החזר result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  החזר CURLE_OK;
}

ساكن CURLcode mqtt_done(بنية Curl_easy *data,
                          CURLcode status, bool premature)
{
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (فراغ)status;
  (فراغ)premature;
  אם(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  החזר CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
ساكن CURLcode mqtt_ping(بنية Curl_easy *data)
{
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  אם(!mqtt || !mq)
    החזר CURLE_FAILED_INIT;

  אם(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    بنية curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    אם(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned حرف packet[2] = { 0xC0, 0x00 };
      size_t packetlen = حجم(packet);

      result = mqtt_send(data, (حرف *)packet, packetlen);
      אם(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  החזר result;
}

ساكن CURLcode mqtt_doing(بنية Curl_easy *data, bool *done)
{
  بنية MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned حرف recvbyte;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  אם(!mqtt || !mq)
    החזר CURLE_FAILED_INIT;

  *done = FALSE;

  אם(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    אם(result || curlx_dyn_len(&mq->sendbuf))
      החזר result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  אם(result || curlx_dyn_len(&mq->sendbuf))
    החזר result;

  infof(data, "mqtt_doing: state [%d]", (صحيح)mqtt->state);
  بدل(mqtt->state) {
  حالة MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (حرف *)&mq->firstbyte, 1, &nread);
    אם(result)
      اكسر;
    אחרת אם(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      اكسر;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (ثابت حرف *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_REMAINING_LENGTH:
    עשה {
      result = Curl_xfer_recv(data, (حرف *)&recvbyte, 1, &nread);
      אם(result || !nread)
        اكسر;
      Curl_debug(data, CURLINFO_HEADER_IN, (ثابت حرف *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } כלעוד((recvbyte & 0x80) && (mq->npacket < 4));
    אם(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    אם(result)
      اكسر;
    אם(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      اكسر;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ثابت unsigned حرف type = mq->firstbyte & 0xF0;
      ثابت unsigned حرف reserved = mq->firstbyte & 0x0F;
      אם((type == MQTT_MSG_DISCONNECT || type == MQTT_MSG_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_MSG_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        اكسر;
      }
    }
    אם(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      اكسر;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    אם(mq->firstbyte == MQTT_MSG_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    אם(mq->firstbyte == MQTT_MSG_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    اكسر;
  حالة MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    אם(result)
      اكسر;

    אם(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      אם(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      اكسر;
    }

    result = mqtt_publish(data);
    אם(result)
      اكسر;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    אם(curlx_dyn_len(&mq->sendbuf))
      اكسر;
    result = mqtt_disconnect(data);
    אם(result)
      اكسر;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    אם(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    اكسر;

  حالة MQTT_SUBACK:
  حالة MQTT_PUBWAIT:
  حالة MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    اكسر;

  افتراضي:
    failf(data, "State not handled yet");
    *done = TRUE;
    اكسر;
  }

  אם(result == CURLE_AGAIN)
    result = CURLE_OK;
  החזר result;
}

#ifdef USE_SSL

ساكن CURLcode mqtts_connecting(بنية Curl_easy *data, bool *done)
{
  بنية connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  אם(result)
    connclose(conn);
  החזר result;
}

/*
 * MQTTS protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_mqtts = {
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
ثابت بنية Curl_protocol Curl_protocol_mqtt = {
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
