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
  करा {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } पर्यंत(0)
#define CURL_SB_ACCUM(x, c)                                         \
  करा {                                                              \
    जर((x)->subpointer < ((x)->subbuffer + आकार((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } पर्यंत(0)

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
प्रकारव्याख्या गणना {
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
  पूर्णांक please_negotiate;
  पूर्णांक already_negotiated;
  पूर्णांक us[256];
  पूर्णांक usq[256];
  पूर्णांक us_preferred[256];
  पूर्णांक him[256];
  पूर्णांक himq[256];
  पूर्णांक him_preferred[256];
  पूर्णांक subnegotiation[256];
  स्थिरांक अक्षर *subopt_ttype;          /* Set with suboption TTYPE */
  स्थिरांक अक्षर *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  अचिन्हित लघु subopt_wsx;         /* Set with suboption NAWS */
  अचिन्हित लघु subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  संरचना curl_slist *telnet_vars;    /* Environment variables */
  संरचना dynbuf out;                 /* output buffer */

  /* suboptions */
  अचिन्हित अक्षर subbuffer[SUBBUFSIZE];
  अचिन्हित अक्षर *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#अन्यथा
स्थिर शून्य printoption(संरचना Curl_easy *data,
                        स्थिरांक अक्षर *direction, पूर्णांक cmd, पूर्णांक option)
{
  जर(data->set.verbose) {
    जर(cmd == CURL_IAC) {
      जर(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      अन्यथा
        infof(data, "%s IAC %d", direction, option);
    }
    अन्यथा {
      स्थिरांक अक्षर *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      जर(fmt) {
        स्थिरांक अक्षर *opt;
        जर(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        अन्यथा जर(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        अन्यथा
          opt = रिक्त;

        जर(opt)
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

स्थिर शून्य telnet_easy_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना TELNET *tn = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

स्थिर CURLcode init_telnet(संरचना Curl_easy *data)
{
  संरचना TELNET *tn;

  tn = curlx_calloc(1, आकार(संरचना TELNET));
  जर(!tn)
    परत CURLE_OUT_OF_MEMORY;

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

  परत Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

स्थिर शून्य send_negotiation(संरचना Curl_easy *data, पूर्णांक cmd, पूर्णांक option)
{
  अचिन्हित अक्षर buf[3];
  माप_प्रकार nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (अचिन्हित अक्षर)cmd;
  buf[2] = (अचिन्हित अक्षर)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  जर(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

स्थिर शून्य set_remote_option(संरचना Curl_easy *data, संरचना TELNET *tn,
                              पूर्णांक option, पूर्णांक newstate)
{
  जर(newstate == CURL_YES) {
    निवड(tn->him[option]) {
    स्थिती CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      थांबा;

    स्थिती CURL_YES:
      /* Already enabled */
      थांबा;

    स्थिती CURL_WANTNO:
      निवड(tn->himq[option]) {
      स्थिती CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        थांबा;
      स्थिती CURL_OPPOSITE:
        /* Error: already queued an enable request */
        थांबा;
      }
      थांबा;

    स्थिती CURL_WANTYES:
      निवड(tn->himq[option]) {
      स्थिती CURL_EMPTY:
        /* Error: already negotiating for enable */
        थांबा;
      स्थिती CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        थांबा;
      }
      थांबा;
    }
  }
  अन्यथा { /* NO */
    निवड(tn->him[option]) {
    स्थिती CURL_NO:
      /* Already disabled */
      थांबा;

    स्थिती CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      थांबा;

    स्थिती CURL_WANTNO:
      निवड(tn->himq[option]) {
      स्थिती CURL_EMPTY:
        /* Already negotiating for NO */
        थांबा;
      स्थिती CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        थांबा;
      }
      थांबा;

    स्थिती CURL_WANTYES:
      निवड(tn->himq[option]) {
      स्थिती CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        थांबा;
      स्थिती CURL_OPPOSITE:
        थांबा;
      }
      थांबा;
    }
  }
}

स्थिर शून्य set_local_option(संरचना Curl_easy *data, संरचना TELNET *tn,
                             पूर्णांक option, पूर्णांक newstate)
{
  जर(newstate == CURL_YES) {
    निवड(tn->us[option]) {
    स्थिती CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      थांबा;

    स्थिती CURL_YES:
      /* Already enabled */
      थांबा;

    स्थिती CURL_WANTNO:
      निवड(tn->usq[option]) {
      स्थिती CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        थांबा;
      स्थिती CURL_OPPOSITE:
        /* Error: already queued an enable request */
        थांबा;
      }
      थांबा;

    स्थिती CURL_WANTYES:
      निवड(tn->usq[option]) {
      स्थिती CURL_EMPTY:
        /* Error: already negotiating for enable */
        थांबा;
      स्थिती CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        थांबा;
      }
      थांबा;
    }
  }
  अन्यथा { /* NO */
    निवड(tn->us[option]) {
    स्थिती CURL_NO:
      /* Already disabled */
      थांबा;

    स्थिती CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      थांबा;

    स्थिती CURL_WANTNO:
      निवड(tn->usq[option]) {
      स्थिती CURL_EMPTY:
        /* Already negotiating for NO */
        थांबा;
      स्थिती CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        थांबा;
      }
      थांबा;

    स्थिती CURL_WANTYES:
      निवड(tn->usq[option]) {
      स्थिती CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        थांबा;
      स्थिती CURL_OPPOSITE:
        थांबा;
      }
      थांबा;
    }
  }
}

स्थिर शून्य telnet_negotiate(संरचना Curl_easy *data, संरचना TELNET *tn)
{
  पूर्णांक i;

  साठी(i = 0; i < CURL_NTELOPTS; i++) {
    जर(i == CURL_TELOPT_ECHO)
      सुरूठेवा;

    जर(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    जर(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

स्थिर शून्य rec_will(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णांक option)
{
  निवड(tn->him[option]) {
  स्थिती CURL_NO:
    जर(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    अन्यथा
      send_negotiation(data, CURL_DONT, option);

    थांबा;

  स्थिती CURL_YES:
    /* Already enabled */
    थांबा;

  स्थिती CURL_WANTNO:
    निवड(tn->himq[option]) {
    स्थिती CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      थांबा;
    स्थिती CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      थांबा;
    }
    थांबा;

  स्थिती CURL_WANTYES:
    निवड(tn->himq[option]) {
    स्थिती CURL_EMPTY:
      tn->him[option] = CURL_YES;
      थांबा;
    स्थिती CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      थांबा;
    }
    थांबा;
  }
}

स्थिर शून्य rec_wont(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णांक option)
{
  निवड(tn->him[option]) {
  स्थिती CURL_NO:
    /* Already disabled */
    थांबा;

  स्थिती CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    थांबा;

  स्थिती CURL_WANTNO:
    निवड(tn->himq[option]) {
    स्थिती CURL_EMPTY:
      tn->him[option] = CURL_NO;
      थांबा;

    स्थिती CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      थांबा;
    }
    थांबा;

  स्थिती CURL_WANTYES:
    निवड(tn->himq[option]) {
    स्थिती CURL_EMPTY:
      tn->him[option] = CURL_NO;
      थांबा;
    स्थिती CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      थांबा;
    }
    थांबा;
  }
}

स्थिर शून्य printsub(संरचना Curl_easy *data,
                     पूर्णांक direction,                /* '<' or '>' */
                     स्थिरांक अचिन्हित अक्षर *pointer, /* ptr to suboption data */
                     माप_प्रकार length)                /* suboption data length */
{
  जर(data->set.verbose) {
    अचिन्हित पूर्णांक i = 0;
    जर(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      जर(length >= 3) {
        पूर्णांक j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        जर(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          जर(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          अन्यथा जर(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          अन्यथा
            infof(data, "%u ", i);
          जर(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          अन्यथा जर(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          अन्यथा
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      जर(length >= 2)
        length -= 2;
      अन्यथा /* bad input */
        परत;
    }
    जर(length <= 1) {
      infof(data, "(Empty suboption?)");
      परत;
    }

    जर(CURL_TELOPT_OK(pointer[0])) {
      निवड(pointer[0]) {
      स्थिती CURL_TELOPT_TTYPE:
      स्थिती CURL_TELOPT_XDISPLOC:
      स्थिती CURL_TELOPT_NEW_ENVIRON:
      स्थिती CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        थांबा;
      पूर्वनिर्धारित:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        थांबा;
      }
    }
    अन्यथा
      infof(data, "%d (unknown)", pointer[0]);

    निवड(pointer[0]) {
    स्थिती CURL_TELOPT_NAWS:
      जर(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      थांबा;
    पूर्वनिर्धारित:
      निवड(pointer[1]) {
      स्थिती CURL_TELQUAL_IS:
        infof(data, " IS");
        थांबा;
      स्थिती CURL_TELQUAL_SEND:
        infof(data, " SEND");
        थांबा;
      स्थिती CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        थांबा;
      स्थिती CURL_TELQUAL_NAME:
        infof(data, " NAME");
        थांबा;
      }

      निवड(pointer[0]) {
      स्थिती CURL_TELOPT_TTYPE:
      स्थिती CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (पूर्णांक)((length > 2) ? (length - 2) : 0), &pointer[2]);
        थांबा;
      स्थिती CURL_TELOPT_NEW_ENVIRON:
        जर(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          साठी(i = 3; i < length; i++) {
            निवड(pointer[i]) {
            स्थिती CURL_NEW_ENV_VAR:
              infof(data, ", ");
              थांबा;
            स्थिती CURL_NEW_ENV_मूल्य:
              infof(data, " = ");
              थांबा;
            पूर्वनिर्धारित:
              infof(data, "%c", pointer[i]);
              थांबा;
            }
          }
        }
        थांबा;
      पूर्वनिर्धारित:
        साठी(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        थांबा;
      }
    }
  }
}

/* Escape and send a telnet data block */
स्थिर CURLcode send_telnet_data(संरचना Curl_easy *data,
                                 संरचना TELNET *tn,
                                 स्थिरांक अक्षर *buffer, ssize_t nread)
{
  माप_प्रकार i, outlen;
  स्थिरांक अचिन्हित अक्षर *outbuf;
  CURLcode result = CURLE_OK;
  माप_प्रकार bytes_written;
  माप_प्रकार total_written = 0;
  संरचना connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  जर(nread < 0)
    परत CURLE_TOO_LARGE;

  जर(स_प्रथम(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    साठी(i = 0; i < (माप_प्रकार)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      जर(!result && ((अचिन्हित अक्षर)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  अन्यथा {
    outlen = (माप_प्रकार)nread;
    outbuf = (स्थिरांक अचिन्हित अक्षर *)buffer;
  }
  पर्यंत(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    संरचना pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    जर(timeout_ms < 0)
      परत CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    निवड(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    स्थिती -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      थांबा;
    स्थिती 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      थांबा;
    पूर्वनिर्धारित:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      थांबा;
    }
  }

  परत result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
स्थिर शून्य sendsuboption(संरचना Curl_easy *data,
                          संरचना TELNET *tn, पूर्णांक option)
{
  माप_प्रकार nwritten;
  अचिन्हित लघु x, y;
  स्थिरांक अचिन्हित अक्षर *uc1, *uc2;
  CURLcode result;

  निवड(option) {
  स्थिती CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (स्थिरांक अचिन्हित अक्षर *)&x;
    uc2 = (स्थिरांक अचिन्हित अक्षर *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (स्थिरांक अचिन्हित अक्षर *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    जर(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (स्थिरांक अक्षर *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    जर(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    थांबा;
  }
}

स्थिर शून्य rec_do(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णांक option)
{
  निवड(tn->us[option]) {
  स्थिती CURL_NO:
    जर(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      जर(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    अन्यथा जर(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    अन्यथा
      send_negotiation(data, CURL_WONT, option);
    थांबा;

  स्थिती CURL_YES:
    /* Already enabled */
    थांबा;

  स्थिती CURL_WANTNO:
    निवड(tn->usq[option]) {
    स्थिती CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      थांबा;
    स्थिती CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      थांबा;
    }
    थांबा;

  स्थिती CURL_WANTYES:
    निवड(tn->usq[option]) {
    स्थिती CURL_EMPTY:
      tn->us[option] = CURL_YES;
      जर(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      थांबा;
    स्थिती CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      थांबा;
    }
    थांबा;
  }
}

स्थिर शून्य rec_dont(संरचना Curl_easy *data, संरचना TELNET *tn, पूर्णांक option)
{
  निवड(tn->us[option]) {
  स्थिती CURL_NO:
    /* Already disabled */
    थांबा;

  स्थिती CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    थांबा;

  स्थिती CURL_WANTNO:
    निवड(tn->usq[option]) {
    स्थिती CURL_EMPTY:
      tn->us[option] = CURL_NO;
      थांबा;

    स्थिती CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      थांबा;
    }
    थांबा;

  स्थिती CURL_WANTYES:
    निवड(tn->usq[option]) {
    स्थिती CURL_EMPTY:
      tn->us[option] = CURL_NO;
      थांबा;
    स्थिती CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      थांबा;
    }
    थांबा;
  }
}

स्थिर bool str_is_nonascii(स्थिरांक अक्षर *str)
{
  अक्षर c;
  पर्यंत((c = *str++) != 0)
    जर(c & 0x80)
      परत TRUE;

  परत FALSE;
}

स्थिर CURLcode check_telnet_options(संरचना Curl_easy *data,
                                     संरचना TELNET *tn)
{
  संरचना curl_slist *head;
  संरचना curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  जर(data->state.creds) {
    अक्षर buffer[256];
    जर(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, आकार(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    जर(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = रिक्त;
      परत CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  साठी(head = data->set.telnet_options; head && !result; head = head->next) {
    माप_प्रकार olen;
    स्थिरांक अक्षर *option = head->data;
    स्थिरांक अक्षर *arg;
    स्थिरांक अक्षर *sep = म_अक्षर(option, '=');
    जर(sep) {
      olen = sep - option;
      arg = ++sep;
      जर(str_is_nonascii(arg))
        सुरूठेवा;
      निवड(olen) {
      स्थिती 5:
        /* Terminal type */
        जर(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          थांबा;
        }
        result = CURLE_UNKNOWN_OPTION;
        थांबा;

      स्थिती 8:
        /* Display variable */
        जर(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          थांबा;
        }
        result = CURLE_UNKNOWN_OPTION;
        थांबा;

      स्थिती 7:
        /* Environment variable */
        जर(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          जर(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            थांबा;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        अन्यथा
          result = CURLE_UNKNOWN_OPTION;
        थांबा;

      स्थिती 2:
        /* Window Size */
        जर(curl_strnequal(option, "WS", 2)) {
          स्थिरांक अक्षर *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          जर(curlx_str_संख्या(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_संख्या(&p, &y, 0xffff)) {
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
        थांबा;

      स्थिती 6:
        /* To take care or not of the 8th bit in data exchange */
        जर(curl_strnequal(option, "BINARY", 6)) {
          स्थिरांक अक्षर *p = arg;
          curl_off_t द्विमान_option;
          जर(!curlx_str_संख्या(&p, &द्विमान_option, 1) &&
             (द्विमान_option != 1)) {
            tn->us_preferred[CURL_TELOPT_द्विमान] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_द्विमान] = CURL_NO;
          }
        }
        अन्यथा
          result = CURLE_UNKNOWN_OPTION;
        थांबा;
      पूर्वनिर्धारित:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        थांबा;
      }
    }
    अन्यथा {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  जर(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = रिक्त;
  }

  परत result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
स्थिर bool bad_option(स्थिरांक अक्षर *data)
{
  परत !data || !!म_अक्षर(data, CURL_IAC);
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
  अचिन्हित अक्षर temp[2048];
  माप_प्रकार nwritten;
  माप_प्रकार len;
  CURLcode result = CURLE_OK;

  जर(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    परत CURLE_OK;

  printsub(data, '<', (स्थिरांक अचिन्हित अक्षर *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  निवड(CURL_SB_GET(tn)) {
  स्थिती CURL_TELOPT_TTYPE:
    जर(bad_option(tn->subopt_ttype))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    जर(म_माप(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      परत CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((अक्षर *)temp, आकार(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    जर(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      परत result;
    }
    printsub(data, '>', &temp[2], len-2);
    थांबा;
  स्थिती CURL_TELOPT_XDISPLOC:
    जर(bad_option(tn->subopt_xdisploc))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    जर(म_माप(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      परत CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((अक्षर *)temp, आकार(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    जर(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      परत result;
    }
    printsub(data, '>', &temp[2], len - 2);
    थांबा;
  स्थिती CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((अक्षर *)temp, आकार(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    साठी(v = tn->telnet_vars; v; v = v->next) {
      माप_प्रकार tmplen = (म_माप(v->data) + 1);
      जर(bad_option(v->data))
        परत CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      जर(len + tmplen < आकार(temp) - 6) {
        स्थिरांक अक्षर *s = म_अक्षर(v->data, ',');
        जर(!s)
          len += curl_msnprintf((अक्षर *)&temp[len], आकार(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        अन्यथा {
          माप_प्रकार vlen = s - v->data;
          len += curl_msnprintf((अक्षर *)&temp[len], आकार(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (पूर्णांक)vlen, v->data, CURL_NEW_ENV_मूल्य, ++s);
        }
      }
    }
    curl_msnprintf((अक्षर *)&temp[len], आकार(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    जर(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      परत result;
    }
    printsub(data, '>', &temp[2], len - 2);
    थांबा;
  }
  परत CURLE_OK;
}

स्थिर CURLcode telrcv(संरचना Curl_easy *data,
                       संरचना TELNET *tn,
                       स्थिरांक अचिन्हित अक्षर *inbuf, /* Data received from
                                                      socket */
                       ssize_t मोजणी)              /* Number of bytes
                                                      received */
{
  अचिन्हित अक्षर c;
  CURLcode result;
  पूर्णांक in = 0;
  पूर्णांक startwrite = -1;

#define startskipping()                                          \
  जर(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (स्थिरांक अक्षर *)&inbuf[startwrite], \
                               in-startwrite);                   \
    जर(result)                                                   \
      परत result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  जर(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  पर्यंत(मोजणी--) {
    c = inbuf[in];

    निवड(tn->telrcv_state) {
    स्थिती CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      जर(c == '\0') {
        startskipping();
        थांबा;   /* Ignore \0 after CR */
      }
      writebyte();
      थांबा;

    स्थिती CURL_TS_DATA:
      जर(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        थांबा;
      }
      अन्यथा जर(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      थांबा;

    स्थिती CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      निवड(c) {
      स्थिती CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        थांबा;
      स्थिती CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        थांबा;
      स्थिती CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        थांबा;
      स्थिती CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        थांबा;
      स्थिती CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        थांबा;
      स्थिती CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        थांबा;
      स्थिती CURL_DM:
      स्थिती CURL_NOP:
      स्थिती CURL_GA:
      पूर्वनिर्धारित:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        थांबा;
      }
      थांबा;

    स्थिती CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      थांबा;

    स्थिती CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      थांबा;

    स्थिती CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      थांबा;

    स्थिती CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      थांबा;

    स्थिती CURL_TS_SB:
      जर(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      अन्यथा
        CURL_SB_ACCUM(tn, c);
      थांबा;

    स्थिती CURL_TS_SE:
      जर(c != CURL_SE) {
        जर(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          परत CURLE_RECV_ERROR;
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
        जर(result)
          परत result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      थांबा;
    }
    ++in;
  }
  bufferflush();
  परत CURLE_OK;
}

स्थिर CURLcode telnet_done(संरचना Curl_easy *data,
                            CURLcode status, bool premature)
{
  (शून्य)status;
  (शून्य)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  परत CURLE_OK;
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
  DWORD obj_मोजणी;
  DWORD wait_timeout;
  DWORD readfile_read;
#अन्यथा
  timediff_t interval_ms;
  संरचना pollfd pfd[2];
  पूर्णांक poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  अक्षर buffer[4 * 1024];
  संरचना TELNET *tn;

  *done = TRUE; /* unconditionally */

  result = init_telnet(data);
  जर(result)
    परत result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  जर(!tn)
    परत CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  जर(result)
    परत result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  जर(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    परत CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  जर(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    परत CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_आदान_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  जर(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_मोजणी = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  अन्यथा {
    obj_मोजणी = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  पर्यंत(keepon) {
    स्थिरांक DWORD buf_size = (DWORD)आकार(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_मोजणी, objs,
                                           FALSE, wait_timeout);
    निवड(waitret) {

    स्थिती WAIT_TIMEOUT: {
      साठी(;;) {
        जर(data->set.is_fread_set) {
          माप_प्रकार n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          जर(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            थांबा;
          }

          जर(n == CURL_READFUNC_PAUSE)
            थांबा;

          जर(n == 0)                        /* no bytes */
            थांबा;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        अन्यथा {
          /* read from stdin */
          जर(!PeekNamedPipe(stdin_handle, रिक्त, 0, रिक्त,
                            &readfile_read, रिक्त)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            थांबा;
          }

          जर(!readfile_read)
            थांबा;

          जर(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, रिक्त)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            थांबा;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        जर(result) {
          keepon = FALSE;
          थांबा;
        }
      }
    }
    थांबा;

    स्थिती WAIT_OBJECT_0 + 1: {
      जर(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, रिक्त)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        थांबा;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      जर(result) {
        keepon = FALSE;
        थांबा;
      }
    }
    थांबा;

    स्थिती WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      जर(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        पूर्णांक sockerr = SOCKERRNO;
        जर(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        थांबा;
      }
      जर(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        माप_प्रकार nread;
        result = Curl_xfer_recv(data, buffer, आकार(buffer), &nread);
        /* read would have blocked. Loop again */
        जर(result == CURLE_AGAIN)
          थांबा;
        /* returned not-zero, this an error */
        अन्यथा जर(result) {
          keepon = FALSE;
          थांबा;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        अन्यथा जर(!nread) {
          keepon = FALSE;
          थांबा;
        }

        result = telrcv(data, tn, (अचिन्हित अक्षर *)buffer, nread);
        जर(result) {
          keepon = FALSE;
          थांबा;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        जर(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      जर(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      थांबा;
    }
    } /* switch */

    जर(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  जर(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#अन्यथा
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  जर(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  अन्यथा {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((खाता *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    जर(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  पर्यंत(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    निवड(Curl_poll(pfd, (अचिन्हित पूर्णांक)poll_cnt, interval_ms)) {
    स्थिती -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      सुरूठेवा;
    स्थिती 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    पूर्वनिर्धारित:                    /* read! */
      जर(pfd[0].revents & POLLIN) {
        /* read data from network */
        माप_प्रकार nread;
        result = Curl_xfer_recv(data, buffer, आकार(buffer), &nread);
        /* read would have blocked. Loop again */
        जर(result == CURLE_AGAIN)
          थांबा;
        /* returned not-zero, this an error */
        जर(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          जर(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          थांबा;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        अन्यथा जर(!nread) {
          keepon = FALSE;
          थांबा;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (अचिन्हित अक्षर *)buffer, nread);
        जर(result) {
          keepon = FALSE;
          थांबा;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        जर(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      जर(poll_cnt == 2) {
        जर(pfd[1].revents & POLLIN) { /* read from in file */
          snread = पठ(pfd[1].fd, buffer, आकार(buffer));
        }
      }
      अन्यथा {
        /* read from user-supplied method */
        snread = (पूर्णांक)data->state.fread_func(buffer, 1, आकार(buffer),
                                             data->state.in);
        जर(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          थांबा;
        }
        जर(snread == CURL_READFUNC_PAUSE)
          थांबा;
      }

      जर(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        जर(result) {
          keepon = FALSE;
          थांबा;
        }
        Curl_pgrs_upload_inc(data, (माप_प्रकार)snread);
      }
      अन्यथा जर(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      थांबा;
    } /* poll switch statement */

    जर(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    जर(!result) {
      result = Curl_pgrsUpdate(data);
      जर(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  परत result;
}

/*
 * TELNET protocol handler.
 */
स्थिरांक संरचना Curl_protocol Curl_protocol_telnet = {
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
