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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"

#ifndef CURL_DISABLE_MQTT

#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "mqtt.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"

/* first byte is command.
   second byte is for flags. */
#define MQTT_ସନ୍ଦେଶ_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_ସନ୍ଦେଶ_PUBLISH    0x30
#define MQTT_ସନ୍ଦେଶ_SUBSCRIBE  0x82
#define MQTT_ସନ୍ଦେଶ_SUBACK     0x90
#define MQTT_ସନ୍ଦେଶ_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_ସନ୍ଦେଶ_PINGRESP   0xD0

#define MQTT_CONNACK_LEN  2
#define MQTT_SUBACK_LEN   3
#define MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#define CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

ଗଣନା mqttstate {
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

ସଂରଚନା mqtt_conn {
  ଗଣନା mqttstate state;
  ଗଣନା mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା packetid;
};

/* protocol-specific transfer-related data */
ସଂରଚନା MQTT {
  ସଂରଚନା dynbuf sendbuf;
  /* when receiving */
  ସଂରଚନା dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned ଅକ୍ଷର pkt_hd[4]; /* for decoding the arriving packet length */
  ସଂରଚନା curltime lastTime; /* last time we sent or received data */
  unsigned ଅକ୍ଷର firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

ସ୍ଥିର ଶୂନ୍ୟ mqtt_easy_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା MQTT *mq = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

ସ୍ଥିର ଶୂନ୍ୟ mqtt_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  curlx_free(entry);
}

ସ୍ଥିର CURLcode mqtt_setup_conn(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  ସଂରଚନା mqtt_conn *mqtt;
  ସଂରଚନା MQTT *mq;

  mqtt = curlx_calloc(1, ଆକାର(*mqtt));
  ଯଦି(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, ଆକାର(ସଂରଚନା MQTT));
  ଯଦି(!mq)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  ଯଦି(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode mqtt_send(ସଂରଚନା Curl_easy *data,
                          ଅଚଳ ଅକ୍ଷର *buf, size_t len)
{
  size_t n;
  CURLcode result;
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  ଯଦି(!mq)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  ଯଦି(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  ଫେରନ୍ତୁ result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
ସ୍ଥିର CURLcode mqtt_flush(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  ଯଦି(!mq)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  ଯଦି(!len)
    ଫେରନ୍ତୁ CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  ଫେରନ୍ତୁ curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
ସ୍ଥିର CURLcode mqtt_pollset(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  ଯଦି(mq && curlx_dyn_len(&mq->sendbuf))
    ଫେରନ୍ତୁ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  ଫେରନ୍ତୁ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mqtt_encode_len(ଅକ୍ଷର *buf, size_t len)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ପାଇଁ(i = 0; (len > 0) && (i < 4); i++) {
    unsigned ଅକ୍ଷର encoded;
    encoded = len % 0x80;
    len /= 0x80;
    ଯଦି(len)
      encoded |= 0x80;
    buf[i] = (ଅକ୍ଷର)encoded;
  }

  ଫେରନ୍ତୁ i;
}

/* add the passwd to the CONNECT packet */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_passwd(ଅଚଳ ଅକ୍ଷର *passwd, ଅଚଳ size_t plen,
                      ଅକ୍ଷର *pkt, ଅଚଳ size_t start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା remain_pos)
{
  /* magic number that need to be set properly */
  ଅଚଳ size_t conn_flags_pos = remain_pos + 8;
  ଯଦି(plen > 0xffff)
    ଫେରନ୍ତୁ 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (ଅକ୍ଷର)((plen >> 8) & 0xFF);
  pkt[start + 1] = (ଅକ୍ଷର)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  ଫେରନ୍ତୁ 0;
}

/* add user to the CONNECT packet */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_user(ଅଚଳ ଅକ୍ଷର *username, ଅଚଳ size_t ulen,
                    unsigned ଅକ୍ଷର *pkt, ଅଚଳ size_t start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା remain_pos)
{
  /* magic number that need to be set properly */
  ଅଚଳ size_t conn_flags_pos = remain_pos + 8;
  ଯଦି(ulen > 0xffff)
    ଫେରନ୍ତୁ 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned ଅକ୍ଷର)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned ଅକ୍ଷର)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  ଫେରନ୍ତୁ 0;
}

/* add client ID to the CONNECT packet */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_client_id(ଅଚଳ ଅକ୍ଷର *client_id, ଅଚଳ size_t client_id_len,
                         ଅକ୍ଷର *pkt, ଅଚଳ size_t start)
{
  ଯଦି(client_id_len != MQTT_CLIENTID_LEN)
    ଫେରନ୍ତୁ 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  ଫେରନ୍ତୁ 0;
}

/* Set initial values of CONNECT packet */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_connpack(ଅକ୍ଷର *packet, ଅକ୍ଷର *remain, ପୂର୍ଣ୍ଣସଂଖ୍ୟା remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_ସନ୍ଦେଶ_CONNECT;
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
  ଫେରନ୍ତୁ remain_pos + 10;
}

ସ୍ଥିର CURLcode mqtt_connect(ସଂରଚନା Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  /* remain length */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା remain_pos = 0;
  ଅକ୍ଷର remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  ଅକ୍ଷର client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ଅଚଳ size_t clen = CURL_CSTRLEN("curl");
  ଅକ୍ଷର *packet = ଖାଲି;

  /* extracting username from request */
  ସଂରଚନା Curl_creds *creds = data->state.creds;
  ଅଚଳ size_t ulen = creds ? strlen(creds->user) : 0;
  ଅଚଳ size_t plen = creds ? strlen(creds->passwd) : 0;
  ଅଚଳ size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  ଯଦି(packetlen > 0xFFFFFFF)
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  ଯଦି(!packet)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned ଅକ୍ଷର *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  ଯଦି(rc) {
    failf(data, "Client ID length mismatched: [%zu]", strlen(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    ଯାଅ end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  ଯଦି(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned ଅକ୍ଷର *)packet, start_user, remain_pos);
    ଯଦି(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      ଯାଅ end;
    }
  }

  /* if passwd was provided, add it to the packet */
  ଯଦି(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    ଯଦି(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      ଯାଅ end;
    }
  }

  ଯଦି(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  ଯଦି(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode mqtt_disconnect(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ mqtt_send(data, "\xe0\x00", 2);
}

ସ୍ଥିର CURLcode mqtt_recv_atleast(ସଂରଚନା Curl_easy *data, size_t nbytes)
{
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  ଯଦି(!mq)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  ଯଦି(rlen < nbytes) {
    unsigned ଅକ୍ଷର readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < ଆକାର(readbuf));
    result = Curl_xfer_recv(data, (ଅକ୍ଷର *)readbuf, nbytes - rlen, &nread);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଯଦି(!nread) /* EOF */
       ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    ଯଦି(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  ଫେରନ୍ତୁ (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

ସ୍ଥିର ଶୂନ୍ୟ mqtt_recv_consume(ସଂରଚନା Curl_easy *data, size_t nbytes)
{
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  ଯଦି(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    ଯଦି(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    ଅନ୍ୟଥା
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

ସ୍ଥିର CURLcode mqtt_verify_connack(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  ଅଚଳ ଅକ୍ଷର *ptr;

  DEBUGASSERT(mq);
  ଯଦି(!mq)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  ଯଦି(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned ଅକ୍ଷର)ptr[0], (unsigned ଅକ୍ଷର)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode mqtt_get_topic(ସଂରଚନା Curl_easy *data,
                               ଅକ୍ଷର **topic, size_t *topiclen)
{
  ଅଚଳ ଅକ୍ଷର *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  ଯଦି(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    ଯଦି(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  ଅନ୍ୟଥା
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode mqtt_subscribe(ସଂରଚନା Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ଅକ୍ଷର *topic = ଖାଲି;
  size_t topiclen;
  unsigned ଅକ୍ଷର *packet = ଖାଲି;
  size_t packetlen;
  ଅକ୍ଷର encodedsize[4];
  size_t n;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ଯଦି(!mqtt)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  ଯଦି(result)
    ଯାଅ fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((ଅକ୍ଷର *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  ଯଦି(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ fail;
  }

  packet[0] = MQTT_ସନ୍ଦେଶ_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (ଅଚଳ ଅକ୍ଷର *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  ଫେରନ୍ତୁ result;
}

/*
 * Called when the first byte was already read.
 */
ସ୍ଥିର CURLcode mqtt_verify_suback(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  ଅଚଳ ଅକ୍ଷର *ptr;

  ଯଦି(!mqtt || !mq)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  ଯଦି(result)
    ଯାଅ fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  ଯଦି(((unsigned ଅକ୍ଷର)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned ଅକ୍ଷର)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    ଯାଅ fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  ଫେରନ୍ତୁ result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

ସ୍ଥିର CURLcode mqtt_publish(ସଂରଚନା Curl_easy *data)
{
  CURLcode result;
  ଅକ୍ଷର *payload = data->set.postfields;
  size_t payloadlen;
  ଅକ୍ଷର *topic = ଖାଲି;
  size_t topiclen;
  unsigned ଅକ୍ଷର *pkt = ଖାଲି;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  ଅକ୍ଷର encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  ଯଦି(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ଯଦି(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    ଯଦି(postfieldsize > 0) /* off_t does not fit into size_t */
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  ଯଦି(result)
    ଯାଅ fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  ଯଦି((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    ଯାଅ fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  ଯଦି(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    ଯାଅ fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  ଯଦି(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_ସନ୍ଦେଶ_PUBLISH;
  memcpy(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  memcpy(&pkt[i], topic, topiclen);
  i += topiclen;
  memcpy(&pkt[i], payload, payloadlen);
  i += payloadlen;
  result = mqtt_send(data, (ଅଚଳ ଅକ୍ଷର *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  ଫେରନ୍ତୁ result;
}

/* return FALSE on success, TRUE on error */
ସ୍ଥିର bool mqtt_decode_len(size_t *lenp, ଅଚଳ unsigned ଅକ୍ଷର *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned ଅକ୍ଷର encoded = 128;

  ପାଇଁ(i = 0; (i < buflen) && (encoded & 128); i++) {
    ଯଦି(i == 4)
      ଫେରନ୍ତୁ TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  ଯଦି(encoded & 128)
    /* truncated size */
    ଫେରନ୍ତୁ TRUE;

  *lenp = len;
  ଫେରନ୍ତୁ FALSE;
}

#ଯଦି defined(DEBUGBUILD) && defined(CURLVERBOSE)
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ statenames[] = {
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
ସ୍ଥିର ଶୂନ୍ୟ mqstate(ସଂରଚନା Curl_easy *data,
                    ଗଣନା mqttstate state,
                    ଗଣନା mqttstate nextstate) /* used if state == FIRST */
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  ଯଦି(!mqtt)
    ଫେରନ୍ତୁ;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  ଯଦି(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

ସ୍ଥିର CURLcode mqtt_read_publish(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  ସଂରଚନା mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned ଅକ୍ଷର packet;

  DEBUGASSERT(mqtt);
  ଯଦି(!mqtt || !mq)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ବାଛ(mqtt->state) {
MQTT_SUBACK_COMING:
  ସ୍ଥିତି MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି MQTT_SUBACK:
  ସ୍ଥିତି MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    ଯଦି(packet == MQTT_ସନ୍ଦେଶ_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    ଅନ୍ୟଥା ଯଦି(packet == MQTT_ସନ୍ଦେଶ_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      ଯାଅ MQTT_SUBACK_COMING;
    }
    ଅନ୍ୟଥା ଯଦି(packet == MQTT_ସନ୍ଦେଶ_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      ଯାଅ end;
    }
    ଅନ୍ୟଥା {
      result = CURLE_WEIRD_SERVER_REPLY;
      ଯାଅ end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    ଯଦି(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      ଯାଅ end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  ସ୍ଥିତି MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    ଅକ୍ଷର buffer[4 * 1024];
    size_t rest = mq->npacket;
    ଯଦି(rest > ଆକାର(buffer))
      rest = ଆକାର(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    ଯଦି(result) {
      ଯଦି(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      ଯାଅ end;
    }
    ଯଦି(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      ଯାଅ end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    ଯଦି(result)
      ଯାଅ end;

    mq->npacket -= nread;
    ଯଦି(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    ଭାଙ୍ଗ;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUGASSERT(ଖାଲି); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    ଯାଅ end;
  }
end:
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode mqtt_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  ଯଦି(!mq)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  ଯଦି(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଫେରନ୍ତୁ result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode mqtt_done(ସଂରଚନା Curl_easy *data,
                          CURLcode status, bool premature)
{
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (ଶୂନ୍ୟ)status;
  (ଶୂନ୍ୟ)premature;
  ଯଦି(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
ସ୍ଥିର CURLcode mqtt_ping(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ଯଦି(!mqtt || !mq)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    ସଂରଚନା curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    ଯଦି(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned ଅକ୍ଷର packet[2] = { 0xC0, 0x00 };
      size_t packetlen = ଆକାର(packet);

      result = mqtt_send(data, (ଅକ୍ଷର *)packet, packetlen);
      ଯଦି(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode mqtt_doing(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned ଅକ୍ଷର recvbyte;
  ସଂରଚନା mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  ଯଦି(!mqtt || !mq)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  *done = FALSE;

  ଯଦି(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    ଯଦି(result || curlx_dyn_len(&mq->sendbuf))
      ଫେରନ୍ତୁ result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  ଯଦି(result || curlx_dyn_len(&mq->sendbuf))
    ଫେରନ୍ତୁ result;

  infof(data, "mqtt_doing: state [%d]", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)mqtt->state);
  ବାଛ(mqtt->state) {
  ସ୍ଥିତି MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (ଅକ୍ଷର *)&mq->firstbyte, 1, &nread);
    ଯଦି(result)
      ଭାଙ୍ଗ;
    ଅନ୍ୟଥା ଯଦି(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      ଭାଙ୍ଗ;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (ଅଚଳ ଅକ୍ଷର *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  ସ୍ଥିତି MQTT_REMAINING_LENGTH:
    କର {
      result = Curl_xfer_recv(data, (ଅକ୍ଷର *)&recvbyte, 1, &nread);
      ଯଦି(result || !nread)
        ଭାଙ୍ଗ;
      Curl_debug(data, CURLINFO_HEADER_IN, (ଅଚଳ ଅକ୍ଷର *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } ଯେତେବେଳେ((recvbyte & 0x80) && (mq->npacket < 4));
    ଯଦି(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    ଯଦି(result)
      ଭାଙ୍ଗ;
    ଯଦି(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      ଭାଙ୍ଗ;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ଅଚଳ unsigned ଅକ୍ଷର type = mq->firstbyte & 0xF0;
      ଅଚଳ unsigned ଅକ୍ଷର reserved = mq->firstbyte & 0x0F;
      ଯଦି((type == MQTT_ସନ୍ଦେଶ_DISCONNECT || type == MQTT_ସନ୍ଦେଶ_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_ସନ୍ଦେଶ_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        ଭାଙ୍ଗ;
      }
    }
    ଯଦି(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      ଭାଙ୍ଗ;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    ଯଦି(mq->firstbyte == MQTT_ସନ୍ଦେଶ_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    ଯଦି(mq->firstbyte == MQTT_ସନ୍ଦେଶ_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    ଯଦି(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      ଯଦି(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      ଭାଙ୍ଗ;
    }

    result = mqtt_publish(data);
    ଯଦି(result)
      ଭାଙ୍ଗ;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  ସ୍ଥିତି MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    ଯଦି(curlx_dyn_len(&mq->sendbuf))
      ଭାଙ୍ଗ;
    result = mqtt_disconnect(data);
    ଯଦି(result)
      ଭାଙ୍ଗ;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  ସ୍ଥିତି MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    ଯଦି(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି MQTT_SUBACK:
  ସ୍ଥିତି MQTT_PUBWAIT:
  ସ୍ଥିତି MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "State not handled yet");
    *done = TRUE;
    ଭାଙ୍ଗ;
  }

  ଯଦି(result == CURLE_AGAIN)
    result = CURLE_OK;
  ଫେରନ୍ତୁ result;
}

#ifdef USE_SSL

ସ୍ଥିର CURLcode mqtts_connecting(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  ଯଦି(result)
    connclose(conn);
  ଫେରନ୍ତୁ result;
}

/*
 * MQTTS protocol.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_mqtts = {
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
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_mqtt = {
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
