/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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
#include "telnet.h"

#ifndef CURL_DISABLE_TELNET

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include "url.h"
#include "transfer.h"
#include "sendf.h"
#include "curl_trc.h"
#include "progress.h"
#include "arpa_telnet.h"
#include "connect.h"
#include "select.h"
#include "curlx/strparse.h"

#define SUBBUFSIZE 512

#define CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#define CURL_SB_TERM(x)            \
  do {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } ໃນຂະນະທີ່(0)
#define CURL_SB_ACCUM(x, c)                                         \
  do {                                                              \
    ຖ້າ((x)->subpointer < ((x)->subbuffer + parimaa_na((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } ໃນຂະນະທີ່(0)

#define CURL_SB_GET(x) ((*(x)->subpointer++) & 0xff)
#define CURL_SB_LEN(x) ((x)->subend - (x)->subpointer)

/* For posterity:
#define CURL_SB_PEEK(x) (*(x)->subpointer & 0xff)
#define CURL_SB_EOF(x)  ((x)->subpointer >= (x)->subend) */

/* For negotiation compliant to RFC 1143 */
#define CURL_NO          0
#define CURL_YES         1
#define CURL_WANTYES     2
#define CURL_WANTNO      3

#define CURL_EMPTY       0
#define CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#define CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
typedef ga_nanaa {
  CURL_TS_DATA = 0,
  CURL_TS_IAC,
  CURL_TS_WILL,
  CURL_TS_WONT,
  CURL_TS_DO,
  CURL_TS_DONT,
  CURL_TS_CR,
  CURL_TS_SB,   /* sub-option collection */
  CURL_TS_SE    /* looking for sub-option end */
} TelnetReceive;

sa_nracanaa TELNET {
  ຈຳນວນເຕັມ please_negotiate;
  ຈຳນວນເຕັມ already_negotiated;
  ຈຳນວນເຕັມ us[256];
  ຈຳນວນເຕັມ usq[256];
  ຈຳນວນເຕັມ us_preferred[256];
  ຈຳນວນເຕັມ him[256];
  ຈຳນວນເຕັມ himq[256];
  ຈຳນວນເຕັມ him_preferred[256];
  ຈຳນວນເຕັມ subnegotiation[256];
  const var_na *subopt_ttype;          /* Set with suboption TTYPE */
  const var_na *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  unsigned short subopt_wsx;         /* Set with suboption NAWS */
  unsigned short subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  sa_nracanaa curl_slist *telnet_vars;    /* Environment variables */
  sa_nracanaa dynbuf out;                 /* output buffer */

  /* suboptions */
  unsigned var_na subbuffer[SUBBUFSIZE];
  unsigned var_na *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#ບໍ່ດັ່ງນັ້ນ
static void printoption(sa_nracanaa Curl_easy *data,
                        const var_na *direction, ຈຳນວນເຕັມ cmd, ຈຳນວນເຕັມ option)
{
  ຖ້າ(data->set.verbose) {
    ຖ້າ(cmd == CURL_IAC) {
      ຖ້າ(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      ບໍ່ດັ່ງນັ້ນ
        infof(data, "%s IAC %d", direction, option);
    }
    ບໍ່ດັ່ງນັ້ນ {
      const var_na *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      ຖ້າ(fmt) {
        const var_na *opt;
        ຖ້າ(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        ບໍ່ດັ່ງນັ້ນ
          opt = NULL;

        ຖ້າ(opt)
          infof(data, "%s %s %s", direction, fmt, opt);
        ບໍ່ດັ່ງນັ້ນ
          infof(data, "%s %s %d", direction, fmt, option);
      }
      ບໍ່ດັ່ງນັ້ນ
        infof(data, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

static void telnet_easy_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa TELNET *tn = entry;
  (void)key;
  (void)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

static CURLcode init_telnet(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa TELNET *tn;

  tn = curlx_calloc(1, parimaa_na(sa_nracanaa TELNET));
  ຖ້າ(!tn)
    ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&tn->out, 0xffff);

  tn->telrcv_state = CURL_TS_DATA;

  /* Init suboptions */
  CURL_SB_CLEAR(tn);

  /* Set the options we want by default */
  tn->us_preferred[CURL_TELOPT_SGA] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_SGA] = CURL_YES;

  /* To be compliant with previous releases of libcurl we enable this option
     by default. This behavior can be changed with the "BINARY" option in
     CURLOPT_TELNETOPTIONS */
  tn->us_preferred[CURL_TELOPT_BINARY] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_BINARY] = CURL_YES;

  /* We must allow the server to echo what we sent but it is not necessary
     to request the server to do so (it might force the server to close
     the connection). Hence, we ignore ECHO in the negotiate function */
  tn->him_preferred[CURL_TELOPT_ECHO] = CURL_YES;

  /* Set the subnegotiation fields to send information after negotiation
     passed (do/will)

     Default values are (0,0) initialized by calloc.
     According to the RFC1013 it is valid:
     A value equal to zero is acceptable for the width (or height), and means
     that no character width (or height) is being sent. In this case, the width
     (or height) that will be assumed by the Telnet server is operating system
     specific (it will probably be based upon the terminal type information
     that may have been sent using the TERMINAL TYPE Telnet option). */
  tn->subnegotiation[CURL_TELOPT_NAWS] = CURL_YES;

  ສົ່ງຄືນ Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

static void send_negotiation(sa_nracanaa Curl_easy *data, ຈຳນວນເຕັມ cmd, ຈຳນວນເຕັມ option)
{
  unsigned var_na buf[3];
  size_t nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (unsigned var_na)cmd;
  buf[2] = (unsigned var_na)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  ຖ້າ(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

static void set_remote_option(sa_nracanaa Curl_easy *data, sa_nracanaa TELNET *tn,
                              ຈຳນວນເຕັມ option, ຈຳນວນເຕັມ newstate)
{
  ຖ້າ(newstate == CURL_YES) {
    switch(tn->him[option]) {
    pak_sa CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      bha_nga;

    pak_sa CURL_YES:
      /* Already enabled */
      bha_nga;

    pak_sa CURL_WANTNO:
      switch(tn->himq[option]) {
      pak_sa CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        bha_nga;
      pak_sa CURL_OPPOSITE:
        /* Error: already queued an enable request */
        bha_nga;
      }
      bha_nga;

    pak_sa CURL_WANTYES:
      switch(tn->himq[option]) {
      pak_sa CURL_EMPTY:
        /* Error: already negotiating for enable */
        bha_nga;
      pak_sa CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        bha_nga;
      }
      bha_nga;
    }
  }
  ບໍ່ດັ່ງນັ້ນ { /* NO */
    switch(tn->him[option]) {
    pak_sa CURL_NO:
      /* Already disabled */
      bha_nga;

    pak_sa CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      bha_nga;

    pak_sa CURL_WANTNO:
      switch(tn->himq[option]) {
      pak_sa CURL_EMPTY:
        /* Already negotiating for NO */
        bha_nga;
      pak_sa CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        bha_nga;
      }
      bha_nga;

    pak_sa CURL_WANTYES:
      switch(tn->himq[option]) {
      pak_sa CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        bha_nga;
      pak_sa CURL_OPPOSITE:
        bha_nga;
      }
      bha_nga;
    }
  }
}

static void set_local_option(sa_nracanaa Curl_easy *data, sa_nracanaa TELNET *tn,
                             ຈຳນວນເຕັມ option, ຈຳນວນເຕັມ newstate)
{
  ຖ້າ(newstate == CURL_YES) {
    switch(tn->us[option]) {
    pak_sa CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      bha_nga;

    pak_sa CURL_YES:
      /* Already enabled */
      bha_nga;

    pak_sa CURL_WANTNO:
      switch(tn->usq[option]) {
      pak_sa CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        bha_nga;
      pak_sa CURL_OPPOSITE:
        /* Error: already queued an enable request */
        bha_nga;
      }
      bha_nga;

    pak_sa CURL_WANTYES:
      switch(tn->usq[option]) {
      pak_sa CURL_EMPTY:
        /* Error: already negotiating for enable */
        bha_nga;
      pak_sa CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        bha_nga;
      }
      bha_nga;
    }
  }
  ບໍ່ດັ່ງນັ້ນ { /* NO */
    switch(tn->us[option]) {
    pak_sa CURL_NO:
      /* Already disabled */
      bha_nga;

    pak_sa CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      bha_nga;

    pak_sa CURL_WANTNO:
      switch(tn->usq[option]) {
      pak_sa CURL_EMPTY:
        /* Already negotiating for NO */
        bha_nga;
      pak_sa CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        bha_nga;
      }
      bha_nga;

    pak_sa CURL_WANTYES:
      switch(tn->usq[option]) {
      pak_sa CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        bha_nga;
      pak_sa CURL_OPPOSITE:
        bha_nga;
      }
      bha_nga;
    }
  }
}

static void telnet_negotiate(sa_nracanaa Curl_easy *data, sa_nracanaa TELNET *tn)
{
  ຈຳນວນເຕັມ i;

  ສຳລັບ(i = 0; i < CURL_NTELOPTS; i++) {
    ຖ້າ(i == CURL_TELOPT_ECHO)
      continue;

    ຖ້າ(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    ຖ້າ(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

static void rec_will(sa_nracanaa Curl_easy *data, sa_nracanaa TELNET *tn, ຈຳນວນເຕັມ option)
{
  switch(tn->him[option]) {
  pak_sa CURL_NO:
    ຖ້າ(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    ບໍ່ດັ່ງນັ້ນ
      send_negotiation(data, CURL_DONT, option);

    bha_nga;

  pak_sa CURL_YES:
    /* Already enabled */
    bha_nga;

  pak_sa CURL_WANTNO:
    switch(tn->himq[option]) {
    pak_sa CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      bha_nga;
    pak_sa CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      bha_nga;
    }
    bha_nga;

  pak_sa CURL_WANTYES:
    switch(tn->himq[option]) {
    pak_sa CURL_EMPTY:
      tn->him[option] = CURL_YES;
      bha_nga;
    pak_sa CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      bha_nga;
    }
    bha_nga;
  }
}

static void rec_wont(sa_nracanaa Curl_easy *data, sa_nracanaa TELNET *tn, ຈຳນວນເຕັມ option)
{
  switch(tn->him[option]) {
  pak_sa CURL_NO:
    /* Already disabled */
    bha_nga;

  pak_sa CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    bha_nga;

  pak_sa CURL_WANTNO:
    switch(tn->himq[option]) {
    pak_sa CURL_EMPTY:
      tn->him[option] = CURL_NO;
      bha_nga;

    pak_sa CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      bha_nga;
    }
    bha_nga;

  pak_sa CURL_WANTYES:
    switch(tn->himq[option]) {
    pak_sa CURL_EMPTY:
      tn->him[option] = CURL_NO;
      bha_nga;
    pak_sa CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      bha_nga;
    }
    bha_nga;
  }
}

static void printsub(sa_nracanaa Curl_easy *data,
                     ຈຳນວນເຕັມ direction,                /* '<' or '>' */
                     const unsigned var_na *pointer, /* ptr to suboption data */
                     size_t length)                /* suboption data length */
{
  ຖ້າ(data->set.verbose) {
    unsigned ຈຳນວນເຕັມ i = 0;
    ຖ້າ(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      ຖ້າ(length >= 3) {
        ຈຳນວນເຕັມ j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        ຖ້າ(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          ຖ້າ(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          ບໍ່ດັ່ງນັ້ນ ຖ້າ(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          ບໍ່ດັ່ງນັ້ນ
            infof(data, "%u ", i);
          ຖ້າ(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          ບໍ່ດັ່ງນັ້ນ ຖ້າ(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          ບໍ່ດັ່ງນັ້ນ
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      ຖ້າ(length >= 2)
        length -= 2;
      ບໍ່ດັ່ງນັ້ນ /* bad input */
        ສົ່ງຄືນ;
    }
    ຖ້າ(length <= 1) {
      infof(data, "(Empty suboption?)");
      ສົ່ງຄືນ;
    }

    ຖ້າ(CURL_TELOPT_OK(pointer[0])) {
      switch(pointer[0]) {
      pak_sa CURL_TELOPT_TTYPE:
      pak_sa CURL_TELOPT_XDISPLOC:
      pak_sa CURL_TELOPT_NEW_ENVIRON:
      pak_sa CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        bha_nga;
      default:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        bha_nga;
      }
    }
    ບໍ່ດັ່ງນັ້ນ
      infof(data, "%d (unknown)", pointer[0]);

    switch(pointer[0]) {
    pak_sa CURL_TELOPT_NAWS:
      ຖ້າ(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      bha_nga;
    default:
      switch(pointer[1]) {
      pak_sa CURL_TELQUAL_IS:
        infof(data, " IS");
        bha_nga;
      pak_sa CURL_TELQUAL_SEND:
        infof(data, " SEND");
        bha_nga;
      pak_sa CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        bha_nga;
      pak_sa CURL_TELQUAL_NAME:
        infof(data, " NAME");
        bha_nga;
      }

      switch(pointer[0]) {
      pak_sa CURL_TELOPT_TTYPE:
      pak_sa CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (ຈຳນວນເຕັມ)((length > 2) ? (length - 2) : 0), &pointer[2]);
        bha_nga;
      pak_sa CURL_TELOPT_NEW_ENVIRON:
        ຖ້າ(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          ສຳລັບ(i = 3; i < length; i++) {
            switch(pointer[i]) {
            pak_sa CURL_NEW_ENV_VAR:
              infof(data, ", ");
              bha_nga;
            pak_sa CURL_NEW_ENV_VALUE:
              infof(data, " = ");
              bha_nga;
            default:
              infof(data, "%c", pointer[i]);
              bha_nga;
            }
          }
        }
        bha_nga;
      default:
        ສຳລັບ(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        bha_nga;
      }
    }
  }
}

/* Escape and send a telnet data block */
static CURLcode send_telnet_data(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa TELNET *tn,
                                 const var_na *buffer, ssize_t nread)
{
  size_t i, outlen;
  const unsigned var_na *outbuf;
  CURLcode result = CURLE_OK;
  size_t bytes_written;
  size_t total_written = 0;
  sa_nracanaa connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  ຖ້າ(nread < 0)
    ສົ່ງຄືນ CURLE_TOO_LARGE;

  ຖ້າ(memchr(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    ສຳລັບ(i = 0; i < (size_t)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      ຖ້າ(!result && ((unsigned var_na)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  ບໍ່ດັ່ງນັ້ນ {
    outlen = (size_t)nread;
    outbuf = (const unsigned var_na *)buffer;
  }
  ໃນຂະນະທີ່(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    sa_nracanaa pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    ຖ້າ(timeout_ms < 0)
      ສົ່ງຄືນ CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    switch(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    pak_sa -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      bha_nga;
    pak_sa 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      bha_nga;
    default:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      bha_nga;
    }
  }

  ສົ່ງຄືນ result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
static void sendsuboption(sa_nracanaa Curl_easy *data,
                          sa_nracanaa TELNET *tn, ຈຳນວນເຕັມ option)
{
  size_t nwritten;
  unsigned short x, y;
  const unsigned var_na *uc1, *uc2;
  CURLcode result;

  switch(option) {
  pak_sa CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (const unsigned var_na *)&x;
    uc2 = (const unsigned var_na *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (const unsigned var_na *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    ຖ້າ(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (const var_na *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    ຖ້າ(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    bha_nga;
  }
}

static void rec_do(sa_nracanaa Curl_easy *data, sa_nracanaa TELNET *tn, ຈຳນວນເຕັມ option)
{
  switch(tn->us[option]) {
  pak_sa CURL_NO:
    ຖ້າ(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      ຖ້າ(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    ບໍ່ດັ່ງນັ້ນ
      send_negotiation(data, CURL_WONT, option);
    bha_nga;

  pak_sa CURL_YES:
    /* Already enabled */
    bha_nga;

  pak_sa CURL_WANTNO:
    switch(tn->usq[option]) {
    pak_sa CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      bha_nga;
    pak_sa CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      bha_nga;
    }
    bha_nga;

  pak_sa CURL_WANTYES:
    switch(tn->usq[option]) {
    pak_sa CURL_EMPTY:
      tn->us[option] = CURL_YES;
      ຖ້າ(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      bha_nga;
    pak_sa CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      bha_nga;
    }
    bha_nga;
  }
}

static void rec_dont(sa_nracanaa Curl_easy *data, sa_nracanaa TELNET *tn, ຈຳນວນເຕັມ option)
{
  switch(tn->us[option]) {
  pak_sa CURL_NO:
    /* Already disabled */
    bha_nga;

  pak_sa CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    bha_nga;

  pak_sa CURL_WANTNO:
    switch(tn->usq[option]) {
    pak_sa CURL_EMPTY:
      tn->us[option] = CURL_NO;
      bha_nga;

    pak_sa CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      bha_nga;
    }
    bha_nga;

  pak_sa CURL_WANTYES:
    switch(tn->usq[option]) {
    pak_sa CURL_EMPTY:
      tn->us[option] = CURL_NO;
      bha_nga;
    pak_sa CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      bha_nga;
    }
    bha_nga;
  }
}

static bool str_is_nonascii(const var_na *str)
{
  var_na c;
  ໃນຂະນະທີ່((c = *str++) != 0)
    ຖ້າ(c & 0x80)
      ສົ່ງຄືນ TRUE;

  ສົ່ງຄືນ FALSE;
}

static CURLcode check_telnet_options(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa TELNET *tn)
{
  sa_nracanaa curl_slist *head;
  sa_nracanaa curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  ຖ້າ(data->state.creds) {
    var_na buffer[256];
    ຖ້າ(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, parimaa_na(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    ຖ້າ(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = NULL;
      ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  ສຳລັບ(head = data->set.telnet_options; head && !result; head = head->next) {
    size_t olen;
    const var_na *option = head->data;
    const var_na *arg;
    const var_na *sep = strchr(option, '=');
    ຖ້າ(sep) {
      olen = sep - option;
      arg = ++sep;
      ຖ້າ(str_is_nonascii(arg))
        continue;
      switch(olen) {
      pak_sa 5:
        /* Terminal type */
        ຖ້າ(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          bha_nga;
        }
        result = CURLE_UNKNOWN_OPTION;
        bha_nga;

      pak_sa 8:
        /* Display variable */
        ຖ້າ(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          bha_nga;
        }
        result = CURLE_UNKNOWN_OPTION;
        bha_nga;

      pak_sa 7:
        /* Environment variable */
        ຖ້າ(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          ຖ້າ(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            bha_nga;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        ບໍ່ດັ່ງນັ້ນ
          result = CURLE_UNKNOWN_OPTION;
        bha_nga;

      pak_sa 2:
        /* Window Size */
        ຖ້າ(curl_strnequal(option, "WS", 2)) {
          const var_na *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          ຖ້າ(curlx_str_number(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_number(&p, &y, 0xffff)) {
            failf(data, "Syntax error in telnet option: %s", head->data);
            result = CURLE_SETOPT_OPTION_SYNTAX;
          }
          ບໍ່ດັ່ງນັ້ນ {
            tn->subopt_wsx = (unsigned short)x;
            tn->subopt_wsy = (unsigned short)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        ບໍ່ດັ່ງນັ້ນ
          result = CURLE_UNKNOWN_OPTION;
        bha_nga;

      pak_sa 6:
        /* To take care or not of the 8th bit in data exchange */
        ຖ້າ(curl_strnequal(option, "BINARY", 6)) {
          const var_na *p = arg;
          curl_off_t binary_option;
          ຖ້າ(!curlx_str_number(&p, &binary_option, 1) &&
             (binary_option != 1)) {
            tn->us_preferred[CURL_TELOPT_BINARY] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_BINARY] = CURL_NO;
          }
        }
        ບໍ່ດັ່ງນັ້ນ
          result = CURLE_UNKNOWN_OPTION;
        bha_nga;
      default:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        bha_nga;
      }
    }
    ບໍ່ດັ່ງນັ້ນ {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  ຖ້າ(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = NULL;
  }

  ສົ່ງຄືນ result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
static bool bad_option(const var_na *data)
{
  ສົ່ງຄືນ !data || !!strchr(data, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
static CURLcode suboption(sa_nracanaa Curl_easy *data, sa_nracanaa TELNET *tn)
{
  sa_nracanaa curl_slist *v;
  unsigned var_na temp[2048];
  size_t nwritten;
  size_t len;
  CURLcode result = CURLE_OK;

  ຖ້າ(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    ສົ່ງຄືນ CURLE_OK;

  printsub(data, '<', (const unsigned var_na *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  switch(CURL_SB_GET(tn)) {
  pak_sa CURL_TELOPT_TTYPE:
    ຖ້າ(bad_option(tn->subopt_ttype))
      ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;
    ຖ້າ(strlen(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      ສົ່ງຄືນ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((var_na *)temp, parimaa_na(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    ຖ້າ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ສົ່ງຄືນ result;
    }
    printsub(data, '>', &temp[2], len-2);
    bha_nga;
  pak_sa CURL_TELOPT_XDISPLOC:
    ຖ້າ(bad_option(tn->subopt_xdisploc))
      ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;
    ຖ້າ(strlen(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      ສົ່ງຄືນ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((var_na *)temp, parimaa_na(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    ຖ້າ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ສົ່ງຄືນ result;
    }
    printsub(data, '>', &temp[2], len - 2);
    bha_nga;
  pak_sa CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((var_na *)temp, parimaa_na(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    ສຳລັບ(v = tn->telnet_vars; v; v = v->next) {
      size_t tmplen = (strlen(v->data) + 1);
      ຖ້າ(bad_option(v->data))
        ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      ຖ້າ(len + tmplen < parimaa_na(temp) - 6) {
        const var_na *s = strchr(v->data, ',');
        ຖ້າ(!s)
          len += curl_msnprintf((var_na *)&temp[len], parimaa_na(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        ບໍ່ດັ່ງນັ້ນ {
          size_t vlen = s - v->data;
          len += curl_msnprintf((var_na *)&temp[len], parimaa_na(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (ຈຳນວນເຕັມ)vlen, v->data, CURL_NEW_ENV_VALUE, ++s);
        }
      }
    }
    curl_msnprintf((var_na *)&temp[len], parimaa_na(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    ຖ້າ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ສົ່ງຄືນ result;
    }
    printsub(data, '>', &temp[2], len - 2);
    bha_nga;
  }
  ສົ່ງຄືນ CURLE_OK;
}

static CURLcode telrcv(sa_nracanaa Curl_easy *data,
                       sa_nracanaa TELNET *tn,
                       const unsigned var_na *inbuf, /* Data received from
                                                      socket */
                       ssize_t count)              /* Number of bytes
                                                      received */
{
  unsigned var_na c;
  CURLcode result;
  ຈຳນວນເຕັມ in = 0;
  ຈຳນວນເຕັມ startwrite = -1;

#define startskipping()                                          \
  ຖ້າ(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (const var_na *)&inbuf[startwrite], \
                               in-startwrite);                   \
    ຖ້າ(result)                                                   \
      ສົ່ງຄືນ result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  ຖ້າ(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  ໃນຂະນະທີ່(count--) {
    c = inbuf[in];

    switch(tn->telrcv_state) {
    pak_sa CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      ຖ້າ(c == '\0') {
        startskipping();
        bha_nga;   /* Ignore \0 after CR */
      }
      writebyte();
      bha_nga;

    pak_sa CURL_TS_DATA:
      ຖ້າ(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        bha_nga;
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      bha_nga;

    pak_sa CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      switch(c) {
      pak_sa CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        bha_nga;
      pak_sa CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        bha_nga;
      pak_sa CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        bha_nga;
      pak_sa CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        bha_nga;
      pak_sa CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        bha_nga;
      pak_sa CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        bha_nga;
      pak_sa CURL_DM:
      pak_sa CURL_NOP:
      pak_sa CURL_GA:
      default:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        bha_nga;
      }
      bha_nga;

    pak_sa CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      bha_nga;

    pak_sa CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      bha_nga;

    pak_sa CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      bha_nga;

    pak_sa CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      bha_nga;

    pak_sa CURL_TS_SB:
      ຖ້າ(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      ບໍ່ດັ່ງນັ້ນ
        CURL_SB_ACCUM(tn, c);
      bha_nga;

    pak_sa CURL_TS_SE:
      ຖ້າ(c != CURL_SE) {
        ຖ້າ(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          ສົ່ງຄືນ CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      ບໍ່ດັ່ງນັ້ນ {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        result = suboption(data, tn);   /* handle sub-option */
        ຖ້າ(result)
          ສົ່ງຄືນ result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      bha_nga;
    }
    ++in;
  }
  bufferflush();
  ສົ່ງຄືນ CURLE_OK;
}

static CURLcode telnet_done(sa_nracanaa Curl_easy *data,
                            CURLcode status, bool premature)
{
  (void)status;
  (void)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  ສົ່ງຄືນ CURLE_OK;
}

static CURLcode telnet_do(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result;
  sa_nracanaa connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ifdef USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_count;
  DWORD wait_timeout;
  DWORD readfile_read;
#ບໍ່ດັ່ງນັ້ນ
  timediff_t interval_ms;
  sa_nracanaa pollfd pfd[2];
  ຈຳນວນເຕັມ poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  var_na buffer[4 * 1024];
  sa_nracanaa TELNET *tn;

  *done = TRUE; /* unconditionally */

  result = init_telnet(data);
  ຖ້າ(result)
    ສົ່ງຄືນ result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  ຖ້າ(!tn)
    ສົ່ງຄືນ CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  ຖ້າ(result)
    ສົ່ງຄືນ result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  ຖ້າ(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    ສົ່ງຄືນ CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  ຖ້າ(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    ສົ່ງຄືນ CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_INPUT_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  ຖ້າ(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_count = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  ບໍ່ດັ່ງນັ້ນ {
    obj_count = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  ໃນຂະນະທີ່(keepon) {
    const DWORD buf_size = (DWORD)parimaa_na(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_count, objs,
                                           FALSE, wait_timeout);
    switch(waitret) {

    pak_sa WAIT_TIMEOUT: {
      ສຳລັບ(;;) {
        ຖ້າ(data->set.is_fread_set) {
          size_t n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          ຖ້າ(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            bha_nga;
          }

          ຖ້າ(n == CURL_READFUNC_PAUSE)
            bha_nga;

          ຖ້າ(n == 0)                        /* no bytes */
            bha_nga;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        ບໍ່ດັ່ງນັ້ນ {
          /* read from stdin */
          ຖ້າ(!PeekNamedPipe(stdin_handle, NULL, 0, NULL,
                            &readfile_read, NULL)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            bha_nga;
          }

          ຖ້າ(!readfile_read)
            bha_nga;

          ຖ້າ(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, NULL)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            bha_nga;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        ຖ້າ(result) {
          keepon = FALSE;
          bha_nga;
        }
      }
    }
    bha_nga;

    pak_sa WAIT_OBJECT_0 + 1: {
      ຖ້າ(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, NULL)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        bha_nga;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      ຖ້າ(result) {
        keepon = FALSE;
        bha_nga;
      }
    }
    bha_nga;

    pak_sa WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      ຖ້າ(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        ຈຳນວນເຕັມ sockerr = SOCKERRNO;
        ຖ້າ(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        bha_nga;
      }
      ຖ້າ(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, parimaa_na(buffer), &nread);
        /* read would have blocked. Loop again */
        ຖ້າ(result == CURLE_AGAIN)
          bha_nga;
        /* returned not-zero, this an error */
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(result) {
          keepon = FALSE;
          bha_nga;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(!nread) {
          keepon = FALSE;
          bha_nga;
        }

        result = telrcv(data, tn, (unsigned var_na *)buffer, nread);
        ຖ້າ(result) {
          keepon = FALSE;
          bha_nga;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ຖ້າ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      ຖ້າ(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      bha_nga;
    }
    } /* switch */

    ຖ້າ(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  ຖ້າ(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#ບໍ່ດັ່ງນັ້ນ
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  ຖ້າ(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  ບໍ່ດັ່ງນັ້ນ {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    ຖ້າ(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  ໃນຂະນະທີ່(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    switch(Curl_poll(pfd, (unsigned ຈຳນວນເຕັມ)poll_cnt, interval_ms)) {
    pak_sa -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      continue;
    pak_sa 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    default:                    /* read! */
      ຖ້າ(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, parimaa_na(buffer), &nread);
        /* read would have blocked. Loop again */
        ຖ້າ(result == CURLE_AGAIN)
          bha_nga;
        /* returned not-zero, this an error */
        ຖ້າ(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          ຖ້າ(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          bha_nga;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(!nread) {
          keepon = FALSE;
          bha_nga;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (unsigned var_na *)buffer, nread);
        ຖ້າ(result) {
          keepon = FALSE;
          bha_nga;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ຖ້າ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      ຖ້າ(poll_cnt == 2) {
        ຖ້າ(pfd[1].revents & POLLIN) { /* read from in file */
          snread = read(pfd[1].fd, buffer, parimaa_na(buffer));
        }
      }
      ບໍ່ດັ່ງນັ້ນ {
        /* read from user-supplied method */
        snread = (ຈຳນວນເຕັມ)data->state.fread_func(buffer, 1, parimaa_na(buffer),
                                             data->state.in);
        ຖ້າ(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          bha_nga;
        }
        ຖ້າ(snread == CURL_READFUNC_PAUSE)
          bha_nga;
      }

      ຖ້າ(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        ຖ້າ(result) {
          keepon = FALSE;
          bha_nga;
        }
        Curl_pgrs_upload_inc(data, (size_t)snread);
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      bha_nga;
    } /* poll switch statement */

    ຖ້າ(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    ຖ້າ(!result) {
      result = Curl_pgrsUpdate(data);
      ຖ້າ(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  ສົ່ງຄືນ result;
}

/*
 * TELNET protocol handler.
 */
const sa_nracanaa Curl_protocol Curl_protocol_telnet = {
  ZERO_NULL,                            /* setup_connection */
  telnet_do,                            /* do_it */
  telnet_done,                          /* done */
  ZERO_NULL,                            /* do_more */
  ZERO_NULL,                            /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  ZERO_NULL,                            /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  ZERO_NULL,                            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#endif /* !CURL_DISABLE_TELNET */
