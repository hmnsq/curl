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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "telnet.h"

#ifndef CURL_DISABLE_TELNET

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#અન્તર્ભાવ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#અન્તર્ભાવ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#અન્તર્ભાવ <sys/param.h>
#endif

#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "arpa_telnet.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "curlx/strparse.h"

#define SUBBUFSIZE 512

#define CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#define CURL_SB_TERM(x)            \
  કરો {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } જ્યારે(0)
#define CURL_SB_ACCUM(x, c)                                         \
  કરો {                                                              \
    જો((x)->subpointer < ((x)->subbuffer + કદ((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } જ્યારે(0)

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
પ્રકારવ્યાખ્યા ગણના {
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

સંરચના TELNET {
  પૂર્ણાંક please_negotiate;
  પૂર્ણાંક already_negotiated;
  પૂર્ણાંક us[256];
  પૂર્ણાંક usq[256];
  પૂર્ણાંક us_preferred[256];
  પૂર્ણાંક him[256];
  પૂર્ણાંક himq[256];
  પૂર્ણાંક him_preferred[256];
  પૂર્ણાંક subnegotiation[256];
  અચલ અક્ષર *subopt_ttype;          /* Set with suboption TTYPE */
  અચલ અક્ષર *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  unsigned short subopt_wsx;         /* Set with suboption NAWS */
  unsigned short subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  સંરચના curl_slist *telnet_vars;    /* Environment variables */
  સંરચના dynbuf out;                 /* output buffer */

  /* suboptions */
  unsigned અક્ષર subbuffer[SUBBUFSIZE];
  unsigned અક્ષર *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#નહીંતર
સ્થિર શૂન્ય printoption(સંરચના Curl_easy *data,
                        અચલ અક્ષર *direction, પૂર્ણાંક cmd, પૂર્ણાંક option)
{
  જો(data->set.verbose) {
    જો(cmd == CURL_IAC) {
      જો(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      નહીંતર
        infof(data, "%s IAC %d", direction, option);
    }
    નહીંતર {
      અચલ અક્ષર *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      જો(fmt) {
        અચલ અક્ષર *opt;
        જો(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        નહીંતર જો(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        નહીંતર
          opt = ખાલી;

        જો(opt)
          infof(data, "%s %s %s", direction, fmt, opt);
        નહીંતર
          infof(data, "%s %s %d", direction, fmt, option);
      }
      નહીંતર
        infof(data, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

સ્થિર શૂન્ય telnet_easy_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના TELNET *tn = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

સ્થિર CURLcode init_telnet(સંરચના Curl_easy *data)
{
  સંરચના TELNET *tn;

  tn = curlx_calloc(1, કદ(સંરચના TELNET));
  જો(!tn)
    પાછા CURLE_OUT_OF_MEMORY;

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
  tn->us_preferred[CURL_TELOPT_દ્વિઅંકી] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_દ્વિઅંકી] = CURL_YES;

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

  પાછા Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

સ્થિર શૂન્ય send_negotiation(સંરચના Curl_easy *data, પૂર્ણાંક cmd, પૂર્ણાંક option)
{
  unsigned અક્ષર buf[3];
  size_t nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (unsigned અક્ષર)cmd;
  buf[2] = (unsigned અક્ષર)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  જો(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

સ્થિર શૂન્ય set_remote_option(સંરચના Curl_easy *data, સંરચના TELNET *tn,
                              પૂર્ણાંક option, પૂર્ણાંક newstate)
{
  જો(newstate == CURL_YES) {
    પસંદગી(tn->him[option]) {
    સ્થિતિ CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      તોડો;

    સ્થિતિ CURL_YES:
      /* Already enabled */
      તોડો;

    સ્થિતિ CURL_WANTNO:
      પસંદગી(tn->himq[option]) {
      સ્થિતિ CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        તોડો;
      સ્થિતિ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        તોડો;
      }
      તોડો;

    સ્થિતિ CURL_WANTYES:
      પસંદગી(tn->himq[option]) {
      સ્થિતિ CURL_EMPTY:
        /* Error: already negotiating for enable */
        તોડો;
      સ્થિતિ CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        તોડો;
      }
      તોડો;
    }
  }
  નહીંતર { /* NO */
    પસંદગી(tn->him[option]) {
    સ્થિતિ CURL_NO:
      /* Already disabled */
      તોડો;

    સ્થિતિ CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      તોડો;

    સ્થિતિ CURL_WANTNO:
      પસંદગી(tn->himq[option]) {
      સ્થિતિ CURL_EMPTY:
        /* Already negotiating for NO */
        તોડો;
      સ્થિતિ CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        તોડો;
      }
      તોડો;

    સ્થિતિ CURL_WANTYES:
      પસંદગી(tn->himq[option]) {
      સ્થિતિ CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        તોડો;
      સ્થિતિ CURL_OPPOSITE:
        તોડો;
      }
      તોડો;
    }
  }
}

સ્થિર શૂન્ય set_local_option(સંરચના Curl_easy *data, સંરચના TELNET *tn,
                             પૂર્ણાંક option, પૂર્ણાંક newstate)
{
  જો(newstate == CURL_YES) {
    પસંદગી(tn->us[option]) {
    સ્થિતિ CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      તોડો;

    સ્થિતિ CURL_YES:
      /* Already enabled */
      તોડો;

    સ્થિતિ CURL_WANTNO:
      પસંદગી(tn->usq[option]) {
      સ્થિતિ CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        તોડો;
      સ્થિતિ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        તોડો;
      }
      તોડો;

    સ્થિતિ CURL_WANTYES:
      પસંદગી(tn->usq[option]) {
      સ્થિતિ CURL_EMPTY:
        /* Error: already negotiating for enable */
        તોડો;
      સ્થિતિ CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        તોડો;
      }
      તોડો;
    }
  }
  નહીંતર { /* NO */
    પસંદગી(tn->us[option]) {
    સ્થિતિ CURL_NO:
      /* Already disabled */
      તોડો;

    સ્થિતિ CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      તોડો;

    સ્થિતિ CURL_WANTNO:
      પસંદગી(tn->usq[option]) {
      સ્થિતિ CURL_EMPTY:
        /* Already negotiating for NO */
        તોડો;
      સ્થિતિ CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        તોડો;
      }
      તોડો;

    સ્થિતિ CURL_WANTYES:
      પસંદગી(tn->usq[option]) {
      સ્થિતિ CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        તોડો;
      સ્થિતિ CURL_OPPOSITE:
        તોડો;
      }
      તોડો;
    }
  }
}

સ્થિર શૂન્ય telnet_negotiate(સંરચના Curl_easy *data, સંરચના TELNET *tn)
{
  પૂર્ણાંક i;

  માટે(i = 0; i < CURL_NTELOPTS; i++) {
    જો(i == CURL_TELOPT_ECHO)
      ચાલુ;

    જો(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    જો(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

સ્થિર શૂન્ય rec_will(સંરચના Curl_easy *data, સંરચના TELNET *tn, પૂર્ણાંક option)
{
  પસંદગી(tn->him[option]) {
  સ્થિતિ CURL_NO:
    જો(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    નહીંતર
      send_negotiation(data, CURL_DONT, option);

    તોડો;

  સ્થિતિ CURL_YES:
    /* Already enabled */
    તોડો;

  સ્થિતિ CURL_WANTNO:
    પસંદગી(tn->himq[option]) {
    સ્થિતિ CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      તોડો;
    સ્થિતિ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      તોડો;
    }
    તોડો;

  સ્થિતિ CURL_WANTYES:
    પસંદગી(tn->himq[option]) {
    સ્થિતિ CURL_EMPTY:
      tn->him[option] = CURL_YES;
      તોડો;
    સ્થિતિ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      તોડો;
    }
    તોડો;
  }
}

સ્થિર શૂન્ય rec_wont(સંરચના Curl_easy *data, સંરચના TELNET *tn, પૂર્ણાંક option)
{
  પસંદગી(tn->him[option]) {
  સ્થિતિ CURL_NO:
    /* Already disabled */
    તોડો;

  સ્થિતિ CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    તોડો;

  સ્થિતિ CURL_WANTNO:
    પસંદગી(tn->himq[option]) {
    સ્થિતિ CURL_EMPTY:
      tn->him[option] = CURL_NO;
      તોડો;

    સ્થિતિ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      તોડો;
    }
    તોડો;

  સ્થિતિ CURL_WANTYES:
    પસંદગી(tn->himq[option]) {
    સ્થિતિ CURL_EMPTY:
      tn->him[option] = CURL_NO;
      તોડો;
    સ્થિતિ CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      તોડો;
    }
    તોડો;
  }
}

સ્થિર શૂન્ય printsub(સંરચના Curl_easy *data,
                     પૂર્ણાંક direction,                /* '<' or '>' */
                     અચલ unsigned અક્ષર *pointer, /* ptr to suboption data */
                     size_t length)                /* suboption data length */
{
  જો(data->set.verbose) {
    unsigned પૂર્ણાંક i = 0;
    જો(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      જો(length >= 3) {
        પૂર્ણાંક j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        જો(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          જો(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          નહીંતર જો(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          નહીંતર
            infof(data, "%u ", i);
          જો(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          નહીંતર જો(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          નહીંતર
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      જો(length >= 2)
        length -= 2;
      નહીંતર /* bad input */
        પાછા;
    }
    જો(length <= 1) {
      infof(data, "(Empty suboption?)");
      પાછા;
    }

    જો(CURL_TELOPT_OK(pointer[0])) {
      પસંદગી(pointer[0]) {
      સ્થિતિ CURL_TELOPT_TTYPE:
      સ્થિતિ CURL_TELOPT_XDISPLOC:
      સ્થિતિ CURL_TELOPT_NEW_ENVIRON:
      સ્થિતિ CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        તોડો;
      મૂળભૂત:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        તોડો;
      }
    }
    નહીંતર
      infof(data, "%d (unknown)", pointer[0]);

    પસંદગી(pointer[0]) {
    સ્થિતિ CURL_TELOPT_NAWS:
      જો(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      તોડો;
    મૂળભૂત:
      પસંદગી(pointer[1]) {
      સ્થિતિ CURL_TELQUAL_IS:
        infof(data, " IS");
        તોડો;
      સ્થિતિ CURL_TELQUAL_SEND:
        infof(data, " SEND");
        તોડો;
      સ્થિતિ CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        તોડો;
      સ્થિતિ CURL_TELQUAL_NAME:
        infof(data, " NAME");
        તોડો;
      }

      પસંદગી(pointer[0]) {
      સ્થિતિ CURL_TELOPT_TTYPE:
      સ્થિતિ CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (પૂર્ણાંક)((length > 2) ? (length - 2) : 0), &pointer[2]);
        તોડો;
      સ્થિતિ CURL_TELOPT_NEW_ENVIRON:
        જો(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          માટે(i = 3; i < length; i++) {
            પસંદગી(pointer[i]) {
            સ્થિતિ CURL_NEW_ENV_VAR:
              infof(data, ", ");
              તોડો;
            સ્થિતિ CURL_NEW_ENV_કિંમત:
              infof(data, " = ");
              તોડો;
            મૂળભૂત:
              infof(data, "%c", pointer[i]);
              તોડો;
            }
          }
        }
        તોડો;
      મૂળભૂત:
        માટે(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        તોડો;
      }
    }
  }
}

/* Escape and send a telnet data block */
સ્થિર CURLcode send_telnet_data(સંરચના Curl_easy *data,
                                 સંરચના TELNET *tn,
                                 અચલ અક્ષર *buffer, ssize_t nread)
{
  size_t i, outlen;
  અચલ unsigned અક્ષર *outbuf;
  CURLcode result = CURLE_OK;
  size_t bytes_written;
  size_t total_written = 0;
  સંરચના connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  જો(nread < 0)
    પાછા CURLE_TOO_LARGE;

  જો(memchr(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    માટે(i = 0; i < (size_t)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      જો(!result && ((unsigned અક્ષર)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  નહીંતર {
    outlen = (size_t)nread;
    outbuf = (અચલ unsigned અક્ષર *)buffer;
  }
  જ્યારે(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    સંરચના pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    જો(timeout_ms < 0)
      પાછા CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    પસંદગી(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    સ્થિતિ -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      તોડો;
    સ્થિતિ 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      તોડો;
    મૂળભૂત:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      તોડો;
    }
  }

  પાછા result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
સ્થિર શૂન્ય sendsuboption(સંરચના Curl_easy *data,
                          સંરચના TELNET *tn, પૂર્ણાંક option)
{
  size_t nwritten;
  unsigned short x, y;
  અચલ unsigned અક્ષર *uc1, *uc2;
  CURLcode result;

  પસંદગી(option) {
  સ્થિતિ CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (અચલ unsigned અક્ષર *)&x;
    uc2 = (અચલ unsigned અક્ષર *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (અચલ unsigned અક્ષર *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    જો(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (અચલ અક્ષર *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    જો(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    તોડો;
  }
}

સ્થિર શૂન્ય rec_do(સંરચના Curl_easy *data, સંરચના TELNET *tn, પૂર્ણાંક option)
{
  પસંદગી(tn->us[option]) {
  સ્થિતિ CURL_NO:
    જો(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      જો(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    નહીંતર જો(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    નહીંતર
      send_negotiation(data, CURL_WONT, option);
    તોડો;

  સ્થિતિ CURL_YES:
    /* Already enabled */
    તોડો;

  સ્થિતિ CURL_WANTNO:
    પસંદગી(tn->usq[option]) {
    સ્થિતિ CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      તોડો;
    સ્થિતિ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      તોડો;
    }
    તોડો;

  સ્થિતિ CURL_WANTYES:
    પસંદગી(tn->usq[option]) {
    સ્થિતિ CURL_EMPTY:
      tn->us[option] = CURL_YES;
      જો(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      તોડો;
    સ્થિતિ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      તોડો;
    }
    તોડો;
  }
}

સ્થિર શૂન્ય rec_dont(સંરચના Curl_easy *data, સંરચના TELNET *tn, પૂર્ણાંક option)
{
  પસંદગી(tn->us[option]) {
  સ્થિતિ CURL_NO:
    /* Already disabled */
    તોડો;

  સ્થિતિ CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    તોડો;

  સ્થિતિ CURL_WANTNO:
    પસંદગી(tn->usq[option]) {
    સ્થિતિ CURL_EMPTY:
      tn->us[option] = CURL_NO;
      તોડો;

    સ્થિતિ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      તોડો;
    }
    તોડો;

  સ્થિતિ CURL_WANTYES:
    પસંદગી(tn->usq[option]) {
    સ્થિતિ CURL_EMPTY:
      tn->us[option] = CURL_NO;
      તોડો;
    સ્થિતિ CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      તોડો;
    }
    તોડો;
  }
}

સ્થિર bool str_is_nonascii(અચલ અક્ષર *str)
{
  અક્ષર c;
  જ્યારે((c = *str++) != 0)
    જો(c & 0x80)
      પાછા TRUE;

  પાછા FALSE;
}

સ્થિર CURLcode check_telnet_options(સંરચના Curl_easy *data,
                                     સંરચના TELNET *tn)
{
  સંરચના curl_slist *head;
  સંરચના curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  જો(data->state.creds) {
    અક્ષર buffer[256];
    જો(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, કદ(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    જો(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = ખાલી;
      પાછા CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  માટે(head = data->set.telnet_options; head && !result; head = head->next) {
    size_t olen;
    અચલ અક્ષર *option = head->data;
    અચલ અક્ષર *arg;
    અચલ અક્ષર *sep = strchr(option, '=');
    જો(sep) {
      olen = sep - option;
      arg = ++sep;
      જો(str_is_nonascii(arg))
        ચાલુ;
      પસંદગી(olen) {
      સ્થિતિ 5:
        /* Terminal type */
        જો(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          તોડો;
        }
        result = CURLE_UNKNOWN_OPTION;
        તોડો;

      સ્થિતિ 8:
        /* Display variable */
        જો(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          તોડો;
        }
        result = CURLE_UNKNOWN_OPTION;
        તોડો;

      સ્થિતિ 7:
        /* Environment variable */
        જો(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          જો(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            તોડો;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        નહીંતર
          result = CURLE_UNKNOWN_OPTION;
        તોડો;

      સ્થિતિ 2:
        /* Window Size */
        જો(curl_strnequal(option, "WS", 2)) {
          અચલ અક્ષર *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          જો(curlx_str_સંખ્યા(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_સંખ્યા(&p, &y, 0xffff)) {
            failf(data, "Syntax error in telnet option: %s", head->data);
            result = CURLE_SETOPT_OPTION_SYNTAX;
          }
          નહીંતર {
            tn->subopt_wsx = (unsigned short)x;
            tn->subopt_wsy = (unsigned short)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        નહીંતર
          result = CURLE_UNKNOWN_OPTION;
        તોડો;

      સ્થિતિ 6:
        /* To take care or not of the 8th bit in data exchange */
        જો(curl_strnequal(option, "BINARY", 6)) {
          અચલ અક્ષર *p = arg;
          curl_off_t દ્વિઅંકી_option;
          જો(!curlx_str_સંખ્યા(&p, &દ્વિઅંકી_option, 1) &&
             (દ્વિઅંકી_option != 1)) {
            tn->us_preferred[CURL_TELOPT_દ્વિઅંકી] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_દ્વિઅંકી] = CURL_NO;
          }
        }
        નહીંતર
          result = CURLE_UNKNOWN_OPTION;
        તોડો;
      મૂળભૂત:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        તોડો;
      }
    }
    નહીંતર {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  જો(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = ખાલી;
  }

  પાછા result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
સ્થિર bool bad_option(અચલ અક્ષર *data)
{
  પાછા !data || !!strchr(data, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
સ્થિર CURLcode suboption(સંરચના Curl_easy *data, સંરચના TELNET *tn)
{
  સંરચના curl_slist *v;
  unsigned અક્ષર temp[2048];
  size_t nwritten;
  size_t len;
  CURLcode result = CURLE_OK;

  જો(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    પાછા CURLE_OK;

  printsub(data, '<', (અચલ unsigned અક્ષર *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  પસંદગી(CURL_SB_GET(tn)) {
  સ્થિતિ CURL_TELOPT_TTYPE:
    જો(bad_option(tn->subopt_ttype))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    જો(strlen(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      પાછા CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((અક્ષર *)temp, કદ(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    જો(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      પાછા result;
    }
    printsub(data, '>', &temp[2], len-2);
    તોડો;
  સ્થિતિ CURL_TELOPT_XDISPLOC:
    જો(bad_option(tn->subopt_xdisploc))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    જો(strlen(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      પાછા CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((અક્ષર *)temp, કદ(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    જો(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      પાછા result;
    }
    printsub(data, '>', &temp[2], len - 2);
    તોડો;
  સ્થિતિ CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((અક્ષર *)temp, કદ(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    માટે(v = tn->telnet_vars; v; v = v->next) {
      size_t tmplen = (strlen(v->data) + 1);
      જો(bad_option(v->data))
        પાછા CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      જો(len + tmplen < કદ(temp) - 6) {
        અચલ અક્ષર *s = strchr(v->data, ',');
        જો(!s)
          len += curl_msnprintf((અક્ષર *)&temp[len], કદ(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        નહીંતર {
          size_t vlen = s - v->data;
          len += curl_msnprintf((અક્ષર *)&temp[len], કદ(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (પૂર્ણાંક)vlen, v->data, CURL_NEW_ENV_કિંમત, ++s);
        }
      }
    }
    curl_msnprintf((અક્ષર *)&temp[len], કદ(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    જો(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      પાછા result;
    }
    printsub(data, '>', &temp[2], len - 2);
    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode telrcv(સંરચના Curl_easy *data,
                       સંરચના TELNET *tn,
                       અચલ unsigned અક્ષર *inbuf, /* Data received from
                                                      socket */
                       ssize_t ગણતરી)              /* Number of bytes
                                                      received */
{
  unsigned અક્ષર c;
  CURLcode result;
  પૂર્ણાંક in = 0;
  પૂર્ણાંક startwrite = -1;

#define startskipping()                                          \
  જો(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (અચલ અક્ષર *)&inbuf[startwrite], \
                               in-startwrite);                   \
    જો(result)                                                   \
      પાછા result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  જો(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  જ્યારે(ગણતરી--) {
    c = inbuf[in];

    પસંદગી(tn->telrcv_state) {
    સ્થિતિ CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      જો(c == '\0') {
        startskipping();
        તોડો;   /* Ignore \0 after CR */
      }
      writebyte();
      તોડો;

    સ્થિતિ CURL_TS_DATA:
      જો(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        તોડો;
      }
      નહીંતર જો(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      તોડો;

    સ્થિતિ CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      પસંદગી(c) {
      સ્થિતિ CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        તોડો;
      સ્થિતિ CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        તોડો;
      સ્થિતિ CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        તોડો;
      સ્થિતિ CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        તોડો;
      સ્થિતિ CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        તોડો;
      સ્થિતિ CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        તોડો;
      સ્થિતિ CURL_DM:
      સ્થિતિ CURL_NOP:
      સ્થિતિ CURL_GA:
      મૂળભૂત:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        તોડો;
      }
      તોડો;

    સ્થિતિ CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      તોડો;

    સ્થિતિ CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      તોડો;

    સ્થિતિ CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      તોડો;

    સ્થિતિ CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      તોડો;

    સ્થિતિ CURL_TS_SB:
      જો(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      નહીંતર
        CURL_SB_ACCUM(tn, c);
      તોડો;

    સ્થિતિ CURL_TS_SE:
      જો(c != CURL_SE) {
        જો(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          પાછા CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      નહીંતર {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        result = suboption(data, tn);   /* handle sub-option */
        જો(result)
          પાછા result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      તોડો;
    }
    ++in;
  }
  bufferflush();
  પાછા CURLE_OK;
}

સ્થિર CURLcode telnet_done(સંરચના Curl_easy *data,
                            CURLcode status, bool premature)
{
  (શૂન્ય)status;
  (શૂન્ય)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  પાછા CURLE_OK;
}

સ્થિર CURLcode telnet_do(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result;
  સંરચના connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ifdef USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_ગણતરી;
  DWORD wait_timeout;
  DWORD readfile_read;
#નહીંતર
  timediff_t interval_ms;
  સંરચના pollfd pfd[2];
  પૂર્ણાંક poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  અક્ષર buffer[4 * 1024];
  સંરચના TELNET *tn;

  *done = TRUE; /* unconditionally */

  result = init_telnet(data);
  જો(result)
    પાછા result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  જો(!tn)
    પાછા CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  જો(result)
    પાછા result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  જો(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    પાછા CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  જો(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    પાછા CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_આગત_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  જો(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_ગણતરી = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  નહીંતર {
    obj_ગણતરી = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  જ્યારે(keepon) {
    અચલ DWORD buf_size = (DWORD)કદ(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_ગણતરી, objs,
                                           FALSE, wait_timeout);
    પસંદગી(waitret) {

    સ્થિતિ WAIT_TIMEOUT: {
      માટે(;;) {
        જો(data->set.is_fread_set) {
          size_t n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          જો(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            તોડો;
          }

          જો(n == CURL_READFUNC_PAUSE)
            તોડો;

          જો(n == 0)                        /* no bytes */
            તોડો;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        નહીંતર {
          /* read from stdin */
          જો(!PeekNamedPipe(stdin_handle, ખાલી, 0, ખાલી,
                            &readfile_read, ખાલી)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            તોડો;
          }

          જો(!readfile_read)
            તોડો;

          જો(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, ખાલી)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            તોડો;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        જો(result) {
          keepon = FALSE;
          તોડો;
        }
      }
    }
    તોડો;

    સ્થિતિ WAIT_OBJECT_0 + 1: {
      જો(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, ખાલી)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        તોડો;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      જો(result) {
        keepon = FALSE;
        તોડો;
      }
    }
    તોડો;

    સ્થિતિ WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      જો(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        પૂર્ણાંક sockerr = SOCKERRNO;
        જો(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        તોડો;
      }
      જો(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, કદ(buffer), &nread);
        /* read would have blocked. Loop again */
        જો(result == CURLE_AGAIN)
          તોડો;
        /* returned not-zero, this an error */
        નહીંતર જો(result) {
          keepon = FALSE;
          તોડો;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        નહીંતર જો(!nread) {
          keepon = FALSE;
          તોડો;
        }

        result = telrcv(data, tn, (unsigned અક્ષર *)buffer, nread);
        જો(result) {
          keepon = FALSE;
          તોડો;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        જો(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      જો(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      તોડો;
    }
    } /* switch */

    જો(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  જો(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#નહીંતર
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  જો(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  નહીંતર {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    જો(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  જ્યારે(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    પસંદગી(Curl_poll(pfd, (unsigned પૂર્ણાંક)poll_cnt, interval_ms)) {
    સ્થિતિ -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      ચાલુ;
    સ્થિતિ 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    મૂળભૂત:                    /* read! */
      જો(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, કદ(buffer), &nread);
        /* read would have blocked. Loop again */
        જો(result == CURLE_AGAIN)
          તોડો;
        /* returned not-zero, this an error */
        જો(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          જો(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          તોડો;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        નહીંતર જો(!nread) {
          keepon = FALSE;
          તોડો;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (unsigned અક્ષર *)buffer, nread);
        જો(result) {
          keepon = FALSE;
          તોડો;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        જો(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      જો(poll_cnt == 2) {
        જો(pfd[1].revents & POLLIN) { /* read from in file */
          snread = પઠ(pfd[1].fd, buffer, કદ(buffer));
        }
      }
      નહીંતર {
        /* read from user-supplied method */
        snread = (પૂર્ણાંક)data->state.fread_func(buffer, 1, કદ(buffer),
                                             data->state.in);
        જો(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          તોડો;
        }
        જો(snread == CURL_READFUNC_PAUSE)
          તોડો;
      }

      જો(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        જો(result) {
          keepon = FALSE;
          તોડો;
        }
        Curl_pgrs_upload_inc(data, (size_t)snread);
      }
      નહીંતર જો(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      તોડો;
    } /* poll switch statement */

    જો(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    જો(!result) {
      result = Curl_pgrsUpdate(data);
      જો(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  પાછા result;
}

/*
 * TELNET protocol handler.
 */
અચલ સંરચના Curl_protocol Curl_protocol_telnet = {
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
