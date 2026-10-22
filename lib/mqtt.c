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
#define MQTT_संदेश_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_संदेश_PUBLISH    0x30
#define MQTT_संदेश_SUBSCRIBE  0x82
#define MQTT_संदेश_SUBACK     0x90
#define MQTT_संदेश_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_संदेश_PINGRESP   0xD0

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
  अचिन्हित पूर्णांक packetid;
};

/* protocol-specific transfer-related data */
संरचना MQTT {
  संरचना dynbuf sendbuf;
  /* when receiving */
  संरचना dynbuf recvbuf;
  माप_प्रकार npacket; /* byte counter */
  माप_प्रकार remaining_length;
  अचिन्हित अक्षर pkt_hd[4]; /* for decoding the arriving packet length */
  संरचना curltime lastTime; /* last time we sent or received data */
  अचिन्हित अक्षर firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

स्थिर शून्य mqtt_easy_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना MQTT *mq = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

स्थिर शून्य mqtt_conn_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  (शून्य)किल्ली;
  (शून्य)klen;
  curlx_free(entry);
}

स्थिर CURLcode mqtt_setup_conn(संरचना Curl_easy *data,
                                संरचना connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  संरचना mqtt_conn *mqtt;
  संरचना MQTT *mq;

  mqtt = curlx_calloc(1, आकार(*mqtt));
  जर(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    परत CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, आकार(संरचना MQTT));
  जर(!mq)
    परत CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  जर(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    परत CURLE_OUT_OF_MEMORY;
  परत CURLE_OK;
}

स्थिर CURLcode mqtt_send(संरचना Curl_easy *data,
                          स्थिरांक अक्षर *buf, माप_प्रकार len)
{
  माप_प्रकार n;
  CURLcode result;
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  जर(!mq)
    परत CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  जर(result)
    परत result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  जर(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  परत result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
स्थिर CURLcode mqtt_flush(संरचना Curl_easy *data)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  माप_प्रकार len, n;
  CURLcode result;

  जर(!mq)
    परत CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  जर(!len)
    परत CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  जर(result)
    परत result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  परत curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
स्थिर CURLcode mqtt_pollset(संरचना Curl_easy *data,
                             संरचना easy_pollset *ps)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  जर(mq && curlx_dyn_len(&mq->sendbuf))
    परत Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  परत Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

स्थिर पूर्णांक mqtt_encode_len(अक्षर *buf, माप_प्रकार len)
{
  पूर्णांक i;

  साठी(i = 0; (len > 0) && (i < 4); i++) {
    अचिन्हित अक्षर encoded;
    encoded = len % 0x80;
    len /= 0x80;
    जर(len)
      encoded |= 0x80;
    buf[i] = (अक्षर)encoded;
  }

  परत i;
}

/* add the passwd to the CONNECT packet */
स्थिर पूर्णांक add_passwd(स्थिरांक अक्षर *passwd, स्थिरांक माप_प्रकार plen,
                      अक्षर *pkt, स्थिरांक माप_प्रकार start, पूर्णांक remain_pos)
{
  /* magic number that need to be set properly */
  स्थिरांक माप_प्रकार conn_flags_pos = remain_pos + 8;
  जर(plen > 0xffff)
    परत 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (अक्षर)((plen >> 8) & 0xFF);
  pkt[start + 1] = (अक्षर)(plen & 0xFF);
  स_नकल(&pkt[start + 2], passwd, plen);
  परत 0;
}

/* add user to the CONNECT packet */
स्थिर पूर्णांक add_user(स्थिरांक अक्षर *username, स्थिरांक माप_प्रकार ulen,
                    अचिन्हित अक्षर *pkt, स्थिरांक माप_प्रकार start, पूर्णांक remain_pos)
{
  /* magic number that need to be set properly */
  स्थिरांक माप_प्रकार conn_flags_pos = remain_pos + 8;
  जर(ulen > 0xffff)
    परत 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (अचिन्हित अक्षर)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (अचिन्हित अक्षर)(ulen & 0xFF);
  स_नकल(&pkt[start + 2], username, ulen);
  परत 0;
}

/* add client ID to the CONNECT packet */
स्थिर पूर्णांक add_client_id(स्थिरांक अक्षर *client_id, स्थिरांक माप_प्रकार client_id_len,
                         अक्षर *pkt, स्थिरांक माप_प्रकार start)
{
  जर(client_id_len != MQTT_CLIENTID_LEN)
    परत 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  स_नकल(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  परत 0;
}

/* Set initial values of CONNECT packet */
स्थिर पूर्णांक init_connpack(अक्षर *packet, अक्षर *remain, पूर्णांक remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_संदेश_CONNECT;
  /* remaining length field */
  स_नकल(&packet[1], remain, remain_pos);
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
  परत remain_pos + 10;
}

स्थिर CURLcode mqtt_connect(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  पूर्णांक pos = 0;
  पूर्णांक rc = 0;
  /* remain length */
  पूर्णांक remain_pos = 0;
  अक्षर remain[4] = { 0 };
  माप_प्रकार packetlen = 0;
  माप_प्रकार start_user = 0;
  माप_प्रकार start_pwd = 0;
  अक्षर client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  स्थिरांक माप_प्रकार clen = CURL_CSTRLEN("curl");
  अक्षर *packet = रिक्त;

  /* extracting username from request */
  संरचना Curl_creds *creds = data->state.creds;
  स्थिरांक माप_प्रकार ulen = creds ? म_माप(creds->user) : 0;
  स्थिरांक माप_प्रकार plen = creds ? म_माप(creds->passwd) : 0;
  स्थिरांक माप_प्रकार payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  जर(packetlen > 0xFFFFFFF)
    परत CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  जर(!packet)
    परत CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (अचिन्हित अक्षर *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, म_माप(client_id), packet, pos + 1);
  जर(rc) {
    failf(data, "Client ID length mismatched: [%zu]", म_माप(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    जा end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  जर(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (अचिन्हित अक्षर *)packet, start_user, remain_pos);
    जर(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      जा end;
    }
  }

  /* if passwd was provided, add it to the packet */
  जर(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    जर(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      जा end;
    }
  }

  जर(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  जर(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  परत result;
}

स्थिर CURLcode mqtt_disconnect(संरचना Curl_easy *data)
{
  परत mqtt_send(data, "\xe0\x00", 2);
}

स्थिर CURLcode mqtt_recv_atleast(संरचना Curl_easy *data, माप_प्रकार nbytes)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  माप_प्रकार rlen;
  CURLcode result;

  जर(!mq)
    परत CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  जर(rlen < nbytes) {
    अचिन्हित अक्षर readbuf[1024];
    माप_प्रकार nread;

    DEBUGASSERT(nbytes - rlen < आकार(readbuf));
    result = Curl_xfer_recv(data, (अक्षर *)readbuf, nbytes - rlen, &nread);
    जर(result)
      परत result;
    जर(!nread) /* EOF */
       परत CURLE_RECV_ERROR;
    जर(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      परत CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  परत (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

स्थिर शून्य mqtt_recv_consume(संरचना Curl_easy *data, माप_प्रकार nbytes)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  जर(mq) {
    माप_प्रकार rlen = curlx_dyn_len(&mq->recvbuf);
    जर(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    अन्यथा
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

स्थिर CURLcode mqtt_verify_connack(संरचना Curl_easy *data)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  स्थिरांक अक्षर *ptr;

  DEBUGASSERT(mq);
  जर(!mq)
    परत CURLE_FAILED_INIT;
  जर(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    परत CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  जर(result)
    परत result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  जर(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (अचिन्हित अक्षर)ptr[0], (अचिन्हित अक्षर)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    परत CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  परत CURLE_OK;
}

स्थिर CURLcode mqtt_get_topic(संरचना Curl_easy *data,
                               अक्षर **topic, माप_प्रकार *topiclen)
{
  स्थिरांक अक्षर *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  जर(म_माप(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    जर(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  अन्यथा
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  परत result;
}

स्थिर CURLcode mqtt_subscribe(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  अक्षर *topic = रिक्त;
  माप_प्रकार topiclen;
  अचिन्हित अक्षर *packet = रिक्त;
  माप_प्रकार packetlen;
  अक्षर encodedsize[4];
  माप_प्रकार n;
  संरचना connectdata *conn = data->conn;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  जर(!mqtt)
    परत CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  जर(result)
    जा fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((अक्षर *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  जर(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    जा fail;
  }

  packet[0] = MQTT_संदेश_SUBSCRIBE;
  स_नकल(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  स_नकल(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (स्थिरांक अक्षर *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  परत result;
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
  स्थिरांक अक्षर *ptr;

  जर(!mqtt || !mq)
    परत CURLE_FAILED_INIT;

  जर(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    परत CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  जर(result)
    जा fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  जर(((अचिन्हित अक्षर)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((अचिन्हित अक्षर)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    जा fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  परत result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

स्थिर CURLcode mqtt_publish(संरचना Curl_easy *data)
{
  CURLcode result;
  अक्षर *payload = data->set.postfields;
  माप_प्रकार payloadlen;
  अक्षर *topic = रिक्त;
  माप_प्रकार topiclen;
  अचिन्हित अक्षर *pkt = रिक्त;
  माप_प्रकार i = 0;
  माप_प्रकार remaininglength;
  माप_प्रकार encodelen;
  अक्षर encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  जर(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }
  जर(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    जर(postfieldsize > 0) /* off_t does not fit into size_t */
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = म_माप(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  जर(result)
    जा fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  जर((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    जा fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  जर(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    जा fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  जर(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    जा fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_संदेश_PUBLISH;
  स_नकल(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  स_नकल(&pkt[i], topic, topiclen);
  i += topiclen;
  स_नकल(&pkt[i], payload, payloadlen);
  i += payloadlen;
  result = mqtt_send(data, (स्थिरांक अक्षर *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  परत result;
}

/* return FALSE on success, TRUE on error */
स्थिर bool mqtt_decode_len(माप_प्रकार *lenp, स्थिरांक अचिन्हित अक्षर *buf,
                            माप_प्रकार buflen)
{
  माप_प्रकार len = 0;
  माप_प्रकार mult = 1;
  माप_प्रकार i;
  अचिन्हित अक्षर encoded = 128;

  साठी(i = 0; (i < buflen) && (encoded & 128); i++) {
    जर(i == 4)
      परत TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  जर(encoded & 128)
    /* truncated size */
    परत TRUE;

  *lenp = len;
  परत FALSE;
}

#जर defined(DEBUGBUILD) && defined(CURLVERBOSE)
स्थिर स्थिरांक अक्षर * स्थिरांक statenames[] = {
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
  जर(!mqtt)
    परत;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  जर(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

स्थिर CURLcode mqtt_read_publish(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  माप_प्रकार nread;
  माप_प्रकार remlen;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  अचिन्हित अक्षर packet;

  DEBUGASSERT(mqtt);
  जर(!mqtt || !mq)
    परत CURLE_FAILED_INIT;

  निवड(mqtt->state) {
MQTT_SUBACK_COMING:
  स्थिती MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    जर(result)
      थांबा;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    थांबा;

  स्थिती MQTT_SUBACK:
  स्थिती MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    जर(packet == MQTT_संदेश_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    अन्यथा जर(packet == MQTT_संदेश_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      जा MQTT_SUBACK_COMING;
    }
    अन्यथा जर(packet == MQTT_संदेश_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      जा end;
    }
    अन्यथा {
      result = CURLE_WEIRD_SERVER_REPLY;
      जा end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    जर(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      जा end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  स्थिती MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    अक्षर buffer[4 * 1024];
    माप_प्रकार rest = mq->npacket;
    जर(rest > आकार(buffer))
      rest = आकार(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    जर(result) {
      जर(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      जा end;
    }
    जर(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      जा end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    जर(result)
      जा end;

    mq->npacket -= nread;
    जर(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    थांबा;
  }
  पूर्वनिर्धारित:
    DEBUGASSERT(रिक्त); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    जा end;
  }
end:
  परत result;
}

स्थिर CURLcode mqtt_do(संरचना Curl_easy *data, bool *done)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  जर(!mq)
    परत CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  जर(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (पूर्णांक)result);
    परत result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  परत CURLE_OK;
}

स्थिर CURLcode mqtt_done(संरचना Curl_easy *data,
                          CURLcode status, bool premature)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (शून्य)status;
  (शून्य)premature;
  जर(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  परत CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
स्थिर CURLcode mqtt_ping(संरचना Curl_easy *data)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  जर(!mqtt || !mq)
    परत CURLE_FAILED_INIT;

  जर(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    संरचना curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    जर(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      अचिन्हित अक्षर packet[2] = { 0xC0, 0x00 };
      माप_प्रकार packetlen = आकार(packet);

      result = mqtt_send(data, (अक्षर *)packet, packetlen);
      जर(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  परत result;
}

स्थिर CURLcode mqtt_doing(संरचना Curl_easy *data, bool *done)
{
  संरचना MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  माप_प्रकार nread;
  अचिन्हित अक्षर recvbyte;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  जर(!mqtt || !mq)
    परत CURLE_FAILED_INIT;

  *done = FALSE;

  जर(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    जर(result || curlx_dyn_len(&mq->sendbuf))
      परत result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  जर(result || curlx_dyn_len(&mq->sendbuf))
    परत result;

  infof(data, "mqtt_doing: state [%d]", (पूर्णांक)mqtt->state);
  निवड(mqtt->state) {
  स्थिती MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (अक्षर *)&mq->firstbyte, 1, &nread);
    जर(result)
      थांबा;
    अन्यथा जर(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      थांबा;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (स्थिरांक अक्षर *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  स्थिती MQTT_REMAINING_LENGTH:
    करा {
      result = Curl_xfer_recv(data, (अक्षर *)&recvbyte, 1, &nread);
      जर(result || !nread)
        थांबा;
      Curl_debug(data, CURLINFO_HEADER_IN, (स्थिरांक अक्षर *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } पर्यंत((recvbyte & 0x80) && (mq->npacket < 4));
    जर(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    जर(result)
      थांबा;
    जर(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      थांबा;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      स्थिरांक अचिन्हित अक्षर type = mq->firstbyte & 0xF0;
      स्थिरांक अचिन्हित अक्षर reserved = mq->firstbyte & 0x0F;
      जर((type == MQTT_संदेश_DISCONNECT || type == MQTT_संदेश_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_संदेश_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        थांबा;
      }
    }
    जर(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      थांबा;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    जर(mq->firstbyte == MQTT_संदेश_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    जर(mq->firstbyte == MQTT_संदेश_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    थांबा;
  स्थिती MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    जर(result)
      थांबा;

    जर(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      जर(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      थांबा;
    }

    result = mqtt_publish(data);
    जर(result)
      थांबा;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  स्थिती MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    जर(curlx_dyn_len(&mq->sendbuf))
      थांबा;
    result = mqtt_disconnect(data);
    जर(result)
      थांबा;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  स्थिती MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    जर(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    थांबा;

  स्थिती MQTT_SUBACK:
  स्थिती MQTT_PUBWAIT:
  स्थिती MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    थांबा;

  पूर्वनिर्धारित:
    failf(data, "State not handled yet");
    *done = TRUE;
    थांबा;
  }

  जर(result == CURLE_AGAIN)
    result = CURLE_OK;
  परत result;
}

#ifdef USE_SSL

स्थिर CURLcode mqtts_connecting(संरचना Curl_easy *data, bool *done)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  जर(result)
    connclose(conn);
  परत result;
}

/*
 * MQTTS protocol.
 */
स्थिरांक संरचना Curl_protocol Curl_protocol_mqtts = {
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
स्थिरांक संरचना Curl_protocol Curl_protocol_mqtt = {
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
