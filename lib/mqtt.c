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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"

#ifndef CURL_DISABLE_MQTT

#अन्तर्भाव "transfer.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "mqtt.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"

/* first byte is command.
   second byte is for flags. */
#define MQTT_सन्देशः_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_सन्देशः_PUBLISH    0x30
#define MQTT_सन्देशः_SUBSCRIBE  0x82
#define MQTT_सन्देशः_SUBACK     0x90
#define MQTT_सन्देशः_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_सन्देशः_PINGRESP   0xD0

#define MQTT_CONNACK_LEN  2
#define MQTT_SUBACK_LEN   3
#define MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#define CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

गणना mqttstate {
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

संरचना mqtt_conn {
  गणना mqttstate state;
  गणना mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned पूर्णाङ्क packetid;
};

/* protocol-specific transfer-related data */
संरचना MQTT {
  संरचना dynbuf sendbuf;
  /* when receiving */
  संरचना dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned वर्ण pkt_hd[4]; /* for decoding the arriving packet length */
  संरचना curltime lastTime; /* last time we sent or received data */
  unsigned वर्ण firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

स्थिर शून्य mqtt_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना MQTT *mq = entry;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

स्थिर शून्य mqtt_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_free(entry);
}

स्थिर CURLcode mqtt_setup_conn(संरचना Curl_easy *data,
                                संरचना connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  संरचना mqtt_conn *mqtt;
  संरचना MQTT *mq;

  mqtt = curlx_calloc(1, परिमाण(*mqtt));
  जइ(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, परिमाण(संरचना MQTT));
  जइ(!mq)
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  जइ(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mqtt_send(संरचना Curl_easy *data,
                          ध्रुव वर्ण *buf, size_t len)
{
  size_t n;
  CURLcode result;
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  जइ(!mq)
    प्रत्यागम CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  जइ(result)
    प्रत्यागम result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  जइ(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  प्रत्यागम result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
स्थिर CURLcode mqtt_flush(संरचना Curl_easy *data)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  जइ(!mq)
    प्रत्यागम CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  जइ(!len)
    प्रत्यागम CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  जइ(result)
    प्रत्यागम result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  प्रत्यागम curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
स्थिर CURLcode mqtt_pollset(संरचना Curl_easy *data,
                             संरचना easy_pollset *ps)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  जइ(mq && curlx_dyn_len(&mq->sendbuf))
    प्रत्यागम Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  प्रत्यागम Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

स्थिर पूर्णाङ्क mqtt_encode_len(वर्ण *buf, size_t len)
{
  पूर्णाङ्क i;

  प्रति(i = 0; (len > 0) && (i < 4); i++) {
    unsigned वर्ण encoded;
    encoded = len % 0x80;
    len /= 0x80;
    जइ(len)
      encoded |= 0x80;
    buf[i] = (वर्ण)encoded;
  }

  प्रत्यागम i;
}

/* add the passwd to the CONNECT packet */
स्थिर पूर्णाङ्क add_passwd(ध्रुव वर्ण *passwd, ध्रुव size_t plen,
                      वर्ण *pkt, ध्रुव size_t start, पूर्णाङ्क remain_pos)
{
  /* magic number that need to be set properly */
  ध्रुव size_t conn_flags_pos = remain_pos + 8;
  जइ(plen > 0xffff)
    प्रत्यागम 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (वर्ण)((plen >> 8) & 0xFF);
  pkt[start + 1] = (वर्ण)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  प्रत्यागम 0;
}

/* add user to the CONNECT packet */
स्थिर पूर्णाङ्क add_user(ध्रुव वर्ण *username, ध्रुव size_t ulen,
                    unsigned वर्ण *pkt, ध्रुव size_t start, पूर्णाङ्क remain_pos)
{
  /* magic number that need to be set properly */
  ध्रुव size_t conn_flags_pos = remain_pos + 8;
  जइ(ulen > 0xffff)
    प्रत्यागम 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned वर्ण)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned वर्ण)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  प्रत्यागम 0;
}

/* add client ID to the CONNECT packet */
स्थिर पूर्णाङ्क add_client_id(ध्रुव वर्ण *client_id, ध्रुव size_t client_id_len,
                         वर्ण *pkt, ध्रुव size_t start)
{
  जइ(client_id_len != MQTT_CLIENTID_LEN)
    प्रत्यागम 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  प्रत्यागम 0;
}

/* Set initial values of CONNECT packet */
स्थिर पूर्णाङ्क init_connpack(वर्ण *packet, वर्ण *remain, पूर्णाङ्क remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_सन्देशः_CONNECT;
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
  प्रत्यागम remain_pos + 10;
}

स्थिर CURLcode mqtt_connect(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  पूर्णाङ्क pos = 0;
  पूर्णाङ्क rc = 0;
  /* remain length */
  पूर्णाङ्क remain_pos = 0;
  वर्ण remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  वर्ण client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ध्रुव size_t clen = CURL_CSTRLEN("curl");
  वर्ण *packet = रिक्त;

  /* extracting username from request */
  संरचना Curl_creds *creds = data->state.creds;
  ध्रुव size_t ulen = creds ? strlen(creds->user) : 0;
  ध्रुव size_t plen = creds ? strlen(creds->passwd) : 0;
  ध्रुव size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  जइ(packetlen > 0xFFFFFFF)
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  जइ(!packet)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned वर्ण *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  जइ(rc) {
    failf(data, "Client ID length mismatched: [%zu]", strlen(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    गच्छ end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  जइ(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned वर्ण *)packet, start_user, remain_pos);
    जइ(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      गच्छ end;
    }
  }

  /* if passwd was provided, add it to the packet */
  जइ(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    जइ(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      गच्छ end;
    }
  }

  जइ(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  जइ(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  प्रत्यागम result;
}

स्थिर CURLcode mqtt_disconnect(संरचना Curl_easy *data)
{
  प्रत्यागम mqtt_send(data, "\xe0\x00", 2);
}

स्थिर CURLcode mqtt_recv_atleast(संरचना Curl_easy *data, size_t nbytes)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  जइ(!mq)
    प्रत्यागम CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  जइ(rlen < nbytes) {
    unsigned वर्ण readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < परिमाण(readbuf));
    result = Curl_xfer_recv(data, (वर्ण *)readbuf, nbytes - rlen, &nread);
    जइ(result)
      प्रत्यागम result;
    जइ(!nread) /* EOF */
       प्रत्यागम CURLE_RECV_ERROR;
    जइ(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  प्रत्यागम (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

स्थिर शून्य mqtt_recv_consume(संरचना Curl_easy *data, size_t nbytes)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  जइ(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    जइ(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    अण्णहा
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

स्थिर CURLcode mqtt_verify_connack(संरचना Curl_easy *data)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  ध्रुव वर्ण *ptr;

  DEBUGASSERT(mq);
  जइ(!mq)
    प्रत्यागम CURLE_FAILED_INIT;
  जइ(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  जइ(result)
    प्रत्यागम result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  जइ(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned वर्ण)ptr[0], (unsigned वर्ण)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mqtt_get_topic(संरचना Curl_easy *data,
                               वर्ण **topic, size_t *topiclen)
{
  ध्रुव वर्ण *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  जइ(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    जइ(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  अण्णहा
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  प्रत्यागम result;
}

स्थिर CURLcode mqtt_subscribe(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  वर्ण *topic = रिक्त;
  size_t topiclen;
  unsigned वर्ण *packet = रिक्त;
  size_t packetlen;
  वर्ण encodedsize[4];
  size_t n;
  संरचना connectdata *conn = data->conn;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  जइ(!mqtt)
    प्रत्यागम CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  जइ(result)
    गच्छ fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((वर्ण *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  जइ(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail;
  }

  packet[0] = MQTT_सन्देशः_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (ध्रुव वर्ण *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  प्रत्यागम result;
}

/*
 * Called when the first byte was already read.
 */
स्थिर CURLcode mqtt_verify_suback(संरचना Curl_easy *data)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  संरचना connectdata *conn = data->conn;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  ध्रुव वर्ण *ptr;

  जइ(!mqtt || !mq)
    प्रत्यागम CURLE_FAILED_INIT;

  जइ(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  जइ(result)
    गच्छ fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  जइ(((unsigned वर्ण)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned वर्ण)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    गच्छ fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  प्रत्यागम result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

स्थिर CURLcode mqtt_publish(संरचना Curl_easy *data)
{
  CURLcode result;
  वर्ण *payload = data->set.postfields;
  size_t payloadlen;
  वर्ण *topic = रिक्त;
  size_t topiclen;
  unsigned वर्ण *pkt = रिक्त;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  वर्ण encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  जइ(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }
  जइ(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    जइ(postfieldsize > 0) /* off_t does not fit into size_t */
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  जइ(result)
    गच्छ fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  जइ((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    गच्छ fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  जइ(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    गच्छ fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  जइ(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_सन्देशः_PUBLISH;
  memcpy(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  memcpy(&pkt[i], topic, topiclen);
  i += topiclen;
  memcpy(&pkt[i], payload, payloadlen);
  i += payloadlen;
  result = mqtt_send(data, (ध्रुव वर्ण *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  प्रत्यागम result;
}

/* return FALSE on success, TRUE on error */
स्थिर bool mqtt_decode_len(size_t *lenp, ध्रुव unsigned वर्ण *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned वर्ण encoded = 128;

  प्रति(i = 0; (i < buflen) && (encoded & 128); i++) {
    जइ(i == 4)
      प्रत्यागम TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  जइ(encoded & 128)
    /* truncated size */
    प्रत्यागम TRUE;

  *lenp = len;
  प्रत्यागम FALSE;
}

#जइ defined(DEBUGBUILD) && defined(CURLVERBOSE)
स्थिर ध्रुव वर्ण * ध्रुव statenames[] = {
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
स्थिर शून्य mqstate(संरचना Curl_easy *data,
                    गणना mqttstate state,
                    गणना mqttstate nextstate) /* used if state == FIRST */
{
  संरचना connectdata *conn = data->conn;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  जइ(!mqtt)
    प्रत्यागम;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  जइ(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

स्थिर CURLcode mqtt_read_publish(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned वर्ण packet;

  DEBUGASSERT(mqtt);
  जइ(!mqtt || !mq)
    प्रत्यागम CURLE_FAILED_INIT;

  विकल्प(mqtt->state) {
MQTT_SUBACK_COMING:
  पक्ष MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    जइ(result)
      भङ्ग;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    भङ्ग;

  पक्ष MQTT_SUBACK:
  पक्ष MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    जइ(packet == MQTT_सन्देशः_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    अण्णहा जइ(packet == MQTT_सन्देशः_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      गच्छ MQTT_SUBACK_COMING;
    }
    अण्णहा जइ(packet == MQTT_सन्देशः_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      गच्छ end;
    }
    अण्णहा {
      result = CURLE_WEIRD_SERVER_REPLY;
      गच्छ end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    जइ(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      गच्छ end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  पक्ष MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    वर्ण buffer[4 * 1024];
    size_t rest = mq->npacket;
    जइ(rest > परिमाण(buffer))
      rest = परिमाण(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    जइ(result) {
      जइ(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      गच्छ end;
    }
    जइ(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      गच्छ end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    जइ(result)
      गच्छ end;

    mq->npacket -= nread;
    जइ(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    भङ्ग;
  }
  स्वभाव:
    DEBUGASSERT(रिक्त); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    गच्छ end;
  }
end:
  प्रत्यागम result;
}

स्थिर CURLcode mqtt_do(संरचना Curl_easy *data, bool *done)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  जइ(!mq)
    प्रत्यागम CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  जइ(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (पूर्णाङ्क)result);
    प्रत्यागम result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mqtt_done(संरचना Curl_easy *data,
                          CURLcode status, bool premature)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (शून्य)status;
  (शून्य)premature;
  जइ(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  प्रत्यागम CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
स्थिर CURLcode mqtt_ping(संरचना Curl_easy *data)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  जइ(!mqtt || !mq)
    प्रत्यागम CURLE_FAILED_INIT;

  जइ(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    संरचना curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    जइ(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned वर्ण packet[2] = { 0xC0, 0x00 };
      size_t packetlen = परिमाण(packet);

      result = mqtt_send(data, (वर्ण *)packet, packetlen);
      जइ(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  प्रत्यागम result;
}

स्थिर CURLcode mqtt_doing(संरचना Curl_easy *data, bool *done)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned वर्ण recvbyte;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  जइ(!mqtt || !mq)
    प्रत्यागम CURLE_FAILED_INIT;

  *done = FALSE;

  जइ(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    जइ(result || curlx_dyn_len(&mq->sendbuf))
      प्रत्यागम result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  जइ(result || curlx_dyn_len(&mq->sendbuf))
    प्रत्यागम result;

  infof(data, "mqtt_doing: state [%d]", (पूर्णाङ्क)mqtt->state);
  विकल्प(mqtt->state) {
  पक्ष MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (वर्ण *)&mq->firstbyte, 1, &nread);
    जइ(result)
      भङ्ग;
    अण्णहा जइ(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      भङ्ग;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (ध्रुव वर्ण *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  पक्ष MQTT_REMAINING_LENGTH:
    करेहि {
      result = Curl_xfer_recv(data, (वर्ण *)&recvbyte, 1, &nread);
      जइ(result || !nread)
        भङ्ग;
      Curl_debug(data, CURLINFO_HEADER_IN, (ध्रुव वर्ण *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } जाव((recvbyte & 0x80) && (mq->npacket < 4));
    जइ(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    जइ(result)
      भङ्ग;
    जइ(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      भङ्ग;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ध्रुव unsigned वर्ण type = mq->firstbyte & 0xF0;
      ध्रुव unsigned वर्ण reserved = mq->firstbyte & 0x0F;
      जइ((type == MQTT_सन्देशः_DISCONNECT || type == MQTT_सन्देशः_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_सन्देशः_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        भङ्ग;
      }
    }
    जइ(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      भङ्ग;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    जइ(mq->firstbyte == MQTT_सन्देशः_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    जइ(mq->firstbyte == MQTT_सन्देशः_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    भङ्ग;
  पक्ष MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    जइ(result)
      भङ्ग;

    जइ(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      जइ(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      भङ्ग;
    }

    result = mqtt_publish(data);
    जइ(result)
      भङ्ग;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  पक्ष MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    जइ(curlx_dyn_len(&mq->sendbuf))
      भङ्ग;
    result = mqtt_disconnect(data);
    जइ(result)
      भङ्ग;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  पक्ष MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    जइ(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    भङ्ग;

  पक्ष MQTT_SUBACK:
  पक्ष MQTT_PUBWAIT:
  पक्ष MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    भङ्ग;

  स्वभाव:
    failf(data, "State not handled yet");
    *done = TRUE;
    भङ्ग;
  }

  जइ(result == CURLE_AGAIN)
    result = CURLE_OK;
  प्रत्यागम result;
}

#ifdef USE_SSL

स्थिर CURLcode mqtts_connecting(संरचना Curl_easy *data, bool *done)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  जइ(result)
    connclose(conn);
  प्रत्यागम result;
}

/*
 * MQTTS protocol.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_mqtts = {
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
ध्रुव संरचना Curl_protocol Curl_protocol_mqtt = {
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
