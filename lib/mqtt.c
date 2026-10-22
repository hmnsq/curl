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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_MQTT

#అన్తర్భావ "transfer.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "mqtt.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"

/* first byte is command.
   second byte is for flags. */
#నిర్వచించు MQTT_సందేశం_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#నిర్వచించు MQTT_సందేశం_PUBLISH    0x30
#నిర్వచించు MQTT_సందేశం_SUBSCRIBE  0x82
#నిర్వచించు MQTT_సందేశం_SUBACK     0x90
#నిర్వచించు MQTT_సందేశం_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#నిర్వచించు MQTT_సందేశం_PINGRESP   0xD0

#నిర్వచించు MQTT_CONNACK_LEN  2
#నిర్వచించు MQTT_SUBACK_LEN   3
#నిర్వచించు MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#నిర్వచించు CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#నిర్వచించు CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

గణన mqttstate {
  MQTT_మొదటి,             /* 0 */
  MQTT_మిగిలిన_నిడివి,  /* 1 */
  MQTT_CONNACK,           /* 2 */
  MQTT_SUBACK,            /* 3 */
  MQTT_SUBACK_COMING,     /* 4 - the SUBACK remainder */
  MQTT_PUBWAIT,    /* 5 - wait for publish */
  MQTT_PUB_REMAIN,  /* 6 - wait for the remainder of the publish */
  MQTT_POST_DRAIN,        /* 7 - finish sending PUBLISH */
  MQTT_DISCONNECT_DRAIN,  /* 8 - finish sending DISCONNECT */

  MQTT_NOSTATE /* 9 - never used an actual state */
};

నిర్మాణం mqtt_conn {
  గణన mqttstate state;
  గణన mqttstate nextstate; /* switch to this after remaining length is
పూర్తి*/
  గుర్తులేని పూర్ణాంకం packetid;
};

/* protocol-specific transfer-related data */
నిర్మాణం MQTT {
  నిర్మాణం dynbuf sendbuf;
  /* when receiving */
  నిర్మాణం dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t మిగిలిన_నిడివి;
  గుర్తులేని అక్షరం pkt_hd[4]; /* for decoding the arriving packet length */
  నిర్మాణం curltime చివరి_సమయం; /* last time we sent or received data */
  గుర్తులేని అక్షరం firstbyte;
  బిట్టు(pingsent); /* 1 while we wait for ping response */
};

స్థిర శూన్య mqtt_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం MQTT *mq = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

స్థిర శూన్య mqtt_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curlx_free(నమోదు_2);
}

స్థిర CURLcode mqtt_అమరిక_conn(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  నిర్మాణం mqtt_conn *mqtt;
  నిర్మాణం MQTT *mq;

  mqtt = curlx_calloc(1, పరిమాణం(*mqtt));
  ఒకవేళ(!mqtt ||
     Curl_conn_meta_సమితి(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  mq = curlx_calloc(1, పరిమాణం(నిర్మాణం MQTT));
  ఒకవేళ(!mq)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  ఒకవేళ(Curl_meta_సమితి(దత్తాంశం, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode mqtt_send(నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి అక్షరం *buf, size_t len)
{
  size_t n;
  CURLcode ఫలితం;
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);

  ఒకవేళ(!mq)
    తిరిగి CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  ఫలితం = Curl_xfer_send(దత్తాంశం, buf, len, FALSE, &n);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  mq->చివరి_సమయం = *Curl_pgrs_now(దత్తాంశం);
  Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_బహిర్గతం, buf, n);
  ఒకవేళ(len != n)
    ఫలితం = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  తిరిగి ఫలితం;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
స్థిర CURLcode mqtt_flush(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode ఫలితం;

  ఒకవేళ(!mq)
    తిరిగి CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  ఒకవేళ(!len)
    తిరిగి CURLE_OK;

  ఫలితం = Curl_xfer_send(దత్తాంశం, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  mq->చివరి_సమయం = *Curl_pgrs_now(దత్తాంశం);
  Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_బహిర్గతం, curlx_dyn_ptr(&mq->sendbuf), n);
  తిరిగి curlx_dyn_తోక(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
స్థితులు*/
స్థిర CURLcode mqtt_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  ఒకవేళ(mq && curlx_dyn_len(&mq->sendbuf))
    తిరిగి Curl_pollset_కూడు_బహిర్గతం(దత్తాంశం, ps, దత్తాంశం->conn->sock[FIRSTSOCKET]);
  తిరిగి Curl_pollset_కూడు_లోపల(దత్తాంశం, ps, దత్తాంశం->conn->sock[FIRSTSOCKET]);
}

స్థిర పూర్ణాంకం mqtt_encode_len(అక్షరం *buf, size_t len)
{
  పూర్ణాంకం i;

  కొరకు(i = 0; (len > 0) && (i < 4); i++) {
    గుర్తులేని అక్షరం encoded;
    encoded = len % 0x80;
    len /= 0x80;
    ఒకవేళ(len)
      encoded |= 0x80;
    buf[i] = (అక్షరం)encoded;
  }

  తిరిగి i;
}

/* add the passwd to the CONNECT packet */
స్థిర పూర్ణాంకం కూడు_passwd(స్థిరరాశి అక్షరం *passwd, స్థిరరాశి size_t plen,
                      అక్షరం *pkt, స్థిరరాశి size_t ప్రారంభం, పూర్ణాంకం remain_pos)
{
  /* magic number that need to be set properly */
  స్థిరరాశి size_t conn_flags_pos = remain_pos + 8;
  ఒకవేళ(plen > 0xffff)
    తిరిగి 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[ప్రారంభం] = (అక్షరం)((plen >> 8) & 0xFF);
  pkt[ప్రారంభం + 1] = (అక్షరం)(plen & 0xFF);
  జ్ఞాపకం_నకలు(&pkt[ప్రారంభం + 2], passwd, plen);
  తిరిగి 0;
}

/* add user to the CONNECT packet */
స్థిర పూర్ణాంకం కూడు_user(స్థిరరాశి అక్షరం *username, స్థిరరాశి size_t ulen,
                    గుర్తులేని అక్షరం *pkt, స్థిరరాశి size_t ప్రారంభం, పూర్ణాంకం remain_pos)
{
  /* magic number that need to be set properly */
  స్థిరరాశి size_t conn_flags_pos = remain_pos + 8;
  ఒకవేళ(ulen > 0xffff)
    తిరిగి 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[ప్రారంభం] = (గుర్తులేని అక్షరం)((ulen >> 8) & 0xFF);
  pkt[ప్రారంభం + 1] = (గుర్తులేని అక్షరం)(ulen & 0xFF);
  జ్ఞాపకం_నకలు(&pkt[ప్రారంభం + 2], username, ulen);
  తిరిగి 0;
}

/* add client ID to the CONNECT packet */
స్థిర పూర్ణాంకం కూడు_client_id(స్థిరరాశి అక్షరం *client_id, స్థిరరాశి size_t client_id_len,
                         అక్షరం *pkt, స్థిరరాశి size_t ప్రారంభం)
{
  ఒకవేళ(client_id_len != MQTT_CLIENTID_LEN)
    తిరిగి 1;
  pkt[ప్రారంభం] = 0x00;
  pkt[ప్రారంభం + 1] = MQTT_CLIENTID_LEN;
  జ్ఞాపకం_నకలు(&pkt[ప్రారంభం + 2], client_id, MQTT_CLIENTID_LEN);
  తిరిగి 0;
}

/* Set initial values of CONNECT packet */
స్థిర పూర్ణాంకం init_connpack(అక్షరం *packet, అక్షరం *remain, పూర్ణాంకం remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_సందేశం_CONNECT;
  /* remaining length field */
  జ్ఞాపకం_నకలు(&packet[1], remain, remain_pos);
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
  తిరిగి remain_pos + 10;
}

స్థిర CURLcode mqtt_connect(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం pos = 0;
  పూర్ణాంకం rc = 0;
  /* remain length */
  పూర్ణాంకం remain_pos = 0;
  అక్షరం remain[4] = { 0 };
  size_t packetlen = 0;
  size_t ప్రారంభం_user = 0;
  size_t ప్రారంభం_pwd = 0;
  అక్షరం client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  స్థిరరాశి size_t clen = CURL_CSTRLEN("curl");
  అక్షరం *packet = ఖాళీ;

  /* extracting username from request */
  నిర్మాణం Curl_creds *creds = దత్తాంశం->state.creds;
  స్థిరరాశి size_t ulen = creds ? అక్షరశ్రేణి_పొడవు(creds->user) : 0;
  స్థిరరాశి size_t plen = creds ? అక్షరశ్రేణి_పొడవు(creds->passwd) : 0;
  స్థిరరాశి size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  ఒకవేళ(packetlen > 0xFFFFFFF)
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  packet = curlx_calloc(1, packetlen);
  ఒకవేళ(!packet)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  ఫలితం = Curl_rand_alnum(దత్తాంశం, (గుర్తులేని అక్షరం *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = కూడు_client_id(client_id, అక్షరశ్రేణి_పొడవు(client_id), packet, pos + 1);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "Client ID length mismatched: [%zu]", అక్షరశ్రేణి_పొడవు(client_id));
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
    వెళ్ళు చివర;
  }
  infof(దత్తాంశం, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  ప్రారంభం_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  ప్రారంభం_pwd = ప్రారంభం_user + ulen;
  /* if username was provided, add it to the packet */
  ఒకవేళ(ulen) {
    ప్రారంభం_pwd += 2;

    rc = కూడు_user(creds->user, ulen,
                  (గుర్తులేని అక్షరం *)packet, ప్రారంభం_user, remain_pos);
    ఒకవేళ(rc) {
      failf(దత్తాంశం, "Username too long: [%zu]", ulen);
      ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
      వెళ్ళు చివర;
    }
  }

  /* if passwd was provided, add it to the packet */
  ఒకవేళ(plen) {
    rc = కూడు_passwd(creds->passwd, plen, packet, ప్రారంభం_pwd, remain_pos);
    ఒకవేళ(rc) {
      failf(దత్తాంశం, "Password too long: [%zu]", plen);
      ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
      వెళ్ళు చివర;
    }
  }

  ఒకవేళ(!ఫలితం)
    ఫలితం = mqtt_send(దత్తాంశం, packet, packetlen);

చివర:
  ఒకవేళ(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&దత్తాంశం->state.creds);
  తిరిగి ఫలితం;
}

స్థిర CURLcode mqtt_disconnect(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి mqtt_send(దత్తాంశం, "\xe0\x00", 2);
}

స్థిర CURLcode mqtt_recv_atleast(నిర్మాణం Curl_easy *దత్తాంశం, size_t nbytes)
{
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode ఫలితం;

  ఒకవేళ(!mq)
    తిరిగి CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  ఒకవేళ(rlen < nbytes) {
    గుర్తులేని అక్షరం readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < పరిమాణం(readbuf));
    ఫలితం = Curl_xfer_recv(దత్తాంశం, (అక్షరం *)readbuf, nbytes - rlen, &nread);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఒకవేళ(!nread) /* EOF */
       తిరిగి CURLE_RECV_ERROR;
    ఒకవేళ(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  తిరిగి (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

స్థిర శూన్య mqtt_recv_consume(నిర్మాణం Curl_easy *దత్తాంశం, size_t nbytes)
{
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  ఒకవేళ(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    ఒకవేళ(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    లేకపోతే
      curlx_dyn_తోక(&mq->recvbuf, rlen - nbytes);
  }
}

స్థిర CURLcode mqtt_verify_connack(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  CURLcode ఫలితం;
  స్థిరరాశి అక్షరం *ptr;

  DEBUGASSERT(mq);
  ఒకవేళ(!mq)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(mq->మిగిలిన_నిడివి != 2) {
    failf(దత్తాంశం, "CONNACK expected Remaining Length 2, got %zu",
          mq->మిగిలిన_నిడివి);
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }

  ఫలితం = mqtt_recv_atleast(దత్తాంశం, MQTT_CONNACK_LEN);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, ptr, MQTT_CONNACK_LEN);

  ఒకవేళ(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(దత్తాంశం, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (గుర్తులేని అక్షరం)ptr[0], (గుర్తులేని అక్షరం)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }
  mqtt_recv_consume(దత్తాంశం, MQTT_CONNACK_LEN);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode mqtt_get_topic(నిర్మాణం Curl_easy *దత్తాంశం,
                               అక్షరం **topic, size_t *topiclen)
{
  స్థిరరాశి అక్షరం *మార్గం = దత్తాంశం->state.పైకి.మార్గం;
  CURLcode ఫలితం = CURLE_URL_MALFORMAT;
  ఒకవేళ(అక్షరశ్రేణి_పొడవు(మార్గం) > 1) {
    ఫలితం = Curl_urldecode(మార్గం + 1, 0, topic, topiclen, REJECT_CTRL);
    ఒకవేళ(!ఫలితం && (*topiclen > 0xffff)) {
      failf(దత్తాంశం, "Too long MQTT topic");
      ఫలితం = CURLE_URL_MALFORMAT;
    }
  }
  లేకపోతే
    failf(దత్తాంశం, "No MQTT topic found. Forgot to URL encode it?");

  తిరిగి ఫలితం;
}

స్థిర CURLcode mqtt_subscribe(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_OK;
  అక్షరం *topic = ఖాళీ;
  size_t topiclen;
  గుర్తులేని అక్షరం *packet = ఖాళీ;
  size_t packetlen;
  అక్షరం encodedsize[4];
  size_t n;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ఒకవేళ(!mqtt)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = mqtt_get_topic(దత్తాంశం, &topic, &topiclen);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((అక్షరం *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  ఒకవేళ(!packet) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు fail;
  }

  packet[0] = MQTT_సందేశం_SUBSCRIBE;
  జ్ఞాపకం_నకలు(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  జ్ఞాపకం_నకలు(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  ఫలితం = mqtt_send(దత్తాంశం, (స్థిరరాశి అక్షరం *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  తిరిగి ఫలితం;
}

/*
 * Called when the first byte was already read.
 */
స్థిర CURLcode mqtt_verify_suback(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode ఫలితం;
  స్థిరరాశి అక్షరం *ptr;

  ఒకవేళ(!mqtt || !mq)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(mq->మిగిలిన_నిడివి != 3) {
    failf(దత్తాంశం, "SUBACK expected Remaining Length 3, got %zu",
          mq->మిగిలిన_నిడివి);
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }

  ఫలితం = mqtt_recv_atleast(దత్తాంశం, MQTT_SUBACK_LEN);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, ptr, MQTT_SUBACK_LEN);

  ఒకవేళ(((గుర్తులేని అక్షరం)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((గుర్తులేని అక్షరం)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
    వెళ్ళు fail;
  }
  mqtt_recv_consume(దత్తాంశం, MQTT_SUBACK_LEN);
fail:
  తిరిగి ఫలితం;
}

#నిర్వచించు MAX_MQTT_సందేశం_కొలత 0xFFFFFFF

స్థిర CURLcode mqtt_publish(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;
  అక్షరం *payload = దత్తాంశం->సమితి.postfields;
  size_t payloadlen;
  అక్షరం *topic = ఖాళీ;
  size_t topiclen;
  గుర్తులేని అక్షరం *pkt = ఖాళీ;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  అక్షరం encodedbytes[4];
  curl_off_t postfieldsize = దత్తాంశం->సమితి.postfieldsize;

  ఒకవేళ(!payload) {
    DEBUGF(infof(దత్తాంశం, "mqtt_publish without payload, return bad arg"));
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
  ఒకవేళ(!curlx_sotouz_సరిపోతుంది(postfieldsize, &payloadlen)) {
    ఒకవేళ(postfieldsize > 0) /* off_t does not fit into size_t */
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    payloadlen = అక్షరశ్రేణి_పొడవు(payload);
  }

  ఫలితం = mqtt_get_topic(దత్తాంశం, &topic, &topiclen);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  ఒకవేళ((SIZE_MAX - topiclen - 2) < payloadlen) {
    ఫలితం = CURLE_TOO_LARGE;
    వెళ్ళు fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  ఒకవేళ(remaininglength > (MAX_MQTT_సందేశం_కొలత - encodelen - 1)) {
    ఫలితం = CURLE_TOO_LARGE;
    వెళ్ళు fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  ఒకవేళ(!pkt) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_సందేశం_PUBLISH;
  జ్ఞాపకం_నకలు(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  జ్ఞాపకం_నకలు(&pkt[i], topic, topiclen);
  i += topiclen;
  జ్ఞాపకం_నకలు(&pkt[i], payload, payloadlen);
  i += payloadlen;
  ఫలితం = mqtt_send(దత్తాంశం, (స్థిరరాశి అక్షరం *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  తిరిగి ఫలితం;
}

/* return FALSE on success, TRUE on error */
స్థిర bool mqtt_decode_len(size_t *lenp, స్థిరరాశి గుర్తులేని అక్షరం *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  గుర్తులేని అక్షరం encoded = 128;

  కొరకు(i = 0; (i < buflen) && (encoded & 128); i++) {
    ఒకవేళ(i == 4)
      తిరిగి TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  ఒకవేళ(encoded & 128)
    /* truncated size */
    తిరిగి TRUE;

  *lenp = len;
  తిరిగి FALSE;
}

#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి statenames[] = {
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
#అయితే_ముగింపు

/* The only way to change state */
స్థిర శూన్య mqstate(నిర్మాణం Curl_easy *దత్తాంశం,
                    గణన mqttstate state,
                    గణన mqttstate nextstate) /* used if state == FIRST */
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  ఒకవేళ(!mqtt)
    తిరిగి;
#నిర్వచితమైతే DEBUGBUILD
  infof(దత్తాంశం, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_మొదటి) ? statenames[nextstate] : "");
#అయితే_ముగింపు
  mqtt->state = state;
  ఒకవేళ(state == MQTT_మొదటి)
    mqtt->nextstate = nextstate;
}

స్థిర CURLcode mqtt_read_publish(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  size_t nread;
  size_t remlen;
  నిర్మాణం mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  గుర్తులేని అక్షరం packet;

  DEBUGASSERT(mqtt);
  ఒకవేళ(!mqtt || !mq)
    తిరిగి CURLE_FAILED_INIT;

  ఎంపిక(mqtt->state) {
MQTT_SUBACK_COMING:
  స్థితి MQTT_SUBACK_COMING:
    ఫలితం = mqtt_verify_suback(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      ఆపు;

    mqstate(దత్తాంశం, MQTT_మొదటి, MQTT_PUBWAIT);
    ఆపు;

  స్థితి MQTT_SUBACK:
  స్థితి MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    ఒకవేళ(packet == MQTT_సందేశం_PUBLISH)
      mqstate(దత్తాంశం, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    లేకపోతే ఒకవేళ(packet == MQTT_సందేశం_SUBACK) {
      mqstate(దత్తాంశం, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      వెళ్ళు MQTT_SUBACK_COMING;
    }
    లేకపోతే ఒకవేళ(packet == MQTT_సందేశం_DISCONNECT) {
      infof(దత్తాంశం, "Got DISCONNECT");
      *పూర్తి = TRUE;
      వెళ్ళు చివర;
    }
    లేకపోతే {
      ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
      వెళ్ళు చివర;
    }

    /* -- switched state -- */
    remlen = mq->మిగిలిన_నిడివి;
    infof(దత్తాంశం, "Remaining length: %zu bytes", remlen);
    ఒకవేళ(దత్తాంశం->సమితి.max_filesize &&
       (curl_off_t)remlen > దత్తాంశం->సమితి.max_filesize) {
      failf(దత్తాంశం, "Maximum file size exceeded");
      ఫలితం = CURLE_FILESIZE_EXCEEDED;
      వెళ్ళు చివర;
    }
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, remlen);
    దత్తాంశం->req.bytecount = 0;
    దత్తాంశం->req.కొలత = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  స్థితి MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    అక్షరం బఫరు[4 * 1024];
    size_t rest = mq->npacket;
    ఒకవేళ(rest > పరిమాణం(బఫరు))
      rest = పరిమాణం(బఫరు);
    ఫలితం = Curl_xfer_recv(దత్తాంశం, బఫరు, rest, &nread);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_AGAIN) {
        infof(దత్తాంశం, "EEEE AAAAGAIN");
      }
      వెళ్ళు చివర;
    }
    ఒకవేళ(!nread) {
      infof(దత్తాంశం, "server disconnected");
      ఫలితం = CURLE_PARTIAL_FILE;
      వెళ్ళు చివర;
    }

    /* we received something */
    mq->చివరి_సమయం = *Curl_pgrs_now(దత్తాంశం);

    /* if QoS is set, message contains packet id */
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, బఫరు, nread);
    ఒకవేళ(ఫలితం)
      వెళ్ళు చివర;

    mq->npacket -= nread;
    ఒకవేళ(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(దత్తాంశం, MQTT_మొదటి, MQTT_PUBWAIT);
    ఆపు;
  }
  అప్రమేయ:
    DEBUGASSERT(ఖాళీ); /* illegal state */
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
    వెళ్ళు చివర;
  }
చివర:
  తిరిగి ఫలితం;
}

స్థిర CURLcode mqtt_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  CURLcode ఫలితం = CURLE_OK;
  *పూర్తి = FALSE; /* unconditionally */

  ఒకవేళ(!mq)
    తిరిగి CURLE_FAILED_INIT;
  mq->చివరి_సమయం = *Curl_pgrs_now(దత్తాంశం);
  mq->pingsent = FALSE;

  ఫలితం = mqtt_connect(దత్తాంశం);
  ఒకవేళ(ఫలితం) {
    failf(దత్తాంశం, "Error %d sending MQTT CONNECT request", (పూర్ణాంకం)ఫలితం);
    తిరిగి ఫలితం;
  }
  mqstate(దత్తాంశం, MQTT_మొదటి, MQTT_CONNACK);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode mqtt_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                          CURLcode స్థితి_2, bool premature)
{
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  (శూన్య)స్థితి_2;
  (శూన్య)premature;
  ఒకవేళ(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  తిరిగి CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
స్థిర CURLcode mqtt_ping(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ఒకవేళ(!mqtt || !mq)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(mqtt->state == MQTT_మొదటి &&
     !mq->pingsent &&
     దత్తాంశం->సమితి.upkeep_interval_ms > 0) {
    నిర్మాణం curltime t = *Curl_pgrs_now(దత్తాంశం);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->చివరి_సమయం);

    ఒకవేళ(diff > దత్తాంశం->సమితి.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      గుర్తులేని అక్షరం packet[2] = { 0xC0, 0x00 };
      size_t packetlen = పరిమాణం(packet);

      ఫలితం = mqtt_send(దత్తాంశం, (అక్షరం *)packet, packetlen);
      ఒకవేళ(!ఫలితం) {
        mq->pingsent = TRUE;
      }
      infof(దత్తాంశం, "mqtt_ping: sent ping request.");
    }
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode mqtt_doing(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం MQTT *mq = Curl_meta_get(దత్తాంశం, CURL_META_MQTT_EASY);
  CURLcode ఫలితం = CURLE_OK;
  size_t nread;
  గుర్తులేని అక్షరం recvbyte;
  నిర్మాణం mqtt_conn *mqtt = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_MQTT_CONN);

  ఒకవేళ(!mqtt || !mq)
    తిరిగి CURLE_FAILED_INIT;

  *పూర్తి = FALSE;

  ఒకవేళ(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    ఫలితం = mqtt_flush(దత్తాంశం);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    ఒకవేళ(ఫలితం || curlx_dyn_len(&mq->sendbuf))
      తిరిగి ఫలితం;
  }

  ఫలితం = mqtt_ping(దత్తాంశం);
  /* Packet processing may send more output, so finish PINGREQ first. */
  ఒకవేళ(ఫలితం || curlx_dyn_len(&mq->sendbuf))
    తిరిగి ఫలితం;

  infof(దత్తాంశం, "mqtt_doing: state [%d]", (పూర్ణాంకం)mqtt->state);
  ఎంపిక(mqtt->state) {
  స్థితి MQTT_మొదటి:
    /* Read the initial byte only */
    ఫలితం = Curl_xfer_recv(దత్తాంశం, (అక్షరం *)&mq->firstbyte, 1, &nread);
    ఒకవేళ(ఫలితం)
      ఆపు;
    లేకపోతే ఒకవేళ(!nread) {
      failf(దత్తాంశం, "Connection disconnected");
      *పూర్తి = TRUE;
      ఫలితం = CURLE_RECV_ERROR;
      ఆపు;
    }
    Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, (స్థిరరాశి అక్షరం *)&mq->firstbyte, 1);

    /* we received something */
    mq->చివరి_సమయం = *Curl_pgrs_now(దత్తాంశం);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(దత్తాంశం, MQTT_మిగిలిన_నిడివి, MQTT_NOSTATE);
    FALLTHROUGH();
  స్థితి MQTT_మిగిలిన_నిడివి:
    చేయి {
      ఫలితం = Curl_xfer_recv(దత్తాంశం, (అక్షరం *)&recvbyte, 1, &nread);
      ఒకవేళ(ఫలితం || !nread)
        ఆపు;
      Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, (స్థిరరాశి అక్షరం *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } అయితే((recvbyte & 0x80) && (mq->npacket < 4));
    ఒకవేళ(!ఫలితం && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
    ఒకవేళ(ఫలితం)
      ఆపు;
    ఒకవేళ(mqtt_decode_len(&mq->మిగిలిన_నిడివి, mq->pkt_hd, mq->npacket)) {
      ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
      ఆపు;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      స్థిరరాశి గుర్తులేని అక్షరం type = mq->firstbyte & 0xF0;
      స్థిరరాశి గుర్తులేని అక్షరం reserved = mq->firstbyte & 0x0F;
      ఒకవేళ((type == MQTT_సందేశం_DISCONNECT || type == MQTT_సందేశం_PINGRESP) &&
         (mq->మిగిలిన_నిడివి || reserved)) {
        failf(దత్తాంశం,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_సందేశం_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->మిగిలిన_నిడివి, mq->firstbyte);
        ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
        ఆపు;
      }
    }
    ఒకవేళ(mq->మిగిలిన_నిడివి) {
      mqstate(దత్తాంశం, mqtt->nextstate, MQTT_NOSTATE);
      ఆపు;
    }
    mqstate(దత్తాంశం, MQTT_మొదటి, MQTT_మొదటి);

    ఒకవేళ(mq->firstbyte == MQTT_సందేశం_DISCONNECT) {
      infof(దత్తాంశం, "Got DISCONNECT");
      *పూర్తి = TRUE;
    }

    /* ping response */
    ఒకవేళ(mq->firstbyte == MQTT_సందేశం_PINGRESP) {
      infof(దత్తాంశం, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(దత్తాంశం, MQTT_మొదటి, MQTT_PUBWAIT);
    }
    ఆపు;
  స్థితి MQTT_CONNACK:
    ఫలితం = mqtt_verify_connack(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      ఆపు;

    ఒకవేళ(దత్తాంశం->state.httpreq != HTTPREQ_POST) {
      ఫలితం = mqtt_subscribe(దత్తాంశం);
      ఒకవేళ(!ఫలితం)
        mqstate(దత్తాంశం, MQTT_మొదటి, MQTT_SUBACK);
      ఆపు;
    }

    ఫలితం = mqtt_publish(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      ఆపు;
    mqstate(దత్తాంశం, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  స్థితి MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    ఒకవేళ(curlx_dyn_len(&mq->sendbuf))
      ఆపు;
    ఫలితం = mqtt_disconnect(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      ఆపు;
    mqstate(దత్తాంశం, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  స్థితి MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    ఒకవేళ(!curlx_dyn_len(&mq->sendbuf))
      *పూర్తి = TRUE;
    ఆపు;

  స్థితి MQTT_SUBACK:
  స్థితి MQTT_PUBWAIT:
  స్థితి MQTT_PUB_REMAIN:
    ఫలితం = mqtt_read_publish(దత్తాంశం, పూర్తి);
    ఆపు;

  అప్రమేయ:
    failf(దత్తాంశం, "State not handled yet");
    *పూర్తి = TRUE;
    ఆపు;
  }

  ఒకవేళ(ఫలితం == CURLE_AGAIN)
    ఫలితం = CURLE_OK;
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే USE_SSL

స్థిర CURLcode mqtts_connecting(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం;

  ఫలితం = Curl_conn_connect(దత్తాంశం, FIRSTSOCKET, TRUE, పూర్తి);
  ఒకవేళ(ఫలితం)
    connclose(conn);
  తిరిగి ఫలితం;
}

/*
 * MQTTS protocol.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_mqtts = {
  mqtt_అమరిక_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_పూర్తి,                          /* done */
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

#అయితే_ముగింపు

/*
 * MQTT protocol.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_mqtt = {
  mqtt_అమరిక_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_పూర్తి,                          /* done */
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

#అయితే_ముగింపు /* CURL_DISABLE_MQTT */
