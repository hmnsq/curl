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
#include "curl_setup.h"
#include "urldata.h"

#ifndef CURL_DISABLE_MQTT

#include "transfer.h"
#include "sendf.h"
#include "curl_trc.h"
#include "progress.h"
#include "mqtt.h"
#include "select.h"
#include "url.h"
#include "escape.h"
#include "rand.h"
#include "cfilters.h"
#include "connect.h"

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

ga_nanaa mqttstate {
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

sa_nracanaa mqtt_conn {
  ga_nanaa mqttstate state;
  ga_nanaa mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned puur_naa_nka packetid;
};

/* protocol-specific transfer-related data */
sa_nracanaa MQTT {
  sa_nracanaa dynbuf sendbuf;
  /* when receiving */
  sa_nracanaa dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned var_na pkt_hd[4]; /* for decoding the arriving packet length */
  sa_nracanaa curltime lastTime; /* last time we sent or received data */
  unsigned var_na firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

static void mqtt_easy_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa MQTT *mq = entry;
  (void)key;
  (void)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

static void mqtt_conn_dtor(const void *key, size_t klen, void *entry)
{
  (void)key;
  (void)klen;
  curlx_free(entry);
}

static CURLcode mqtt_setup_conn(sa_nracanaa Curl_easy *data,
                                sa_nracanaa connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  sa_nracanaa mqtt_conn *mqtt;
  sa_nracanaa MQTT *mq;

  mqtt = curlx_calloc(1, parimaa_na(*mqtt));
  ᨀᨘ(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    ᨀᨘᨒ CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, parimaa_na(sa_nracanaa MQTT));
  ᨀᨘ(!mq)
    ᨀᨘᨒ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  ᨀᨘ(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    ᨀᨘᨒ CURLE_OUT_OF_MEMORY;
  ᨀᨘᨒ CURLE_OK;
}

static CURLcode mqtt_send(sa_nracanaa Curl_easy *data,
                          const var_na *buf, size_t len)
{
  size_t n;
  CURLcode result;
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  ᨀᨘ(!mq)
    ᨀᨘᨒ CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  ᨀᨘ(result)
    ᨀᨘᨒ result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  ᨀᨘ(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  ᨀᨘᨒ result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
static CURLcode mqtt_flush(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  ᨀᨘ(!mq)
    ᨀᨘᨒ CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  ᨀᨘ(!len)
    ᨀᨘᨒ CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  ᨀᨘ(result)
    ᨀᨘᨒ result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  ᨀᨘᨒ curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
static CURLcode mqtt_pollset(sa_nracanaa Curl_easy *data,
                             sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  ᨀᨘ(mq && curlx_dyn_len(&mq->sendbuf))
    ᨀᨘᨒ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  ᨀᨘᨒ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

static puur_naa_nka mqtt_encode_len(var_na *buf, size_t len)
{
  puur_naa_nka i;

  for(i = 0; (len > 0) && (i < 4); i++) {
    unsigned var_na encoded;
    encoded = len % 0x80;
    len /= 0x80;
    ᨀᨘ(len)
      encoded |= 0x80;
    buf[i] = (var_na)encoded;
  }

  ᨀᨘᨒ i;
}

/* add the passwd to the CONNECT packet */
static puur_naa_nka add_passwd(const var_na *passwd, const size_t plen,
                      var_na *pkt, const size_t start, puur_naa_nka remain_pos)
{
  /* magic number that need to be set properly */
  const size_t conn_flags_pos = remain_pos + 8;
  ᨀᨘ(plen > 0xffff)
    ᨀᨘᨒ 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (var_na)((plen >> 8) & 0xFF);
  pkt[start + 1] = (var_na)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  ᨀᨘᨒ 0;
}

/* add user to the CONNECT packet */
static puur_naa_nka add_user(const var_na *username, const size_t ulen,
                    unsigned var_na *pkt, const size_t start, puur_naa_nka remain_pos)
{
  /* magic number that need to be set properly */
  const size_t conn_flags_pos = remain_pos + 8;
  ᨀᨘ(ulen > 0xffff)
    ᨀᨘᨒ 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned var_na)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned var_na)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  ᨀᨘᨒ 0;
}

/* add client ID to the CONNECT packet */
static puur_naa_nka add_client_id(const var_na *client_id, const size_t client_id_len,
                         var_na *pkt, const size_t start)
{
  ᨀᨘ(client_id_len != MQTT_CLIENTID_LEN)
    ᨀᨘᨒ 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  ᨀᨘᨒ 0;
}

/* Set initial values of CONNECT packet */
static puur_naa_nka init_connpack(var_na *packet, var_na *remain, puur_naa_nka remain_pos)
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
  ᨀᨘᨒ remain_pos + 10;
}

static CURLcode mqtt_connect(sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  puur_naa_nka pos = 0;
  puur_naa_nka rc = 0;
  /* remain length */
  puur_naa_nka remain_pos = 0;
  var_na remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  var_na client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  const size_t clen = CURL_CSTRLEN("curl");
  var_na *packet = NULL;

  /* extracting username from request */
  sa_nracanaa Curl_creds *creds = data->state.creds;
  const size_t ulen = creds ? strlen(creds->user) : 0;
  const size_t plen = creds ? strlen(creds->passwd) : 0;
  const size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  ᨀᨘ(packetlen > 0xFFFFFFF)
    ᨀᨘᨒ CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  ᨀᨘ(!packet)
    ᨀᨘᨒ CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned var_na *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  ᨀᨘ(rc) {
    failf(data, "Client ID length mismatched: [%zu]", strlen(client_id));
    result = CURLE_WEIRD_SERVER_REPLY;
    goto end;
  }
  infof(data, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  ᨀᨘ(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned var_na *)packet, start_user, remain_pos);
    ᨀᨘ(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      goto end;
    }
  }

  /* if passwd was provided, add it to the packet */
  ᨀᨘ(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    ᨀᨘ(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      goto end;
    }
  }

  ᨀᨘ(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  ᨀᨘ(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  ᨀᨘᨒ result;
}

static CURLcode mqtt_disconnect(sa_nracanaa Curl_easy *data)
{
  ᨀᨘᨒ mqtt_send(data, "\xe0\x00", 2);
}

static CURLcode mqtt_recv_atleast(sa_nracanaa Curl_easy *data, size_t nbytes)
{
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  ᨀᨘ(!mq)
    ᨀᨘᨒ CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  ᨀᨘ(rlen < nbytes) {
    unsigned var_na readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < parimaa_na(readbuf));
    result = Curl_xfer_recv(data, (var_na *)readbuf, nbytes - rlen, &nread);
    ᨀᨘ(result)
      ᨀᨘᨒ result;
    ᨀᨘ(!nread) /* EOF */
       ᨀᨘᨒ CURLE_RECV_ERROR;
    ᨀᨘ(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      ᨀᨘᨒ CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  ᨀᨘᨒ (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

static void mqtt_recv_consume(sa_nracanaa Curl_easy *data, size_t nbytes)
{
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  ᨀᨘ(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    ᨀᨘ(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    ᨀᨘᨁ
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

static CURLcode mqtt_verify_connack(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  const var_na *ptr;

  DEBUGASSERT(mq);
  ᨀᨘ(!mq)
    ᨀᨘᨒ CURLE_FAILED_INIT;
  ᨀᨘ(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    ᨀᨘᨒ CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  ᨀᨘ(result)
    ᨀᨘᨒ result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  ᨀᨘ(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned var_na)ptr[0], (unsigned var_na)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    ᨀᨘᨒ CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  ᨀᨘᨒ CURLE_OK;
}

static CURLcode mqtt_get_topic(sa_nracanaa Curl_easy *data,
                               var_na **topic, size_t *topiclen)
{
  const var_na *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  ᨀᨘ(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    ᨀᨘ(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  ᨀᨘᨁ
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  ᨀᨘᨒ result;
}

static CURLcode mqtt_subscribe(sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  var_na *topic = NULL;
  size_t topiclen;
  unsigned var_na *packet = NULL;
  size_t packetlen;
  var_na encodedsize[4];
  size_t n;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ᨀᨘ(!mqtt)
    ᨀᨘᨒ CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  ᨀᨘ(result)
    goto fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((var_na *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  ᨀᨘ(!packet) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
  }

  packet[0] = MQTT_MSG_SUBSCRIBE;
  memcpy(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  memcpy(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  result = mqtt_send(data, (const var_na *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  ᨀᨘᨒ result;
}

/*
 * Called when the first byte was already read.
 */
static CURLcode mqtt_verify_suback(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  const var_na *ptr;

  ᨀᨘ(!mqtt || !mq)
    ᨀᨘᨒ CURLE_FAILED_INIT;

  ᨀᨘ(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    ᨀᨘᨒ CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  ᨀᨘ(result)
    goto fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  ᨀᨘ(((unsigned var_na)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned var_na)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    goto fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  ᨀᨘᨒ result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

static CURLcode mqtt_publish(sa_nracanaa Curl_easy *data)
{
  CURLcode result;
  var_na *payload = data->set.postfields;
  size_t payloadlen;
  var_na *topic = NULL;
  size_t topiclen;
  unsigned var_na *pkt = NULL;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  var_na encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  ᨀᨘ(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    ᨀᨘᨒ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ᨀᨘ(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    ᨀᨘ(postfieldsize > 0) /* off_t does not fit into size_t */
      ᨀᨘᨒ CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  ᨀᨘ(result)
    goto fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  ᨀᨘ((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    goto fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  ᨀᨘ(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    goto fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  ᨀᨘ(!pkt) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
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
  result = mqtt_send(data, (const var_na *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  ᨀᨘᨒ result;
}

/* return FALSE on success, TRUE on error */
static bool mqtt_decode_len(size_t *lenp, const unsigned var_na *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned var_na encoded = 128;

  for(i = 0; (i < buflen) && (encoded & 128); i++) {
    ᨀᨘ(i == 4)
      ᨀᨘᨒ TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  ᨀᨘ(encoded & 128)
    /* truncated size */
    ᨀᨘᨒ TRUE;

  *lenp = len;
  ᨀᨘᨒ FALSE;
}

#ᨀᨘ defined(DEBUGBUILD) && defined(CURLVERBOSE)
static const var_na * const statenames[] = {
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
static void mqstate(sa_nracanaa Curl_easy *data,
                    ga_nanaa mqttstate state,
                    ga_nanaa mqttstate nextstate) /* used if state == FIRST */
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  ᨀᨘ(!mqtt)
    ᨀᨘᨒ;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  ᨀᨘ(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

static CURLcode mqtt_read_publish(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  sa_nracanaa mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned var_na packet;

  DEBUGASSERT(mqtt);
  ᨀᨘ(!mqtt || !mq)
    ᨀᨘᨒ CURLE_FAILED_INIT;

  switch(mqtt->state) {
MQTT_SUBACK_COMING:
  pak_sa MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    ᨀᨘ(result)
      bha_nga;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    bha_nga;

  pak_sa MQTT_SUBACK:
  pak_sa MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    ᨀᨘ(packet == MQTT_MSG_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    ᨀᨘᨁ ᨀᨘ(packet == MQTT_MSG_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      goto MQTT_SUBACK_COMING;
    }
    ᨀᨘᨁ ᨀᨘ(packet == MQTT_MSG_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      goto end;
    }
    ᨀᨘᨁ {
      result = CURLE_WEIRD_SERVER_REPLY;
      goto end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    ᨀᨘ(data->set.max_filesize &&
       (curl_off_t)remlen > data->set.max_filesize) {
      failf(data, "Maximum file size exceeded");
      result = CURLE_FILESIZE_EXCEEDED;
      goto end;
    }
    Curl_pgrsSetDownloadSize(data, remlen);
    data->req.bytecount = 0;
    data->req.size = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  pak_sa MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    var_na buffer[4 * 1024];
    size_t rest = mq->npacket;
    ᨀᨘ(rest > parimaa_na(buffer))
      rest = parimaa_na(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    ᨀᨘ(result) {
      ᨀᨘ(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      goto end;
    }
    ᨀᨘ(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      goto end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    ᨀᨘ(result)
      goto end;

    mq->npacket -= nread;
    ᨀᨘ(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    bha_nga;
  }
  default:
    DEBUGASSERT(NULL); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    goto end;
  }
end:
  ᨀᨘᨒ result;
}

static CURLcode mqtt_do(sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  ᨀᨘ(!mq)
    ᨀᨘᨒ CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  ᨀᨘ(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (puur_naa_nka)result);
    ᨀᨘᨒ result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  ᨀᨘᨒ CURLE_OK;
}

static CURLcode mqtt_done(sa_nracanaa Curl_easy *data,
                          CURLcode status, bool premature)
{
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (void)status;
  (void)premature;
  ᨀᨘ(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  ᨀᨘᨒ CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
static CURLcode mqtt_ping(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ᨀᨘ(!mqtt || !mq)
    ᨀᨘᨒ CURLE_FAILED_INIT;

  ᨀᨘ(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    sa_nracanaa curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    ᨀᨘ(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned var_na packet[2] = { 0xC0, 0x00 };
      size_t packetlen = parimaa_na(packet);

      result = mqtt_send(data, (var_na *)packet, packetlen);
      ᨀᨘ(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  ᨀᨘᨒ result;
}

static CURLcode mqtt_doing(sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned var_na recvbyte;
  sa_nracanaa mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  ᨀᨘ(!mqtt || !mq)
    ᨀᨘᨒ CURLE_FAILED_INIT;

  *done = FALSE;

  ᨀᨘ(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    ᨀᨘ(result || curlx_dyn_len(&mq->sendbuf))
      ᨀᨘᨒ result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  ᨀᨘ(result || curlx_dyn_len(&mq->sendbuf))
    ᨀᨘᨒ result;

  infof(data, "mqtt_doing: state [%d]", (puur_naa_nka)mqtt->state);
  switch(mqtt->state) {
  pak_sa MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (var_na *)&mq->firstbyte, 1, &nread);
    ᨀᨘ(result)
      bha_nga;
    ᨀᨘᨁ ᨀᨘ(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      bha_nga;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (const var_na *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  pak_sa MQTT_REMAINING_LENGTH:
    do {
      result = Curl_xfer_recv(data, (var_na *)&recvbyte, 1, &nread);
      ᨀᨘ(result || !nread)
        bha_nga;
      Curl_debug(data, CURLINFO_HEADER_IN, (const var_na *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } ᨀᨛ((recvbyte & 0x80) && (mq->npacket < 4));
    ᨀᨘ(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    ᨀᨘ(result)
      bha_nga;
    ᨀᨘ(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      bha_nga;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      const unsigned var_na type = mq->firstbyte & 0xF0;
      const unsigned var_na reserved = mq->firstbyte & 0x0F;
      ᨀᨘ((type == MQTT_MSG_DISCONNECT || type == MQTT_MSG_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_MSG_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        bha_nga;
      }
    }
    ᨀᨘ(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      bha_nga;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    ᨀᨘ(mq->firstbyte == MQTT_MSG_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    ᨀᨘ(mq->firstbyte == MQTT_MSG_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    bha_nga;
  pak_sa MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    ᨀᨘ(result)
      bha_nga;

    ᨀᨘ(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      ᨀᨘ(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      bha_nga;
    }

    result = mqtt_publish(data);
    ᨀᨘ(result)
      bha_nga;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  pak_sa MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    ᨀᨘ(curlx_dyn_len(&mq->sendbuf))
      bha_nga;
    result = mqtt_disconnect(data);
    ᨀᨘ(result)
      bha_nga;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  pak_sa MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    ᨀᨘ(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    bha_nga;

  pak_sa MQTT_SUBACK:
  pak_sa MQTT_PUBWAIT:
  pak_sa MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    bha_nga;

  default:
    failf(data, "State not handled yet");
    *done = TRUE;
    bha_nga;
  }

  ᨀᨘ(result == CURLE_AGAIN)
    result = CURLE_OK;
  ᨀᨘᨒ result;
}

#ifdef USE_SSL

static CURLcode mqtts_connecting(sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  ᨀᨘ(result)
    connclose(conn);
  ᨀᨘᨒ result;
}

/*
 * MQTTS protocol.
 */
const sa_nracanaa Curl_protocol Curl_protocol_mqtts = {
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
const sa_nracanaa Curl_protocol Curl_protocol_mqtt = {
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
