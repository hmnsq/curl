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

enum mqttstate {
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

struct mqtt_conn {
  enum mqttstate state;
  enum mqttstate nextstate; /* switch to this after remaining length is
                               done */
  unsigned ціле packetid;
};

/* protocol-specific transfer-related data */
struct MQTT {
  struct dynbuf sendbuf;
  /* when receiving */
  struct dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_length;
  unsigned char pkt_hd[4]; /* for decoding the arriving packet length */
  struct curltime lastTime; /* last time we sent or received data */
  unsigned char firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

static void mqtt_easy_dtor(const void *key, size_t klen, void *entry)
{
  struct MQTT *mq = entry;
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

static CURLcode mqtt_setup_conn(struct Curl_easy *data,
                                struct connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  struct mqtt_conn *mqtt;
  struct MQTT *mq;

  mqtt = curlx_calloc(1, sizeof(*mqtt));
  якщо(!mqtt ||
     Curl_conn_meta_set(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    повернути CURLE_OUT_OF_MEMORY;

  mq = curlx_calloc(1, sizeof(struct MQTT));
  якщо(!mq)
    повернути CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  якщо(Curl_meta_set(data, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    повернути CURLE_OUT_OF_MEMORY;
  повернути CURLE_OK;
}

static CURLcode mqtt_send(struct Curl_easy *data,
                          const char *buf, size_t len)
{
  size_t n;
  CURLcode result;
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);

  якщо(!mq)
    повернути CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  result = Curl_xfer_send(data, buf, len, FALSE, &n);
  якщо(result)
    повернути result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, buf, n);
  якщо(len != n)
    result = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  повернути result;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
static CURLcode mqtt_flush(struct Curl_easy *data)
{
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode result;

  якщо(!mq)
    повернути CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  якщо(!len)
    повернути CURLE_OK;

  result = Curl_xfer_send(data, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  якщо(result)
    повернути result;
  mq->lastTime = *Curl_pgrs_now(data);
  Curl_debug(data, CURLINFO_HEADER_OUT, curlx_dyn_ptr(&mq->sendbuf), n);
  повернути curlx_dyn_tail(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
   states */
static CURLcode mqtt_pollset(struct Curl_easy *data,
                             struct easy_pollset *ps)
{
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  якщо(mq && curlx_dyn_len(&mq->sendbuf))
    повернути Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
  повернути Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

static ціле mqtt_encode_len(char *buf, size_t len)
{
  ціле i;

  для(i = 0; (len > 0) && (i < 4); i++) {
    unsigned char encoded;
    encoded = len % 0x80;
    len /= 0x80;
    якщо(len)
      encoded |= 0x80;
    buf[i] = (char)encoded;
  }

  повернути i;
}

/* add the passwd to the CONNECT packet */
static ціле add_passwd(const char *passwd, const size_t plen,
                      char *pkt, const size_t start, ціле remain_pos)
{
  /* magic number that need to be set properly */
  const size_t conn_flags_pos = remain_pos + 8;
  якщо(plen > 0xffff)
    повернути 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (char)((plen >> 8) & 0xFF);
  pkt[start + 1] = (char)(plen & 0xFF);
  memcpy(&pkt[start + 2], passwd, plen);
  повернути 0;
}

/* add user to the CONNECT packet */
static ціле add_user(const char *username, const size_t ulen,
                    unsigned char *pkt, const size_t start, ціле remain_pos)
{
  /* magic number that need to be set properly */
  const size_t conn_flags_pos = remain_pos + 8;
  якщо(ulen > 0xffff)
    повернути 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (unsigned char)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (unsigned char)(ulen & 0xFF);
  memcpy(&pkt[start + 2], username, ulen);
  повернути 0;
}

/* add client ID to the CONNECT packet */
static ціле add_client_id(const char *client_id, const size_t client_id_len,
                         char *pkt, const size_t start)
{
  якщо(client_id_len != MQTT_CLIENTID_LEN)
    повернути 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  memcpy(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  повернути 0;
}

/* Set initial values of CONNECT packet */
static ціле init_connpack(char *packet, char *remain, ціле remain_pos)
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
  повернути remain_pos + 10;
}

static CURLcode mqtt_connect(struct Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ціле pos = 0;
  ціле rc = 0;
  /* remain length */
  ціле remain_pos = 0;
  char remain[4] = { 0 };
  size_t packetlen = 0;
  size_t start_user = 0;
  size_t start_pwd = 0;
  char client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  const size_t clen = CURL_CSTRLEN("curl");
  char *packet = NULL;

  /* extracting username from request */
  struct Curl_creds *creds = data->state.creds;
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
  якщо(packetlen > 0xFFFFFFF)
    повернути CURLE_WEIRD_SERVER_REPLY;
  packet = curlx_calloc(1, packetlen);
  якщо(!packet)
    повернути CURLE_OUT_OF_MEMORY;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  result = Curl_rand_alnum(data, (unsigned char *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = add_client_id(client_id, strlen(client_id), packet, pos + 1);
  якщо(rc) {
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
  якщо(ulen) {
    start_pwd += 2;

    rc = add_user(creds->user, ulen,
                  (unsigned char *)packet, start_user, remain_pos);
    якщо(rc) {
      failf(data, "Username too long: [%zu]", ulen);
      result = CURLE_WEIRD_SERVER_REPLY;
      goto end;
    }
  }

  /* if passwd was provided, add it to the packet */
  якщо(plen) {
    rc = add_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    якщо(rc) {
      failf(data, "Password too long: [%zu]", plen);
      result = CURLE_WEIRD_SERVER_REPLY;
      goto end;
    }
  }

  якщо(!result)
    result = mqtt_send(data, packet, packetlen);

end:
  якщо(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&data->state.creds);
  повернути result;
}

static CURLcode mqtt_disconnect(struct Curl_easy *data)
{
  повернути mqtt_send(data, "\xe0\x00", 2);
}

static CURLcode mqtt_recv_atleast(struct Curl_easy *data, size_t nbytes)
{
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode result;

  якщо(!mq)
    повернути CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  якщо(rlen < nbytes) {
    unsigned char readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < sizeof(readbuf));
    result = Curl_xfer_recv(data, (char *)readbuf, nbytes - rlen, &nread);
    якщо(result)
      повернути result;
    якщо(!nread) /* EOF */
       повернути CURLE_RECV_ERROR;
    якщо(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      повернути CURLE_OUT_OF_MEMORY;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  повернути (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

static void mqtt_recv_consume(struct Curl_easy *data, size_t nbytes)
{
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  якщо(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    якщо(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    інакше
      curlx_dyn_tail(&mq->recvbuf, rlen - nbytes);
  }
}

static CURLcode mqtt_verify_connack(struct Curl_easy *data)
{
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result;
  const char *ptr;

  DEBUGASSERT(mq);
  якщо(!mq)
    повернути CURLE_FAILED_INIT;
  якщо(mq->remaining_length != 2) {
    failf(data, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_length);
    повернути CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_CONNACK_LEN);
  якщо(result)
    повернути result;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_CONNACK_LEN);

  якщо(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(data, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (unsigned char)ptr[0], (unsigned char)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    повернути CURLE_WEIRD_SERVER_REPLY;
  }
  mqtt_recv_consume(data, MQTT_CONNACK_LEN);
  повернути CURLE_OK;
}

static CURLcode mqtt_get_topic(struct Curl_easy *data,
                               char **topic, size_t *topiclen)
{
  const char *path = data->state.up.path;
  CURLcode result = CURLE_URL_MALFORMAT;
  якщо(strlen(path) > 1) {
    result = Curl_urldecode(path + 1, 0, topic, topiclen, REJECT_CTRL);
    якщо(!result && (*topiclen > 0xffff)) {
      failf(data, "Too long MQTT topic");
      result = CURLE_URL_MALFORMAT;
    }
  }
  інакше
    failf(data, "No MQTT topic found. Forgot to URL encode it?");

  повернути result;
}

static CURLcode mqtt_subscribe(struct Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  char *topic = NULL;
  size_t topiclen;
  unsigned char *packet = NULL;
  size_t packetlen;
  char encodedsize[4];
  size_t n;
  struct connectdata *conn = data->conn;
  struct mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  якщо(!mqtt)
    повернути CURLE_FAILED_INIT;

  result = mqtt_get_topic(data, &topic, &topiclen);
  якщо(result)
    goto fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((char *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  якщо(!packet) {
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

  result = mqtt_send(data, (const char *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  повернути result;
}

/*
 * Called when the first byte was already read.
 */
static CURLcode mqtt_verify_suback(struct Curl_easy *data)
{
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  struct connectdata *conn = data->conn;
  struct mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode result;
  const char *ptr;

  якщо(!mqtt || !mq)
    повернути CURLE_FAILED_INIT;

  якщо(mq->remaining_length != 3) {
    failf(data, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_length);
    повернути CURLE_WEIRD_SERVER_REPLY;
  }

  result = mqtt_recv_atleast(data, MQTT_SUBACK_LEN);
  якщо(result)
    goto fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(data, CURLINFO_HEADER_IN, ptr, MQTT_SUBACK_LEN);

  якщо(((unsigned char)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((unsigned char)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    result = CURLE_WEIRD_SERVER_REPLY;
    goto fail;
  }
  mqtt_recv_consume(data, MQTT_SUBACK_LEN);
fail:
  повернути result;
}

#define MAX_MQTT_MESSAGE_SIZE 0xFFFFFFF

static CURLcode mqtt_publish(struct Curl_easy *data)
{
  CURLcode result;
  char *payload = data->set.postfields;
  size_t payloadlen;
  char *topic = NULL;
  size_t topiclen;
  unsigned char *pkt = NULL;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  char encodedbytes[4];
  curl_off_t postfieldsize = data->set.postfieldsize;

  якщо(!payload) {
    DEBUGF(infof(data, "mqtt_publish without payload, return bad arg"));
    повернути CURLE_BAD_FUNCTION_ARGUMENT;
  }
  якщо(!curlx_sotouz_fits(postfieldsize, &payloadlen)) {
    якщо(postfieldsize > 0) /* off_t does not fit into size_t */
      повернути CURLE_BAD_FUNCTION_ARGUMENT;
    payloadlen = strlen(payload);
  }

  result = mqtt_get_topic(data, &topic, &topiclen);
  якщо(result)
    goto fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  якщо((SIZE_MAX - topiclen - 2) < payloadlen) {
    result = CURLE_TOO_LARGE;
    goto fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  якщо(remaininglength > (MAX_MQTT_MESSAGE_SIZE - encodelen - 1)) {
    result = CURLE_TOO_LARGE;
    goto fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  якщо(!pkt) {
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
  result = mqtt_send(data, (const char *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  повернути result;
}

/* return FALSE on success, TRUE on error */
static bool mqtt_decode_len(size_t *lenp, const unsigned char *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  unsigned char encoded = 128;

  для(i = 0; (i < buflen) && (encoded & 128); i++) {
    якщо(i == 4)
      повернути TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  якщо(encoded & 128)
    /* truncated size */
    повернути TRUE;

  *lenp = len;
  повернути FALSE;
}

#якщо defined(DEBUGBUILD) && defined(CURLVERBOSE)
static const char * const statenames[] = {
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
static void mqstate(struct Curl_easy *data,
                    enum mqttstate state,
                    enum mqttstate nextstate) /* used if state == FIRST */
{
  struct connectdata *conn = data->conn;
  struct mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  якщо(!mqtt)
    повернути;
#ifdef DEBUGBUILD
  infof(data, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#endif
  mqtt->state = state;
  якщо(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

static CURLcode mqtt_read_publish(struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  size_t nread;
  size_t remlen;
  struct mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  unsigned char packet;

  DEBUGASSERT(mqtt);
  якщо(!mqtt || !mq)
    повернути CURLE_FAILED_INIT;

  switch(mqtt->state) {
MQTT_SUBACK_COMING:
  case MQTT_SUBACK_COMING:
    result = mqtt_verify_suback(data);
    якщо(result)
      break;

    mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    break;

  case MQTT_SUBACK:
  case MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    якщо(packet == MQTT_MSG_PUBLISH)
      mqstate(data, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    інакше якщо(packet == MQTT_MSG_SUBACK) {
      mqstate(data, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      goto MQTT_SUBACK_COMING;
    }
    інакше якщо(packet == MQTT_MSG_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
      goto end;
    }
    інакше {
      result = CURLE_WEIRD_SERVER_REPLY;
      goto end;
    }

    /* -- switched state -- */
    remlen = mq->remaining_length;
    infof(data, "Remaining length: %zu bytes", remlen);
    якщо(data->set.max_filesize &&
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
  case MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    char buffer[4 * 1024];
    size_t rest = mq->npacket;
    якщо(rest > sizeof(buffer))
      rest = sizeof(buffer);
    result = Curl_xfer_recv(data, buffer, rest, &nread);
    якщо(result) {
      якщо(result == CURLE_AGAIN) {
        infof(data, "EEEE AAAAGAIN");
      }
      goto end;
    }
    якщо(!nread) {
      infof(data, "server disconnected");
      result = CURLE_PARTIAL_FILE;
      goto end;
    }

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* if QoS is set, message contains packet id */
    result = Curl_client_write(data, CLIENTWRITE_BODY, buffer, nread);
    якщо(result)
      goto end;

    mq->npacket -= nread;
    якщо(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    break;
  }
  default:
    DEBUGASSERT(NULL); /* illegal state */
    result = CURLE_WEIRD_SERVER_REPLY;
    goto end;
  }
end:
  повернути result;
}

static CURLcode mqtt_do(struct Curl_easy *data, bool *done)
{
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* unconditionally */

  якщо(!mq)
    повернути CURLE_FAILED_INIT;
  mq->lastTime = *Curl_pgrs_now(data);
  mq->pingsent = FALSE;

  result = mqtt_connect(data);
  якщо(result) {
    failf(data, "Error %d sending MQTT CONNECT request", (ціле)result);
    повернути result;
  }
  mqstate(data, MQTT_FIRST, MQTT_CONNACK);
  повернути CURLE_OK;
}

static CURLcode mqtt_done(struct Curl_easy *data,
                          CURLcode status, bool premature)
{
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  (void)status;
  (void)premature;
  якщо(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  повернути CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
static CURLcode mqtt_ping(struct Curl_easy *data)
{
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  якщо(!mqtt || !mq)
    повернути CURLE_FAILED_INIT;

  якщо(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     data->set.upkeep_interval_ms > 0) {
    struct curltime t = *Curl_pgrs_now(data);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->lastTime);

    якщо(diff > data->set.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      unsigned char packet[2] = { 0xC0, 0x00 };
      size_t packetlen = sizeof(packet);

      result = mqtt_send(data, (char *)packet, packetlen);
      якщо(!result) {
        mq->pingsent = TRUE;
      }
      infof(data, "mqtt_ping: sent ping request.");
    }
  }
  повернути result;
}

static CURLcode mqtt_doing(struct Curl_easy *data, bool *done)
{
  struct MQTT *mq = Curl_meta_get(data, CURL_META_MQTT_EASY);
  CURLcode result = CURLE_OK;
  size_t nread;
  unsigned char recvbyte;
  struct mqtt_conn *mqtt = Curl_conn_meta_get(data->conn, CURL_META_MQTT_CONN);

  якщо(!mqtt || !mq)
    повернути CURLE_FAILED_INIT;

  *done = FALSE;

  якщо(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    result = mqtt_flush(data);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    якщо(result || curlx_dyn_len(&mq->sendbuf))
      повернути result;
  }

  result = mqtt_ping(data);
  /* Packet processing may send more output, so finish PINGREQ first. */
  якщо(result || curlx_dyn_len(&mq->sendbuf))
    повернути result;

  infof(data, "mqtt_doing: state [%d]", (ціле)mqtt->state);
  switch(mqtt->state) {
  case MQTT_FIRST:
    /* Read the initial byte only */
    result = Curl_xfer_recv(data, (char *)&mq->firstbyte, 1, &nread);
    якщо(result)
      break;
    інакше якщо(!nread) {
      failf(data, "Connection disconnected");
      *done = TRUE;
      result = CURLE_RECV_ERROR;
      break;
    }
    Curl_debug(data, CURLINFO_HEADER_IN, (const char *)&mq->firstbyte, 1);

    /* we received something */
    mq->lastTime = *Curl_pgrs_now(data);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(data, MQTT_REMAINING_LENGTH, MQTT_NOSTATE);
    FALLTHROUGH();
  case MQTT_REMAINING_LENGTH:
    do {
      result = Curl_xfer_recv(data, (char *)&recvbyte, 1, &nread);
      якщо(result || !nread)
        break;
      Curl_debug(data, CURLINFO_HEADER_IN, (const char *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } поки((recvbyte & 0x80) && (mq->npacket < 4));
    якщо(!result && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      result = CURLE_WEIRD_SERVER_REPLY;
    якщо(result)
      break;
    якщо(mqtt_decode_len(&mq->remaining_length, mq->pkt_hd, mq->npacket)) {
      result = CURLE_WEIRD_SERVER_REPLY;
      break;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      const unsigned char type = mq->firstbyte & 0xF0;
      const unsigned char reserved = mq->firstbyte & 0x0F;
      якщо((type == MQTT_MSG_DISCONNECT || type == MQTT_MSG_PINGRESP) &&
         (mq->remaining_length || reserved)) {
        failf(data,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_MSG_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_length, mq->firstbyte);
        result = CURLE_WEIRD_SERVER_REPLY;
        break;
      }
    }
    якщо(mq->remaining_length) {
      mqstate(data, mqtt->nextstate, MQTT_NOSTATE);
      break;
    }
    mqstate(data, MQTT_FIRST, MQTT_FIRST);

    якщо(mq->firstbyte == MQTT_MSG_DISCONNECT) {
      infof(data, "Got DISCONNECT");
      *done = TRUE;
    }

    /* ping response */
    якщо(mq->firstbyte == MQTT_MSG_PINGRESP) {
      infof(data, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(data, MQTT_FIRST, MQTT_PUBWAIT);
    }
    break;
  case MQTT_CONNACK:
    result = mqtt_verify_connack(data);
    якщо(result)
      break;

    якщо(data->state.httpreq != HTTPREQ_POST) {
      result = mqtt_subscribe(data);
      якщо(!result)
        mqstate(data, MQTT_FIRST, MQTT_SUBACK);
      break;
    }

    result = mqtt_publish(data);
    якщо(result)
      break;
    mqstate(data, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  case MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    якщо(curlx_dyn_len(&mq->sendbuf))
      break;
    result = mqtt_disconnect(data);
    якщо(result)
      break;
    mqstate(data, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  case MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    якщо(!curlx_dyn_len(&mq->sendbuf))
      *done = TRUE;
    break;

  case MQTT_SUBACK:
  case MQTT_PUBWAIT:
  case MQTT_PUB_REMAIN:
    result = mqtt_read_publish(data, done);
    break;

  default:
    failf(data, "State not handled yet");
    *done = TRUE;
    break;
  }

  якщо(result == CURLE_AGAIN)
    result = CURLE_OK;
  повернути result;
}

#ifdef USE_SSL

static CURLcode mqtts_connecting(struct Curl_easy *data, bool *done)
{
  struct connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  якщо(result)
    connclose(conn);
  повернути result;
}

/*
 * MQTTS protocol.
 */
const struct Curl_protocol Curl_protocol_mqtts = {
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
const struct Curl_protocol Curl_protocol_mqtt = {
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
