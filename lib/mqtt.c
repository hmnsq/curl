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

#यदि_अपरिभाषितम् CURL_DISABLE_MQTT

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
#परिभाषय MQTT_सन्देशः_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#परिभाषय MQTT_सन्देशः_PUBLISH    0x30
#परिभाषय MQTT_सन्देशः_SUBSCRIBE  0x82
#परिभाषय MQTT_सन्देशः_SUBACK     0x90
#परिभाषय MQTT_सन्देशः_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#परिभाषय MQTT_सन्देशः_PINGRESP   0xD0

#परिभाषय MQTT_CONNACK_LEN  2
#परिभाषय MQTT_SUBACK_LEN   3
#परिभाषय MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#परिभाषय CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#परिभाषय CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

गणना mqttstate {
  MQTT_FIRST,             /* 0 */
  MQTT_REMAINING_दीर्घता,  /* 1 */
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
समाप्तम्*/
  अचिह्नित पूर्णाङ्क packetid;
};

/* protocol-specific transfer-related data */
संरचना MQTT {
  संरचना dynbuf sendbuf;
  /* when receiving */
  संरचना dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t remaining_दीर्घता;
  अचिह्नित वर्ण pkt_hd[4]; /* for decoding the arriving packet length */
  संरचना curltime चरमः_Time; /* last time we sent or received data */
  अचिह्नित वर्ण firstbyte;
  BIT(pingsent); /* 1 while we wait for ping response */
};

स्थिर शून्य mqtt_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना MQTT *mq = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

स्थिर शून्य mqtt_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_free(प्रविष्टिः);
}

स्थिर CURLcode mqtt_सज्जता_conn(संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  संरचना mqtt_conn *mqtt;
  संरचना MQTT *mq;

  mqtt = curlx_calloc(1, परिमाण(*mqtt));
  यदि(!mqtt ||
     Curl_conn_meta_समुच्चयः(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  mq = curlx_calloc(1, परिमाण(संरचना MQTT));
  यदि(!mq)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  यदि(Curl_meta_समुच्चयः(दत्त, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mqtt_send(संरचना Curl_easy *दत्त,
                          ध्रुव वर्ण *buf, size_t len)
{
  size_t n;
  CURLcode परिणाम;
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);

  यदि(!mq)
    प्रत्यागम CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  परिणाम = Curl_xfer_send(दत्त, buf, len, FALSE, &n);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  mq->चरमः_Time = *Curl_pgrs_now(दत्त);
  Curl_debug(दत्त, CURLINFO_शीर्षकम्_बहिर्गमनम्, buf, n);
  यदि(len != n)
    परिणाम = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  प्रत्यागम परिणाम;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
स्थिर CURLcode mqtt_flush(संरचना Curl_easy *दत्त)
{
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode परिणाम;

  यदि(!mq)
    प्रत्यागम CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  यदि(!len)
    प्रत्यागम CURLE_OK;

  परिणाम = Curl_xfer_send(दत्त, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  mq->चरमः_Time = *Curl_pgrs_now(दत्त);
  Curl_debug(दत्त, CURLINFO_शीर्षकम्_बहिर्गमनम्, curlx_dyn_ptr(&mq->sendbuf), n);
  प्रत्यागम curlx_dyn_पुच्छ(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
अवस्थाः*/
स्थिर CURLcode mqtt_pollset(संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps)
{
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  यदि(mq && curlx_dyn_len(&mq->sendbuf))
    प्रत्यागम Curl_pollset_सङ्कलय_बहिर्गमनम्(दत्त, ps, दत्त->conn->sock[FIRSTSOCKET]);
  प्रत्यागम Curl_pollset_सङ्कलय_अन्तर्(दत्त, ps, दत्त->conn->sock[FIRSTSOCKET]);
}

स्थिर पूर्णाङ्क mqtt_encode_len(वर्ण *buf, size_t len)
{
  पूर्णाङ्क i;

  प्रति(i = 0; (len > 0) && (i < 4); i++) {
    अचिह्नित वर्ण encoded;
    encoded = len % 0x80;
    len /= 0x80;
    यदि(len)
      encoded |= 0x80;
    buf[i] = (वर्ण)encoded;
  }

  प्रत्यागम i;
}

/* add the passwd to the CONNECT packet */
स्थिर पूर्णाङ्क सङ्कलय_passwd(ध्रुव वर्ण *passwd, ध्रुव size_t plen,
                      वर्ण *pkt, ध्रुव size_t start, पूर्णाङ्क remain_pos)
{
  /* magic number that need to be set properly */
  ध्रुव size_t conn_flags_pos = remain_pos + 8;
  यदि(plen > 0xffff)
    प्रत्यागम 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[start] = (वर्ण)((plen >> 8) & 0xFF);
  pkt[start + 1] = (वर्ण)(plen & 0xFF);
  स्मृतिं_प्रतिलिख(&pkt[start + 2], passwd, plen);
  प्रत्यागम 0;
}

/* add user to the CONNECT packet */
स्थिर पूर्णाङ्क सङ्कलय_user(ध्रुव वर्ण *username, ध्रुव size_t ulen,
                    अचिह्नित वर्ण *pkt, ध्रुव size_t start, पूर्णाङ्क remain_pos)
{
  /* magic number that need to be set properly */
  ध्रुव size_t conn_flags_pos = remain_pos + 8;
  यदि(ulen > 0xffff)
    प्रत्यागम 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[start] = (अचिह्नित वर्ण)((ulen >> 8) & 0xFF);
  pkt[start + 1] = (अचिह्नित वर्ण)(ulen & 0xFF);
  स्मृतिं_प्रतिलिख(&pkt[start + 2], username, ulen);
  प्रत्यागम 0;
}

/* add client ID to the CONNECT packet */
स्थिर पूर्णाङ्क सङ्कलय_client_id(ध्रुव वर्ण *client_id, ध्रुव size_t client_id_len,
                         वर्ण *pkt, ध्रुव size_t start)
{
  यदि(client_id_len != MQTT_CLIENTID_LEN)
    प्रत्यागम 1;
  pkt[start] = 0x00;
  pkt[start + 1] = MQTT_CLIENTID_LEN;
  स्मृतिं_प्रतिलिख(&pkt[start + 2], client_id, MQTT_CLIENTID_LEN);
  प्रत्यागम 0;
}

/* Set initial values of CONNECT packet */
स्थिर पूर्णाङ्क init_connpack(वर्ण *packet, वर्ण *remain, पूर्णाङ्क remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_सन्देशः_CONNECT;
  /* remaining length field */
  स्मृतिं_प्रतिलिख(&packet[1], remain, remain_pos);
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

स्थिर CURLcode mqtt_connect(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_OK;
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
  संरचना Curl_creds *creds = दत्त->state.creds;
  ध्रुव size_t ulen = creds ? वाक्यदैर्घ्यम्(creds->user) : 0;
  ध्रुव size_t plen = creds ? वाक्यदैर्घ्यम्(creds->passwd) : 0;
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
  यदि(packetlen > 0xFFFFFFF)
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  packet = curlx_calloc(1, packetlen);
  यदि(!packet)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  परिणाम = Curl_rand_alnum(दत्त, (अचिह्नित वर्ण *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = सङ्कलय_client_id(client_id, वाक्यदैर्घ्यम्(client_id), packet, pos + 1);
  यदि(rc) {
    failf(दत्त, "Client ID length mismatched: [%zu]", वाक्यदैर्घ्यम्(client_id));
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    गच्छ अन्तः;
  }
  infof(दत्त, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  start_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  start_pwd = start_user + ulen;
  /* if username was provided, add it to the packet */
  यदि(ulen) {
    start_pwd += 2;

    rc = सङ्कलय_user(creds->user, ulen,
                  (अचिह्नित वर्ण *)packet, start_user, remain_pos);
    यदि(rc) {
      failf(दत्त, "Username too long: [%zu]", ulen);
      परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
      गच्छ अन्तः;
    }
  }

  /* if passwd was provided, add it to the packet */
  यदि(plen) {
    rc = सङ्कलय_passwd(creds->passwd, plen, packet, start_pwd, remain_pos);
    यदि(rc) {
      failf(दत्त, "Password too long: [%zu]", plen);
      परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
      गच्छ अन्तः;
    }
  }

  यदि(!परिणाम)
    परिणाम = mqtt_send(दत्त, packet, packetlen);

अन्तः:
  यदि(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&दत्त->state.creds);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode mqtt_disconnect(संरचना Curl_easy *दत्त)
{
  प्रत्यागम mqtt_send(दत्त, "\xe0\x00", 2);
}

स्थिर CURLcode mqtt_recv_atleast(संरचना Curl_easy *दत्त, size_t nbytes)
{
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode परिणाम;

  यदि(!mq)
    प्रत्यागम CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  यदि(rlen < nbytes) {
    अचिह्नित वर्ण readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < परिमाण(readbuf));
    परिणाम = Curl_xfer_recv(दत्त, (वर्ण *)readbuf, nbytes - rlen, &nread);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    यदि(!nread) /* EOF */
       प्रत्यागम CURLE_RECV_ERROR;
    यदि(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  प्रत्यागम (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

स्थिर शून्य mqtt_recv_consume(संरचना Curl_easy *दत्त, size_t nbytes)
{
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  यदि(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    यदि(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    अन्यथा
      curlx_dyn_पुच्छ(&mq->recvbuf, rlen - nbytes);
  }
}

स्थिर CURLcode mqtt_verify_connack(संरचना Curl_easy *दत्त)
{
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  CURLcode परिणाम;
  ध्रुव वर्ण *ptr;

  DEBUGASSERT(mq);
  यदि(!mq)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(mq->remaining_दीर्घता != 2) {
    failf(दत्त, "CONNACK expected Remaining Length 2, got %zu",
          mq->remaining_दीर्घता);
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  परिणाम = mqtt_recv_atleast(दत्त, MQTT_CONNACK_LEN);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, ptr, MQTT_CONNACK_LEN);

  यदि(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(दत्त, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (अचिह्नित वर्ण)ptr[0], (अचिह्नित वर्ण)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }
  mqtt_recv_consume(दत्त, MQTT_CONNACK_LEN);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mqtt_get_topic(संरचना Curl_easy *दत्त,
                               वर्ण **topic, size_t *topiclen)
{
  ध्रुव वर्ण *मार्गः = दत्त->state.ऊर्ध्वम्.मार्गः;
  CURLcode परिणाम = CURLE_URL_MALFORMAT;
  यदि(वाक्यदैर्घ्यम्(मार्गः) > 1) {
    परिणाम = Curl_urldecode(मार्गः + 1, 0, topic, topiclen, REJECT_CTRL);
    यदि(!परिणाम && (*topiclen > 0xffff)) {
      failf(दत्त, "Too long MQTT topic");
      परिणाम = CURLE_URL_MALFORMAT;
    }
  }
  अन्यथा
    failf(दत्त, "No MQTT topic found. Forgot to URL encode it?");

  प्रत्यागम परिणाम;
}

स्थिर CURLcode mqtt_subscribe(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_OK;
  वर्ण *topic = रिक्त;
  size_t topiclen;
  अचिह्नित वर्ण *packet = रिक्त;
  size_t packetlen;
  वर्ण encodedsize[4];
  size_t n;
  संरचना connectdata *conn = दत्त->conn;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  यदि(!mqtt)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = mqtt_get_topic(दत्त, &topic, &topiclen);
  यदि(परिणाम)
    गच्छ fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((वर्ण *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  यदि(!packet) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ fail;
  }

  packet[0] = MQTT_सन्देशः_SUBSCRIBE;
  स्मृतिं_प्रतिलिख(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  स्मृतिं_प्रतिलिख(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  परिणाम = mqtt_send(दत्त, (ध्रुव वर्ण *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  प्रत्यागम परिणाम;
}

/*
 * Called when the first byte was already read.
 */
स्थिर CURLcode mqtt_verify_suback(संरचना Curl_easy *दत्त)
{
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  संरचना connectdata *conn = दत्त->conn;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode परिणाम;
  ध्रुव वर्ण *ptr;

  यदि(!mqtt || !mq)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(mq->remaining_दीर्घता != 3) {
    failf(दत्त, "SUBACK expected Remaining Length 3, got %zu",
          mq->remaining_दीर्घता);
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  परिणाम = mqtt_recv_atleast(दत्त, MQTT_SUBACK_LEN);
  यदि(परिणाम)
    गच्छ fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, ptr, MQTT_SUBACK_LEN);

  यदि(((अचिह्नित वर्ण)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((अचिह्नित वर्ण)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    गच्छ fail;
  }
  mqtt_recv_consume(दत्त, MQTT_SUBACK_LEN);
fail:
  प्रत्यागम परिणाम;
}

#परिभाषय MAX_MQTT_सन्देशः_आकार 0xFFFFFFF

स्थिर CURLcode mqtt_publish(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;
  वर्ण *payload = दत्त->समुच्चयः.postfields;
  size_t payloadlen;
  वर्ण *topic = रिक्त;
  size_t topiclen;
  अचिह्नित वर्ण *pkt = रिक्त;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  वर्ण encodedbytes[4];
  curl_off_t postfieldsize = दत्त->समुच्चयः.postfieldsize;

  यदि(!payload) {
    DEBUGF(infof(दत्त, "mqtt_publish without payload, return bad arg"));
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
  यदि(!curlx_sotouz_माति(postfieldsize, &payloadlen)) {
    यदि(postfieldsize > 0) /* off_t does not fit into size_t */
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    payloadlen = वाक्यदैर्घ्यम्(payload);
  }

  परिणाम = mqtt_get_topic(दत्त, &topic, &topiclen);
  यदि(परिणाम)
    गच्छ fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  यदि((SIZE_MAX - topiclen - 2) < payloadlen) {
    परिणाम = CURLE_TOO_LARGE;
    गच्छ fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  यदि(remaininglength > (MAX_MQTT_सन्देशः_आकार - encodelen - 1)) {
    परिणाम = CURLE_TOO_LARGE;
    गच्छ fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  यदि(!pkt) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_सन्देशः_PUBLISH;
  स्मृतिं_प्रतिलिख(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  स्मृतिं_प्रतिलिख(&pkt[i], topic, topiclen);
  i += topiclen;
  स्मृतिं_प्रतिलिख(&pkt[i], payload, payloadlen);
  i += payloadlen;
  परिणाम = mqtt_send(दत्त, (ध्रुव वर्ण *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  प्रत्यागम परिणाम;
}

/* return FALSE on success, TRUE on error */
स्थिर bool mqtt_decode_len(size_t *lenp, ध्रुव अचिह्नित वर्ण *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  अचिह्नित वर्ण encoded = 128;

  प्रति(i = 0; (i < buflen) && (encoded & 128); i++) {
    यदि(i == 4)
      प्रत्यागम TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  यदि(encoded & 128)
    /* truncated size */
    प्रत्यागम TRUE;

  *lenp = len;
  प्रत्यागम FALSE;
}

#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
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
#यदि_अन्तः

/* The only way to change state */
स्थिर शून्य mqstate(संरचना Curl_easy *दत्त,
                    गणना mqttstate state,
                    गणना mqttstate nextstate) /* used if state == FIRST */
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  यदि(!mqtt)
    प्रत्यागम;
#यदि_परिभाषितम् DEBUGBUILD
  infof(दत्त, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_FIRST) ? statenames[nextstate] : "");
#यदि_अन्तः
  mqtt->state = state;
  यदि(state == MQTT_FIRST)
    mqtt->nextstate = nextstate;
}

स्थिर CURLcode mqtt_read_publish(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  size_t nread;
  size_t remlen;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  अचिह्नित वर्ण packet;

  DEBUGASSERT(mqtt);
  यदि(!mqtt || !mq)
    प्रत्यागम CURLE_FAILED_INIT;

  विकल्प(mqtt->state) {
MQTT_SUBACK_COMING:
  पक्ष MQTT_SUBACK_COMING:
    परिणाम = mqtt_verify_suback(दत्त);
    यदि(परिणाम)
      भङ्ग;

    mqstate(दत्त, MQTT_FIRST, MQTT_PUBWAIT);
    भङ्ग;

  पक्ष MQTT_SUBACK:
  पक्ष MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    यदि(packet == MQTT_सन्देशः_PUBLISH)
      mqstate(दत्त, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    अन्यथा यदि(packet == MQTT_सन्देशः_SUBACK) {
      mqstate(दत्त, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      गच्छ MQTT_SUBACK_COMING;
    }
    अन्यथा यदि(packet == MQTT_सन्देशः_DISCONNECT) {
      infof(दत्त, "Got DISCONNECT");
      *समाप्तम् = TRUE;
      गच्छ अन्तः;
    }
    अन्यथा {
      परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
      गच्छ अन्तः;
    }

    /* -- switched state -- */
    remlen = mq->remaining_दीर्घता;
    infof(दत्त, "Remaining length: %zu bytes", remlen);
    यदि(दत्त->समुच्चयः.max_filesize &&
       (curl_off_t)remlen > दत्त->समुच्चयः.max_filesize) {
      failf(दत्त, "Maximum file size exceeded");
      परिणाम = CURLE_FILESIZE_EXCEEDED;
      गच्छ अन्तः;
    }
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, remlen);
    दत्त->req.bytecount = 0;
    दत्त->req.आकार = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  पक्ष MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    वर्ण बफर[4 * 1024];
    size_t rest = mq->npacket;
    यदि(rest > परिमाण(बफर))
      rest = परिमाण(बफर);
    परिणाम = Curl_xfer_recv(दत्त, बफर, rest, &nread);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_AGAIN) {
        infof(दत्त, "EEEE AAAAGAIN");
      }
      गच्छ अन्तः;
    }
    यदि(!nread) {
      infof(दत्त, "server disconnected");
      परिणाम = CURLE_PARTIAL_FILE;
      गच्छ अन्तः;
    }

    /* we received something */
    mq->चरमः_Time = *Curl_pgrs_now(दत्त);

    /* if QoS is set, message contains packet id */
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, बफर, nread);
    यदि(परिणाम)
      गच्छ अन्तः;

    mq->npacket -= nread;
    यदि(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(दत्त, MQTT_FIRST, MQTT_PUBWAIT);
    भङ्ग;
  }
  स्वभाव:
    DEBUGASSERT(रिक्त); /* illegal state */
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    गच्छ अन्तः;
  }
अन्तः:
  प्रत्यागम परिणाम;
}

स्थिर CURLcode mqtt_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  CURLcode परिणाम = CURLE_OK;
  *समाप्तम् = FALSE; /* unconditionally */

  यदि(!mq)
    प्रत्यागम CURLE_FAILED_INIT;
  mq->चरमः_Time = *Curl_pgrs_now(दत्त);
  mq->pingsent = FALSE;

  परिणाम = mqtt_connect(दत्त);
  यदि(परिणाम) {
    failf(दत्त, "Error %d sending MQTT CONNECT request", (पूर्णाङ्क)परिणाम);
    प्रत्यागम परिणाम;
  }
  mqstate(दत्त, MQTT_FIRST, MQTT_CONNACK);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mqtt_समाप्तम्(संरचना Curl_easy *दत्त,
                          CURLcode स्थितिः, bool premature)
{
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  (शून्य)स्थितिः;
  (शून्य)premature;
  यदि(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  प्रत्यागम CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
स्थिर CURLcode mqtt_ping(संरचना Curl_easy *दत्त)
{
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  यदि(!mqtt || !mq)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(mqtt->state == MQTT_FIRST &&
     !mq->pingsent &&
     दत्त->समुच्चयः.upkeep_interval_ms > 0) {
    संरचना curltime t = *Curl_pgrs_now(दत्त);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->चरमः_Time);

    यदि(diff > दत्त->समुच्चयः.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      अचिह्नित वर्ण packet[2] = { 0xC0, 0x00 };
      size_t packetlen = परिमाण(packet);

      परिणाम = mqtt_send(दत्त, (वर्ण *)packet, packetlen);
      यदि(!परिणाम) {
        mq->pingsent = TRUE;
      }
      infof(दत्त, "mqtt_ping: sent ping request.");
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode mqtt_doing(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना MQTT *mq = Curl_meta_get(दत्त, CURL_META_MQTT_EASY);
  CURLcode परिणाम = CURLE_OK;
  size_t nread;
  अचिह्नित वर्ण recvbyte;
  संरचना mqtt_conn *mqtt = Curl_conn_meta_get(दत्त->conn, CURL_META_MQTT_CONN);

  यदि(!mqtt || !mq)
    प्रत्यागम CURLE_FAILED_INIT;

  *समाप्तम् = FALSE;

  यदि(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    परिणाम = mqtt_flush(दत्त);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    यदि(परिणाम || curlx_dyn_len(&mq->sendbuf))
      प्रत्यागम परिणाम;
  }

  परिणाम = mqtt_ping(दत्त);
  /* Packet processing may send more output, so finish PINGREQ first. */
  यदि(परिणाम || curlx_dyn_len(&mq->sendbuf))
    प्रत्यागम परिणाम;

  infof(दत्त, "mqtt_doing: state [%d]", (पूर्णाङ्क)mqtt->state);
  विकल्प(mqtt->state) {
  पक्ष MQTT_FIRST:
    /* Read the initial byte only */
    परिणाम = Curl_xfer_recv(दत्त, (वर्ण *)&mq->firstbyte, 1, &nread);
    यदि(परिणाम)
      भङ्ग;
    अन्यथा यदि(!nread) {
      failf(दत्त, "Connection disconnected");
      *समाप्तम् = TRUE;
      परिणाम = CURLE_RECV_ERROR;
      भङ्ग;
    }
    Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, (ध्रुव वर्ण *)&mq->firstbyte, 1);

    /* we received something */
    mq->चरमः_Time = *Curl_pgrs_now(दत्त);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(दत्त, MQTT_REMAINING_दीर्घता, MQTT_NOSTATE);
    FALLTHROUGH();
  पक्ष MQTT_REMAINING_दीर्घता:
    कुरु {
      परिणाम = Curl_xfer_recv(दत्त, (वर्ण *)&recvbyte, 1, &nread);
      यदि(परिणाम || !nread)
        भङ्ग;
      Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, (ध्रुव वर्ण *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } यावत्((recvbyte & 0x80) && (mq->npacket < 4));
    यदि(!परिणाम && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    यदि(परिणाम)
      भङ्ग;
    यदि(mqtt_decode_len(&mq->remaining_दीर्घता, mq->pkt_hd, mq->npacket)) {
      परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
      भङ्ग;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ध्रुव अचिह्नित वर्ण type = mq->firstbyte & 0xF0;
      ध्रुव अचिह्नित वर्ण reserved = mq->firstbyte & 0x0F;
      यदि((type == MQTT_सन्देशः_DISCONNECT || type == MQTT_सन्देशः_PINGRESP) &&
         (mq->remaining_दीर्घता || reserved)) {
        failf(दत्त,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_सन्देशः_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->remaining_दीर्घता, mq->firstbyte);
        परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
        भङ्ग;
      }
    }
    यदि(mq->remaining_दीर्घता) {
      mqstate(दत्त, mqtt->nextstate, MQTT_NOSTATE);
      भङ्ग;
    }
    mqstate(दत्त, MQTT_FIRST, MQTT_FIRST);

    यदि(mq->firstbyte == MQTT_सन्देशः_DISCONNECT) {
      infof(दत्त, "Got DISCONNECT");
      *समाप्तम् = TRUE;
    }

    /* ping response */
    यदि(mq->firstbyte == MQTT_सन्देशः_PINGRESP) {
      infof(दत्त, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(दत्त, MQTT_FIRST, MQTT_PUBWAIT);
    }
    भङ्ग;
  पक्ष MQTT_CONNACK:
    परिणाम = mqtt_verify_connack(दत्त);
    यदि(परिणाम)
      भङ्ग;

    यदि(दत्त->state.httpreq != HTTPREQ_POST) {
      परिणाम = mqtt_subscribe(दत्त);
      यदि(!परिणाम)
        mqstate(दत्त, MQTT_FIRST, MQTT_SUBACK);
      भङ्ग;
    }

    परिणाम = mqtt_publish(दत्त);
    यदि(परिणाम)
      भङ्ग;
    mqstate(दत्त, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  पक्ष MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    यदि(curlx_dyn_len(&mq->sendbuf))
      भङ्ग;
    परिणाम = mqtt_disconnect(दत्त);
    यदि(परिणाम)
      भङ्ग;
    mqstate(दत्त, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  पक्ष MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    यदि(!curlx_dyn_len(&mq->sendbuf))
      *समाप्तम् = TRUE;
    भङ्ग;

  पक्ष MQTT_SUBACK:
  पक्ष MQTT_PUBWAIT:
  पक्ष MQTT_PUB_REMAIN:
    परिणाम = mqtt_read_publish(दत्त, समाप्तम्);
    भङ्ग;

  स्वभाव:
    failf(दत्त, "State not handled yet");
    *समाप्तम् = TRUE;
    भङ्ग;
  }

  यदि(परिणाम == CURLE_AGAIN)
    परिणाम = CURLE_OK;
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् USE_SSL

स्थिर CURLcode mqtts_connecting(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम;

  परिणाम = Curl_conn_connect(दत्त, FIRSTSOCKET, TRUE, समाप्तम्);
  यदि(परिणाम)
    connclose(conn);
  प्रत्यागम परिणाम;
}

/*
 * MQTTS protocol.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_mqtts = {
  mqtt_सज्जता_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_समाप्तम्,                          /* done */
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

#यदि_अन्तः

/*
 * MQTT protocol.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_mqtt = {
  mqtt_सज्जता_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_समाप्तम्,                          /* done */
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

#यदि_अन्तः /* CURL_DISABLE_MQTT */
