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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"

#ifndef CURL_DISABLE_MQTT

#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "mqtt.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"

/* first byte is command.
   second byte is for flags. */
#define MQTT_സന്ദേശം_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#define MQTT_സന്ദേശം_PUBLISH    0x30
#define MQTT_സന്ദേശം_SUBSCRIBE  0x82
#define MQTT_സന്ദേശം_SUBACK     0x90
#define MQTT_സന്ദേശം_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#define MQTT_സന്ദേശം_PINGRESP   0xD0

#define MQTT_CONNACK_LEN  2
#define MQTT_SUBACK_LEN   3
#define MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#define CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

എണ്ണൽ mqttstate {
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

ഘടന mqtt_conn {
  എണ്ണൽ mqttstate state;
  എണ്ണൽ mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned പൂർണ്ണസംഖ്യ packetid;
};

/* protocol-specific transfer-related data */
ഘടന MQTT {
  ഘടന dynbuf sendbuf;
  /* when receiving */
  ഘടന dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned അക്ഷരം pkt_hd[4]; /* for decoding the arriving packet length */
  ഘടന curltime lastTime; /* last time we sent or received data */
  unsigned അക്ഷരം firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

സ്ഥിതം ശൂന്യം mqtt_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന MQTT *mq = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

സ്ഥിതം ശൂന്യം mqtt_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  curlx_free(entry);
}

സ്ഥിതം CURLcode mqtt_setup_conn(ഘടന Curl_easy *data,
                                ഘടന connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  ഘടന mqtt_conn *mqtt;
  ഘടന MQTT *mq;

  mqtt = curlx_calloc(1, വലുപ്പം(*mqtt));
  എങ്കിൽ(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, വലുപ്പം(ഘടന MQTT));
  എങ്കിൽ(!mq)
    തിരികെ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  എങ്കിൽ(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode mqtt_send(ഘടന Curl_easy *data,
                          സ്ഥിരം അക്ഷരം *buf, size_t len)
{
  size_t n;
  CURLcode result;
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  എങ്കിൽ(!mq)
    തിരികെ CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  എങ്കിൽ(result)
    തിരികെ result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  എങ്കിൽ(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  തിരികെ result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
സ്ഥിതം CURLcode mqtt_flush(ഘടന Curl_easy *data)
{
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  എങ്കിൽ(!mq)
    തിരികെ CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  എങ്കിൽ(!len)
    തിരികെ CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  എങ്കിൽ(result)
    തിരികെ result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  തിരികെ curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
സ്ഥിതം CURLcode mqtt_pollset(ഘടന Curl_easy *data,
                             ഘടന easy_pollset *ps)
{
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  എങ്കിൽ(mq && curlx_dyn_len(&mq->sendbuf))
    തിരികെ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  തിരികെ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mqtt_encode_len(അക്ഷരം *buf, size_t len)
{
  പൂർണ്ണസംഖ്യ i;

  വേണ്ടി(i = 0; (len > 0) && (i < 4); i++) {
    unsigned അക്ഷരം encoded;
    encoded = len % 0x80;
    len /= 0x80;
    എങ്കിൽ(len)
      encoded |= 0x80;
    buf[i] = (അക്ഷരം)encoded;
  }

  തിരികെ i;
}

/* add the passwd to the CONNECT packet */
സ്ഥിതം പൂർണ്ണസംഖ്യ add_passwd(സ്ഥിരം അക്ഷരം *passwd, സ്ഥിരം size_t plen,
                      അക്ഷരം *pkt, സ്ഥിരം size_t start, പൂർണ്ണസംഖ്യ remain_pos)
{
  /* magic number that need to be set properly */
  സ്ഥിരം size_t conn_flags_pos = remain_pos + 8;
  എങ്കിൽ(plen > 0xffff)
    തിരികെ 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (അക്ഷരം)((plen >> 8) & 0xFF);
  pkt[start + 1] = (അക്ഷരം)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  തിരികെ 0;
}

/* add user to the CONNECT packet */
സ്ഥിതം പൂർണ്ണസംഖ്യ add_user(സ്ഥിരം അക്ഷരം *username, സ്ഥിരം size_t ulen,
                    unsigned അക്ഷരം *pkt, സ്ഥിരം size_t start, പൂർണ്ണസംഖ്യ remain_pos)
{
  /* magic number that need to be set properly */
  സ്ഥിരം size_t conn_flags_pos = remain_pos + 8;
  എങ്കിൽ(ulen > 0xffff)
    തിരികെ 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned അക്ഷരം)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned അക്ഷരം)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  തിരികെ 0;
}

/* add client ID to the CONNECT packet */
സ്ഥിതം പൂർണ്ണസംഖ്യ add_client_id(സ്ഥിരം അക്ഷരം *client_id, സ്ഥിരം size_t client_id_len,
                         അക്ഷരം *pkt, സ്ഥിരം size_t start)
{
  എങ്കിൽ(client_id_len != MQTT_CLIENTID_LEN)
    തിരികെ 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  തിരികെ 0;
}

/* Set initial values of CONNECT packet */
സ്ഥിതം പൂർണ്ണസംഖ്യ init_connpack(അക്ഷരം *packet, അക്ഷരം *remain, പൂർണ്ണസംഖ്യ remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_സന്ദേശം_CONNECT;
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
  തിരികെ remain_pos + 10;
}

സ്ഥിതം CURLcode mqtt_connect(ഘടന Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ pos = 0;
  പൂർണ്ണസംഖ്യ rc = 0;
  /* remain length */
  പൂർണ്ണസംഖ്യ remain_pos = 0;
  അക്ഷരം remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  അക്ഷരം client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  സ്ഥിരം size_t clen = CURL_CSTRLEN("curl");
  അക്ഷരം *packet = ശൂന്യ;

  /* extracting username from request */
  ഘടന Curl_creds *creds = data->state.creds;
  സ്ഥിരം size_t ulen = creds ? strlen(creds->user) : 0;
  സ്ഥിരം size_t plen = creds ? strlen(creds->passwd) : 0;
  സ്ഥിരം size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  എങ്കിൽ(packetlen > 0xFFFFFFF)
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  എങ്കിൽ(!packet)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned അക്ഷരം *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  എങ്കിൽ(rc) {
    failf(data, "Client ID length mismatched: [%zu]", strlen(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    പോകുക end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  എങ്കിൽ(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned അക്ഷരം *)packet, start_user, remain_pos);
    എങ്കിൽ(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      പോകുക end;
    }
  }

  /* if passwd was provided, add it to the packet */
  എങ്കിൽ(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    എങ്കിൽ(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      പോകുക end;
    }
  }

  എങ്കിൽ(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  എങ്കിൽ(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  തിരികെ result;
}

സ്ഥിതം CURLcode mqtt_disconnect(ഘടന Curl_easy *data)
{
  തിരികെ mqtt_send(data, "\xe0\x00", 2);
}

സ്ഥിതം CURLcode mqtt_recv_atleast(ഘടന Curl_easy *data, size_t nbytes)
{
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  എങ്കിൽ(!mq)
    തിരികെ CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  എങ്കിൽ(rlen < nbytes) {
    unsigned അക്ഷരം readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < വലുപ്പം(readbuf));
    result = Curl_xfer_recv(data, (അക്ഷരം *)readbuf, nbytes - rlen, &nread);
    എങ്കിൽ(result)
      തിരികെ result;
    എങ്കിൽ(!nread) /* EOF */
       തിരികെ CURLE_RECV_ERROR;
    എങ്കിൽ(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      തിരികെ CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  തിരികെ (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

സ്ഥിതം ശൂന്യം mqtt_recv_consume(ഘടന Curl_easy *data, size_t nbytes)
{
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  എങ്കിൽ(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    എങ്കിൽ(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    അല്ലെങ്കിൽ
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

സ്ഥിതം CURLcode mqtt_verify_connack(ഘടന Curl_easy *data)
{
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  സ്ഥിരം അക്ഷരം *ptr;

  DEBUGASSERT(mq);
  എങ്കിൽ(!mq)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  എങ്കിൽ(result)
    തിരികെ result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  എങ്കിൽ(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned അക്ഷരം)ptr[0], (unsigned അക്ഷരം)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode mqtt_get_topic(ഘടന Curl_easy *data,
                               അക്ഷരം **topic, size_t *topiclen)
{
  സ്ഥിരം അക്ഷരം *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  എങ്കിൽ(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    എങ്കിൽ(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  അല്ലെങ്കിൽ
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  തിരികെ result;
}

സ്ഥിതം CURLcode mqtt_subscribe(ഘടന Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  അക്ഷരം *topic = ശൂന്യ;
  size_t topiclen;
  unsigned അക്ഷരം *packet = ശൂന്യ;
  size_t packetlen;
  അക്ഷരം encodedsize[4];
  size_t n;
  ഘടന connectdata *conn = data->conn;
  ഘടന mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  എങ്കിൽ(!mqtt)
    തിരികെ CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  എങ്കിൽ(result)
    പോകുക fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((അക്ഷരം *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  എങ്കിൽ(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക fail;
  }

  packet[0] = MQTT_സന്ദേശം_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (സ്ഥിരം അക്ഷരം *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  തിരികെ result;
}

/*
 * Called when the first byte was already read.
 */
സ്ഥിതം CURLcode mqtt_verify_suback(ഘടന Curl_easy *data)
{
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  ഘടന connectdata *conn = data->conn;
  ഘടന mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  സ്ഥിരം അക്ഷരം *ptr;

  എങ്കിൽ(!mqtt || !mq)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  എങ്കിൽ(result)
    പോകുക fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  എങ്കിൽ(((unsigned അക്ഷരം)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned അക്ഷരം)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    പോകുക fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  തിരികെ result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

സ്ഥിതം CURLcode mqtt_publish(ഘടന Curl_easy *data)
{
  CURLcode result;
  അക്ഷരം *payload = data->set.postfields;
  size_t payloadlen;
  അക്ഷരം *topic = ശൂന്യ;
  size_t topiclen;
  unsigned അക്ഷരം *pkt = ശൂന്യ;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  അക്ഷരം encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  എങ്കിൽ(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  എങ്കിൽ(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    എങ്കിൽ(postfieldsize > 0) /* off_t does not fit into size_t */
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  എങ്കിൽ(result)
    പോകുക fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  എങ്കിൽ((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    പോകുക fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  എങ്കിൽ(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    പോകുക fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  എങ്കിൽ(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_സന്ദേശം_PUBLISH;
  memcpy(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  memcpy(&pkt[i], topic, topiclen);
  i += topiclen;
  memcpy(&pkt[i], payload, payloadlen);
  i += payloadlen;
  result = mqtt_send(data, (സ്ഥിരം അക്ഷരം *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  തിരികെ result;
}

/* return FALSE on success, TRUE on error */
സ്ഥിതം bool mqtt_decode_len(size_t *lenp, സ്ഥിരം unsigned അക്ഷരം *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned അക്ഷരം encoded = 128;

  വേണ്ടി(i = 0; (i < buflen) && (encoded & 128); i++) {
    എങ്കിൽ(i == 4)
      തിരികെ TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  എങ്കിൽ(encoded & 128)
    /* truncated size */
    തിരികെ TRUE;

  *lenp = len;
  തിരികെ FALSE;
}

#എങ്കിൽ defined(DEBUGBUILD) && defined(CURLVERBOSE)
സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം statenames[] = {
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
സ്ഥിതം ശൂന്യം mqstate(ഘടന Curl_easy *data,
                    എണ്ണൽ mqttstate state,
                    എണ്ണൽ mqttstate nextstate) /* used if state == FIRST */
{
  ഘടന connectdata *conn = data->conn;
  ഘടന mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  എങ്കിൽ(!mqtt)
    തിരികെ;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  എങ്കിൽ(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

സ്ഥിതം CURLcode mqtt_read_publish(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  ഘടന mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned അക്ഷരം packet;

  DEBUGASSERT(mqtt);
  എങ്കിൽ(!mqtt || !mq)
    തിരികെ CURLE_FAILED_INIT;

  തിരഞ്ഞെടുപ്പ്(mqtt->state) {
MQTT_SUBACK_COMING:
  അവസ്ഥ MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    എങ്കിൽ(result)
      നിർത്തുക;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    നിർത്തുക;

  അവസ്ഥ MQTT_SUBACK:
  അവസ്ഥ MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    എങ്കിൽ(packet == MQTT_സന്ദേശം_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    അല്ലെങ്കിൽ എങ്കിൽ(packet == MQTT_സന്ദേശം_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      പോകുക MQTT_SUBACK_COMING;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(packet == MQTT_സന്ദേശം_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      പോകുക end;
    }
    അല്ലെങ്കിൽ {
      result = CURLE_WEIRD_SERVER_REPLY;
      പോകുക end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    എങ്കിൽ(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      പോകുക end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  അവസ്ഥ MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    അക്ഷരം buffer[4 * 1024];
    size_t rest = mq->npacket;
    എങ്കിൽ(rest > വലുപ്പം(buffer))
      rest = വലുപ്പം(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      പോകുക end;
    }
    എങ്കിൽ(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      പോകുക end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    എങ്കിൽ(result)
      പോകുക end;

    mq->npacket -= nread;
    എങ്കിൽ(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    നിർത്തുക;
  }
  സ്ഥിരസ്ഥിതി:
    DEBUGASSERT(ശൂന്യ); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    പോകുക end;
  }
end:
  തിരികെ result;
}

സ്ഥിതം CURLcode mqtt_do(ഘടന Curl_easy *data, bool *done)
{
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  എങ്കിൽ(!mq)
    തിരികെ CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  എങ്കിൽ(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (പൂർണ്ണസംഖ്യ)result);
    തിരികെ result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode mqtt_done(ഘടന Curl_easy *data,
                          CURLcode status, bool premature)
{
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (ശൂന്യം)status;
  (ശൂന്യം)premature;
  എങ്കിൽ(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  തിരികെ CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
സ്ഥിതം CURLcode mqtt_ping(ഘടന Curl_easy *data)
{
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  എങ്കിൽ(!mqtt || !mq)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    ഘടന curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    എങ്കിൽ(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned അക്ഷരം packet[2] = { 0xC0, 0x00 };
      size_t packetlen = വലുപ്പം(packet);

      result = mqtt_send(data, (അക്ഷരം *)packet, packetlen);
      എങ്കിൽ(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode mqtt_doing(ഘടന Curl_easy *data, bool *done)
{
  ഘടന MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned അക്ഷരം recvbyte;
  ഘടന mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  എങ്കിൽ(!mqtt || !mq)
    തിരികെ CURLE_FAILED_INIT;

  *done = FALSE;

  എങ്കിൽ(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    എങ്കിൽ(result || curlx_dyn_len(&mq->sendbuf))
      തിരികെ result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  എങ്കിൽ(result || curlx_dyn_len(&mq->sendbuf))
    തിരികെ result;

  infof(data, "mqtt_doing: state [%d]", (പൂർണ്ണസംഖ്യ)mqtt->state);
  തിരഞ്ഞെടുപ്പ്(mqtt->state) {
  അവസ്ഥ MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (അക്ഷരം *)&mq->firstbyte, 1, &nread);
    എങ്കിൽ(result)
      നിർത്തുക;
    അല്ലെങ്കിൽ എങ്കിൽ(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      നിർത്തുക;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (സ്ഥിരം അക്ഷരം *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  അവസ്ഥ MQTT_REMAINING_LENGTH:
    ചെയ്യുക {
      result = Curl_xfer_recv(data, (അക്ഷരം *)&recvbyte, 1, &nread);
      എങ്കിൽ(result || !nread)
        നിർത്തുക;
      Curl_debug(data, CURLINFO_HEADER_IN, (സ്ഥിരം അക്ഷരം *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } ആകുമ്പോൾ((recvbyte & 0x80) && (mq->npacket < 4));
    എങ്കിൽ(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    എങ്കിൽ(result)
      നിർത്തുക;
    എങ്കിൽ(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      നിർത്തുക;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      സ്ഥിരം unsigned അക്ഷരം type = mq->firstbyte & 0xF0;
      സ്ഥിരം unsigned അക്ഷരം reserved = mq->firstbyte & 0x0F;
      എങ്കിൽ((type == MQTT_സന്ദേശം_DISCONNECT || type == MQTT_സന്ദേശം_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_സന്ദേശം_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        നിർത്തുക;
      }
    }
    എങ്കിൽ(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      നിർത്തുക;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    എങ്കിൽ(mq->firstbyte == MQTT_സന്ദേശം_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    എങ്കിൽ(mq->firstbyte == MQTT_സന്ദേശം_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    നിർത്തുക;
  അവസ്ഥ MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    എങ്കിൽ(result)
      നിർത്തുക;

    എങ്കിൽ(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      എങ്കിൽ(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      നിർത്തുക;
    }

    result = mqtt_publish(data);
    എങ്കിൽ(result)
      നിർത്തുക;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  അവസ്ഥ MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    എങ്കിൽ(curlx_dyn_len(&mq->sendbuf))
      നിർത്തുക;
    result = mqtt_disconnect(data);
    എങ്കിൽ(result)
      നിർത്തുക;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  അവസ്ഥ MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    എങ്കിൽ(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    നിർത്തുക;

  അവസ്ഥ MQTT_SUBACK:
  അവസ്ഥ MQTT_PUBWAIT:
  അവസ്ഥ MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    failf(data, "State not handled yet");
    *done = TRUE;
    നിർത്തുക;
  }

  എങ്കിൽ(result == CURLE_AGAIN)
    result = CURLE_OK;
  തിരികെ result;
}

#ifdef USE_SSL

സ്ഥിതം CURLcode mqtts_connecting(ഘടന Curl_easy *data, bool *done)
{
  ഘടന connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  എങ്കിൽ(result)
    connclose(conn);
  തിരികെ result;
}

/*
 * MQTTS protocol.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_mqtts = {
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
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_mqtt = {
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
