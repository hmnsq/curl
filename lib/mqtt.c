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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"

#ifndef CURL_DISABLE_MQTT

#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "mqtt.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"

/* first byte is command.
   second byte is for flags. */
#define MQTT_বার্তা_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_বার্তা_PUBLISH    0x30
#define MQTT_বার্তা_SUBSCRIBE  0x82
#define MQTT_বার্তা_SUBACK     0x90
#define MQTT_বার্তা_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_বার্তা_PINGRESP   0xD0

#define MQTT_CONNACK_LEN  2
#define MQTT_SUBACK_LEN   3
#define MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#define CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

গণনা mqttstate {
  MQTT_FIRST,             /* 0 */
  MQTT_REMAINING_দৈর্ঘ্য,  /* 1 */
  MQTT_CONNACK,           /* 2 */
  MQTT_SUBACK,            /* 3 */
  MQTT_SUBACK_COMING,     /* 4 - the SUBACK remainder */
  MQTT_PUBWAIT,    /* 5 - wait for publish */
  MQTT_PUB_REMAIN,  /* 6 - wait for the remainder of the publish */
  MQTT_POST_DRAIN,        /* 7 - finish sending PUBLISH */
  MQTT_DISCONNECT_DRAIN,  /* 8 - finish sending DISCONNECT */

  MQTT_NOSTATE /* 9 - never used an actual state */
};

কাঠামো mqtt_conn {
  গণনা mqttstate state;
  গণনা mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned পূর্ণসংখ্যা packetid;
};

/* protocol-specific transfer-related data */
কাঠামো MQTT {
  কাঠামো dynbuf sendbuf;
  /* when receiving */
  কাঠামো dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_দৈর্ঘ্য;
  unsigned অক্ষর pkt_hd[4]; /* for decoding the arriving packet length */
  কাঠামো curltime lastTime; /* last time we sent or received data */
  unsigned অক্ষর firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

স্থির শূন্য mqtt_easy_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো MQTT *mq = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

স্থির শূন্য mqtt_conn_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_free(entry);
}

স্থির CURLcode mqtt_setup_conn(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  কাঠামো mqtt_conn *mqtt;
  কাঠামো MQTT *mq;

  mqtt = curlx_calloc(1, আকার(*mqtt));
  যদি(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, আকার(কাঠামো MQTT));
  যদি(!mq)
    ফেরত CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  যদি(Curl_meta_set(তথ্য, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;
  ফেরত CURLE_OK;
}

স্থির CURLcode mqtt_send(কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক অক্ষর *buf, size_t len)
{
  size_t n;
  CURLcode ফলাফল;
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);

  যদি(!mq)
    ফেরত CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  ফলাফল = Curl_xfer_send(তথ্য, buf, len, FALSE, &n);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  mq->lastTime = *Curl_pgrs_now(তথ্য);
  Curl_debug(তথ্য, CURLINFO_HEADER_OUT, buf, n);
  যদি(len != n)
    ফলাফল = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  ফেরত ফলাফল;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
স্থির CURLcode mqtt_flush(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode ফলাফল;

  যদি(!mq)
    ফেরত CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  যদি(!len)
    ফেরত CURLE_OK;

  ফলাফল = Curl_xfer_send(তথ্য, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  mq->lastTime = *Curl_pgrs_now(তথ্য);
  Curl_debug(তথ্য, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  ফেরত curlx_dyn_লেজ(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
স্থির CURLcode mqtt_pollset(কাঠামো Curl_easy *তথ্য,
                             কাঠামো easy_pollset *ps)
{
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  যদি(mq && curlx_dyn_len(&mq->sendbuf))
    ফেরত Curl_pollset_add_out(তথ্য, ps, তথ্য->conn->sock[FIRSTSOCKET]);
  ফেরত Curl_pollset_add_in(তথ্য, ps, তথ্য->conn->sock[FIRSTSOCKET]);
}

স্থির পূর্ণসংখ্যা mqtt_encode_len(অক্ষর *buf, size_t len)
{
  পূর্ণসংখ্যা i;

  জন্য(i = 0; (len > 0) && (i < 4); i++) {
    unsigned অক্ষর encoded;
    encoded = len % 0x80;
    len /= 0x80;
    যদি(len)
      encoded |= 0x80;
    buf[i] = (অক্ষর)encoded;
  }

  ফেরত i;
}

/* add the passwd to the CONNECT packet */
স্থির পূর্ণসংখ্যা add_passwd(ধ্রুবক অক্ষর *passwd, ধ্রুবক size_t plen,
                      অক্ষর *pkt, ধ্রুবক size_t start, পূর্ণসংখ্যা remain_pos)
{
  /* magic number that need to be set properly */
  ধ্রুবক size_t conn_flags_pos = remain_pos + 8;
  যদি(plen > 0xffff)
    ফেরত 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (অক্ষর)((plen >> 8) & 0xFF);
  pkt[start + 1] = (অক্ষর)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  ফেরত 0;
}

/* add user to the CONNECT packet */
স্থির পূর্ণসংখ্যা add_user(ধ্রুবক অক্ষর *username, ধ্রুবক size_t ulen,
                    unsigned অক্ষর *pkt, ধ্রুবক size_t start, পূর্ণসংখ্যা remain_pos)
{
  /* magic number that need to be set properly */
  ধ্রুবক size_t conn_flags_pos = remain_pos + 8;
  যদি(ulen > 0xffff)
    ফেরত 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned অক্ষর)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned অক্ষর)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  ফেরত 0;
}

/* add client ID to the CONNECT packet */
স্থির পূর্ণসংখ্যা add_client_id(ধ্রুবক অক্ষর *client_id, ধ্রুবক size_t client_id_len,
                         অক্ষর *pkt, ধ্রুবক size_t start)
{
  যদি(client_id_len != MQTT_CLIENTID_LEN)
    ফেরত 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  ফেরত 0;
}

/* Set initial values of CONNECT packet */
স্থির পূর্ণসংখ্যা init_connpack(অক্ষর *packet, অক্ষর *remain, পূর্ণসংখ্যা remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_বার্তা_CONNECT;
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
  ফেরত remain_pos + 10;
}

স্থির CURLcode mqtt_connect(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা pos = 0;
  পূর্ণসংখ্যা rc = 0;
  /* remain length */
  পূর্ণসংখ্যা remain_pos = 0;
  অক্ষর remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  অক্ষর client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ধ্রুবক size_t clen = CURL_CSTRLEN("curl");
  অক্ষর *packet = ফাঁকা;

  /* extracting username from request */
  কাঠামো Curl_creds *creds = তথ্য->state.creds;
  ধ্রুবক size_t ulen = creds ? strlen(creds->user) : 0;
  ধ্রুবক size_t plen = creds ? strlen(creds->passwd) : 0;
  ধ্রুবক size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
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
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  যদি(!packet)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  ফলাফল = Curl_rand_alnum(তথ্য, (unsigned অক্ষর *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  যদি(rc) {
    failf(তথ্য, "Client ID length mismatched: [%zu]", strlen(client_id));
    ফলাফল = CURLE_WEIRD_SERVER_REPLY;
    যাও end;
  }
  infof(তথ্য, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  যদি(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned অক্ষর *)packet, start_user, remain_pos);
    যদি(rc) {
      failf(তথ্য, "Username too long: [%zu]", ulen);
      ফলাফল = CURLE_WEIRD_SERVER_REPLY;
      যাও end;
    }
  }

  /* if passwd was provided, add it to the packet */
  যদি(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    যদি(rc) {
      failf(তথ্য, "Password too long: [%zu]", plen);
      ফলাফল = CURLE_WEIRD_SERVER_REPLY;
      যাও end;
    }
  }

  যদি(!ফলাফল)
    ফলাফল = mqtt_send(তথ্য, packet, packetlen);

end:
  যদি(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&তথ্য->state.creds);
  ফেরত ফলাফল;
}

স্থির CURLcode mqtt_disconnect(কাঠামো Curl_easy *তথ্য)
{
  ফেরত mqtt_send(তথ্য, "\xe0\x00", 2);
}

স্থির CURLcode mqtt_recv_atleast(কাঠামো Curl_easy *তথ্য, size_t nbytes)
{
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode ফলাফল;

  যদি(!mq)
    ফেরত CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  যদি(rlen < nbytes) {
    unsigned অক্ষর readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < আকার(readbuf));
    ফলাফল = Curl_xfer_recv(তথ্য, (অক্ষর *)readbuf, nbytes - rlen, &nread);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    যদি(!nread) /* EOF */
       ফেরত CURLE_RECV_ERROR;
    যদি(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      ফেরত CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  ফেরত (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

স্থির শূন্য mqtt_recv_consume(কাঠামো Curl_easy *তথ্য, size_t nbytes)
{
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  যদি(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    যদি(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    অন্যথা
      curlx_dyn_লেজ(&mq->recvbuf, rlen - nbytes);
  }
}

স্থির CURLcode mqtt_verify_connack(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  CURLcode ফলাফল;
  ধ্রুবক অক্ষর *ptr;

  DEBUGASSERT(mq);
  যদি(!mq)
    ফেরত CURLE_FAILED_INIT;
  যদি(mq->remaining_দৈর্ঘ্য != 2) {
    failf(তথ্য, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_দৈর্ঘ্য);
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }

  ফলাফল = mqtt_recv_atleast(তথ্য, MQTT_CONNACK_LEN);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(তথ্য, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  যদি(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(তথ্য, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned অক্ষর)ptr[0], (unsigned অক্ষর)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(তথ্য, MQTT_CONNACK_LEN);
  ফেরত CURLE_OK;
}

স্থির CURLcode mqtt_get_topic(কাঠামো Curl_easy *তথ্য,
                               অক্ষর **topic, size_t *topiclen)
{
  ধ্রুবক অক্ষর *path = তথ্য->state.up.path;
  CURLcode ফলাফল = CURLE_URL_MALFORMAT;
  যদি(strlen(path) > 1) {
    ফলাফল = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    যদি(!ফলাফল && (*topiclen > 0xffff)) {
      failf(তথ্য, "Too long MQTT topic");
      ফলাফল = CURLE_URL_MALFORMAT;
    }
  }
  অন্যথা
    failf(তথ্য, "No MQTT topic found. Forgot to URL encode it?");

  ফেরত ফলাফল;
}

স্থির CURLcode mqtt_subscribe(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল = CURLE_OK;
  অক্ষর *topic = ফাঁকা;
  size_t topiclen;
  unsigned অক্ষর *packet = ফাঁকা;
  size_t packetlen;
  অক্ষর encodedsize[4];
  size_t n;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  যদি(!mqtt)
    ফেরত CURLE_FAILED_INIT;

  ফলাফল = mqtt_get_topic(তথ্য, &topic, &topiclen);
  যদি(ফলাফল)
    যাও fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((অক্ষর *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  যদি(!packet) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও fail;
  }

  packet[0] = MQTT_বার্তা_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  ফলাফল = mqtt_send(তথ্য, (ধ্রুবক অক্ষর *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  ফেরত ফলাফল;
}

/*
 * Called when the first byte was already read.
 */
স্থির CURLcode mqtt_verify_suback(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode ফলাফল;
  ধ্রুবক অক্ষর *ptr;

  যদি(!mqtt || !mq)
    ফেরত CURLE_FAILED_INIT;

  যদি(mq->remaining_দৈর্ঘ্য != 3) {
    failf(তথ্য, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_দৈর্ঘ্য);
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }

  ফলাফল = mqtt_recv_atleast(তথ্য, MQTT_SUBACK_LEN);
  যদি(ফলাফল)
    যাও fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(তথ্য, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  যদি(((unsigned অক্ষর)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned অক্ষর)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    ফলাফল = CURLE_WEIRD_SERVER_REPLY;
    যাও fail;
  }
  mqtt_recv_consume(তথ্য, MQTT_SUBACK_LEN);
fail:
  ফেরত ফলাফল;
}

#define MAX_MQTT_MESSAGE_আকার 0xFFFFFFF

স্থির CURLcode mqtt_publish(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;
  অক্ষর *payload = তথ্য->set.postfields;
  size_t payloadlen;
  অক্ষর *topic = ফাঁকা;
  size_t topiclen;
  unsigned অক্ষর *pkt = ফাঁকা;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  অক্ষর encodedbytes[4];
  curl_off_t postfieldsize = তথ্য->set.postfieldsize;

  যদি(!payload) {
    DEBUGF(infof(তথ্য, "mqtt_publish without payload, return bad arg"));
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
  যদি(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    যদি(postfieldsize > 0) /* off_t does not fit into size_t */
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  ফলাফল = mqtt_get_topic(তথ্য, &topic, &topiclen);
  যদি(ফলাফল)
    যাও fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  যদি((SIZE_MAX - topiclen - 2) < payloadlen) {
    ফলাফল = CURLE_TOO_LARGE;
    যাও fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  যদি(remaininglength > (MAX_MQTT_MESSAGE_আকার - encodelen - 1)) {
    ফলাফল = CURLE_TOO_LARGE;
    যাও fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  যদি(!pkt) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_বার্তা_PUBLISH;
  memcpy(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  memcpy(&pkt[i], topic, topiclen);
  i += topiclen;
  memcpy(&pkt[i], payload, payloadlen);
  i += payloadlen;
  ফলাফল = mqtt_send(তথ্য, (ধ্রুবক অক্ষর *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  ফেরত ফলাফল;
}

/* return FALSE on success, TRUE on error */
স্থির bool mqtt_decode_len(size_t *lenp, ধ্রুবক unsigned অক্ষর *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned অক্ষর encoded = 128;

  জন্য(i = 0; (i < buflen) && (encoded & 128); i++) {
    যদি(i == 4)
      ফেরত TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  যদি(encoded & 128)
    /* truncated size */
    ফেরত TRUE;

  *lenp = len;
  ফেরত FALSE;
}

#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
স্থির ধ্রুবক অক্ষর * ধ্রুবক statenames[] = {
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
স্থির শূন্য mqstate(কাঠামো Curl_easy *তথ্য,
                    গণনা mqttstate state,
                    গণনা mqttstate nextstate) /* used if state == FIRST */
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  যদি(!mqtt)
    ফেরত;
#ifdef DEBUGBUILD
  infof(তথ্য, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  যদি(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

স্থির CURLcode mqtt_read_publish(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  size_t nread;
  size_t remlen;
  কাঠামো mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  unsigned অক্ষর packet;

  DEBUGASSERT(mqtt);
  যদি(!mqtt || !mq)
    ফেরত CURLE_FAILED_INIT;

  নির্বাচন(mqtt->state) {
MQTT_SUBACK_COMING:
  ক্ষেত্র MQTT_SUBACK_COMING:
    ফলাফল = mqtt_verify_suback(তথ্য);
    যদি(ফলাফল)
      বিরতি;

    mqstate(তথ্য, MQTT_FIRST, MQTT_PUBWAIT);
    বিরতি;

  ক্ষেত্র MQTT_SUBACK:
  ক্ষেত্র MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    যদি(packet == MQTT_বার্তা_PUBLISH)
      mqstate(তথ্য, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    অন্যথা যদি(packet == MQTT_বার্তা_SUBACK) {
      mqstate(তথ্য, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      যাও MQTT_SUBACK_COMING;
    }
    অন্যথা যদি(packet == MQTT_বার্তা_DISCONNECT) {
      infof(তথ্য, "Got DISCONNECT");
      *done = TRUE;
      যাও end;
    }
    অন্যথা {
      ফলাফল = CURLE_WEIRD_SERVER_REPLY;
      যাও end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_দৈর্ঘ্য;
    infof(তথ্য, "Remaining length: %zu bytes", remlen);
    যদি(তথ্য->set.max_filesize &&
       (curl_off_t)remlen > তথ্য->set.max_filesize) {
      failf(তথ্য, "Maximum file size exceeded");
      ফলাফল = CURLE_FILESIZE_EXCEEDED;
      যাও end;
    }
    Curl_pgrs_Set_Download_আকার(তথ্য, remlen);
    তথ্য->req.bytecount = 0;
    তথ্য->req.আকার_2 = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  ক্ষেত্র MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    অক্ষর বাফার[4 * 1024];
    size_t rest = mq->npacket;
    যদি(rest > আকার(বাফার))
      rest = আকার(বাফার);
    ফলাফল = Curl_xfer_recv(তথ্য, বাফার, rest, &nread);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_AGAIN) {
        infof(তথ্য, "EEEE AAAAGAIN");
      }
      যাও end;
    }
    যদি(!nread) {
      infof(তথ্য, "server disconnected");
      ফলাফল = CURLE_PARTIAL_FILE;
      যাও end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(তথ্য);

    /* if QoS is set, message contains packet id */
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, বাফার, nread);
    যদি(ফলাফল)
      যাও end;

    mq->npacket -= nread;
    যদি(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(তথ্য, MQTT_FIRST, MQTT_PUBWAIT);
    বিরতি;
  }
  পূর্বনির্ধারিত:
    DEBUGASSERT(ফাঁকা); /* illegal state */
    ফলাফল = CURLE_WEIRD_SERVER_REPLY;
    যাও end;
  }
end:
  ফেরত ফলাফল;
}

স্থির CURLcode mqtt_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  CURLcode ফলাফল = CURLE_OK;
  *done = FALSE; /* unconditionally */

  যদি(!mq)
    ফেরত CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(তথ্য);
  mq->pingsent = FALSE;

  ফলাফল = mqtt_connect(তথ্য);
  যদি(ফলাফল) {
    failf(তথ্য, "Error %d sending MQTT CONNECT request", (পূর্ণসংখ্যা)ফলাফল);
    ফেরত ফলাফল;
  }
  mqstate(তথ্য, MQTT_FIRST, MQTT_CONNACK);
  ফেরত CURLE_OK;
}

স্থির CURLcode mqtt_done(কাঠামো Curl_easy *তথ্য,
                          CURLcode status, bool premature)
{
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  (শূন্য)status;
  (শূন্য)premature;
  যদি(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  ফেরত CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
স্থির CURLcode mqtt_ping(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  যদি(!mqtt || !mq)
    ফেরত CURLE_FAILED_INIT;

  যদি(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     তথ্য->set.upkeep_interval_ms > 0) {
    কাঠামো curltime t = *Curl_pgrs_now(তথ্য);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    যদি(diff > তথ্য->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned অক্ষর packet[2] = { 0xC0, 0x00 };
      size_t packetlen = আকার(packet);

      ফলাফল = mqtt_send(তথ্য, (অক্ষর *)packet, packetlen);
      যদি(!ফলাফল) {
        mq->pingsent = TRUE;
      }
      infof(তথ্য, "mqtt_ping: sent ping request.");
    }
  }
  ফেরত ফলাফল;
}

স্থির CURLcode mqtt_doing(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো MQTT *mq = Curl_meta_get(তথ্য, CURL_META_MQTT_EASY);
  CURLcode ফলাফল = CURLE_OK;
  size_t nread;
  unsigned অক্ষর recvbyte;
  কাঠামো mqtt_conn *mqtt = Curl_conn_meta_get(তথ্য->conn, CURL_META_MQTT_CONN);

  যদি(!mqtt || !mq)
    ফেরত CURLE_FAILED_INIT;

  *done = FALSE;

  যদি(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    ফলাফল = mqtt_flush(তথ্য);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    যদি(ফলাফল || curlx_dyn_len(&mq->sendbuf))
      ফেরত ফলাফল;
  }

  ফলাফল = mqtt_ping(তথ্য);
  /* Packet processing may send more output, so finish PINGREQ first. */
  যদি(ফলাফল || curlx_dyn_len(&mq->sendbuf))
    ফেরত ফলাফল;

  infof(তথ্য, "mqtt_doing: state [%d]", (পূর্ণসংখ্যা)mqtt->state);
  নির্বাচন(mqtt->state) {
  ক্ষেত্র MQTT_FIRST:
    /* Read the initial byte only */
    ফলাফল = Curl_xfer_recv(তথ্য, (অক্ষর *)&mq->firstbyte, 1, &nread);
    যদি(ফলাফল)
      বিরতি;
    অন্যথা যদি(!nread) {
      failf(তথ্য, "Connection disconnected");
      *done = TRUE;
      ফলাফল = CURLE_RECV_ERROR;
      বিরতি;
    }
    Curl_debug(তথ্য, CURLINFO_HEADER_IN, (ধ্রুবক অক্ষর *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(তথ্য);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(তথ্য, MQTT_REMAINING_দৈর্ঘ্য, MQTT_NOSTATE);
    FALLTHROUGH();
  ক্ষেত্র MQTT_REMAINING_দৈর্ঘ্য:
    করো {
      ফলাফল = Curl_xfer_recv(তথ্য, (অক্ষর *)&recvbyte, 1, &nread);
      যদি(ফলাফল || !nread)
        বিরতি;
      Curl_debug(তথ্য, CURLINFO_HEADER_IN, (ধ্রুবক অক্ষর *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } যখন((recvbyte & 0x80) && (mq->npacket < 4));
    যদি(!ফলাফল && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      ফলাফল = CURLE_WEIRD_SERVER_REPLY;
    যদি(ফলাফল)
      বিরতি;
    যদি(mqtt_decode_len(&mq->remaining_দৈর্ঘ্য, mq->pkt_hd, mq->npacket)) {
      ফলাফল = CURLE_WEIRD_SERVER_REPLY;
      বিরতি;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ধ্রুবক unsigned অক্ষর type = mq->firstbyte & 0xF0;
      ধ্রুবক unsigned অক্ষর reserved = mq->firstbyte & 0x0F;
      যদি((type == MQTT_বার্তা_DISCONNECT || type == MQTT_বার্তা_PINGRESP) &&
         (mq->remaining_দৈর্ঘ্য || reserved)) {
        failf(তথ্য,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_বার্তা_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_দৈর্ঘ্য, mq->firstbyte);
        ফলাফল = CURLE_WEIRD_SERVER_REPLY;
        বিরতি;
      }
    }
    যদি(mq->remaining_দৈর্ঘ্য) {
      mqstate(তথ্য, mqtt->nextstate, MQTT_NOSTATE);
      বিরতি;
    }
    mqstate(তথ্য, MQTT_FIRST, MQTT_FIRST);

    যদি(mq->firstbyte == MQTT_বার্তা_DISCONNECT) {
      infof(তথ্য, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    যদি(mq->firstbyte == MQTT_বার্তা_PINGRESP) {
      infof(তথ্য, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(তথ্য, MQTT_FIRST, MQTT_PUBWAIT);
    }
    বিরতি;
  ক্ষেত্র MQTT_CONNACK:
    ফলাফল = mqtt_verify_connack(তথ্য);
    যদি(ফলাফল)
      বিরতি;

    যদি(তথ্য->state.httpreq != HTTPREQ_POST) {
      ফলাফল = mqtt_subscribe(তথ্য);
      যদি(!ফলাফল)
        mqstate(তথ্য, MQTT_FIRST, MQTT_SUBACK);
      বিরতি;
    }

    ফলাফল = mqtt_publish(তথ্য);
    যদি(ফলাফল)
      বিরতি;
    mqstate(তথ্য, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  ক্ষেত্র MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    যদি(curlx_dyn_len(&mq->sendbuf))
      বিরতি;
    ফলাফল = mqtt_disconnect(তথ্য);
    যদি(ফলাফল)
      বিরতি;
    mqstate(তথ্য, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  ক্ষেত্র MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    যদি(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    বিরতি;

  ক্ষেত্র MQTT_SUBACK:
  ক্ষেত্র MQTT_PUBWAIT:
  ক্ষেত্র MQTT_PUB_REMAIN:
    ফলাফল = mqtt_read_publish(তথ্য, done);
    বিরতি;

  পূর্বনির্ধারিত:
    failf(তথ্য, "State not handled yet");
    *done = TRUE;
    বিরতি;
  }

  যদি(ফলাফল == CURLE_AGAIN)
    ফলাফল = CURLE_OK;
  ফেরত ফলাফল;
}

#ifdef USE_SSL

স্থির CURLcode mqtts_connecting(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল;

  ফলাফল = Curl_conn_connect(তথ্য, FIRSTSOCKET, TRUE, done);
  যদি(ফলাফল)
    connclose(conn);
  ফেরত ফলাফল;
}

/*
 * MQTTS protocol.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_mqtts = {
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
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_mqtt = {
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
