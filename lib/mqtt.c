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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"

#ifndef CURL_DISABLE_MQTT

#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "mqtt.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"

/* first byte is command.
   second byte is for flags. */
#define MQTT_સંદેશ_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_સંદેશ_PUBLISH    0x30
#define MQTT_સંદેશ_SUBSCRIBE  0x82
#define MQTT_સંદેશ_SUBACK     0x90
#define MQTT_સંદેશ_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_સંદેશ_PINGRESP   0xD0

#define MQTT_CONNACK_LEN  2
#define MQTT_SUBACK_LEN   3
#define MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#define CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

ગણના mqttstate {
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

સંરચના mqtt_conn {
  ગણના mqttstate state;
  ગણના mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned પૂર્ણાંક packetid;
};

/* protocol-specific transfer-related data */
સંરચના MQTT {
  સંરચના dynbuf sendbuf;
  /* when receiving */
  સંરચના dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned અક્ષર pkt_hd[4]; /* for decoding the arriving packet length */
  સંરચના curltime lastTime; /* last time we sent or received data */
  unsigned અક્ષર firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

સ્થિર શૂન્ય mqtt_easy_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના MQTT *mq = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

સ્થિર શૂન્ય mqtt_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  curlx_free(entry);
}

સ્થિર CURLcode mqtt_setup_conn(સંરચના Curl_easy *data,
                                સંરચના connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  સંરચના mqtt_conn *mqtt;
  સંરચના MQTT *mq;

  mqtt = curlx_calloc(1, કદ(*mqtt));
  જો(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, કદ(સંરચના MQTT));
  જો(!mq)
    પાછા CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  જો(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    પાછા CURLE_OUT_OF_MEMORY;
  પાછા CURLE_OK;
}

સ્થિર CURLcode mqtt_send(સંરચના Curl_easy *data,
                          અચલ અક્ષર *buf, size_t len)
{
  size_t n;
  CURLcode result;
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  જો(!mq)
    પાછા CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  જો(result)
    પાછા result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  જો(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  પાછા result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
સ્થિર CURLcode mqtt_flush(સંરચના Curl_easy *data)
{
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  જો(!mq)
    પાછા CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  જો(!len)
    પાછા CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  જો(result)
    પાછા result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  પાછા curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
સ્થિર CURLcode mqtt_pollset(સંરચના Curl_easy *data,
                             સંરચના easy_pollset *ps)
{
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  જો(mq && curlx_dyn_len(&mq->sendbuf))
    પાછા Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  પાછા Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

સ્થિર પૂર્ણાંક mqtt_encode_len(અક્ષર *buf, size_t len)
{
  પૂર્ણાંક i;

  માટે(i = 0; (len > 0) && (i < 4); i++) {
    unsigned અક્ષર encoded;
    encoded = len % 0x80;
    len /= 0x80;
    જો(len)
      encoded |= 0x80;
    buf[i] = (અક્ષર)encoded;
  }

  પાછા i;
}

/* add the passwd to the CONNECT packet */
સ્થિર પૂર્ણાંક add_passwd(અચલ અક્ષર *passwd, અચલ size_t plen,
                      અક્ષર *pkt, અચલ size_t start, પૂર્ણાંક remain_pos)
{
  /* magic number that need to be set properly */
  અચલ size_t conn_flags_pos = remain_pos + 8;
  જો(plen > 0xffff)
    પાછા 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (અક્ષર)((plen >> 8) & 0xFF);
  pkt[start + 1] = (અક્ષર)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  પાછા 0;
}

/* add user to the CONNECT packet */
સ્થિર પૂર્ણાંક add_user(અચલ અક્ષર *username, અચલ size_t ulen,
                    unsigned અક્ષર *pkt, અચલ size_t start, પૂર્ણાંક remain_pos)
{
  /* magic number that need to be set properly */
  અચલ size_t conn_flags_pos = remain_pos + 8;
  જો(ulen > 0xffff)
    પાછા 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned અક્ષર)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned અક્ષર)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  પાછા 0;
}

/* add client ID to the CONNECT packet */
સ્થિર પૂર્ણાંક add_client_id(અચલ અક્ષર *client_id, અચલ size_t client_id_len,
                         અક્ષર *pkt, અચલ size_t start)
{
  જો(client_id_len != MQTT_CLIENTID_LEN)
    પાછા 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  પાછા 0;
}

/* Set initial values of CONNECT packet */
સ્થિર પૂર્ણાંક init_connpack(અક્ષર *packet, અક્ષર *remain, પૂર્ણાંક remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_સંદેશ_CONNECT;
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
  પાછા remain_pos + 10;
}

સ્થિર CURLcode mqtt_connect(સંરચના Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  પૂર્ણાંક pos = 0;
  પૂર્ણાંક rc = 0;
  /* remain length */
  પૂર્ણાંક remain_pos = 0;
  અક્ષર remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  અક્ષર client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  અચલ size_t clen = CURL_CSTRLEN("curl");
  અક્ષર *packet = ખાલી;

  /* extracting username from request */
  સંરચના Curl_creds *creds = data->state.creds;
  અચલ size_t ulen = creds ? strlen(creds->user) : 0;
  અચલ size_t plen = creds ? strlen(creds->passwd) : 0;
  અચલ size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  જો(packetlen > 0xFFFFFFF)
    પાછા CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  જો(!packet)
    પાછા CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned અક્ષર *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  જો(rc) {
    failf(data, "Client ID length mismatched: [%zu]", strlen(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    જાઓ end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  જો(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned અક્ષર *)packet, start_user, remain_pos);
    જો(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      જાઓ end;
    }
  }

  /* if passwd was provided, add it to the packet */
  જો(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    જો(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      જાઓ end;
    }
  }

  જો(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  જો(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  પાછા result;
}

સ્થિર CURLcode mqtt_disconnect(સંરચના Curl_easy *data)
{
  પાછા mqtt_send(data, "\xe0\x00", 2);
}

સ્થિર CURLcode mqtt_recv_atleast(સંરચના Curl_easy *data, size_t nbytes)
{
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  જો(!mq)
    પાછા CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  જો(rlen < nbytes) {
    unsigned અક્ષર readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < કદ(readbuf));
    result = Curl_xfer_recv(data, (અક્ષર *)readbuf, nbytes - rlen, &nread);
    જો(result)
      પાછા result;
    જો(!nread) /* EOF */
       પાછા CURLE_RECV_ERROR;
    જો(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      પાછા CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  પાછા (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

સ્થિર શૂન્ય mqtt_recv_consume(સંરચના Curl_easy *data, size_t nbytes)
{
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  જો(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    જો(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    નહીંતર
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

સ્થિર CURLcode mqtt_verify_connack(સંરચના Curl_easy *data)
{
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  અચલ અક્ષર *ptr;

  DEBUGASSERT(mq);
  જો(!mq)
    પાછા CURLE_FAILED_INIT;
  જો(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    પાછા CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  જો(result)
    પાછા result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  જો(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned અક્ષર)ptr[0], (unsigned અક્ષર)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    પાછા CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  પાછા CURLE_OK;
}

સ્થિર CURLcode mqtt_get_topic(સંરચના Curl_easy *data,
                               અક્ષર **topic, size_t *topiclen)
{
  અચલ અક્ષર *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  જો(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    જો(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  નહીંતર
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  પાછા result;
}

સ્થિર CURLcode mqtt_subscribe(સંરચના Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  અક્ષર *topic = ખાલી;
  size_t topiclen;
  unsigned અક્ષર *packet = ખાલી;
  size_t packetlen;
  અક્ષર encodedsize[4];
  size_t n;
  સંરચના connectdata *conn = data->conn;
  સંરચના mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  જો(!mqtt)
    પાછા CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  જો(result)
    જાઓ fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((અક્ષર *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  જો(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ fail;
  }

  packet[0] = MQTT_સંદેશ_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (અચલ અક્ષર *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  પાછા result;
}

/*
 * Called when the first byte was already read.
 */
સ્થિર CURLcode mqtt_verify_suback(સંરચના Curl_easy *data)
{
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  સંરચના connectdata *conn = data->conn;
  સંરચના mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  અચલ અક્ષર *ptr;

  જો(!mqtt || !mq)
    પાછા CURLE_FAILED_INIT;

  જો(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    પાછા CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  જો(result)
    જાઓ fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  જો(((unsigned અક્ષર)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned અક્ષર)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    જાઓ fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  પાછા result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

સ્થિર CURLcode mqtt_publish(સંરચના Curl_easy *data)
{
  CURLcode result;
  અક્ષર *payload = data->set.postfields;
  size_t payloadlen;
  અક્ષર *topic = ખાલી;
  size_t topiclen;
  unsigned અક્ષર *pkt = ખાલી;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  અક્ષર encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  જો(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
  જો(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    જો(postfieldsize > 0) /* off_t does not fit into size_t */
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  જો(result)
    જાઓ fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  જો((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    જાઓ fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  જો(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    જાઓ fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  જો(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_સંદેશ_PUBLISH;
  memcpy(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  memcpy(&pkt[i], topic, topiclen);
  i += topiclen;
  memcpy(&pkt[i], payload, payloadlen);
  i += payloadlen;
  result = mqtt_send(data, (અચલ અક્ષર *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  પાછા result;
}

/* return FALSE on success, TRUE on error */
સ્થિર bool mqtt_decode_len(size_t *lenp, અચલ unsigned અક્ષર *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned અક્ષર encoded = 128;

  માટે(i = 0; (i < buflen) && (encoded & 128); i++) {
    જો(i == 4)
      પાછા TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  જો(encoded & 128)
    /* truncated size */
    પાછા TRUE;

  *lenp = len;
  પાછા FALSE;
}

#જો defined(DEBUGBUILD) && defined(CURLVERBOSE)
સ્થિર અચલ અક્ષર * અચલ statenames[] = {
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
સ્થિર શૂન્ય mqstate(સંરચના Curl_easy *data,
                    ગણના mqttstate state,
                    ગણના mqttstate nextstate) /* used if state == FIRST */
{
  સંરચના connectdata *conn = data->conn;
  સંરચના mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  જો(!mqtt)
    પાછા;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  જો(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

સ્થિર CURLcode mqtt_read_publish(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  સંરચના mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned અક્ષર packet;

  DEBUGASSERT(mqtt);
  જો(!mqtt || !mq)
    પાછા CURLE_FAILED_INIT;

  પસંદગી(mqtt->state) {
MQTT_SUBACK_COMING:
  સ્થિતિ MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    જો(result)
      તોડો;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    તોડો;

  સ્થિતિ MQTT_SUBACK:
  સ્થિતિ MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    જો(packet == MQTT_સંદેશ_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    નહીંતર જો(packet == MQTT_સંદેશ_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      જાઓ MQTT_SUBACK_COMING;
    }
    નહીંતર જો(packet == MQTT_સંદેશ_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      જાઓ end;
    }
    નહીંતર {
      result = CURLE_WEIRD_SERVER_REPLY;
      જાઓ end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    જો(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      જાઓ end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  સ્થિતિ MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    અક્ષર buffer[4 * 1024];
    size_t rest = mq->npacket;
    જો(rest > કદ(buffer))
      rest = કદ(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    જો(result) {
      જો(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      જાઓ end;
    }
    જો(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      જાઓ end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    જો(result)
      જાઓ end;

    mq->npacket -= nread;
    જો(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    તોડો;
  }
  મૂળભૂત:
    DEBUGASSERT(ખાલી); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    જાઓ end;
  }
end:
  પાછા result;
}

સ્થિર CURLcode mqtt_do(સંરચના Curl_easy *data, bool *done)
{
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  જો(!mq)
    પાછા CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  જો(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (પૂર્ણાંક)result);
    પાછા result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  પાછા CURLE_OK;
}

સ્થિર CURLcode mqtt_done(સંરચના Curl_easy *data,
                          CURLcode status, bool premature)
{
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (શૂન્ય)status;
  (શૂન્ય)premature;
  જો(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  પાછા CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
સ્થિર CURLcode mqtt_ping(સંરચના Curl_easy *data)
{
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  જો(!mqtt || !mq)
    પાછા CURLE_FAILED_INIT;

  જો(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    સંરચના curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    જો(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned અક્ષર packet[2] = { 0xC0, 0x00 };
      size_t packetlen = કદ(packet);

      result = mqtt_send(data, (અક્ષર *)packet, packetlen);
      જો(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  પાછા result;
}

સ્થિર CURLcode mqtt_doing(સંરચના Curl_easy *data, bool *done)
{
  સંરચના MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned અક્ષર recvbyte;
  સંરચના mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  જો(!mqtt || !mq)
    પાછા CURLE_FAILED_INIT;

  *done = FALSE;

  જો(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    જો(result || curlx_dyn_len(&mq->sendbuf))
      પાછા result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  જો(result || curlx_dyn_len(&mq->sendbuf))
    પાછા result;

  infof(data, "mqtt_doing: state [%d]", (પૂર્ણાંક)mqtt->state);
  પસંદગી(mqtt->state) {
  સ્થિતિ MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (અક્ષર *)&mq->firstbyte, 1, &nread);
    જો(result)
      તોડો;
    નહીંતર જો(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      તોડો;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (અચલ અક્ષર *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  સ્થિતિ MQTT_REMAINING_LENGTH:
    કરો {
      result = Curl_xfer_recv(data, (અક્ષર *)&recvbyte, 1, &nread);
      જો(result || !nread)
        તોડો;
      Curl_debug(data, CURLINFO_HEADER_IN, (અચલ અક્ષર *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } જ્યારે((recvbyte & 0x80) && (mq->npacket < 4));
    જો(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    જો(result)
      તોડો;
    જો(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      તોડો;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      અચલ unsigned અક્ષર type = mq->firstbyte & 0xF0;
      અચલ unsigned અક્ષર reserved = mq->firstbyte & 0x0F;
      જો((type == MQTT_સંદેશ_DISCONNECT || type == MQTT_સંદેશ_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_સંદેશ_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        તોડો;
      }
    }
    જો(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      તોડો;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    જો(mq->firstbyte == MQTT_સંદેશ_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    જો(mq->firstbyte == MQTT_સંદેશ_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    તોડો;
  સ્થિતિ MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    જો(result)
      તોડો;

    જો(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      જો(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      તોડો;
    }

    result = mqtt_publish(data);
    જો(result)
      તોડો;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  સ્થિતિ MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    જો(curlx_dyn_len(&mq->sendbuf))
      તોડો;
    result = mqtt_disconnect(data);
    જો(result)
      તોડો;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  સ્થિતિ MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    જો(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    તોડો;

  સ્થિતિ MQTT_SUBACK:
  સ્થિતિ MQTT_PUBWAIT:
  સ્થિતિ MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    તોડો;

  મૂળભૂત:
    failf(data, "State not handled yet");
    *done = TRUE;
    તોડો;
  }

  જો(result == CURLE_AGAIN)
    result = CURLE_OK;
  પાછા result;
}

#ifdef USE_SSL

સ્થિર CURLcode mqtts_connecting(સંરચના Curl_easy *data, bool *done)
{
  સંરચના connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  જો(result)
    connclose(conn);
  પાછા result;
}

/*
 * MQTTS protocol.
 */
અચલ સંરચના Curl_protocol Curl_protocol_mqtts = {
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
અચલ સંરચના Curl_protocol Curl_protocol_mqtt = {
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
