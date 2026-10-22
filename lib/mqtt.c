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
#समावेश "curl_setup.h"
#समावेश "urldata.h"

#अगर_अघोषित CURL_DISABLE_MQTT

#समावेश "transfer.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "mqtt.h"
#समावेश "select.h"
#समावेश "url.h"
#समावेश "escape.h"
#समावेश "rand.h"
#समावेश "cfilters.h"
#समावेश "connect.h"

/* first byte is command.
   second byte is for flags. */
#घोषणा MQTT_संदेश_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#घोषणा MQTT_संदेश_PUBLISH    0x30
#घोषणा MQTT_संदेश_SUBSCRIBE  0x82
#घोषणा MQTT_संदेश_SUBACK     0x90
#घोषणा MQTT_संदेश_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#घोषणा MQTT_संदेश_PINGRESP   0xD0

#घोषणा MQTT_CONNACK_LEN  2
#घोषणा MQTT_SUBACK_LEN   3
#घोषणा MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#घोषणा CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#घोषणा CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

क्रमागत mqttstate {
  MQTT_FIRST,             /* 0 */
  MQTT_REMAINING_लंबाई,  /* 1 */
  MQTT_CONNACK,           /* 2 */
  MQTT_SUBACK,            /* 3 */
  MQTT_SUBACK_COMING,     /* 4 - the SUBACK remainder */
  MQTT_PUBWAIT,    /* 5 - wait for publish */
  MQTT_PUB_REMAIN,  /* 6 - wait for the remainder of the publish */
  MQTT_POST_DRAIN,        /* 7 - finish sending PUBLISH */
  MQTT_DISCONNECT_DRAIN,  /* 8 - finish sending DISCONNECT */

  MQTT_NOSTATE /* 9 - never used an actual state */
};

काष्ठा mqtt_conn {
  क्रमागत mqttstate state;
  क्रमागत mqttstate nextstate; /* switch to this after remaining length is
                               done */
  अचिन्हित पूर्णांक packetid;
};

/* protocol-specific transfer-related data */
काष्ठा MQTT {
  काष्ठा dynbuf sendbuf;
  /* when receiving */
  काष्ठा dynbuf recvbuf;
  माप_प्रकार npacket; /* byte counter */
  माप_प्रकार remaining_लंबाई;
  अचिन्हित अक्षर pkt_hd[4]; /* for decoding the arriving packet length */
  काष्ठा curltime lastTime; /* last time we sent or received data */
  अचिन्हित अक्षर firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

अटल व्योम mqtt_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा MQTT *mq = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

अटल व्योम mqtt_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  (व्योम)कुंजी;
  (व्योम)klen;
  curlx_free(entry);
}

अटल CURLcode mqtt_setup_conn(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  काष्ठा mqtt_conn *mqtt;
  काष्ठा MQTT *mq;

  mqtt = curlx_calloc(1, माप(*mqtt));
  अगर(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, माप(काष्ठा MQTT));
  अगर(!mq)
    वापस CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  अगर(Curl_meta_set(तथ्य, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    वापस CURLE_OUT_OF_MEMORY;
  वापस CURLE_OK;
}

अटल CURLcode mqtt_send(काष्ठा Curl_easy *तथ्य,
                          स्थिर अक्षर *buf, माप_प्रकार len)
{
  माप_प्रकार n;
  CURLcode परिणाम;
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);

  अगर(!mq)
    वापस CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  परिणाम = Curl_xfer_send(तथ्य, buf, len, FALSE, &n);
  अगर(परिणाम)
    वापस परिणाम;
  mq->lastTime = *Curl_pgrs_now(तथ्य);
  Curl_debug(तथ्य, CURLINFO_HEADER_OUT, buf, n);
  अगर(len != n)
    परिणाम = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  वापस परिणाम;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
अटल CURLcode mqtt_flush(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  माप_प्रकार len, n;
  CURLcode परिणाम;

  अगर(!mq)
    वापस CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  अगर(!len)
    वापस CURLE_OK;

  परिणाम = Curl_xfer_send(तथ्य, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  अगर(परिणाम)
    वापस परिणाम;
  mq->lastTime = *Curl_pgrs_now(तथ्य);
  Curl_debug(तथ्य, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  वापस curlx_dyn_पुच्छ(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
अटल CURLcode mqtt_pollset(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps)
{
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  अगर(mq && curlx_dyn_len(&mq->sendbuf))
    वापस Curl_pollset_add_out(तथ्य, ps, तथ्य->conn->sock[FIRSTSOCKET]);
  वापस Curl_pollset_add_in(तथ्य, ps, तथ्य->conn->sock[FIRSTSOCKET]);
}

अटल पूर्णांक mqtt_encode_len(अक्षर *buf, माप_प्रकार len)
{
  पूर्णांक i;

  क्रम(i = 0; (len > 0) && (i < 4); i++) {
    अचिन्हित अक्षर encoded;
    encoded = len % 0x80;
    len /= 0x80;
    अगर(len)
      encoded |= 0x80;
    buf[i] = (अक्षर)encoded;
  }

  वापस i;
}

/* add the passwd to the CONNECT packet */
अटल पूर्णांक add_passwd(स्थिर अक्षर *passwd, स्थिर माप_प्रकार plen,
                      अक्षर *pkt, स्थिर माप_प्रकार start, पूर्णांक remain_pos)
{
  /* magic number that need to be set properly */
  स्थिर माप_प्रकार conn_flags_pos = remain_pos + 8;
  अगर(plen > 0xffff)
    वापस 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (अक्षर)((plen >> 8) & 0xFF);
  pkt[start + 1] = (अक्षर)(plen & 0xFF);
  स_नकल(&pkt[start + 2], passwd, plen);
  वापस 0;
}

/* add user to the CONNECT packet */
अटल पूर्णांक add_user(स्थिर अक्षर *username, स्थिर माप_प्रकार ulen,
                    अचिन्हित अक्षर *pkt, स्थिर माप_प्रकार start, पूर्णांक remain_pos)
{
  /* magic number that need to be set properly */
  स्थिर माप_प्रकार conn_flags_pos = remain_pos + 8;
  अगर(ulen > 0xffff)
    वापस 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (अचिन्हित अक्षर)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (अचिन्हित अक्षर)(ulen & 0xFF);
  स_नकल(&pkt[start + 2], username, ulen);
  वापस 0;
}

/* add client ID to the CONNECT packet */
अटल पूर्णांक add_client_id(स्थिर अक्षर *client_id, स्थिर माप_प्रकार client_id_len,
                         अक्षर *pkt, स्थिर माप_प्रकार start)
{
  अगर(client_id_len != MQTT_CLIENTID_LEN)
    वापस 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  स_नकल(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  वापस 0;
}

/* Set initial values of CONNECT packet */
अटल पूर्णांक init_connpack(अक्षर *packet, अक्षर *remain, पूर्णांक remain_pos)
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
  वापस remain_pos + 10;
}

अटल CURLcode mqtt_connect(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक pos = 0;
  पूर्णांक rc = 0;
  /* remain length */
  पूर्णांक remain_pos = 0;
  अक्षर remain[4] = { 0 };
  माप_प्रकार packetlen = 0;
  माप_प्रकार start_user = 0;
  माप_प्रकार start_pwd = 0;
  अक्षर client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  स्थिर माप_प्रकार clen = CURL_CSTRLEN("curl");
  अक्षर *packet = शून्य;

  /* extracting username from request */
  काष्ठा Curl_creds *creds = तथ्य->state.creds;
  स्थिर माप_प्रकार ulen = creds ? म_माप(creds->user) : 0;
  स्थिर माप_प्रकार plen = creds ? म_माप(creds->passwd) : 0;
  स्थिर माप_प्रकार payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  अगर(packetlen > 0xFFFFFFF)
    वापस CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  अगर(!packet)
    वापस CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  परिणाम = Curl_rand_alnum(तथ्य, (अचिन्हित अक्षर *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, म_माप(client_id), packet, pos + 1);
  अगर(rc) {
    failf(तथ्य, "Client ID length mismatched: [%zu]", म_माप(client_id));
    परिणाम = CURLE_WEIRD_SERVER_REPLY;
    जाओ end;
  }
  infof(तथ्य, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  अगर(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (अचिन्हित अक्षर *)packet, start_user, remain_pos);
    अगर(rc) {
      failf(तथ्य, "Username too long: [%zu]", ulen);
      परिणाम = CURLE_WEIRD_SERVER_REPLY;
      जाओ end;
    }
  }

  /* if passwd was provided, add it to the packet */
  अगर(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    अगर(rc) {
      failf(तथ्य, "Password too long: [%zu]", plen);
      परिणाम = CURLE_WEIRD_SERVER_REPLY;
      जाओ end;
    }
  }

  अगर(!परिणाम)
    परिणाम = mqtt_send(तथ्य, packet, packetlen);

end:
  अगर(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&तथ्य->state.creds);
  वापस परिणाम;
}

अटल CURLcode mqtt_disconnect(काष्ठा Curl_easy *तथ्य)
{
  वापस mqtt_send(तथ्य, "\xe0\x00", 2);
}

अटल CURLcode mqtt_recv_atleast(काष्ठा Curl_easy *तथ्य, माप_प्रकार nbytes)
{
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  माप_प्रकार rlen;
  CURLcode परिणाम;

  अगर(!mq)
    वापस CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  अगर(rlen < nbytes) {
    अचिन्हित अक्षर readbuf[1024];
    माप_प्रकार nread;

    DEBUGASSERT(nbytes - rlen < माप(readbuf));
    परिणाम = Curl_xfer_recv(तथ्य, (अक्षर *)readbuf, nbytes - rlen, &nread);
    अगर(परिणाम)
      वापस परिणाम;
    अगर(!nread) /* EOF */
       वापस CURLE_RECV_ERROR;
    अगर(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      वापस CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  वापस (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

अटल व्योम mqtt_recv_consume(काष्ठा Curl_easy *तथ्य, माप_प्रकार nbytes)
{
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  अगर(mq) {
    माप_प्रकार rlen = curlx_dyn_len(&mq->recvbuf);
    अगर(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    अन्यथा
      curlx_dyn_पुच्छ(&mq->recvbuf, rlen - nbytes);
  }
}

अटल CURLcode mqtt_verify_connack(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  CURLcode परिणाम;
  स्थिर अक्षर *ptr;

  DEBUGASSERT(mq);
  अगर(!mq)
    वापस CURLE_FAILED_INIT;
  अगर(mq->remaining_लंबाई != 2) {
    failf(तथ्य, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_लंबाई);
    वापस CURLE_WEIRD_SERVER_REPLY;
  }

  परिणाम = mqtt_recv_atleast(तथ्य, MQTT_CONNACK_LEN);
  अगर(परिणाम)
    वापस परिणाम;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(तथ्य, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  अगर(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(तथ्य, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (अचिन्हित अक्षर)ptr[0], (अचिन्हित अक्षर)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    वापस CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(तथ्य, MQTT_CONNACK_LEN);
  वापस CURLE_OK;
}

अटल CURLcode mqtt_get_topic(काष्ठा Curl_easy *तथ्य,
                               अक्षर **topic, माप_प्रकार *topiclen)
{
  स्थिर अक्षर *path = तथ्य->state.up.path;
  CURLcode परिणाम = CURLE_URL_MALFORMAT;
  अगर(म_माप(path) > 1) {
    परिणाम = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    अगर(!परिणाम && (*topiclen > 0xffff)) {
      failf(तथ्य, "Too long MQTT topic");
      परिणाम = CURLE_URL_MALFORMAT;
    }
  }
  अन्यथा
    failf(तथ्य, "No MQTT topic found. Forgot to URL encode it?");

  वापस परिणाम;
}

अटल CURLcode mqtt_subscribe(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OK;
  अक्षर *topic = शून्य;
  माप_प्रकार topiclen;
  अचिन्हित अक्षर *packet = शून्य;
  माप_प्रकार packetlen;
  अक्षर encodedsize[4];
  माप_प्रकार n;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  अगर(!mqtt)
    वापस CURLE_FAILED_INIT;

  परिणाम = mqtt_get_topic(तथ्य, &topic, &topiclen);
  अगर(परिणाम)
    जाओ fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((अक्षर *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  अगर(!packet) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ fail;
  }

  packet[0] = MQTT_संदेश_SUBSCRIBE;
  स_नकल(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  स_नकल(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  परिणाम = mqtt_send(तथ्य, (स्थिर अक्षर *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  वापस परिणाम;
}

/*
 * Called when the first byte was already read.
 */
अटल CURLcode mqtt_verify_suback(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode परिणाम;
  स्थिर अक्षर *ptr;

  अगर(!mqtt || !mq)
    वापस CURLE_FAILED_INIT;

  अगर(mq->remaining_लंबाई != 3) {
    failf(तथ्य, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_लंबाई);
    वापस CURLE_WEIRD_SERVER_REPLY;
  }

  परिणाम = mqtt_recv_atleast(तथ्य, MQTT_SUBACK_LEN);
  अगर(परिणाम)
    जाओ fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(तथ्य, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  अगर(((अचिन्हित अक्षर)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((अचिन्हित अक्षर)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    परिणाम = CURLE_WEIRD_SERVER_REPLY;
    जाओ fail;
  }
  mqtt_recv_consume(तथ्य, MQTT_SUBACK_LEN);
fail:
  वापस परिणाम;
}

#घोषणा MAX_MQTT_MESSAGE_आकार 0xFFFFFFF

अटल CURLcode mqtt_publish(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;
  अक्षर *payload = तथ्य->set.postfields;
  माप_प्रकार payloadlen;
  अक्षर *topic = शून्य;
  माप_प्रकार topiclen;
  अचिन्हित अक्षर *pkt = शून्य;
  माप_प्रकार i = 0;
  माप_प्रकार remaininglength;
  माप_प्रकार encodelen;
  अक्षर encodedbytes[4];
  curl_off_t postfieldsize = तथ्य->set.postfieldsize;

  अगर(!payload) {
    DEBUGF(infof(तथ्य, "mqtt_publish without payload, return bad arg"));
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }
  अगर(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    अगर(postfieldsize > 0) /* off_t does not fit into size_t */
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = म_माप(payload);
  }

  परिणाम = mqtt_get_topic(तथ्य, &topic, &topiclen);
  अगर(परिणाम)
    जाओ fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  अगर((SIZE_MAX - topiclen - 2) < payloadlen) {
    परिणाम = CURLE_TOO_LARGE;
    जाओ fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  अगर(remaininglength > (MAX_MQTT_MESSAGE_आकार - encodelen - 1)) {
    परिणाम = CURLE_TOO_LARGE;
    जाओ fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  अगर(!pkt) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ fail;
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
  परिणाम = mqtt_send(तथ्य, (स्थिर अक्षर *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  वापस परिणाम;
}

/* return FALSE on success, TRUE on error */
अटल bool mqtt_decode_len(माप_प्रकार *lenp, स्थिर अचिन्हित अक्षर *buf,
                            माप_प्रकार buflen)
{
  माप_प्रकार len = 0;
  माप_प्रकार mult = 1;
  माप_प्रकार i;
  अचिन्हित अक्षर encoded = 128;

  क्रम(i = 0; (i < buflen) && (encoded & 128); i++) {
    अगर(i == 4)
      वापस TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  अगर(encoded & 128)
    /* truncated size */
    वापस TRUE;

  *lenp = len;
  वापस FALSE;
}

#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
अटल स्थिर अक्षर * स्थिर statenames[] = {
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
#पूर्ण_अगर

/* The only way to change state */
अटल व्योम mqstate(काष्ठा Curl_easy *तथ्य,
                    क्रमागत mqttstate state,
                    क्रमागत mqttstate nextstate) /* used if state == FIRST */
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  अगर(!mqtt)
    वापस;
#अगर_घोषित DEBUGBUILD
  infof(तथ्य, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#पूर्ण_अगर
  mqtt->state = state;
  अगर(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

अटल CURLcode mqtt_read_publish(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  माप_प्रकार nread;
  माप_प्रकार remlen;
  काष्ठा mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  अचिन्हित अक्षर packet;

  DEBUGASSERT(mqtt);
  अगर(!mqtt || !mq)
    वापस CURLE_FAILED_INIT;

  चयन(mqtt->state) {
MQTT_SUBACK_COMING:
  हाल MQTT_SUBACK_COMING:
    परिणाम = mqtt_verify_suback(तथ्य);
    अगर(परिणाम)
      अवरोध;

    mqstate(तथ्य, MQTT_FIRST, MQTT_PUBWAIT);
    अवरोध;

  हाल MQTT_SUBACK:
  हाल MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    अगर(packet == MQTT_संदेश_PUBLISH)
      mqstate(तथ्य, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    अन्यथा अगर(packet == MQTT_संदेश_SUBACK) {
      mqstate(तथ्य, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      जाओ MQTT_SUBACK_COMING;
    }
    अन्यथा अगर(packet == MQTT_संदेश_DISCONNECT) {
      infof(तथ्य, "Got DISCONNECT");
      *done = TRUE;
      जाओ end;
    }
    अन्यथा {
      परिणाम = CURLE_WEIRD_SERVER_REPLY;
      जाओ end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_लंबाई;
    infof(तथ्य, "Remaining length: %zu bytes", remlen);
    अगर(तथ्य->set.max_filesize &&
       (curl_off_t)remlen > तथ्य->set.max_filesize) {
      failf(तथ्य, "Maximum file size exceeded");
      परिणाम = CURLE_FILESIZE_EXCEEDED;
      जाओ end;
    }
    Curl_pgrs_Set_Download_आकार(तथ्य, remlen);
    तथ्य->req.bytecount = 0;
    तथ्य->req.आकार = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  हाल MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    अक्षर बफर[4 * 1024];
    माप_प्रकार rest = mq->npacket;
    अगर(rest > माप(बफर))
      rest = माप(बफर);
    परिणाम = Curl_xfer_recv(तथ्य, बफर, rest, &nread);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_AGAIN) {
        infof(तथ्य, "EEEE AAAAGAIN");
      }
      जाओ end;
    }
    अगर(!nread) {
      infof(तथ्य, "server disconnected");
      परिणाम = CURLE_PARTIAL_FILE;
      जाओ end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(तथ्य);

    /* if QoS is set, message contains packet id */
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, बफर, nread);
    अगर(परिणाम)
      जाओ end;

    mq->npacket -= nread;
    अगर(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(तथ्य, MQTT_FIRST, MQTT_PUBWAIT);
    अवरोध;
  }
  शेष:
    DEBUGASSERT(शून्य); /* illegal state */
    परिणाम = CURLE_WEIRD_SERVER_REPLY;
    जाओ end;
  }
end:
  वापस परिणाम;
}

अटल CURLcode mqtt_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  CURLcode परिणाम = CURLE_OK;
  *done = FALSE; /* unconditionally */

  अगर(!mq)
    वापस CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(तथ्य);
  mq->pingsent = FALSE;

  परिणाम = mqtt_connect(तथ्य);
  अगर(परिणाम) {
    failf(तथ्य, "Error %d sending MQTT CONNECT request", (पूर्णांक)परिणाम);
    वापस परिणाम;
  }
  mqstate(तथ्य, MQTT_FIRST, MQTT_CONNACK);
  वापस CURLE_OK;
}

अटल CURLcode mqtt_done(काष्ठा Curl_easy *तथ्य,
                          CURLcode status, bool premature)
{
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  (व्योम)status;
  (व्योम)premature;
  अगर(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  वापस CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
अटल CURLcode mqtt_ping(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  अगर(!mqtt || !mq)
    वापस CURLE_FAILED_INIT;

  अगर(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     तथ्य->set.upkeep_interval_ms > 0) {
    काष्ठा curltime t = *Curl_pgrs_now(तथ्य);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    अगर(diff > तथ्य->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      अचिन्हित अक्षर packet[2] = { 0xC0, 0x00 };
      माप_प्रकार packetlen = माप(packet);

      परिणाम = mqtt_send(तथ्य, (अक्षर *)packet, packetlen);
      अगर(!परिणाम) {
        mq->pingsent = TRUE;
      }
      infof(तथ्य, "mqtt_ping: sent ping request.");
    }
  }
  वापस परिणाम;
}

अटल CURLcode mqtt_doing(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा MQTT *mq = Curl_meta_get(तथ्य, CURL_META_MQTT_EASY);
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nread;
  अचिन्हित अक्षर recvbyte;
  काष्ठा mqtt_conn *mqtt = Curl_conn_meta_get(तथ्य->conn, CURL_META_MQTT_CONN);

  अगर(!mqtt || !mq)
    वापस CURLE_FAILED_INIT;

  *done = FALSE;

  अगर(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    परिणाम = mqtt_flush(तथ्य);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    अगर(परिणाम || curlx_dyn_len(&mq->sendbuf))
      वापस परिणाम;
  }

  परिणाम = mqtt_ping(तथ्य);
  /* Packet processing may send more output, so finish PINGREQ first. */
  अगर(परिणाम || curlx_dyn_len(&mq->sendbuf))
    वापस परिणाम;

  infof(तथ्य, "mqtt_doing: state [%d]", (पूर्णांक)mqtt->state);
  चयन(mqtt->state) {
  हाल MQTT_FIRST:
    /* Read the initial byte only */
    परिणाम = Curl_xfer_recv(तथ्य, (अक्षर *)&mq->firstbyte, 1, &nread);
    अगर(परिणाम)
      अवरोध;
    अन्यथा अगर(!nread) {
      failf(तथ्य, "Connection disconnected");
      *done = TRUE;
      परिणाम = CURLE_RECV_ERROR;
      अवरोध;
    }
    Curl_debug(तथ्य, CURLINFO_HEADER_IN, (स्थिर अक्षर *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(तथ्य);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(तथ्य, MQTT_REMAINING_लंबाई, MQTT_NOSTATE);
    FALLTHROUGH();
  हाल MQTT_REMAINING_लंबाई:
    करो {
      परिणाम = Curl_xfer_recv(तथ्य, (अक्षर *)&recvbyte, 1, &nread);
      अगर(परिणाम || !nread)
        अवरोध;
      Curl_debug(तथ्य, CURLINFO_HEADER_IN, (स्थिर अक्षर *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } जबतक((recvbyte & 0x80) && (mq->npacket < 4));
    अगर(!परिणाम && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      परिणाम = CURLE_WEIRD_SERVER_REPLY;
    अगर(परिणाम)
      अवरोध;
    अगर(mqtt_decode_len(&mq->remaining_लंबाई, mq->pkt_hd, mq->npacket)) {
      परिणाम = CURLE_WEIRD_SERVER_REPLY;
      अवरोध;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      स्थिर अचिन्हित अक्षर type = mq->firstbyte & 0xF0;
      स्थिर अचिन्हित अक्षर reserved = mq->firstbyte & 0x0F;
      अगर((type == MQTT_संदेश_DISCONNECT || type == MQTT_संदेश_PINGRESP) &&
         (mq->remaining_लंबाई || reserved)) {
        failf(तथ्य,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_संदेश_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_लंबाई, mq->firstbyte);
        परिणाम = CURLE_WEIRD_SERVER_REPLY;
        अवरोध;
      }
    }
    अगर(mq->remaining_लंबाई) {
      mqstate(तथ्य, mqtt->nextstate, MQTT_NOSTATE);
      अवरोध;
    }
    mqstate(तथ्य, MQTT_FIRST, MQTT_FIRST);

    अगर(mq->firstbyte == MQTT_संदेश_DISCONNECT) {
      infof(तथ्य, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    अगर(mq->firstbyte == MQTT_संदेश_PINGRESP) {
      infof(तथ्य, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(तथ्य, MQTT_FIRST, MQTT_PUBWAIT);
    }
    अवरोध;
  हाल MQTT_CONNACK:
    परिणाम = mqtt_verify_connack(तथ्य);
    अगर(परिणाम)
      अवरोध;

    अगर(तथ्य->state.httpreq != HTTPREQ_POST) {
      परिणाम = mqtt_subscribe(तथ्य);
      अगर(!परिणाम)
        mqstate(तथ्य, MQTT_FIRST, MQTT_SUBACK);
      अवरोध;
    }

    परिणाम = mqtt_publish(तथ्य);
    अगर(परिणाम)
      अवरोध;
    mqstate(तथ्य, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  हाल MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    अगर(curlx_dyn_len(&mq->sendbuf))
      अवरोध;
    परिणाम = mqtt_disconnect(तथ्य);
    अगर(परिणाम)
      अवरोध;
    mqstate(तथ्य, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  हाल MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    अगर(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    अवरोध;

  हाल MQTT_SUBACK:
  हाल MQTT_PUBWAIT:
  हाल MQTT_PUB_REMAIN:
    परिणाम = mqtt_read_publish(तथ्य, done);
    अवरोध;

  शेष:
    failf(तथ्य, "State not handled yet");
    *done = TRUE;
    अवरोध;
  }

  अगर(परिणाम == CURLE_AGAIN)
    परिणाम = CURLE_OK;
  वापस परिणाम;
}

#अगर_घोषित USE_SSL

अटल CURLcode mqtts_connecting(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम;

  परिणाम = Curl_conn_connect(तथ्य, FIRSTSOCKET, TRUE, done);
  अगर(परिणाम)
    connclose(conn);
  वापस परिणाम;
}

/*
 * MQTTS protocol.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_mqtts = {
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

#पूर्ण_अगर

/*
 * MQTT protocol.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_mqtt = {
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

#पूर्ण_अगर /* CURL_DISABLE_MQTT */
