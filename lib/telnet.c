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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "telnet.h"

#ifndef CURL_DISABLE_TELNET

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#அந்தர்பாவ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#அந்தர்பாவ <sys/param.h>
#endif

#அந்தர்பாவ "url.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "arpa_telnet.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "curlx/strparse.h"

#define SUBBUFSIZE 512

#define CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#define CURL_SB_TERM(x)            \
  செய் {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } போது(0)
#define CURL_SB_ACCUM(x, c)                                         \
  செய் {                                                              \
    எனில்((x)->subpointer < ((x)->subbuffer + அளவு((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } போது(0)

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
வகைவரை எண்ணிக்கை {
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

கட்டமைப்பு TELNET {
  முழு please_negotiate;
  முழு already_negotiated;
  முழு us[256];
  முழு usq[256];
  முழு us_preferred[256];
  முழு him[256];
  முழு himq[256];
  முழு him_preferred[256];
  முழு subnegotiation[256];
  மாறிலி எழுத்து *subopt_ttype;          /* Set with suboption TTYPE */
  மாறிலி எழுத்து *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  unsigned short subopt_wsx;         /* Set with suboption NAWS */
  unsigned short subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  கட்டமைப்பு curl_slist *telnet_vars;    /* Environment variables */
  கட்டமைப்பு dynbuf out;                 /* output buffer */

  /* suboptions */
  unsigned எழுத்து subbuffer[SUBBUFSIZE];
  unsigned எழுத்து *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#இல்லையெனில்
நிலையான வெறுமை printoption(கட்டமைப்பு Curl_easy *data,
                        மாறிலி எழுத்து *direction, முழு cmd, முழு option)
{
  எனில்(data->set.verbose) {
    எனில்(cmd == CURL_IAC) {
      எனில்(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      இல்லையெனில்
        infof(data, "%s IAC %d", direction, option);
    }
    இல்லையெனில் {
      மாறிலி எழுத்து *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      எனில்(fmt) {
        மாறிலி எழுத்து *opt;
        எனில்(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        இல்லையெனில் எனில்(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        இல்லையெனில்
          opt = வெற்று;

        எனில்(opt)
          infof(data, "%s %s %s", direction, fmt, opt);
        இல்லையெனில்
          infof(data, "%s %s %d", direction, fmt, option);
      }
      இல்லையெனில்
        infof(data, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

நிலையான வெறுமை telnet_easy_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு TELNET *tn = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

நிலையான CURLcode init_telnet(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு TELNET *tn;

  tn = curlx_calloc(1, அளவு(கட்டமைப்பு TELNET));
  எனில்(!tn)
    திரும்பு CURLE_OUT_OF_MEMORY;

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
  tn->us_preferred[CURL_TELOPT_இருமம்] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_இருமம்] = CURL_YES;

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

  திரும்பு Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

நிலையான வெறுமை send_negotiation(கட்டமைப்பு Curl_easy *data, முழு cmd, முழு option)
{
  unsigned எழுத்து buf[3];
  size_t nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (unsigned எழுத்து)cmd;
  buf[2] = (unsigned எழுத்து)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  எனில்(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

நிலையான வெறுமை set_remote_option(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு TELNET *tn,
                              முழு option, முழு newstate)
{
  எனில்(newstate == CURL_YES) {
    தேர்வு(tn->him[option]) {
    நிலை CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      நிறுத்து;

    நிலை CURL_YES:
      /* Already enabled */
      நிறுத்து;

    நிலை CURL_WANTNO:
      தேர்வு(tn->himq[option]) {
      நிலை CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        நிறுத்து;
      நிலை CURL_OPPOSITE:
        /* Error: already queued an enable request */
        நிறுத்து;
      }
      நிறுத்து;

    நிலை CURL_WANTYES:
      தேர்வு(tn->himq[option]) {
      நிலை CURL_EMPTY:
        /* Error: already negotiating for enable */
        நிறுத்து;
      நிலை CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        நிறுத்து;
      }
      நிறுத்து;
    }
  }
  இல்லையெனில் { /* NO */
    தேர்வு(tn->him[option]) {
    நிலை CURL_NO:
      /* Already disabled */
      நிறுத்து;

    நிலை CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      நிறுத்து;

    நிலை CURL_WANTNO:
      தேர்வு(tn->himq[option]) {
      நிலை CURL_EMPTY:
        /* Already negotiating for NO */
        நிறுத்து;
      நிலை CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        நிறுத்து;
      }
      நிறுத்து;

    நிலை CURL_WANTYES:
      தேர்வு(tn->himq[option]) {
      நிலை CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        நிறுத்து;
      நிலை CURL_OPPOSITE:
        நிறுத்து;
      }
      நிறுத்து;
    }
  }
}

நிலையான வெறுமை set_local_option(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு TELNET *tn,
                             முழு option, முழு newstate)
{
  எனில்(newstate == CURL_YES) {
    தேர்வு(tn->us[option]) {
    நிலை CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      நிறுத்து;

    நிலை CURL_YES:
      /* Already enabled */
      நிறுத்து;

    நிலை CURL_WANTNO:
      தேர்வு(tn->usq[option]) {
      நிலை CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        நிறுத்து;
      நிலை CURL_OPPOSITE:
        /* Error: already queued an enable request */
        நிறுத்து;
      }
      நிறுத்து;

    நிலை CURL_WANTYES:
      தேர்வு(tn->usq[option]) {
      நிலை CURL_EMPTY:
        /* Error: already negotiating for enable */
        நிறுத்து;
      நிலை CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        நிறுத்து;
      }
      நிறுத்து;
    }
  }
  இல்லையெனில் { /* NO */
    தேர்வு(tn->us[option]) {
    நிலை CURL_NO:
      /* Already disabled */
      நிறுத்து;

    நிலை CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      நிறுத்து;

    நிலை CURL_WANTNO:
      தேர்வு(tn->usq[option]) {
      நிலை CURL_EMPTY:
        /* Already negotiating for NO */
        நிறுத்து;
      நிலை CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        நிறுத்து;
      }
      நிறுத்து;

    நிலை CURL_WANTYES:
      தேர்வு(tn->usq[option]) {
      நிலை CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        நிறுத்து;
      நிலை CURL_OPPOSITE:
        நிறுத்து;
      }
      நிறுத்து;
    }
  }
}

நிலையான வெறுமை telnet_negotiate(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு TELNET *tn)
{
  முழு i;

  க்கு(i = 0; i < CURL_NTELOPTS; i++) {
    எனில்(i == CURL_TELOPT_ECHO)
      தொடர்;

    எனில்(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    எனில்(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

நிலையான வெறுமை rec_will(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு TELNET *tn, முழு option)
{
  தேர்வு(tn->him[option]) {
  நிலை CURL_NO:
    எனில்(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    இல்லையெனில்
      send_negotiation(data, CURL_DONT, option);

    நிறுத்து;

  நிலை CURL_YES:
    /* Already enabled */
    நிறுத்து;

  நிலை CURL_WANTNO:
    தேர்வு(tn->himq[option]) {
    நிலை CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      நிறுத்து;
    நிலை CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      நிறுத்து;
    }
    நிறுத்து;

  நிலை CURL_WANTYES:
    தேர்வு(tn->himq[option]) {
    நிலை CURL_EMPTY:
      tn->him[option] = CURL_YES;
      நிறுத்து;
    நிலை CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      நிறுத்து;
    }
    நிறுத்து;
  }
}

நிலையான வெறுமை rec_wont(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு TELNET *tn, முழு option)
{
  தேர்வு(tn->him[option]) {
  நிலை CURL_NO:
    /* Already disabled */
    நிறுத்து;

  நிலை CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    நிறுத்து;

  நிலை CURL_WANTNO:
    தேர்வு(tn->himq[option]) {
    நிலை CURL_EMPTY:
      tn->him[option] = CURL_NO;
      நிறுத்து;

    நிலை CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      நிறுத்து;
    }
    நிறுத்து;

  நிலை CURL_WANTYES:
    தேர்வு(tn->himq[option]) {
    நிலை CURL_EMPTY:
      tn->him[option] = CURL_NO;
      நிறுத்து;
    நிலை CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      நிறுத்து;
    }
    நிறுத்து;
  }
}

நிலையான வெறுமை printsub(கட்டமைப்பு Curl_easy *data,
                     முழு direction,                /* '<' or '>' */
                     மாறிலி unsigned எழுத்து *pointer, /* ptr to suboption data */
                     size_t length)                /* suboption data length */
{
  எனில்(data->set.verbose) {
    unsigned முழு i = 0;
    எனில்(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      எனில்(length >= 3) {
        முழு j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        எனில்(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          எனில்(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          இல்லையெனில் எனில்(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          இல்லையெனில்
            infof(data, "%u ", i);
          எனில்(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          இல்லையெனில் எனில்(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          இல்லையெனில்
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      எனில்(length >= 2)
        length -= 2;
      இல்லையெனில் /* bad input */
        திரும்பு;
    }
    எனில்(length <= 1) {
      infof(data, "(Empty suboption?)");
      திரும்பு;
    }

    எனில்(CURL_TELOPT_OK(pointer[0])) {
      தேர்வு(pointer[0]) {
      நிலை CURL_TELOPT_TTYPE:
      நிலை CURL_TELOPT_XDISPLOC:
      நிலை CURL_TELOPT_NEW_ENVIRON:
      நிலை CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        நிறுத்து;
      இயல்பு:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        நிறுத்து;
      }
    }
    இல்லையெனில்
      infof(data, "%d (unknown)", pointer[0]);

    தேர்வு(pointer[0]) {
    நிலை CURL_TELOPT_NAWS:
      எனில்(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      நிறுத்து;
    இயல்பு:
      தேர்வு(pointer[1]) {
      நிலை CURL_TELQUAL_IS:
        infof(data, " IS");
        நிறுத்து;
      நிலை CURL_TELQUAL_SEND:
        infof(data, " SEND");
        நிறுத்து;
      நிலை CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        நிறுத்து;
      நிலை CURL_TELQUAL_NAME:
        infof(data, " NAME");
        நிறுத்து;
      }

      தேர்வு(pointer[0]) {
      நிலை CURL_TELOPT_TTYPE:
      நிலை CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (முழு)((length > 2) ? (length - 2) : 0), &pointer[2]);
        நிறுத்து;
      நிலை CURL_TELOPT_NEW_ENVIRON:
        எனில்(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          க்கு(i = 3; i < length; i++) {
            தேர்வு(pointer[i]) {
            நிலை CURL_NEW_ENV_VAR:
              infof(data, ", ");
              நிறுத்து;
            நிலை CURL_NEW_ENV_மதிப்பு:
              infof(data, " = ");
              நிறுத்து;
            இயல்பு:
              infof(data, "%c", pointer[i]);
              நிறுத்து;
            }
          }
        }
        நிறுத்து;
      இயல்பு:
        க்கு(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        நிறுத்து;
      }
    }
  }
}

/* Escape and send a telnet data block */
நிலையான CURLcode send_telnet_data(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு TELNET *tn,
                                 மாறிலி எழுத்து *buffer, ssize_t nread)
{
  size_t i, outlen;
  மாறிலி unsigned எழுத்து *outbuf;
  CURLcode result = CURLE_OK;
  size_t bytes_written;
  size_t total_written = 0;
  கட்டமைப்பு connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  எனில்(nread < 0)
    திரும்பு CURLE_TOO_LARGE;

  எனில்(memchr(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    க்கு(i = 0; i < (size_t)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      எனில்(!result && ((unsigned எழுத்து)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  இல்லையெனில் {
    outlen = (size_t)nread;
    outbuf = (மாறிலி unsigned எழுத்து *)buffer;
  }
  போது(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    கட்டமைப்பு pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    எனில்(timeout_ms < 0)
      திரும்பு CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    தேர்வு(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    நிலை -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      நிறுத்து;
    நிலை 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      நிறுத்து;
    இயல்பு:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      நிறுத்து;
    }
  }

  திரும்பு result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
நிலையான வெறுமை sendsuboption(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு TELNET *tn, முழு option)
{
  size_t nwritten;
  unsigned short x, y;
  மாறிலி unsigned எழுத்து *uc1, *uc2;
  CURLcode result;

  தேர்வு(option) {
  நிலை CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (மாறிலி unsigned எழுத்து *)&x;
    uc2 = (மாறிலி unsigned எழுத்து *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (மாறிலி unsigned எழுத்து *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    எனில்(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (மாறிலி எழுத்து *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    எனில்(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    நிறுத்து;
  }
}

நிலையான வெறுமை rec_do(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு TELNET *tn, முழு option)
{
  தேர்வு(tn->us[option]) {
  நிலை CURL_NO:
    எனில்(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      எனில்(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    இல்லையெனில் எனில்(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    இல்லையெனில்
      send_negotiation(data, CURL_WONT, option);
    நிறுத்து;

  நிலை CURL_YES:
    /* Already enabled */
    நிறுத்து;

  நிலை CURL_WANTNO:
    தேர்வு(tn->usq[option]) {
    நிலை CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      நிறுத்து;
    நிலை CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      நிறுத்து;
    }
    நிறுத்து;

  நிலை CURL_WANTYES:
    தேர்வு(tn->usq[option]) {
    நிலை CURL_EMPTY:
      tn->us[option] = CURL_YES;
      எனில்(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      நிறுத்து;
    நிலை CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      நிறுத்து;
    }
    நிறுத்து;
  }
}

நிலையான வெறுமை rec_dont(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு TELNET *tn, முழு option)
{
  தேர்வு(tn->us[option]) {
  நிலை CURL_NO:
    /* Already disabled */
    நிறுத்து;

  நிலை CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    நிறுத்து;

  நிலை CURL_WANTNO:
    தேர்வு(tn->usq[option]) {
    நிலை CURL_EMPTY:
      tn->us[option] = CURL_NO;
      நிறுத்து;

    நிலை CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      நிறுத்து;
    }
    நிறுத்து;

  நிலை CURL_WANTYES:
    தேர்வு(tn->usq[option]) {
    நிலை CURL_EMPTY:
      tn->us[option] = CURL_NO;
      நிறுத்து;
    நிலை CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      நிறுத்து;
    }
    நிறுத்து;
  }
}

நிலையான bool str_is_nonascii(மாறிலி எழுத்து *str)
{
  எழுத்து c;
  போது((c = *str++) != 0)
    எனில்(c & 0x80)
      திரும்பு TRUE;

  திரும்பு FALSE;
}

நிலையான CURLcode check_telnet_options(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு TELNET *tn)
{
  கட்டமைப்பு curl_slist *head;
  கட்டமைப்பு curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  எனில்(data->state.creds) {
    எழுத்து buffer[256];
    எனில்(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, அளவு(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    எனில்(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = வெற்று;
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  க்கு(head = data->set.telnet_options; head && !result; head = head->next) {
    size_t olen;
    மாறிலி எழுத்து *option = head->data;
    மாறிலி எழுத்து *arg;
    மாறிலி எழுத்து *sep = strchr(option, '=');
    எனில்(sep) {
      olen = sep - option;
      arg = ++sep;
      எனில்(str_is_nonascii(arg))
        தொடர்;
      தேர்வு(olen) {
      நிலை 5:
        /* Terminal type */
        எனில்(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          நிறுத்து;
        }
        result = CURLE_UNKNOWN_OPTION;
        நிறுத்து;

      நிலை 8:
        /* Display variable */
        எனில்(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          நிறுத்து;
        }
        result = CURLE_UNKNOWN_OPTION;
        நிறுத்து;

      நிலை 7:
        /* Environment variable */
        எனில்(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          எனில்(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            நிறுத்து;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        இல்லையெனில்
          result = CURLE_UNKNOWN_OPTION;
        நிறுத்து;

      நிலை 2:
        /* Window Size */
        எனில்(curl_strnequal(option, "WS", 2)) {
          மாறிலி எழுத்து *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          எனில்(curlx_str_எண்(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_எண்(&p, &y, 0xffff)) {
            failf(data, "Syntax error in telnet option: %s", head->data);
            result = CURLE_SETOPT_OPTION_SYNTAX;
          }
          இல்லையெனில் {
            tn->subopt_wsx = (unsigned short)x;
            tn->subopt_wsy = (unsigned short)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        இல்லையெனில்
          result = CURLE_UNKNOWN_OPTION;
        நிறுத்து;

      நிலை 6:
        /* To take care or not of the 8th bit in data exchange */
        எனில்(curl_strnequal(option, "BINARY", 6)) {
          மாறிலி எழுத்து *p = arg;
          curl_off_t இருமம்_option;
          எனில்(!curlx_str_எண்(&p, &இருமம்_option, 1) &&
             (இருமம்_option != 1)) {
            tn->us_preferred[CURL_TELOPT_இருமம்] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_இருமம்] = CURL_NO;
          }
        }
        இல்லையெனில்
          result = CURLE_UNKNOWN_OPTION;
        நிறுத்து;
      இயல்பு:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        நிறுத்து;
      }
    }
    இல்லையெனில் {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  எனில்(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = வெற்று;
  }

  திரும்பு result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
நிலையான bool bad_option(மாறிலி எழுத்து *data)
{
  திரும்பு !data || !!strchr(data, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
நிலையான CURLcode suboption(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு TELNET *tn)
{
  கட்டமைப்பு curl_slist *v;
  unsigned எழுத்து temp[2048];
  size_t nwritten;
  size_t len;
  CURLcode result = CURLE_OK;

  எனில்(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    திரும்பு CURLE_OK;

  printsub(data, '<', (மாறிலி unsigned எழுத்து *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  தேர்வு(CURL_SB_GET(tn)) {
  நிலை CURL_TELOPT_TTYPE:
    எனில்(bad_option(tn->subopt_ttype))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    எனில்(strlen(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      திரும்பு CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((எழுத்து *)temp, அளவு(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    எனில்(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      திரும்பு result;
    }
    printsub(data, '>', &temp[2], len-2);
    நிறுத்து;
  நிலை CURL_TELOPT_XDISPLOC:
    எனில்(bad_option(tn->subopt_xdisploc))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    எனில்(strlen(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      திரும்பு CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((எழுத்து *)temp, அளவு(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    எனில்(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      திரும்பு result;
    }
    printsub(data, '>', &temp[2], len - 2);
    நிறுத்து;
  நிலை CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((எழுத்து *)temp, அளவு(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    க்கு(v = tn->telnet_vars; v; v = v->next) {
      size_t tmplen = (strlen(v->data) + 1);
      எனில்(bad_option(v->data))
        திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      எனில்(len + tmplen < அளவு(temp) - 6) {
        மாறிலி எழுத்து *s = strchr(v->data, ',');
        எனில்(!s)
          len += curl_msnprintf((எழுத்து *)&temp[len], அளவு(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        இல்லையெனில் {
          size_t vlen = s - v->data;
          len += curl_msnprintf((எழுத்து *)&temp[len], அளவு(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (முழு)vlen, v->data, CURL_NEW_ENV_மதிப்பு, ++s);
        }
      }
    }
    curl_msnprintf((எழுத்து *)&temp[len], அளவு(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    எனில்(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      திரும்பு result;
    }
    printsub(data, '>', &temp[2], len - 2);
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode telrcv(கட்டமைப்பு Curl_easy *data,
                       கட்டமைப்பு TELNET *tn,
                       மாறிலி unsigned எழுத்து *inbuf, /* Data received from
                                                      socket */
                       ssize_t மொத்தம்)              /* Number of bytes
                                                      received */
{
  unsigned எழுத்து c;
  CURLcode result;
  முழு in = 0;
  முழு startwrite = -1;

#define startskipping()                                          \
  எனில்(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (மாறிலி எழுத்து *)&inbuf[startwrite], \
                               in-startwrite);                   \
    எனில்(result)                                                   \
      திரும்பு result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  எனில்(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  போது(மொத்தம்--) {
    c = inbuf[in];

    தேர்வு(tn->telrcv_state) {
    நிலை CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      எனில்(c == '\0') {
        startskipping();
        நிறுத்து;   /* Ignore \0 after CR */
      }
      writebyte();
      நிறுத்து;

    நிலை CURL_TS_DATA:
      எனில்(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        நிறுத்து;
      }
      இல்லையெனில் எனில்(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      நிறுத்து;

    நிலை CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      தேர்வு(c) {
      நிலை CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        நிறுத்து;
      நிலை CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        நிறுத்து;
      நிலை CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        நிறுத்து;
      நிலை CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        நிறுத்து;
      நிலை CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        நிறுத்து;
      நிலை CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        நிறுத்து;
      நிலை CURL_DM:
      நிலை CURL_NOP:
      நிலை CURL_GA:
      இயல்பு:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        நிறுத்து;
      }
      நிறுத்து;

    நிலை CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      நிறுத்து;

    நிலை CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      நிறுத்து;

    நிலை CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      நிறுத்து;

    நிலை CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      நிறுத்து;

    நிலை CURL_TS_SB:
      எனில்(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      இல்லையெனில்
        CURL_SB_ACCUM(tn, c);
      நிறுத்து;

    நிலை CURL_TS_SE:
      எனில்(c != CURL_SE) {
        எனில்(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          திரும்பு CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      இல்லையெனில் {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        result = suboption(data, tn);   /* handle sub-option */
        எனில்(result)
          திரும்பு result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      நிறுத்து;
    }
    ++in;
  }
  bufferflush();
  திரும்பு CURLE_OK;
}

நிலையான CURLcode telnet_done(கட்டமைப்பு Curl_easy *data,
                            CURLcode status, bool premature)
{
  (வெறுமை)status;
  (வெறுமை)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode telnet_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result;
  கட்டமைப்பு connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ifdef USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_மொத்தம்;
  DWORD wait_timeout;
  DWORD readfile_read;
#இல்லையெனில்
  timediff_t interval_ms;
  கட்டமைப்பு pollfd pfd[2];
  முழு poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  எழுத்து buffer[4 * 1024];
  கட்டமைப்பு TELNET *tn;

  *done = TRUE; /* unconditionally */

  result = init_telnet(data);
  எனில்(result)
    திரும்பு result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  எனில்(!tn)
    திரும்பு CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  எனில்(result)
    திரும்பு result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  எனில்(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    திரும்பு CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  எனில்(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    திரும்பு CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_உள்ளீடு_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  எனில்(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_மொத்தம் = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  இல்லையெனில் {
    obj_மொத்தம் = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  போது(keepon) {
    மாறிலி DWORD buf_size = (DWORD)அளவு(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_மொத்தம், objs,
                                           FALSE, wait_timeout);
    தேர்வு(waitret) {

    நிலை WAIT_TIMEOUT: {
      க்கு(;;) {
        எனில்(data->set.is_fread_set) {
          size_t n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          எனில்(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            நிறுத்து;
          }

          எனில்(n == CURL_READFUNC_PAUSE)
            நிறுத்து;

          எனில்(n == 0)                        /* no bytes */
            நிறுத்து;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        இல்லையெனில் {
          /* read from stdin */
          எனில்(!PeekNamedPipe(stdin_handle, வெற்று, 0, வெற்று,
                            &readfile_read, வெற்று)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            நிறுத்து;
          }

          எனில்(!readfile_read)
            நிறுத்து;

          எனில்(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, வெற்று)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            நிறுத்து;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        எனில்(result) {
          keepon = FALSE;
          நிறுத்து;
        }
      }
    }
    நிறுத்து;

    நிலை WAIT_OBJECT_0 + 1: {
      எனில்(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, வெற்று)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        நிறுத்து;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      எனில்(result) {
        keepon = FALSE;
        நிறுத்து;
      }
    }
    நிறுத்து;

    நிலை WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      எனில்(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        முழு sockerr = SOCKERRNO;
        எனில்(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        நிறுத்து;
      }
      எனில்(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, அளவு(buffer), &nread);
        /* read would have blocked. Loop again */
        எனில்(result == CURLE_AGAIN)
          நிறுத்து;
        /* returned not-zero, this an error */
        இல்லையெனில் எனில்(result) {
          keepon = FALSE;
          நிறுத்து;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        இல்லையெனில் எனில்(!nread) {
          keepon = FALSE;
          நிறுத்து;
        }

        result = telrcv(data, tn, (unsigned எழுத்து *)buffer, nread);
        எனில்(result) {
          keepon = FALSE;
          நிறுத்து;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        எனில்(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      எனில்(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      நிறுத்து;
    }
    } /* switch */

    எனில்(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  எனில்(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#இல்லையெனில்
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  எனில்(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  இல்லையெனில் {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    எனில்(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  போது(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    தேர்வு(Curl_poll(pfd, (unsigned முழு)poll_cnt, interval_ms)) {
    நிலை -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      தொடர்;
    நிலை 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    இயல்பு:                    /* read! */
      எனில்(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, அளவு(buffer), &nread);
        /* read would have blocked. Loop again */
        எனில்(result == CURLE_AGAIN)
          நிறுத்து;
        /* returned not-zero, this an error */
        எனில்(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          எனில்(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          நிறுத்து;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        இல்லையெனில் எனில்(!nread) {
          keepon = FALSE;
          நிறுத்து;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (unsigned எழுத்து *)buffer, nread);
        எனில்(result) {
          keepon = FALSE;
          நிறுத்து;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        எனில்(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      எனில்(poll_cnt == 2) {
        எனில்(pfd[1].revents & POLLIN) { /* read from in file */
          snread = பட(pfd[1].fd, buffer, அளவு(buffer));
        }
      }
      இல்லையெனில் {
        /* read from user-supplied method */
        snread = (முழு)data->state.fread_func(buffer, 1, அளவு(buffer),
                                             data->state.in);
        எனில்(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          நிறுத்து;
        }
        எனில்(snread == CURL_READFUNC_PAUSE)
          நிறுத்து;
      }

      எனில்(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        எனில்(result) {
          keepon = FALSE;
          நிறுத்து;
        }
        Curl_pgrs_upload_inc(data, (size_t)snread);
      }
      இல்லையெனில் எனில்(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      நிறுத்து;
    } /* poll switch statement */

    எனில்(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    எனில்(!result) {
      result = Curl_pgrsUpdate(data);
      எனில்(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  திரும்பு result;
}

/*
 * TELNET protocol handler.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_telnet = {
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
