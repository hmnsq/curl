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
 * RFC1870 SMTP Service Extension for Message Size
 * RFC2195 CRAM-MD5 authentication
 * RFC2831 DIGEST-MD5 authentication
 * RFC3207 SMTP over TLS
 * RFC4422 Simple Authentication and Security Layer (SASL)
 * RFC4616 PLAIN authentication
 * RFC4752 The Kerberos V5 ("GSSAPI") SASL Mechanism
 * RFC4954 SMTP Authentication
 * RFC5321 SMTP protocol
 * RFC5890 Internationalized Domain Names for Applications (IDNA)
 * RFC6531 SMTP Extension for Internationalized Email
 * RFC6532 Internationalized Email Headers
 * RFC6749 OAuth 2.0 Authorization Framework
 * RFC8314 Use of TLS for Email Submission and Access
 * Draft   SMTP URL Interface   <draft-earhart-url-smtp-00.txt>
 * Draft   LOGIN SASL Mechanism <draft-murchison-sasl-login-00.txt>
 *
 ***************************************************************************/
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "smtp.h"

#अगर_अघोषित CURL_DISABLE_SMTP

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "transfer.h"
#समावेश "escape.h"
#समावेश "pingpong.h"
#समावेश "mime.h"
#समावेश "vtls/vtls.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "select.h"
#समावेश "url.h"
#समावेश "curl_gethostname.h"
#समावेश "bufref.h"
#समावेश "curl_sasl.h"
#समावेश "idn.h"
#समावेश "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#घोषणा CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#घोषणा CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
प्रकार क्रमागत {
  SMTP_STOP,        /* do nothing state, stops the state machine */
  SMTP_SERVERGREET, /* waiting for the initial greeting immediately after
                       a connect */
  SMTP_EHLO,
  SMTP_HELO,
  SMTP_STARTTLS,
  SMTP_UPGRADETLS,  /* asynchronously upgrade the connection to SSL/TLS
                       (multi mode only) */
  SMTP_AUTH,
  SMTP_COMMAND,     /* VRFY, EXPN, NOOP, RSET and HELP */
  SMTP_MAIL,        /* MAIL FROM */
  SMTP_RCPT,        /* RCPT TO */
  SMTP_तथ्य,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_LAST         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
काष्ठा smtp_conn {
  काष्ठा pingpong pp;
  काष्ठा SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  अक्षर *domain;            /* Client address/name to send in the EHLO */
  BIT(ssldone);            /* Is connect() over SSL done? */
  BIT(tls_supported);      /* StartTLS capability supported by server */
  BIT(आकार_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  BIT(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  BIT(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
काष्ठा SMTP {
  curl_pp_transfer transfer;
  अक्षर *custom;            /* Custom Request */
  काष्ठा curl_slist *rcpt; /* Recipient list */
  पूर्णांक rcpt_last_error;     /* The last error received for RCPT TO command */
  माप_प्रकार eob;              /* Number of bytes of the EOB (End Of Body) that
                              have been received so far */
  BIT(rcpt_had_ok);        /* Whether any of RCPT TO commands (depends on
                              total number of recipients) succeeded so far */
  BIT(trailing_crlf);      /* Specifies if the trailing CRLF is present */
};

/***********************************************************************
 *
 * smtp_parse_url_options()
 *
 * Parse the URL login options.
 */
अटल CURLcode smtp_parse_url_options(काष्ठा connectdata *conn,
                                       काष्ठा smtp_conn *smtpc)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *ptr = conn->options;

  जबतक(!परिणाम && ptr && *ptr) {
    स्थिर अक्षर *कुंजी = ptr;
    स्थिर अक्षर *मूल्य;

    जबतक(*ptr && *ptr != '=')
      ptr++;

    मूल्य = ptr + 1;

    जबतक(*ptr && *ptr != ';')
      ptr++;

    अगर(curl_strnequal(कुंजी, "AUTH=", 5))
      परिणाम = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               मूल्य, ptr - मूल्य);
    अन्यथा
      परिणाम = CURLE_URL_MALFORMAT;

    अगर(*ptr == ';')
      ptr++;
  }

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
अटल CURLcode smtp_parse_url_path(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  स्थिर अक्षर *path = &तथ्य->state.up.path[1]; /* skip leading path */
  अक्षर localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  अगर(!*path) {
    अगर(!Curl_gethostname(localhost, माप(localhost)))
      path = localhost;
    अन्यथा
      path = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  वापस Curl_urldecode(path, 0, &smtpc->domain, शून्य, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
अटल CURLcode smtp_parse_custom_request(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा SMTP *smtp)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *custom = CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  अगर(custom)
    परिणाम = Curl_urldecode(custom, 0, &smtp->custom, शून्य, REJECT_CTRL);

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_parse_address()
 *
 * Parse the fully qualified mailbox address into a local address part and the
 * hostname, converting the hostname to an IDN A-label, as per RFC-5890, if
 * necessary.
 *
 * Parameters:
 *
 * fqma           [in]     - The fully qualified mailbox address (which may or
 *                           may not contain UTF-8 characters).
 * address        [in/out] - A new allocated buffer which holds the local
 *                           address part of the mailbox. This buffer must be
 *                           free'ed by the caller.
 * host           [in/out] - The hostname structure that holds the original,
 *                           and optionally encoded, hostname.
 *                           Curl_free_idnconverted_hostname() must be called
 *                           once the caller has finished with the structure.
 *
 * Returns CURLE_OK on success.
 *
 * Notes:
 *
 * Should a UTF-8 hostname require conversion to IDN ACE and we cannot honor
 * that conversion then we shall return success. This allow the caller to send
 * the data to the server as a U-label (as per RFC-6531 sect. 3.2).
 *
 * If an mailbox '@' separator cannot be located then the mailbox is considered
 * to be either a local mailbox or an invalid mailbox (depending on what the
 * calling function deems it to be) then the input will be returned in
 * the address part with the hostname being NULL.
 */
अटल CURLcode smtp_parse_address(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fqma,
                                   अक्षर **address, काष्ठा hostname *host,
                                   स्थिर अक्षर **suffix)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार लंबाई;
  अक्षर *addressend;
  अक्षर *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  अगर(strpbrk(fqma, "\r\n")) {
    failf(तथ्य, "Refusing to send email address with a CR or LF");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  अगर(!dup)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(fqma[0] != '<') {
    लंबाई = म_माप(dup);
    अगर(लंबाई && dup[लंबाई - 1] == '>')
      dup[लंबाई - 1] = '\0';
  }
  अन्यथा {
    addressend = म_खोजप(dup, '>');
    अगर(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  अगर(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (व्योम)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *address = dup;

  वापस परिणाम;
}

काष्ठा cr_eob_ctx {
  काष्ठा Curl_creader super;
  काष्ठा bufq buf;
  माप_प्रकार n_eob; /* how many EOB bytes we matched so far */
  माप_प्रकार eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

अटल CURLcode cr_eob_init(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_creader *reader)
{
  काष्ठा cr_eob_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  वापस CURLE_OK;
}

अटल व्योम cr_eob_close(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader)
{
  काष्ठा cr_eob_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#घोषणा SMTP_EOB          "\r\n.\r\n"
#घोषणा SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
अटल CURLcode cr_eob_read(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_creader *reader,
                            अक्षर *buf, माप_प्रकार blen,
                            माप_प्रकार *pnread, bool *peos)
{
  काष्ठा cr_eob_ctx *ctx = reader->ctx;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nread, i, start, n;
  bool eos;

  अगर(!ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* Get more and convert it when needed */
    परिणाम = Curl_creader_read(तथ्य, reader->अग्र, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(तथ्य, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (पूर्णांक)परिणाम, nread, eos);
    अगर(परिणाम)
      वापस परिणाम;

    ctx->read_eos = eos;
    अगर(nread) {
      अगर(!ctx->n_eob && !स_प्रथम(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        वापस CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      क्रम(i = start = 0; i < nread; ++i) {
        अगर(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          परिणाम = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          अगर(परिणाम)
            वापस परिणाम;
          परिणाम = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          अगर(परिणाम)
            वापस परिणाम;
          ctx->n_eob = 0;
          start = i;
          अगर(तथ्य->state.infilesize > 0)
            तथ्य->state.infilesize++;
        }

        अगर(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        अगर(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      अगर(start < nread) {
        परिणाम = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        अगर(परिणाम)
          वापस परिणाम;
      }
    }
  }

  *peos = FALSE;

  अगर(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    स्थिर अक्षर *eob = SMTP_EOB;
    CURL_TRC_SMTP(तथ्य, "auto-ending mail body with '\\r\\n.\\r\\n'");
    चयन(ctx->n_eob) {
    हाल 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      अवरोध;
    हाल 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      अवरोध;
    शेष:
      अवरोध;
    }
    परिणाम = Curl_bufq_cwrite(&ctx->buf, eob, म_माप(eob), &n);
    अगर(परिणाम)
      वापस परिणाम;
    ctx->processed_eos = TRUE;
  }

  अगर(!Curl_bufq_is_empty(&ctx->buf)) {
    परिणाम = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  अन्यथा
    *pnread = 0;

  अगर(ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(तथ्य, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(तथ्य, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (पूर्णांक)परिणाम, *pnread, *peos));
  वापस परिणाम;
}

अटल curl_off_t cr_eob_योग_लंबाई(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (व्योम)तथ्य;
  (व्योम)reader;
  वापस -1;
}

अटल स्थिर काष्ठा Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_योग_लंबाई,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  माप(काष्ठा cr_eob_ctx)
};

अटल CURLcode cr_eob_add(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *reader = शून्य;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&reader, तथ्य, &cr_eob, CURL_CR_CONTENT_ENCODE);
  अगर(!परिणाम)
    परिणाम = Curl_creader_add(तथ्य, reader);

  अगर(परिणाम && reader)
    Curl_creader_free(तथ्य, reader);
  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
अटल bool smtp_endofresp(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                           स्थिर अक्षर *पंक्ति, माप_प्रकार len, पूर्णांक *resp)
{
  काष्ठा smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool end = FALSE;
  (व्योम)तथ्य;

  DEBUGASSERT(smtpc);
  अगर(!smtpc)
    वापस FALSE;

  /* Nothing for us */
  अगर(len < 4 || !ISDIGIT(पंक्ति[0]) || !ISDIGIT(पंक्ति[1]) || !ISDIGIT(पंक्ति[2]))
    वापस FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  अगर(पंक्ति[3] == ' ' || len == 5) {
    अक्षर tmpline[6];
    curl_off_t code;
    स्थिर अक्षर *p = tmpline;
    end = TRUE;
    स_नकल(tmpline, पंक्ति, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    अगर(curlx_str_संख्या(&p, &code, len == 5 ? 99999 : 999))
      वापस FALSE;
    *resp = (पूर्णांक)code;

    /* Make sure real server never sends internal value */
    अगर(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  अन्यथा अगर(पंक्ति[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    end = TRUE;
    *resp = 1;  /* Internal response code */
  }

  वापस end;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
अटल CURLcode smtp_get_message(काष्ठा Curl_easy *तथ्य, काष्ठा bufref *out)
{
  काष्ठा smtp_conn *smtpc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_SMTP_CONN);
  अक्षर *message;
  माप_प्रकार len;

  अगर(!smtpc)
    वापस CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  अगर(len > 4) {
    /* Find the start of the message */
    len -= 4;
    क्रम(message += 4; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    जबतक(len--)
      अगर(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        अवरोध;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, शून्य);
  }
  अन्यथा
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, शून्य);

  वापस CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
अटल व्योम smtp_state(काष्ठा Curl_easy *तथ्य,
                       काष्ठा smtp_conn *smtpc,
                       smtpstate newstate)
{
#अगर_घोषित CURLVERBOSE
  /* for debug purposes */
  अटल स्थिर अक्षर * स्थिर names[] = {
    "STOP",
    "SERVERGREET",
    "EHLO",
    "HELO",
    "STARTTLS",
    "UPGRADETLS",
    "AUTH",
    "COMMAND",
    "MAIL",
    "RCPT",
    "DATA",
    "POSTDATA",
    "QUIT",
    /* LAST */
  };

  अगर(smtpc->state != newstate)
    CURL_TRC_SMTP(तथ्य, "state change from %s to %s",
                  names[smtpc->state], names[newstate]);
#अन्यथा
  (व्योम)तथ्य;
#पूर्ण_अगर

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
अटल CURLcode smtp_perform_ehlo(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा smtp_conn *smtpc)
{
  CURLcode परिणाम = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "EHLO %s", smtpc->domain);

  अगर(!परिणाम)
    smtp_state(तथ्य, smtpc, SMTP_EHLO);

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
अटल CURLcode smtp_perform_helo(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा smtp_conn *smtpc)
{
  CURLcode परिणाम = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "HELO %s", smtpc->domain);

  अगर(!परिणाम)
    smtp_state(तथ्य, smtpc, SMTP_HELO);

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
अटल CURLcode smtp_perform_starttls(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "%s", "STARTTLS");

  अगर(!परिणाम)
    smtp_state(तथ्य, smtpc, SMTP_STARTTLS);

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
अटल CURLcode smtp_perform_upgrade_tls(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा smtp_conn *smtpc)
{
#अगर_घोषित USE_SSL
  /* Start the SSL connection */
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  अगर(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    परिणाम = Curl_ssl_cfilter_add(
      तथ्य, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    अगर(परिणाम)
      जाओ out;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  परिणाम = Curl_conn_connect(तथ्य, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(तथ्य, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (पूर्णांक)परिणाम, ssldone));
  अगर(!परिणाम && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    परिणाम = smtp_perform_ehlo(तथ्य, smtpc);
  }
out:
  वापस परिणाम;
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)smtpc;
  वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
अटल CURLcode smtp_perform_auth(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *mech,
                                  स्थिर काष्ठा bufref *initresp)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा smtp_conn *smtpc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_SMTP_CONN);
  स्थिर अक्षर *ir = Curl_bufref_ptr(initresp);

  अगर(!smtpc)
    वापस CURLE_FAILED_INIT;

  अगर(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  अन्यथा {
    /* Send the AUTH command */
    परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "AUTH %s", mech);
  }

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
अटल CURLcode smtp_continue_auth(काष्ठा Curl_easy *तथ्य,
                                   स्थिर अक्षर *mech,
                                   स्थिर काष्ठा bufref *resp)
{
  काष्ठा smtp_conn *smtpc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_SMTP_CONN);

  (व्योम)mech;
  अगर(!smtpc)
    वापस CURLE_FAILED_INIT;
  वापस Curl_pp_sendf(तथ्य, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
अटल CURLcode smtp_cancel_auth(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *mech)
{
  काष्ठा smtp_conn *smtpc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_SMTP_CONN);

  (व्योम)mech;
  अगर(!smtpc)
    वापस CURLE_FAILED_INIT;
  वापस Curl_pp_sendf(तथ्य, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
अटल CURLcode smtp_perform_authentication(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा smtp_conn *smtpc)
{
  CURLcode परिणाम = CURLE_OK;
  saslprogress progress;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  अगर(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, तथ्य)) {
    smtp_state(तथ्य, smtpc, SMTP_STOP);
    वापस परिणाम;
  }

  /* Calculate the SASL login details */
  परिणाम = Curl_sasl_start(&smtpc->sasl, तथ्य, FALSE, &progress);

  अगर(!परिणाम) {
    अगर(progress == SASL_INPROGRESS)
      smtp_state(तथ्य, smtpc, SMTP_AUTH);
    अन्यथा
      परिणाम = Curl_sasl_is_blocked(&smtpc->sasl, तथ्य);
  }

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
अटल CURLcode smtp_perform_command(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा smtp_conn *smtpc,
                                     काष्ठा SMTP *smtp)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    अगर(!smtp->custom || !smtp->custom[0]) {
      अक्षर *address = शून्य;
      काष्ठा hostname host = { शून्य, शून्य, शून्य, शून्य };
      स्थिर अक्षर *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      परिणाम = smtp_parse_address(तथ्य, smtp->rcpt->तथ्य,
                                  &address, &host, &suffix);
      अगर(परिणाम)
        वापस परिणाम;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(address) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "VRFY %s%s%s%s",
                             address,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(address);
    }
    अन्यथा {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !म_भेद(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->तथ्य,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  अन्यथा
    /* Send the non-recipient based command such as HELP */
    परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  अगर(!परिणाम)
    smtp_state(तथ्य, smtpc, SMTP_COMMAND);

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
अटल CURLcode smtp_perform_mail(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा smtp_conn *smtpc,
                                  काष्ठा SMTP *smtp)
{
  अक्षर *from = शून्य;
  अक्षर *auth = शून्य;
  अक्षर *आकार = शून्य;
  स्थिर अक्षर *str;
  CURLcode परिणाम = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(तथ्य, STRING_MAIL_FROM);
  अगर(str) {
    अक्षर *address = शून्य;
    काष्ठा hostname host = { शून्य, शून्य, शून्य, शून्य };
    स्थिर अक्षर *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    परिणाम = smtp_parse_address(तथ्य, str, &address, &host, &suffix);
    अगर(परिणाम)
      जाओ out;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(address) ||
            !Curl_is_ASCII_name(host.name));

    अगर(host.name) {
      from = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    अन्यथा
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", address, suffix);

    curlx_free(address);
  }
  अन्यथा
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  अगर(!from) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(तथ्य, STRING_MAIL_AUTH);
  अगर(str && smtpc->sasl.authused) {
    अगर(str[0] != '\0') {
      अक्षर *address = शून्य;
      काष्ठा hostname host = { शून्य, शून्य, शून्य, शून्य };
      स्थिर अक्षर *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      परिणाम = smtp_parse_address(तथ्य, str, &address, &host, &suffix);
      अगर(परिणाम)
        जाओ out;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      अगर(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(address) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      अगर(host.name) {
        auth = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      अन्यथा
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", address, suffix);
      curlx_free(address);
    }
    अन्यथा
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    अगर(!auth) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }

#अगर_अघोषित CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  अगर(IS_MIME_POST(तथ्य)) {
    curl_mimepart *postp = तथ्य->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(अचिन्हित पूर्णांक)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, तथ्य->set.headers, 0);
    परिणाम = Curl_mime_prepare_headers(तथ्य, postp, शून्य,
                                       शून्य, MIMESTRATEGY_MAIL);

    अगर(!परिणाम && !Curl_checkheaders(तथ्य, STRCONST("Mime-Version")))
      परिणाम = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    अगर(!परिणाम)
      परिणाम = Curl_creader_set_mime(तथ्य, postp);
    अगर(परिणाम)
      जाओ out;
    तथ्य->state.infilesize = Curl_creader_योग_लंबाई(तथ्य);
  }
  अन्यथा
#पूर्ण_अगर
  {
    परिणाम = Curl_creader_set_fread(तथ्य, तथ्य->state.infilesize);
    अगर(परिणाम)
      जाओ out;
  }

  /* Calculate the optional SIZE parameter */
  अगर(smtpc->आकार_supported && तथ्य->state.infilesize > 0) {
    आकार = curl_maprintf("%" FMT_OFF_T, तथ्य->state.infilesize);

    अगर(!आकार) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  अगर(smtpc->utf8_supported && !utf8) {
    काष्ठा curl_slist *rcpt = smtp->rcpt;

    जबतक(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      अगर(!Curl_is_ASCII_name(rcpt->तथ्य))
        utf8 = TRUE;

      rcpt = rcpt->अग्र;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  परिणाम = cr_eob_add(तथ्य);
  अगर(परिणाम)
    जाओ out;

  /* Send the MAIL command */
  परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         आकार ? " SIZE=" : "", /* Optional on SIZE support  */
                         आकार ? आकार : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

out:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(आकार);

  अगर(!परिणाम)
    smtp_state(तथ्य, smtpc, SMTP_MAIL);

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
अटल CURLcode smtp_perform_rcpt_to(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा smtp_conn *smtpc,
                                     काष्ठा SMTP *smtp)
{
  CURLcode परिणाम = CURLE_OK;
  अक्षर *address = शून्य;
  काष्ठा hostname host = { शून्य, शून्य, शून्य, शून्य };
  स्थिर अक्षर *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  परिणाम = smtp_parse_address(तथ्य, smtp->rcpt->तथ्य,
                              &address, &host, &suffix);
  अगर(परिणाम)
    वापस परिणाम;

  /* Send the RCPT TO command */
  अगर(host.name)
    परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           address, host.name, suffix);
  अन्यथा
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "RCPT TO:<%s>%s",
                           address, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(address);

  अगर(!परिणाम)
    smtp_state(तथ्य, smtpc, SMTP_RCPT);

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
अटल CURLcode smtp_perform_quit(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "%s", "QUIT");

  अगर(!परिणाम)
    smtp_state(तथ्य, smtpc, SMTP_QUIT);

  वापस परिणाम;
}

/* For the initial server greeting */
अटल CURLcode smtp_state_servergreet_resp(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा smtp_conn *smtpc,
                                            पूर्णांक smtpcode,
                                            smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  अगर(smtpcode / 100 != 2) {
    failf(तथ्य, "Got unexpected smtp-server response: %d", smtpcode);
    परिणाम = CURLE_WEIRD_SERVER_REPLY;
  }
  अन्यथा
    परिणाम = smtp_perform_ehlo(तथ्य, smtpc);

  वापस परिणाम;
}

/* For STARTTLS responses */
अटल CURLcode smtp_state_starttls_resp(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा smtp_conn *smtpc,
                                         पूर्णांक smtpcode,
                                         smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  /* Pipelining in response is forbidden. */
  अगर(smtpc->pp.overflow)
    वापस CURLE_WEIRD_SERVER_REPLY;

  अगर(smtpcode != 220) {
    अगर(तथ्य->set.use_ssl != CURLUSESSL_TRY) {
      failf(तथ्य, "STARTTLS denied, code %d", smtpcode);
      परिणाम = CURLE_USE_SSL_FAILED;
    }
    अन्यथा
      परिणाम = smtp_perform_authentication(तथ्य, smtpc);
  }
  अन्यथा
    smtp_state(तथ्य, smtpc, SMTP_UPGRADETLS);

  वापस परिणाम;
}

/* For EHLO responses */
अटल CURLcode smtp_state_ehlo_resp(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा smtp_conn *smtpc,
                                     पूर्णांक smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *पंक्ति = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  माप_प्रकार len = smtpc->pp.nfinal;

  (व्योम)instate;

  अगर(smtpcode / 100 != 2 && smtpcode != 1) {
    अगर(तथ्य->set.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(तथ्य->conn, FIRSTSOCKET))
      परिणाम = smtp_perform_helo(तथ्य, smtpc);
    अन्यथा {
      failf(तथ्य, "Remote access denied: %d", smtpcode);
      परिणाम = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  अन्यथा अगर(len >= 4) {
    पंक्ति += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    अगर(len >= 8 && curl_strnequal(पंक्ति, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    अन्यथा अगर(len >= 4 && curl_strnequal(पंक्ति, "SIZE", 4))
      smtpc->आकार_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    अन्यथा अगर(len >= 8 && curl_strnequal(पंक्ति, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    अन्यथा अगर(len >= 5 && curl_strnequal(पंक्ति, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      पंक्ति += 5;
      len -= 5;

      /* Loop through the data line */
      क्रम(;;) {
        माप_प्रकार llen;
        माप_प्रकार wordlen;
        अचिन्हित लघु mechbit;

        जबतक(len && (ISBLANK(*पंक्ति) || ISNEWLINE(*पंक्ति))) {
          पंक्ति++;
          len--;
        }

        अगर(!len)
          अवरोध;

        /* Extract the word */
        क्रम(wordlen = 0; wordlen < len && !ISBLANK(पंक्ति[wordlen]) &&
              !ISNEWLINE(पंक्ति[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(पंक्ति, wordlen, &llen);
        अगर(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        पंक्ति += wordlen;
        len -= wordlen;
      }
    }

    अगर(smtpcode != 1) {
      अगर(तथ्य->set.use_ssl && !Curl_conn_is_ssl(तथ्य->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        अगर(smtpc->tls_supported)
          /* Switch to TLS connection now */
          परिणाम = smtp_perform_starttls(तथ्य, smtpc);
        अन्यथा अगर(तथ्य->set.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          परिणाम = smtp_perform_authentication(तथ्य, smtpc);
        अन्यथा {
          failf(तथ्य, "STARTTLS not supported.");
          परिणाम = CURLE_USE_SSL_FAILED;
        }
      }
      अन्यथा
        परिणाम = smtp_perform_authentication(तथ्य, smtpc);
    }
  }
  अन्यथा {
    failf(तथ्य, "Unexpectedly short EHLO response");
    परिणाम = CURLE_WEIRD_SERVER_REPLY;
  }

  वापस परिणाम;
}

/* For HELO responses */
अटल CURLcode smtp_state_helo_resp(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा smtp_conn *smtpc,
                                     पूर्णांक smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  अगर(smtpcode / 100 != 2) {
    failf(तथ्य, "Remote access denied: %d", smtpcode);
    परिणाम = CURLE_REMOTE_ACCESS_DENIED;
  }
  अन्यथा
    /* End of connect phase */
    smtp_state(तथ्य, smtpc, SMTP_STOP);

  वापस परिणाम;
}

/* For SASL authentication responses */
अटल CURLcode smtp_state_auth_resp(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा smtp_conn *smtpc,
                                     पूर्णांक smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  saslprogress progress;

  (व्योम)instate;

  परिणाम = Curl_sasl_continue(&smtpc->sasl, तथ्य, smtpcode, &progress);
  अगर(!परिणाम)
    चयन(progress) {
    हाल SASL_DONE:
      smtp_state(तथ्य, smtpc, SMTP_STOP);  /* Authenticated */
      अवरोध;
    हाल SASL_IDLE:            /* No mechanism left after cancellation */
      failf(तथ्य, "Authentication cancelled");
      परिणाम = CURLE_LOGIN_DENIED;
      अवरोध;
    शेष:
      अवरोध;
    }

  वापस परिणाम;
}

/* For command responses */
अटल CURLcode smtp_state_command_resp(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा smtp_conn *smtpc,
                                        काष्ठा SMTP *smtp,
                                        पूर्णांक smtpcode,
                                        smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *पंक्ति = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  माप_प्रकार len = smtpc->pp.nfinal;

  (व्योम)instate;

  अगर((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(तथ्य, "Command failed: %d", smtpcode);
    परिणाम = CURLE_WEIRD_SERVER_REPLY;
  }
  अन्यथा {
    अगर(!तथ्य->req.no_body)
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, पंक्ति, len);

    अगर(!परिणाम && (smtpcode != 1)) {
      अगर(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->अग्र;

        अगर(smtp->rcpt) {
          /* Send the next command */
          परिणाम = smtp_perform_command(तथ्य, smtpc, smtp);
        }
        अन्यथा
          /* End of DO phase */
          smtp_state(तथ्य, smtpc, SMTP_STOP);
      }
      अन्यथा
        /* End of DO phase */
        smtp_state(तथ्य, smtpc, SMTP_STOP);
    }
  }

  वापस परिणाम;
}

/* For MAIL responses */
अटल CURLcode smtp_state_mail_resp(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा smtp_conn *smtpc,
                                     काष्ठा SMTP *smtp,
                                     पूर्णांक smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  अगर(smtpcode / 100 != 2) {
    failf(तथ्य, "MAIL failed: %d", smtpcode);
    परिणाम = CURLE_SEND_ERROR;
  }
  अन्यथा
    /* Start the RCPT TO command */
    परिणाम = smtp_perform_rcpt_to(तथ्य, smtpc, smtp);

  वापस परिणाम;
}

/* For RCPT responses */
अटल CURLcode smtp_state_rcpt_resp(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा smtp_conn *smtpc,
                                     काष्ठा SMTP *smtp,
                                     पूर्णांक smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (व्योम)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !तथ्य->set.mail_rcpt_allowfails);

  अगर(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_last_error = smtpcode;

    अगर(is_smtp_blocking_err) {
      failf(तथ्य, "RCPT failed: %d", smtpcode);
      परिणाम = CURLE_SEND_ERROR;
    }
  }
  अन्यथा {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  अगर(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->अग्र;

    अगर(smtp->rcpt)
      /* Send the next RCPT TO command */
      परिणाम = smtp_perform_rcpt_to(तथ्य, smtpc, smtp);
    अन्यथा {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      अगर(!smtp->rcpt_had_ok) {
        failf(तथ्य, "RCPT failed: %d (last error)", smtp->rcpt_last_error);
        परिणाम = CURLE_SEND_ERROR;
      }
      अन्यथा {
        /* Send the DATA command */
        परिणाम = Curl_pp_sendf(तथ्य, &smtpc->pp, "%s", "DATA");

        अगर(!परिणाम)
          smtp_state(तथ्य, smtpc, SMTP_तथ्य);
      }
    }
  }

  वापस परिणाम;
}

/* For DATA response */
अटल CURLcode smtp_state_तथ्य_resp(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा smtp_conn *smtpc,
                                     पूर्णांक smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  अगर(smtpcode != 354) {
    failf(तथ्य, "DATA failed: %d", smtpcode);
    परिणाम = CURLE_SEND_ERROR;
  }
  अन्यथा {
    /* Set the progress upload size */
    Curl_pgrs_Set_Upload_आकार(तथ्य, तथ्य->state.infilesize);

    /* SMTP upload */
    Curl_xfer_setup_send(तथ्य, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(तथ्य, smtpc, SMTP_STOP);
  }

  वापस परिणाम;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
अटल CURLcode smtp_state_postdata_resp(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा smtp_conn *smtpc,
                                         पूर्णांक smtpcode,
                                         smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  (व्योम)instate;

  अगर(smtpcode != 250)
    परिणाम = CURLE_WEIRD_SERVER_REPLY;

  /* End of DONE phase */
  smtp_state(तथ्य, smtpc, SMTP_STOP);

  वापस परिणाम;
}

अटल CURLcode smtp_pp_statemachine(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक smtpcode;
  काष्ठा smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  काष्ठा SMTP *smtp = Curl_meta_get(तथ्य, CURL_META_SMTP_EASY);
  माप_प्रकार nread = 0;

  अगर(!smtpc || !smtp)
    वापस CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  अगर(smtpc->state == SMTP_UPGRADETLS) {
    परिणाम = smtp_perform_upgrade_tls(तथ्य, smtpc);
    अगर(परिणाम || (smtpc->state == SMTP_UPGRADETLS))
      वापस परिणाम;
  }

  /* Flush any data that needs to be sent */
  अगर(smtpc->pp.sendleft)
    वापस Curl_pp_flushsend(तथ्य, &smtpc->pp);

  करो {
    /* Read the response from the server */
    परिणाम = Curl_pp_readresp(तथ्य, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    अगर(परिणाम)
      वापस परिणाम;

    /* Store the latest response for later retrieval if necessary */
    अगर(smtpc->state != SMTP_QUIT && smtpcode != 1)
      तथ्य->info.httpcode = smtpcode;

    अगर(!smtpcode)
      अवरोध;

    /* We have now received a full SMTP server response */
    चयन(smtpc->state) {
    हाल SMTP_SERVERGREET:
      परिणाम = smtp_state_servergreet_resp(तथ्य, smtpc,
                                           smtpcode, smtpc->state);
      अवरोध;

    हाल SMTP_EHLO:
      परिणाम = smtp_state_ehlo_resp(तथ्य, smtpc, smtpcode, smtpc->state);
      अवरोध;

    हाल SMTP_HELO:
      परिणाम = smtp_state_helo_resp(तथ्य, smtpc, smtpcode, smtpc->state);
      अवरोध;

    हाल SMTP_STARTTLS:
      परिणाम = smtp_state_starttls_resp(तथ्य, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      अगर(!परिणाम && (smtpc->state == SMTP_UPGRADETLS))
        जाओ upgrade_tls;
      अवरोध;

    हाल SMTP_AUTH:
      परिणाम = smtp_state_auth_resp(तथ्य, smtpc, smtpcode, smtpc->state);
      अवरोध;

    हाल SMTP_COMMAND:
      परिणाम = smtp_state_command_resp(तथ्य, smtpc, smtp,
                                       smtpcode, smtpc->state);
      अवरोध;

    हाल SMTP_MAIL:
      परिणाम = smtp_state_mail_resp(तथ्य, smtpc, smtp, smtpcode, smtpc->state);
      अवरोध;

    हाल SMTP_RCPT:
      परिणाम = smtp_state_rcpt_resp(तथ्य, smtpc, smtp, smtpcode, smtpc->state);
      अवरोध;

    हाल SMTP_तथ्य:
      परिणाम = smtp_state_तथ्य_resp(तथ्य, smtpc, smtpcode, smtpc->state);
      अवरोध;

    हाल SMTP_POSTDATA:
      परिणाम = smtp_state_postdata_resp(तथ्य, smtpc, smtpcode, smtpc->state);
      अवरोध;

    हाल SMTP_QUIT:
    शेष:
      /* internal error */
      smtp_state(तथ्य, smtpc, SMTP_STOP);
      अवरोध;
    }
  } जबतक(!परिणाम && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  वापस परिणाम;
}

/* Called repeatedly until done from multi.c */
अटल CURLcode smtp_multi_statemach(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा smtp_conn *smtpc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_SMTP_CONN);

  *done = FALSE;
  अगर(!smtpc)
    वापस CURLE_FAILED_INIT;

  परिणाम = Curl_pp_statemach(तथ्य, &smtpc->pp, FALSE, FALSE);
  *done = (smtpc->state == SMTP_STOP);
  वापस परिणाम;
}

अटल CURLcode smtp_block_statemach(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode परिणाम = CURLE_OK;

  जबतक(smtpc->state != SMTP_STOP && !परिणाम)
    परिणाम = Curl_pp_statemach(तथ्य, &smtpc->pp, TRUE, disconnecting);

  वापस परिणाम;
}

/* For the SMTP "protocol connect" and "doing" phases only */
अटल CURLcode smtp_pollset(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps)
{
  काष्ठा smtp_conn *smtpc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_SMTP_CONN);
  वापस smtpc ? Curl_pp_pollset(तथ्य, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
अटल स्थिर काष्ठा SASLproto saslsmtp = {
  "smtp",               /* The service name */
  smtp_perform_auth,    /* Send authentication command */
  smtp_continue_auth,   /* Send authentication continuation */
  smtp_cancel_auth,     /* Cancel authentication */
  smtp_get_message,     /* Get SASL response message */
  512 - 8,              /* Max line len - strlen("AUTH ") - 1 space - CRLF */
  334,                  /* Code received when continuation is expected */
  235,                  /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,    /* Default mechanisms */
  SASL_FLAG_BASE64      /* Configuration flags */
};

/***********************************************************************
 *
 * smtp_connect()
 *
 * This function should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable pointed to by 'done' will be TRUE if the protocol-layer
 * connect phase is done when this function returns, or FALSE if not.
 */
अटल CURLcode smtp_connect(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा smtp_conn *smtpc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_SMTP_CONN);
  CURLcode परिणाम = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  अगर(!smtpc)
    वापस CURLE_FAILED_INIT;

  PINGPONG_SETUP(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, तथ्य, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(तथ्य));

  /* Parse the URL options */
  परिणाम = smtp_parse_url_options(तथ्य->conn, smtpc);
  अगर(परिणाम)
    वापस परिणाम;

  /* Parse the URL path */
  परिणाम = smtp_parse_url_path(तथ्य, smtpc);
  अगर(परिणाम)
    वापस परिणाम;

  /* Start off waiting for the server greeting response */
  smtp_state(तथ्य, smtpc, SMTP_SERVERGREET);

  परिणाम = smtp_multi_statemach(तथ्य, done);

  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_done()
 *
 * The DONE function. This does what needs to be done after a single DO has
 * performed.
 *
 * Input argument is already checked for validity.
 */
अटल CURLcode smtp_done(काष्ठा Curl_easy *तथ्य, CURLcode status,
                          bool premature)
{
  काष्ठा smtp_conn *smtpc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_SMTP_CONN);
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा SMTP *smtp = Curl_meta_get(तथ्य, CURL_META_SMTP_EASY);

  (व्योम)premature;

  अगर(!smtpc)
    वापस CURLE_FAILED_INIT;
  अगर(!smtp)
    वापस CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  अगर(status) {
    CURL_TRC_M(तथ्य, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    परिणाम = status;         /* use the already set error code */
  }
  अन्यथा अगर(!तथ्य->set.connect_only && तथ्य->set.mail_rcpt &&
          (तथ्य->state.upload || IS_MIME_POST(तथ्य))) {

    smtp_state(तथ्य, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    परिणाम = smtp_block_statemach(तथ्य, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(तथ्य, "smtp_done(status=%d, premature=%d) -> %d",
                (पूर्णांक)status, premature, (पूर्णांक)परिणाम);
  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
अटल CURLcode smtp_perform(काष्ठा Curl_easy *तथ्य,
                             काष्ठा smtp_conn *smtpc,
                             काष्ठा SMTP *smtp,
                             bool *connected,
                             bool *dophase_done)
{
  /* This is SMTP and no proxy */
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_SMTP(तथ्य, "smtp_perform(), start");

  अगर(तथ्य->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = तथ्य->set.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_last_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  अगर((तथ्य->state.upload || IS_MIME_POST(तथ्य)) && तथ्य->set.mail_rcpt)
    /* MAIL transfer */
    परिणाम = smtp_perform_mail(तथ्य, smtpc, smtp);
  अन्यथा
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    परिणाम = smtp_perform_command(तथ्य, smtpc, smtp);

  अगर(परिणाम)
    जाओ out;

  /* Run the state-machine */
  परिणाम = smtp_multi_statemach(तथ्य, dophase_done);

  *connected = Curl_conn_is_connected(तथ्य->conn, FIRSTSOCKET);

out:
  CURL_TRC_SMTP(तथ्य, "smtp_perform() -> %d, connected=%d, done=%d",
                (पूर्णांक)परिणाम, *connected, *dophase_done);
  वापस परिणाम;
}

/* Call this when the DO phase has completed */
अटल CURLcode smtp_dophase_done(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा SMTP *smtp,
                                  bool connected)
{
  (व्योम)connected;

  अगर(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(तथ्य);

  वापस CURLE_OK;
}

/***********************************************************************
 *
 * smtp_regular_transfer()
 *
 * The input argument is already checked for validity.
 *
 * Performs all commands done before a regular transfer between a local and a
 * remote host.
 */
अटल CURLcode smtp_regular_transfer(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा smtp_conn *smtpc,
                                      काष्ठा SMTP *smtp,
                                      bool *dophase_done)
{
  CURLcode परिणाम = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  तथ्य->req.आकार = -1;

  /* Set the progress data */
  Curl_pgrsReset(तथ्य);

  /* Carry out the perform */
  परिणाम = smtp_perform(तथ्य, smtpc, smtp, &connected, dophase_done);

  /* Perform post DO phase operations if necessary */
  अगर(!परिणाम && *dophase_done)
    परिणाम = smtp_dophase_done(तथ्य, smtp, connected);

  CURL_TRC_SMTP(तथ्य, "smtp_regular_transfer() -> %d, done=%d",
                (पूर्णांक)परिणाम, *dophase_done);
  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_do()
 *
 * This function is registered as 'curl_do' function. It decodes the path
 * parts etc as a wrapper to the actual DO function (smtp_perform).
 *
 * The input argument is already checked for validity.
 */
अटल CURLcode smtp_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा smtp_conn *smtpc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_SMTP_CONN);
  काष्ठा SMTP *smtp = Curl_meta_get(तथ्य, CURL_META_SMTP_EASY);
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);
  *done = FALSE; /* default to false */
  अगर(!smtpc || !smtp)
    वापस CURLE_FAILED_INIT;

  /* Parse the custom request */
  परिणाम = smtp_parse_custom_request(तथ्य, smtp);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = smtp_regular_transfer(तथ्य, smtpc, smtp, done);
  CURL_TRC_SMTP(तथ्य, "smtp_do() -> %d, done=%d", (पूर्णांक)परिणाम, *done);
  वापस परिणाम;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
अटल CURLcode smtp_disconnect(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn,
                                bool dead_connection)
{
  काष्ठा smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  अगर(!smtpc)
    वापस CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  अगर(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(तथ्य, &smtpc->pp) &&
     !smtp_perform_quit(तथ्य, smtpc))
    (व्योम)smtp_block_statemach(तथ्य, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(तथ्य, "smtp_disconnect(), finished");
  वापस CURLE_OK;
}

/* Called from multi.c while DOing */
अटल CURLcode smtp_doing(काष्ठा Curl_easy *तथ्य, bool *dophase_done)
{
  काष्ठा SMTP *smtp = Curl_meta_get(तथ्य, CURL_META_SMTP_EASY);
  CURLcode परिणाम;

  अगर(!smtp)
    वापस CURLE_FAILED_INIT;
  परिणाम = smtp_multi_statemach(तथ्य, dophase_done);
  अगर(परिणाम)
    DEBUGF(infof(तथ्य, "DO phase failed"));
  अन्यथा अगर(*dophase_done) {
    परिणाम = smtp_dophase_done(तथ्य, smtp, FALSE /* not connected */);

    DEBUGF(infof(तथ्य, "DO phase is complete"));
  }

  CURL_TRC_SMTP(तथ्य, "smtp_doing() -> %d, done=%d", (पूर्णांक)परिणाम,
                *dophase_done);
  वापस परिणाम;
}

अटल व्योम smtp_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा SMTP *smtp = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  curlx_free(smtp);
}

अटल व्योम smtp_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा smtp_conn *smtpc = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

अटल CURLcode smtp_setup_connection(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn)
{
  काष्ठा smtp_conn *smtpc;
  काष्ठा SMTP *smtp;
  CURLcode परिणाम = CURLE_OK;

  smtpc = curlx_calloc(1, माप(*smtpc));
  अगर(!smtpc ||
     Curl_conn_meta_set(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  smtp = curlx_calloc(1, माप(*smtp));
  अगर(!smtp ||
     Curl_meta_set(तथ्य, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    परिणाम = CURLE_OUT_OF_MEMORY;

out:
  CURL_TRC_SMTP(तथ्य, "smtp_setup_connection() -> %d", (पूर्णांक)परिणाम);
  वापस परिणाम;
}

/*
 * SMTP protocol handler.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_smtp = {
  smtp_setup_connection,            /* setup_connection */
  smtp_do,                          /* do_it */
  smtp_done,                        /* done */
  ZERO_NULL,                        /* do_more */
  smtp_connect,                     /* connect_it */
  smtp_multi_statemach,             /* connecting */
  smtp_doing,                       /* doing */
  smtp_pollset,                     /* proto_pollset */
  smtp_pollset,                     /* doing_pollset */
  ZERO_NULL,                        /* domore_pollset */
  ZERO_NULL,                        /* perform_pollset */
  smtp_disconnect,                  /* disconnect */
  ZERO_NULL,                        /* write_resp */
  ZERO_NULL,                        /* write_resp_hd */
  ZERO_NULL,                        /* connection_is_dead */
  ZERO_NULL,                        /* attach connection */
  ZERO_NULL,                        /* follow */
};

#पूर्ण_अगर /* CURL_DISABLE_SMTP */
