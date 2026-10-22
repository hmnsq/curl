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
  गर {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } जबसम्म(0)
#define CURL_SB_ACCUM(x, c)                                         \
  गर {                                                              \
    यदि((x)->subpointer < ((x)->subbuffer + परिमाण((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } जबसम्म(0)

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
  अचर वर्ण *subopt_ttype;          /* Set with suboption TTYPE */
  अचर वर्ण *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  अचिन्हित लघु subopt_wsx;         /* Set with suboption NAWS */
  अचिन्हित लघु subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  संरचना curl_slist *telnet_vars;    /* Environment variables */
  संरचना dynbuf out;                 /* output buffer */

  /* suboptions */
  अचिन्हित वर्ण subbuffer[SUBBUFSIZE];
  अचिन्हित वर्ण *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#अन्यथा
स्थिर शून्य printoption(संरचना Curl_easy *data,
                        अचर वर्ण *direction, पूर्णाङ्क cmd, पूर्णाङ्क option)
{
  यदि(data->set.verbose) {
    यदि(cmd == CURL_IAC) {
      यदि(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      अन्यथा
        infof(data, "%s IAC %d", direction, option);
    }
    अन्यथा {
      अचर वर्ण *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      यदि(fmt) {
        अचर वर्ण *opt;
        यदि(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        अन्यथा यदि(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        अन्यथा
          opt = रिक्त;

        यदि(opt)
          infof(data, "%s %s %s", direction, fmt, opt);
        अन्यथा
          infof(data, "%s %s %d", direction, fmt, option);
      }
      अन्यथा
        infof(data, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

स्थिर शून्य telnet_easy_dtor(अचर शून्य *साँचो, माप_प्रकार klen, शून्य *entry)
{
  संरचना TELNET *tn = entry;
  (शून्य)साँचो;
  (शून्य)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

स्थिर CURLcode init_telnet(संरचना Curl_easy *data)
{
  संरचना TELNET *tn;

  tn = curlx_calloc(1, परिमाण(संरचना TELNET));
  यदि(!tn)
    फर्काऊ CURLE_OUT_OF_MEMORY;

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
  tn->us_preferred[CURL_TELOPT_द्विआधारी] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_द्विआधारी] = CURL_YES;

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

  फर्काऊ Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

स्थिर शून्य send_negotiation(संरचना Curl_easy *data, पूर्णाङ्क cmd, पूर्णाङ्क option)
{
  अचिन्हित वर्ण buf[3];
  माप_प्रकार nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (अचिन्हित वर्ण)cmd;
  buf[2] = (अचिन्हित वर्ण)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  यदि(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

स्थिर शून्य set_remote_option(संरचना Curl_easy *data, संरचना TELNET *tn,
                              पूर्णाङ्क option, पूर्णाङ्क newstate)
{
  यदि(newstate == CURL_YES) {
    विकल्प(tn->him[option]) {
    पक्ष CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      रोक;

    पक्ष CURL_YES:
      /* Already enabled */
      रोक;

    पक्ष CURL_WANTNO:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        रोक;
      पक्ष CURL_OPPOSITE:
        /* Error: already queued an enable request */
        रोक;
      }
      रोक;

    पक्ष CURL_WANTYES:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_EMPTY:
        /* Error: already negotiating for enable */
        रोक;
      पक्ष CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        रोक;
      }
      रोक;
    }
  }
  अन्यथा { /* NO */
    विकल्प(tn->him[option]) {
    पक्ष CURL_NO:
      /* Already disabled */
      रोक;

    पक्ष CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      रोक;

    पक्ष CURL_WANTNO:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_EMPTY:
        /* Already negotiating for NO */
        रोक;
      पक्ष CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        रोक;
      }
      रोक;

    पक्ष CURL_WANTYES:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        रोक;
      पक्ष CURL_OPPOSITE:
        रोक;
      }
      रोक;
    }
  }
}

स्थिर शून्य set_local_option(संरचना Curl_easy *data, संरचना TELNET *tn,
                             पूर्णाङ्क option, पूर्णाङ्क newstate)
{
  यदि(newstate == CURL_YES) {
    विकल्प(tn->us[option]) {
    पक्ष CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      रोक;

    पक्ष CURL_YES:
      /* Already enabled */
      रोक;

    पक्ष CURL_WANTNO:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        रोक;
      पक्ष CURL_OPPOSITE:
        /* Error: already queued an enable request */
        रोक;
      }
      रोक;

    पक्ष CURL_WANTYES:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_EMPTY:
        /* Error: already negotiating for enable */
        रोक;
      पक्ष CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        रोक;
      }
      रोक;
    }
  }
  अन्यथा { /* NO */
    विकल्प(tn->us[option]) {
    पक्ष CURL_NO:
      /* Already disabled */
      रोक;

    पक्ष CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      रोक;

    पक्ष CURL_WANTNO:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_EMPTY:
        /* Already negotiating for NO */
        रोक;
      पक्ष CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        रोक;
      }
      रोक;

    पक्ष CURL_WANTYES:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        रोक;
      पक्ष CURL_OPPOSITE:
        रोक;
      }
      रोक;
    }
  }
}

स्थिर शून्य telnet_negotiate(संरचना Curl_easy *data, संरचना TELNET *tn)
{
  पूर्णाङ्क i;

  लागि(i = 0; i < CURL_NTELOPTS; i++) {
    यदि(i == CURL_TELOPT_ECHO)
      जारी;

    यदि(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    यदि(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

स्थिर शून्य rec_will(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->him[option]) {
  पक्ष CURL_NO:
    यदि(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    अन्यथा
      send_negotiation(data, CURL_DONT, option);

    रोक;

  पक्ष CURL_YES:
    /* Already enabled */
    रोक;

  पक्ष CURL_WANTNO:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      रोक;
    पक्ष CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      रोक;
    }
    रोक;

  पक्ष CURL_WANTYES:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_EMPTY:
      tn->him[option] = CURL_YES;
      रोक;
    पक्ष CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      रोक;
    }
    रोक;
  }
}

स्थिर शून्य rec_wont(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->him[option]) {
  पक्ष CURL_NO:
    /* Already disabled */
    रोक;

  पक्ष CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    रोक;

  पक्ष CURL_WANTNO:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_EMPTY:
      tn->him[option] = CURL_NO;
      रोक;

    पक्ष CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      रोक;
    }
    रोक;

  पक्ष CURL_WANTYES:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_EMPTY:
      tn->him[option] = CURL_NO;
      रोक;
    पक्ष CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      रोक;
    }
    रोक;
  }
}

स्थिर शून्य printsub(संरचना Curl_easy *data,
                     पूर्णाङ्क direction,                /* '<' or '>' */
                     अचर अचिन्हित वर्ण *pointer, /* ptr to suboption data */
                     माप_प्रकार length)                /* suboption data length */
{
  यदि(data->set.verbose) {
    अचिन्हित पूर्णाङ्क i = 0;
    यदि(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      यदि(length >= 3) {
        पूर्णाङ्क j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        यदि(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          यदि(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          अन्यथा यदि(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          अन्यथा
            infof(data, "%u ", i);
          यदि(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          अन्यथा यदि(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          अन्यथा
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      यदि(length >= 2)
        length -= 2;
      अन्यथा /* bad input */
        फर्काऊ;
    }
    यदि(length <= 1) {
      infof(data, "(Empty suboption?)");
      फर्काऊ;
    }

    यदि(CURL_TELOPT_OK(pointer[0])) {
      विकल्प(pointer[0]) {
      पक्ष CURL_TELOPT_TTYPE:
      पक्ष CURL_TELOPT_XDISPLOC:
      पक्ष CURL_TELOPT_NEW_ENVIRON:
      पक्ष CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        रोक;
      स्वभाव:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        रोक;
      }
    }
    अन्यथा
      infof(data, "%d (unknown)", pointer[0]);

    विकल्प(pointer[0]) {
    पक्ष CURL_TELOPT_NAWS:
      यदि(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      रोक;
    स्वभाव:
      विकल्प(pointer[1]) {
      पक्ष CURL_TELQUAL_IS:
        infof(data, " IS");
        रोक;
      पक्ष CURL_TELQUAL_SEND:
        infof(data, " SEND");
        रोक;
      पक्ष CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        रोक;
      पक्ष CURL_TELQUAL_NAME:
        infof(data, " NAME");
        रोक;
      }

      विकल्प(pointer[0]) {
      पक्ष CURL_TELOPT_TTYPE:
      पक्ष CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (पूर्णाङ्क)((length > 2) ? (length - 2) : 0), &pointer[2]);
        रोक;
      पक्ष CURL_TELOPT_NEW_ENVIRON:
        यदि(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          लागि(i = 3; i < length; i++) {
            विकल्प(pointer[i]) {
            पक्ष CURL_NEW_ENV_VAR:
              infof(data, ", ");
              रोक;
            पक्ष CURL_NEW_ENV_मान:
              infof(data, " = ");
              रोक;
            स्वभाव:
              infof(data, "%c", pointer[i]);
              रोक;
            }
          }
        }
        रोक;
      स्वभाव:
        लागि(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        रोक;
      }
    }
  }
}

/* Escape and send a telnet data block */
स्थिर CURLcode send_telnet_data(संरचना Curl_easy *data,
                                 संरचना TELNET *tn,
                                 अचर वर्ण *buffer, ssize_t nread)
{
  माप_प्रकार i, outlen;
  अचर अचिन्हित वर्ण *outbuf;
  CURLcode result = CURLE_OK;
  माप_प्रकार bytes_written;
  माप_प्रकार total_written = 0;
  संरचना connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  यदि(nread < 0)
    फर्काऊ CURLE_TOO_LARGE;

  यदि(स_प्रथम(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    लागि(i = 0; i < (माप_प्रकार)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      यदि(!result && ((अचिन्हित वर्ण)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  अन्यथा {
    outlen = (माप_प्रकार)nread;
    outbuf = (अचर अचिन्हित वर्ण *)buffer;
  }
  जबसम्म(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    संरचना pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    यदि(timeout_ms < 0)
      फर्काऊ CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    विकल्प(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    पक्ष -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      रोक;
    पक्ष 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      रोक;
    स्वभाव:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      रोक;
    }
  }

  फर्काऊ result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
स्थिर शून्य sendsuboption(संरचना Curl_easy *data,
                          संरचना TELNET *tn, पूर्णाङ्क option)
{
  माप_प्रकार nwritten;
  अचिन्हित लघु x, y;
  अचर अचिन्हित वर्ण *uc1, *uc2;
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
    uc1 = (अचर अचिन्हित वर्ण *)&x;
    uc2 = (अचर अचिन्हित वर्ण *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (अचर अचिन्हित वर्ण *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    यदि(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (अचर वर्ण *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    यदि(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    रोक;
  }
}

स्थिर शून्य rec_do(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->us[option]) {
  पक्ष CURL_NO:
    यदि(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      यदि(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    अन्यथा यदि(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    अन्यथा
      send_negotiation(data, CURL_WONT, option);
    रोक;

  पक्ष CURL_YES:
    /* Already enabled */
    रोक;

  पक्ष CURL_WANTNO:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      रोक;
    पक्ष CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      रोक;
    }
    रोक;

  पक्ष CURL_WANTYES:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_EMPTY:
      tn->us[option] = CURL_YES;
      यदि(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      रोक;
    पक्ष CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      रोक;
    }
    रोक;
  }
}

स्थिर शून्य rec_dont(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->us[option]) {
  पक्ष CURL_NO:
    /* Already disabled */
    रोक;

  पक्ष CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    रोक;

  पक्ष CURL_WANTNO:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_EMPTY:
      tn->us[option] = CURL_NO;
      रोक;

    पक्ष CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      रोक;
    }
    रोक;

  पक्ष CURL_WANTYES:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_EMPTY:
      tn->us[option] = CURL_NO;
      रोक;
    पक्ष CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      रोक;
    }
    रोक;
  }
}

स्थिर bool str_is_nonascii(अचर वर्ण *str)
{
  वर्ण c;
  जबसम्म((c = *str++) != 0)
    यदि(c & 0x80)
      फर्काऊ TRUE;

  फर्काऊ FALSE;
}

स्थिर CURLcode check_telnet_options(संरचना Curl_easy *data,
                                     संरचना TELNET *tn)
{
  संरचना curl_slist *head;
  संरचना curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  यदि(data->state.creds) {
    वर्ण buffer[256];
    यदि(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, परिमाण(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    यदि(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = रिक्त;
      फर्काऊ CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  लागि(head = data->set.telnet_options; head && !result; head = head->next) {
    माप_प्रकार olen;
    अचर वर्ण *option = head->data;
    अचर वर्ण *arg;
    अचर वर्ण *sep = म_अक्षर(option, '=');
    यदि(sep) {
      olen = sep - option;
      arg = ++sep;
      यदि(str_is_nonascii(arg))
        जारी;
      विकल्प(olen) {
      पक्ष 5:
        /* Terminal type */
        यदि(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          रोक;
        }
        result = CURLE_UNKNOWN_OPTION;
        रोक;

      पक्ष 8:
        /* Display variable */
        यदि(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          रोक;
        }
        result = CURLE_UNKNOWN_OPTION;
        रोक;

      पक्ष 7:
        /* Environment variable */
        यदि(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          यदि(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            रोक;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        अन्यथा
          result = CURLE_UNKNOWN_OPTION;
        रोक;

      पक्ष 2:
        /* Window Size */
        यदि(curl_strnequal(option, "WS", 2)) {
          अचर वर्ण *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          यदि(curlx_str_सङ्ख्या(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_सङ्ख्या(&p, &y, 0xffff)) {
            failf(data, "Syntax error in telnet option: %s", head->data);
            result = CURLE_SETOPT_OPTION_SYNTAX;
          }
          अन्यथा {
            tn->subopt_wsx = (अचिन्हित लघु)x;
            tn->subopt_wsy = (अचिन्हित लघु)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        अन्यथा
          result = CURLE_UNKNOWN_OPTION;
        रोक;

      पक्ष 6:
        /* To take care or not of the 8th bit in data exchange */
        यदि(curl_strnequal(option, "BINARY", 6)) {
          अचर वर्ण *p = arg;
          curl_off_t द्विआधारी_option;
          यदि(!curlx_str_सङ्ख्या(&p, &द्विआधारी_option, 1) &&
             (द्विआधारी_option != 1)) {
            tn->us_preferred[CURL_TELOPT_द्विआधारी] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_द्विआधारी] = CURL_NO;
          }
        }
        अन्यथा
          result = CURLE_UNKNOWN_OPTION;
        रोक;
      स्वभाव:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        रोक;
      }
    }
    अन्यथा {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  यदि(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = रिक्त;
  }

  फर्काऊ result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
स्थिर bool bad_option(अचर वर्ण *data)
{
  फर्काऊ !data || !!म_अक्षर(data, CURL_IAC);
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
  अचिन्हित वर्ण temp[2048];
  माप_प्रकार nwritten;
  माप_प्रकार len;
  CURLcode result = CURLE_OK;

  यदि(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    फर्काऊ CURLE_OK;

  printsub(data, '<', (अचर अचिन्हित वर्ण *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  विकल्प(CURL_SB_GET(tn)) {
  पक्ष CURL_TELOPT_TTYPE:
    यदि(bad_option(tn->subopt_ttype))
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    यदि(म_माप(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      फर्काऊ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((वर्ण *)temp, परिमाण(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    यदि(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      फर्काऊ result;
    }
    printsub(data, '>', &temp[2], len-2);
    रोक;
  पक्ष CURL_TELOPT_XDISPLOC:
    यदि(bad_option(tn->subopt_xdisploc))
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    यदि(म_माप(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      फर्काऊ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((वर्ण *)temp, परिमाण(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    यदि(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      फर्काऊ result;
    }
    printsub(data, '>', &temp[2], len - 2);
    रोक;
  पक्ष CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((वर्ण *)temp, परिमाण(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    लागि(v = tn->telnet_vars; v; v = v->next) {
      माप_प्रकार tmplen = (म_माप(v->data) + 1);
      यदि(bad_option(v->data))
        फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      यदि(len + tmplen < परिमाण(temp) - 6) {
        अचर वर्ण *s = म_अक्षर(v->data, ',');
        यदि(!s)
          len += curl_msnprintf((वर्ण *)&temp[len], परिमाण(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        अन्यथा {
          माप_प्रकार vlen = s - v->data;
          len += curl_msnprintf((वर्ण *)&temp[len], परिमाण(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (पूर्णाङ्क)vlen, v->data, CURL_NEW_ENV_मान, ++s);
        }
      }
    }
    curl_msnprintf((वर्ण *)&temp[len], परिमाण(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    यदि(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      फर्काऊ result;
    }
    printsub(data, '>', &temp[2], len - 2);
    रोक;
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode telrcv(संरचना Curl_easy *data,
                       संरचना TELNET *tn,
                       अचर अचिन्हित वर्ण *inbuf, /* Data received from
                                                      socket */
                       ssize_t गन्ती)              /* Number of bytes
                                                      received */
{
  अचिन्हित वर्ण c;
  CURLcode result;
  पूर्णाङ्क in = 0;
  पूर्णाङ्क startwrite = -1;

#define startskipping()                                          \
  यदि(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (अचर वर्ण *)&inbuf[startwrite], \
                               in-startwrite);                   \
    यदि(result)                                                   \
      फर्काऊ result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  यदि(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  जबसम्म(गन्ती--) {
    c = inbuf[in];

    विकल्प(tn->telrcv_state) {
    पक्ष CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      यदि(c == '\0') {
        startskipping();
        रोक;   /* Ignore \0 after CR */
      }
      writebyte();
      रोक;

    पक्ष CURL_TS_DATA:
      यदि(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        रोक;
      }
      अन्यथा यदि(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      रोक;

    पक्ष CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      विकल्प(c) {
      पक्ष CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        रोक;
      पक्ष CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        रोक;
      पक्ष CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        रोक;
      पक्ष CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        रोक;
      पक्ष CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        रोक;
      पक्ष CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        रोक;
      पक्ष CURL_DM:
      पक्ष CURL_NOP:
      पक्ष CURL_GA:
      स्वभाव:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        रोक;
      }
      रोक;

    पक्ष CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      रोक;

    पक्ष CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      रोक;

    पक्ष CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      रोक;

    पक्ष CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      रोक;

    पक्ष CURL_TS_SB:
      यदि(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      अन्यथा
        CURL_SB_ACCUM(tn, c);
      रोक;

    पक्ष CURL_TS_SE:
      यदि(c != CURL_SE) {
        यदि(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          फर्काऊ CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      अन्यथा {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        result = suboption(data, tn);   /* handle sub-option */
        यदि(result)
          फर्काऊ result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      रोक;
    }
    ++in;
  }
  bufferflush();
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode telnet_done(संरचना Curl_easy *data,
                            CURLcode status, bool premature)
{
  (शून्य)status;
  (शून्य)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  फर्काऊ CURLE_OK;
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
  DWORD obj_गन्ती;
  DWORD wait_timeout;
  DWORD readfile_read;
#अन्यथा
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
  यदि(result)
    फर्काऊ result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  यदि(!tn)
    फर्काऊ CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  यदि(result)
    फर्काऊ result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  यदि(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    फर्काऊ CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  यदि(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    फर्काऊ CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_आगत_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  यदि(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_गन्ती = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  अन्यथा {
    obj_गन्ती = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  जबसम्म(keepon) {
    अचर DWORD buf_size = (DWORD)परिमाण(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_गन्ती, objs,
                                           FALSE, wait_timeout);
    विकल्प(waitret) {

    पक्ष WAIT_TIMEOUT: {
      लागि(;;) {
        यदि(data->set.is_fread_set) {
          माप_प्रकार n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          यदि(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            रोक;
          }

          यदि(n == CURL_READFUNC_PAUSE)
            रोक;

          यदि(n == 0)                        /* no bytes */
            रोक;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        अन्यथा {
          /* read from stdin */
          यदि(!PeekNamedPipe(stdin_handle, रिक्त, 0, रिक्त,
                            &readfile_read, रिक्त)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            रोक;
          }

          यदि(!readfile_read)
            रोक;

          यदि(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, रिक्त)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            रोक;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        यदि(result) {
          keepon = FALSE;
          रोक;
        }
      }
    }
    रोक;

    पक्ष WAIT_OBJECT_0 + 1: {
      यदि(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, रिक्त)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        रोक;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      यदि(result) {
        keepon = FALSE;
        रोक;
      }
    }
    रोक;

    पक्ष WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      यदि(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        पूर्णाङ्क sockerr = SOCKERRNO;
        यदि(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        रोक;
      }
      यदि(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        माप_प्रकार nread;
        result = Curl_xfer_recv(data, buffer, परिमाण(buffer), &nread);
        /* read would have blocked. Loop again */
        यदि(result == CURLE_AGAIN)
          रोक;
        /* returned not-zero, this an error */
        अन्यथा यदि(result) {
          keepon = FALSE;
          रोक;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        अन्यथा यदि(!nread) {
          keepon = FALSE;
          रोक;
        }

        result = telrcv(data, tn, (अचिन्हित वर्ण *)buffer, nread);
        यदि(result) {
          keepon = FALSE;
          रोक;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        यदि(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      यदि(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      रोक;
    }
    } /* switch */

    यदि(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  यदि(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#अन्यथा
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  यदि(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  अन्यथा {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((खाता *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    यदि(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  जबसम्म(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    विकल्प(Curl_poll(pfd, (अचिन्हित पूर्णाङ्क)poll_cnt, interval_ms)) {
    पक्ष -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      जारी;
    पक्ष 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    स्वभाव:                    /* read! */
      यदि(pfd[0].revents & POLLIN) {
        /* read data from network */
        माप_प्रकार nread;
        result = Curl_xfer_recv(data, buffer, परिमाण(buffer), &nread);
        /* read would have blocked. Loop again */
        यदि(result == CURLE_AGAIN)
          रोक;
        /* returned not-zero, this an error */
        यदि(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          यदि(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          रोक;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        अन्यथा यदि(!nread) {
          keepon = FALSE;
          रोक;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (अचिन्हित वर्ण *)buffer, nread);
        यदि(result) {
          keepon = FALSE;
          रोक;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        यदि(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      यदि(poll_cnt == 2) {
        यदि(pfd[1].revents & POLLIN) { /* read from in file */
          snread = पठ(pfd[1].fd, buffer, परिमाण(buffer));
        }
      }
      अन्यथा {
        /* read from user-supplied method */
        snread = (पूर्णाङ्क)data->state.fread_func(buffer, 1, परिमाण(buffer),
                                             data->state.in);
        यदि(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          रोक;
        }
        यदि(snread == CURL_READFUNC_PAUSE)
          रोक;
      }

      यदि(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        यदि(result) {
          keepon = FALSE;
          रोक;
        }
        Curl_pgrs_upload_inc(data, (माप_प्रकार)snread);
      }
      अन्यथा यदि(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      रोक;
    } /* poll switch statement */

    यदि(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    यदि(!result) {
      result = Curl_pgrsUpdate(data);
      यदि(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  फर्काऊ result;
}

/*
 * TELNET protocol handler.
 */
अचर संरचना Curl_protocol Curl_protocol_telnet = {
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
