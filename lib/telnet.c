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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "telnet.h"

#ifndef CURL_DISABLE_TELNET

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif

#अन्तर्भाव "url.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "arpa_telnet.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "curlx/strparse.h"

#define SUBBUFSIZE 512

#define CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#define CURL_SB_TERM(x)            \
  करेहि {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } जाव(0)
#define CURL_SB_ACCUM(x, c)                                         \
  करेहि {                                                              \
    जइ((x)->subpointer < ((x)->subbuffer + परिमाण((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } जाव(0)

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
प्रकारनिर्देश गणना {
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

संरचना TELNET {
  पूर्णाङ्क please_negotiate;
  पूर्णाङ्क already_negotiated;
  पूर्णाङ्क us[256];
  पूर्णाङ्क usq[256];
  पूर्णाङ्क us_preferred[256];
  पूर्णाङ्क him[256];
  पूर्णाङ्क himq[256];
  पूर्णाङ्क him_preferred[256];
  पूर्णाङ्क subnegotiation[256];
  ध्रुव वर्ण *subopt_ttype;          /* Set with suboption TTYPE */
  ध्रुव वर्ण *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  unsigned short subopt_wsx;         /* Set with suboption NAWS */
  unsigned short subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  संरचना curl_slist *telnet_vars;    /* Environment variables */
  संरचना dynbuf out;                 /* output buffer */

  /* suboptions */
  unsigned वर्ण subbuffer[SUBBUFSIZE];
  unsigned वर्ण *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#अण्णहा
स्थिर शून्य printoption(संरचना Curl_easy *data,
                        ध्रुव वर्ण *direction, पूर्णाङ्क cmd, पूर्णाङ्क option)
{
  जइ(data->set.verbose) {
    जइ(cmd == CURL_IAC) {
      जइ(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      अण्णहा
        infof(data, "%s IAC %d", direction, option);
    }
    अण्णहा {
      ध्रुव वर्ण *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      जइ(fmt) {
        ध्रुव वर्ण *opt;
        जइ(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        अण्णहा जइ(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        अण्णहा
          opt = रिक्त;

        जइ(opt)
          infof(data, "%s %s %s", direction, fmt, opt);
        अण्णहा
          infof(data, "%s %s %d", direction, fmt, option);
      }
      अण्णहा
        infof(data, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

स्थिर शून्य telnet_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना TELNET *tn = entry;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

स्थिर CURLcode init_telnet(संरचना Curl_easy *data)
{
  संरचना TELNET *tn;

  tn = curlx_calloc(1, परिमाण(संरचना TELNET));
  जइ(!tn)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

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
  tn->us_preferred[CURL_TELOPT_द्विमान] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_द्विमान] = CURL_YES;

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

  प्रत्यागम Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

स्थिर शून्य send_negotiation(संरचना Curl_easy *data, पूर्णाङ्क cmd, पूर्णाङ्क option)
{
  unsigned वर्ण buf[3];
  size_t nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (unsigned वर्ण)cmd;
  buf[2] = (unsigned वर्ण)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  जइ(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

स्थिर शून्य set_remote_option(संरचना Curl_easy *data, संरचना TELNET *tn,
                              पूर्णाङ्क option, पूर्णाङ्क newstate)
{
  जइ(newstate == CURL_YES) {
    विकल्प(tn->him[option]) {
    पक्ष CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      भङ्ग;

    पक्ष CURL_YES:
      /* Already enabled */
      भङ्ग;

    पक्ष CURL_WANTNO:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        /* Error: already queued an enable request */
        भङ्ग;
      }
      भङ्ग;

    पक्ष CURL_WANTYES:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_EMPTY:
        /* Error: already negotiating for enable */
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        भङ्ग;
      }
      भङ्ग;
    }
  }
  अण्णहा { /* NO */
    विकल्प(tn->him[option]) {
    पक्ष CURL_NO:
      /* Already disabled */
      भङ्ग;

    पक्ष CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      भङ्ग;

    पक्ष CURL_WANTNO:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_EMPTY:
        /* Already negotiating for NO */
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        भङ्ग;
      }
      भङ्ग;

    पक्ष CURL_WANTYES:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        भङ्ग;
      }
      भङ्ग;
    }
  }
}

स्थिर शून्य set_local_option(संरचना Curl_easy *data, संरचना TELNET *tn,
                             पूर्णाङ्क option, पूर्णाङ्क newstate)
{
  जइ(newstate == CURL_YES) {
    विकल्प(tn->us[option]) {
    पक्ष CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      भङ्ग;

    पक्ष CURL_YES:
      /* Already enabled */
      भङ्ग;

    पक्ष CURL_WANTNO:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        /* Error: already queued an enable request */
        भङ्ग;
      }
      भङ्ग;

    पक्ष CURL_WANTYES:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_EMPTY:
        /* Error: already negotiating for enable */
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        भङ्ग;
      }
      भङ्ग;
    }
  }
  अण्णहा { /* NO */
    विकल्प(tn->us[option]) {
    पक्ष CURL_NO:
      /* Already disabled */
      भङ्ग;

    पक्ष CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      भङ्ग;

    पक्ष CURL_WANTNO:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_EMPTY:
        /* Already negotiating for NO */
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        भङ्ग;
      }
      भङ्ग;

    पक्ष CURL_WANTYES:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        भङ्ग;
      }
      भङ्ग;
    }
  }
}

स्थिर शून्य telnet_negotiate(संरचना Curl_easy *data, संरचना TELNET *tn)
{
  पूर्णाङ्क i;

  प्रति(i = 0; i < CURL_NTELOPTS; i++) {
    जइ(i == CURL_TELOPT_ECHO)
      अनुवर्त;

    जइ(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    जइ(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

स्थिर शून्य rec_will(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->him[option]) {
  पक्ष CURL_NO:
    जइ(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    अण्णहा
      send_negotiation(data, CURL_DONT, option);

    भङ्ग;

  पक्ष CURL_YES:
    /* Already enabled */
    भङ्ग;

  पक्ष CURL_WANTNO:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      भङ्ग;
    }
    भङ्ग;

  पक्ष CURL_WANTYES:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_EMPTY:
      tn->him[option] = CURL_YES;
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      भङ्ग;
    }
    भङ्ग;
  }
}

स्थिर शून्य rec_wont(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->him[option]) {
  पक्ष CURL_NO:
    /* Already disabled */
    भङ्ग;

  पक्ष CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    भङ्ग;

  पक्ष CURL_WANTNO:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_EMPTY:
      tn->him[option] = CURL_NO;
      भङ्ग;

    पक्ष CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      भङ्ग;
    }
    भङ्ग;

  पक्ष CURL_WANTYES:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_EMPTY:
      tn->him[option] = CURL_NO;
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      भङ्ग;
    }
    भङ्ग;
  }
}

स्थिर शून्य printsub(संरचना Curl_easy *data,
                     पूर्णाङ्क direction,                /* '<' or '>' */
                     ध्रुव unsigned वर्ण *pointer, /* ptr to suboption data */
                     size_t length)                /* suboption data length */
{
  जइ(data->set.verbose) {
    unsigned पूर्णाङ्क i = 0;
    जइ(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      जइ(length >= 3) {
        पूर्णाङ्क j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        जइ(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          जइ(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          अण्णहा जइ(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          अण्णहा
            infof(data, "%u ", i);
          जइ(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          अण्णहा जइ(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          अण्णहा
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      जइ(length >= 2)
        length -= 2;
      अण्णहा /* bad input */
        प्रत्यागम;
    }
    जइ(length <= 1) {
      infof(data, "(Empty suboption?)");
      प्रत्यागम;
    }

    जइ(CURL_TELOPT_OK(pointer[0])) {
      विकल्प(pointer[0]) {
      पक्ष CURL_TELOPT_TTYPE:
      पक्ष CURL_TELOPT_XDISPLOC:
      पक्ष CURL_TELOPT_NEW_ENVIRON:
      पक्ष CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        भङ्ग;
      स्वभाव:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        भङ्ग;
      }
    }
    अण्णहा
      infof(data, "%d (unknown)", pointer[0]);

    विकल्प(pointer[0]) {
    पक्ष CURL_TELOPT_NAWS:
      जइ(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      भङ्ग;
    स्वभाव:
      विकल्प(pointer[1]) {
      पक्ष CURL_TELQUAL_IS:
        infof(data, " IS");
        भङ्ग;
      पक्ष CURL_TELQUAL_SEND:
        infof(data, " SEND");
        भङ्ग;
      पक्ष CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        भङ्ग;
      पक्ष CURL_TELQUAL_NAME:
        infof(data, " NAME");
        भङ्ग;
      }

      विकल्प(pointer[0]) {
      पक्ष CURL_TELOPT_TTYPE:
      पक्ष CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (पूर्णाङ्क)((length > 2) ? (length - 2) : 0), &pointer[2]);
        भङ्ग;
      पक्ष CURL_TELOPT_NEW_ENVIRON:
        जइ(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          प्रति(i = 3; i < length; i++) {
            विकल्प(pointer[i]) {
            पक्ष CURL_NEW_ENV_VAR:
              infof(data, ", ");
              भङ्ग;
            पक्ष CURL_NEW_ENV_मानम्:
              infof(data, " = ");
              भङ्ग;
            स्वभाव:
              infof(data, "%c", pointer[i]);
              भङ्ग;
            }
          }
        }
        भङ्ग;
      स्वभाव:
        प्रति(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        भङ्ग;
      }
    }
  }
}

/* Escape and send a telnet data block */
स्थिर CURLcode send_telnet_data(संरचना Curl_easy *data,
                                 संरचना TELNET *tn,
                                 ध्रुव वर्ण *buffer, ssize_t nread)
{
  size_t i, outlen;
  ध्रुव unsigned वर्ण *outbuf;
  CURLcode result = CURLE_OK;
  size_t bytes_written;
  size_t total_written = 0;
  संरचना connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  जइ(nread < 0)
    प्रत्यागम CURLE_TOO_LARGE;

  जइ(memchr(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    प्रति(i = 0; i < (size_t)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      जइ(!result && ((unsigned वर्ण)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  अण्णहा {
    outlen = (size_t)nread;
    outbuf = (ध्रुव unsigned वर्ण *)buffer;
  }
  जाव(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    संरचना pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    जइ(timeout_ms < 0)
      प्रत्यागम CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    विकल्प(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    पक्ष -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      भङ्ग;
    पक्ष 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      भङ्ग;
    स्वभाव:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      भङ्ग;
    }
  }

  प्रत्यागम result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
स्थिर शून्य sendsuboption(संरचना Curl_easy *data,
                          संरचना TELNET *tn, पूर्णाङ्क option)
{
  size_t nwritten;
  unsigned short x, y;
  ध्रुव unsigned वर्ण *uc1, *uc2;
  CURLcode result;

  विकल्प(option) {
  पक्ष CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (ध्रुव unsigned वर्ण *)&x;
    uc2 = (ध्रुव unsigned वर्ण *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (ध्रुव unsigned वर्ण *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    जइ(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (ध्रुव वर्ण *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    जइ(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    भङ्ग;
  }
}

स्थिर शून्य rec_do(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->us[option]) {
  पक्ष CURL_NO:
    जइ(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      जइ(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    अण्णहा जइ(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    अण्णहा
      send_negotiation(data, CURL_WONT, option);
    भङ्ग;

  पक्ष CURL_YES:
    /* Already enabled */
    भङ्ग;

  पक्ष CURL_WANTNO:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      भङ्ग;
    }
    भङ्ग;

  पक्ष CURL_WANTYES:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_EMPTY:
      tn->us[option] = CURL_YES;
      जइ(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      भङ्ग;
    }
    भङ्ग;
  }
}

स्थिर शून्य rec_dont(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->us[option]) {
  पक्ष CURL_NO:
    /* Already disabled */
    भङ्ग;

  पक्ष CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    भङ्ग;

  पक्ष CURL_WANTNO:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_EMPTY:
      tn->us[option] = CURL_NO;
      भङ्ग;

    पक्ष CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      भङ्ग;
    }
    भङ्ग;

  पक्ष CURL_WANTYES:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_EMPTY:
      tn->us[option] = CURL_NO;
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      भङ्ग;
    }
    भङ्ग;
  }
}

स्थिर bool str_is_nonascii(ध्रुव वर्ण *str)
{
  वर्ण c;
  जाव((c = *str++) != 0)
    जइ(c & 0x80)
      प्रत्यागम TRUE;

  प्रत्यागम FALSE;
}

स्थिर CURLcode check_telnet_options(संरचना Curl_easy *data,
                                     संरचना TELNET *tn)
{
  संरचना curl_slist *head;
  संरचना curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  जइ(data->state.creds) {
    वर्ण buffer[256];
    जइ(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, परिमाण(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    जइ(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = रिक्त;
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  प्रति(head = data->set.telnet_options; head && !result; head = head->next) {
    size_t olen;
    ध्रुव वर्ण *option = head->data;
    ध्रुव वर्ण *arg;
    ध्रुव वर्ण *sep = strchr(option, '=');
    जइ(sep) {
      olen = sep - option;
      arg = ++sep;
      जइ(str_is_nonascii(arg))
        अनुवर्त;
      विकल्प(olen) {
      पक्ष 5:
        /* Terminal type */
        जइ(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          भङ्ग;
        }
        result = CURLE_UNKNOWN_OPTION;
        भङ्ग;

      पक्ष 8:
        /* Display variable */
        जइ(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          भङ्ग;
        }
        result = CURLE_UNKNOWN_OPTION;
        भङ्ग;

      पक्ष 7:
        /* Environment variable */
        जइ(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          जइ(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            भङ्ग;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        अण्णहा
          result = CURLE_UNKNOWN_OPTION;
        भङ्ग;

      पक्ष 2:
        /* Window Size */
        जइ(curl_strnequal(option, "WS", 2)) {
          ध्रुव वर्ण *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          जइ(curlx_str_सङ्ख्या(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_सङ्ख्या(&p, &y, 0xffff)) {
            failf(data, "Syntax error in telnet option: %s", head->data);
            result = CURLE_SETOPT_OPTION_SYNTAX;
          }
          अण्णहा {
            tn->subopt_wsx = (unsigned short)x;
            tn->subopt_wsy = (unsigned short)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        अण्णहा
          result = CURLE_UNKNOWN_OPTION;
        भङ्ग;

      पक्ष 6:
        /* To take care or not of the 8th bit in data exchange */
        जइ(curl_strnequal(option, "BINARY", 6)) {
          ध्रुव वर्ण *p = arg;
          curl_off_t द्विमान_option;
          जइ(!curlx_str_सङ्ख्या(&p, &द्विमान_option, 1) &&
             (द्विमान_option != 1)) {
            tn->us_preferred[CURL_TELOPT_द्विमान] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_द्विमान] = CURL_NO;
          }
        }
        अण्णहा
          result = CURLE_UNKNOWN_OPTION;
        भङ्ग;
      स्वभाव:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        भङ्ग;
      }
    }
    अण्णहा {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  जइ(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = रिक्त;
  }

  प्रत्यागम result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
स्थिर bool bad_option(ध्रुव वर्ण *data)
{
  प्रत्यागम !data || !!strchr(data, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
स्थिर CURLcode suboption(संरचना Curl_easy *data, संरचना TELNET *tn)
{
  संरचना curl_slist *v;
  unsigned वर्ण temp[2048];
  size_t nwritten;
  size_t len;
  CURLcode result = CURLE_OK;

  जइ(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    प्रत्यागम CURLE_OK;

  printsub(data, '<', (ध्रुव unsigned वर्ण *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  विकल्प(CURL_SB_GET(tn)) {
  पक्ष CURL_TELOPT_TTYPE:
    जइ(bad_option(tn->subopt_ttype))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    जइ(strlen(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      प्रत्यागम CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((वर्ण *)temp, परिमाण(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    जइ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      प्रत्यागम result;
    }
    printsub(data, '>', &temp[2], len-2);
    भङ्ग;
  पक्ष CURL_TELOPT_XDISPLOC:
    जइ(bad_option(tn->subopt_xdisploc))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    जइ(strlen(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      प्रत्यागम CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((वर्ण *)temp, परिमाण(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    जइ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      प्रत्यागम result;
    }
    printsub(data, '>', &temp[2], len - 2);
    भङ्ग;
  पक्ष CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((वर्ण *)temp, परिमाण(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    प्रति(v = tn->telnet_vars; v; v = v->next) {
      size_t tmplen = (strlen(v->data) + 1);
      जइ(bad_option(v->data))
        प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      जइ(len + tmplen < परिमाण(temp) - 6) {
        ध्रुव वर्ण *s = strchr(v->data, ',');
        जइ(!s)
          len += curl_msnprintf((वर्ण *)&temp[len], परिमाण(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        अण्णहा {
          size_t vlen = s - v->data;
          len += curl_msnprintf((वर्ण *)&temp[len], परिमाण(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (पूर्णाङ्क)vlen, v->data, CURL_NEW_ENV_मानम्, ++s);
        }
      }
    }
    curl_msnprintf((वर्ण *)&temp[len], परिमाण(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    जइ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      प्रत्यागम result;
    }
    printsub(data, '>', &temp[2], len - 2);
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode telrcv(संरचना Curl_easy *data,
                       संरचना TELNET *tn,
                       ध्रुव unsigned वर्ण *inbuf, /* Data received from
                                                      socket */
                       ssize_t गणनम्)              /* Number of bytes
                                                      received */
{
  unsigned वर्ण c;
  CURLcode result;
  पूर्णाङ्क in = 0;
  पूर्णाङ्क startwrite = -1;

#define startskipping()                                          \
  जइ(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (ध्रुव वर्ण *)&inbuf[startwrite], \
                               in-startwrite);                   \
    जइ(result)                                                   \
      प्रत्यागम result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  जइ(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  जाव(गणनम्--) {
    c = inbuf[in];

    विकल्प(tn->telrcv_state) {
    पक्ष CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      जइ(c == '\0') {
        startskipping();
        भङ्ग;   /* Ignore \0 after CR */
      }
      writebyte();
      भङ्ग;

    पक्ष CURL_TS_DATA:
      जइ(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        भङ्ग;
      }
      अण्णहा जइ(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      भङ्ग;

    पक्ष CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      विकल्प(c) {
      पक्ष CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        भङ्ग;
      पक्ष CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        भङ्ग;
      पक्ष CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        भङ्ग;
      पक्ष CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        भङ्ग;
      पक्ष CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        भङ्ग;
      पक्ष CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        भङ्ग;
      पक्ष CURL_DM:
      पक्ष CURL_NOP:
      पक्ष CURL_GA:
      स्वभाव:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        भङ्ग;
      }
      भङ्ग;

    पक्ष CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      भङ्ग;

    पक्ष CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      भङ्ग;

    पक्ष CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      भङ्ग;

    पक्ष CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      भङ्ग;

    पक्ष CURL_TS_SB:
      जइ(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      अण्णहा
        CURL_SB_ACCUM(tn, c);
      भङ्ग;

    पक्ष CURL_TS_SE:
      जइ(c != CURL_SE) {
        जइ(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          प्रत्यागम CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      अण्णहा {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        result = suboption(data, tn);   /* handle sub-option */
        जइ(result)
          प्रत्यागम result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      भङ्ग;
    }
    ++in;
  }
  bufferflush();
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode telnet_done(संरचना Curl_easy *data,
                            CURLcode status, bool premature)
{
  (शून्य)status;
  (शून्य)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode telnet_do(संरचना Curl_easy *data, bool *done)
{
  CURLcode result;
  संरचना connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ifdef USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_गणनम्;
  DWORD wait_timeout;
  DWORD readfile_read;
#अण्णहा
  timediff_t interval_ms;
  संरचना pollfd pfd[2];
  पूर्णाङ्क poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  वर्ण buffer[4 * 1024];
  संरचना TELNET *tn;

  *done = TRUE; /* unconditionally */

  result = init_telnet(data);
  जइ(result)
    प्रत्यागम result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  जइ(!tn)
    प्रत्यागम CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  जइ(result)
    प्रत्यागम result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  जइ(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  जइ(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    प्रत्यागम CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_निवेशः_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  जइ(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_गणनम् = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  अण्णहा {
    obj_गणनम् = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  जाव(keepon) {
    ध्रुव DWORD buf_size = (DWORD)परिमाण(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_गणनम्, objs,
                                           FALSE, wait_timeout);
    विकल्प(waitret) {

    पक्ष WAIT_TIMEOUT: {
      प्रति(;;) {
        जइ(data->set.is_fread_set) {
          size_t n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          जइ(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            भङ्ग;
          }

          जइ(n == CURL_READFUNC_PAUSE)
            भङ्ग;

          जइ(n == 0)                        /* no bytes */
            भङ्ग;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        अण्णहा {
          /* read from stdin */
          जइ(!PeekNamedPipe(stdin_handle, रिक्त, 0, रिक्त,
                            &readfile_read, रिक्त)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            भङ्ग;
          }

          जइ(!readfile_read)
            भङ्ग;

          जइ(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, रिक्त)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            भङ्ग;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        जइ(result) {
          keepon = FALSE;
          भङ्ग;
        }
      }
    }
    भङ्ग;

    पक्ष WAIT_OBJECT_0 + 1: {
      जइ(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, रिक्त)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        भङ्ग;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      जइ(result) {
        keepon = FALSE;
        भङ्ग;
      }
    }
    भङ्ग;

    पक्ष WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      जइ(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        पूर्णाङ्क sockerr = SOCKERRNO;
        जइ(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        भङ्ग;
      }
      जइ(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, परिमाण(buffer), &nread);
        /* read would have blocked. Loop again */
        जइ(result == CURLE_AGAIN)
          भङ्ग;
        /* returned not-zero, this an error */
        अण्णहा जइ(result) {
          keepon = FALSE;
          भङ्ग;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        अण्णहा जइ(!nread) {
          keepon = FALSE;
          भङ्ग;
        }

        result = telrcv(data, tn, (unsigned वर्ण *)buffer, nread);
        जइ(result) {
          keepon = FALSE;
          भङ्ग;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        जइ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      जइ(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      भङ्ग;
    }
    } /* switch */

    जइ(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  जइ(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#अण्णहा
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  जइ(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  अण्णहा {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    जइ(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  जाव(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    विकल्प(Curl_poll(pfd, (unsigned पूर्णाङ्क)poll_cnt, interval_ms)) {
    पक्ष -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      अनुवर्त;
    पक्ष 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    स्वभाव:                    /* read! */
      जइ(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, परिमाण(buffer), &nread);
        /* read would have blocked. Loop again */
        जइ(result == CURLE_AGAIN)
          भङ्ग;
        /* returned not-zero, this an error */
        जइ(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          जइ(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          भङ्ग;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        अण्णहा जइ(!nread) {
          keepon = FALSE;
          भङ्ग;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (unsigned वर्ण *)buffer, nread);
        जइ(result) {
          keepon = FALSE;
          भङ्ग;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        जइ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      जइ(poll_cnt == 2) {
        जइ(pfd[1].revents & POLLIN) { /* read from in file */
          snread = पठ(pfd[1].fd, buffer, परिमाण(buffer));
        }
      }
      अण्णहा {
        /* read from user-supplied method */
        snread = (पूर्णाङ्क)data->state.fread_func(buffer, 1, परिमाण(buffer),
                                             data->state.in);
        जइ(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          भङ्ग;
        }
        जइ(snread == CURL_READFUNC_PAUSE)
          भङ्ग;
      }

      जइ(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        जइ(result) {
          keepon = FALSE;
          भङ्ग;
        }
        Curl_pgrs_upload_inc(data, (size_t)snread);
      }
      अण्णहा जइ(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      भङ्ग;
    } /* poll switch statement */

    जइ(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    जइ(!result) {
      result = Curl_pgrsUpdate(data);
      जइ(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  प्रत्यागम result;
}

/*
 * TELNET protocol handler.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_telnet = {
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
