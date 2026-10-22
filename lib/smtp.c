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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "smtp.h"

#ifndef CURL_DISABLE_SMTP

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "pingpong.h"
#અન્તર્ભાવ "mime.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "curl_gethostname.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "curl_sasl.h"
#અન્તર્ભાવ "idn.h"
#અન્તર્ભાવ "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
પ્રકારવ્યાખ્યા ગણના {
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
  SMTP_DATA,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_LAST         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
સંરચના smtp_conn {
  સંરચના pingpong pp;
  સંરચના SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  અક્ષર *domain;            /* Client address/name to send in the EHLO */
  BIT(ssldone);            /* Is connect() over SSL done? */
  BIT(tls_supported);      /* StartTLS capability supported by server */
  BIT(size_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  BIT(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  BIT(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
સંરચના SMTP {
  curl_pp_transfer transfer;
  અક્ષર *custom;            /* Custom Request */
  સંરચના curl_slist *rcpt; /* Recipient list */
  પૂર્ણાંક rcpt_last_error;     /* The last error received for RCPT TO command */
  size_t eob;              /* Number of bytes of the EOB (End Of Body) that
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
સ્થિર CURLcode smtp_parse_url_options(સંરચના connectdata *conn,
                                       સંરચના smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  અચલ અક્ષર *ptr = conn->options;

  જ્યારે(!result && ptr && *ptr) {
    અચલ અક્ષર *ચાવી = ptr;
    અચલ અક્ષર *કિંમત;

    જ્યારે(*ptr && *ptr != '=')
      ptr++;

    કિંમત = ptr + 1;

    જ્યારે(*ptr && *ptr != ';')
      ptr++;

    જો(curl_strnequal(ચાવી, "AUTH=", 5))
      result = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               કિંમત, ptr - કિંમત);
    નહીંતર
      result = CURLE_URL_MALFORMAT;

    જો(*ptr == ';')
      ptr++;
  }

  પાછા result;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
સ્થિર CURLcode smtp_parse_url_path(સંરચના Curl_easy *data,
                                    સંરચના smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  અચલ અક્ષર *path = &data->state.up.path[1]; /* skip leading path */
  અક્ષર localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  જો(!*path) {
    જો(!Curl_gethostname(localhost, કદ(localhost)))
      path = localhost;
    નહીંતર
      path = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  પાછા Curl_urldecode(path, 0, &smtpc->domain, ખાલી, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
સ્થિર CURLcode smtp_parse_custom_request(સંરચના Curl_easy *data,
                                          સંરચના SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  અચલ અક્ષર *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  જો(custom)
    result = Curl_urldecode(custom, 0, &smtp->custom, ખાલી, REJECT_CTRL);

  પાછા result;
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
સ્થિર CURLcode smtp_parse_address(સંરચના Curl_easy *data, અચલ અક્ષર *fqma,
                                   અક્ષર **address, સંરચના hostname *host,
                                   અચલ અક્ષર **suffix)
{
  CURLcode result = CURLE_OK;
  size_t length;
  અક્ષર *addressend;
  અક્ષર *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  જો(strpbrk(fqma, "\r\n")) {
    failf(data, "Refusing to send email address with a CR or LF");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  જો(!dup)
    પાછા CURLE_OUT_OF_MEMORY;

  જો(fqma[0] != '<') {
    length = strlen(dup);
    જો(length && dup[length - 1] == '>')
      dup[length - 1] = '\0';
  }
  નહીંતર {
    addressend = strrchr(dup, '>');
    જો(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  જો(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (શૂન્ય)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *address = dup;

  પાછા result;
}

સંરચના cr_eob_ctx {
  સંરચના Curl_creader super;
  સંરચના bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

સ્થિર CURLcode cr_eob_init(સંરચના Curl_easy *data,
                            સંરચના Curl_creader *reader)
{
  સંરચના cr_eob_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cr_eob_close(સંરચના Curl_easy *data, સંરચના Curl_creader *reader)
{
  સંરચના cr_eob_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#define SMTP_EOB          "\r\n.\r\n"
#define SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
સ્થિર CURLcode cr_eob_read(સંરચના Curl_easy *data,
                            સંરચના Curl_creader *reader,
                            અક્ષર *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  સંરચના cr_eob_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, i, start, n;
  bool eos;

  જો(!ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* Get more and convert it when needed */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(data, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (પૂર્ણાંક)result, nread, eos);
    જો(result)
      પાછા result;

    ctx->read_eos = eos;
    જો(nread) {
      જો(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        પાછા CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      માટે(i = start = 0; i < nread; ++i) {
        જો(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          જો(result)
            પાછા result;
          result = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          જો(result)
            પાછા result;
          ctx->n_eob = 0;
          start = i;
          જો(data->state.infilesize > 0)
            data->state.infilesize++;
        }

        જો(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        જો(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      જો(start < nread) {
        result = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        જો(result)
          પાછા result;
      }
    }
  }

  *peos = FALSE;

  જો(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    અચલ અક્ષર *eob = SMTP_EOB;
    CURL_TRC_SMTP(data, "auto-ending mail body with '\\r\\n.\\r\\n'");
    પસંદગી(ctx->n_eob) {
    સ્થિતિ 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      તોડો;
    સ્થિતિ 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      તોડો;
    મૂળભૂત:
      તોડો;
    }
    result = Curl_bufq_cwrite(&ctx->buf, eob, strlen(eob), &n);
    જો(result)
      પાછા result;
    ctx->processed_eos = TRUE;
  }

  જો(!Curl_bufq_is_empty(&ctx->buf)) {
    result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  નહીંતર
    *pnread = 0;

  જો(ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(data, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(data, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (પૂર્ણાંક)result, *pnread, *peos));
  પાછા result;
}

સ્થિર curl_off_t cr_eob_total_length(સંરચના Curl_easy *data,
                                      સંરચના Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (શૂન્ય)data;
  (શૂન્ય)reader;
  પાછા -1;
}

સ્થિર અચલ સંરચના Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_total_length,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  કદ(સંરચના cr_eob_ctx)
};

સ્થિર CURLcode cr_eob_add(સંરચના Curl_easy *data)
{
  સંરચના Curl_creader *reader = ખાલી;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_eob, CURL_CR_CONTENT_ENCODE);
  જો(!result)
    result = Curl_creader_add(data, reader);

  જો(result && reader)
    Curl_creader_free(data, reader);
  પાછા result;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
સ્થિર bool smtp_endofresp(સંરચના Curl_easy *data, સંરચના connectdata *conn,
                           અચલ અક્ષર *લીટી, size_t len, પૂર્ણાંક *resp)
{
  સંરચના smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool end = FALSE;
  (શૂન્ય)data;

  DEBUGASSERT(smtpc);
  જો(!smtpc)
    પાછા FALSE;

  /* Nothing for us */
  જો(len < 4 || !ISDIGIT(લીટી[0]) || !ISDIGIT(લીટી[1]) || !ISDIGIT(લીટી[2]))
    પાછા FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  જો(લીટી[3] == ' ' || len == 5) {
    અક્ષર tmpline[6];
    curl_off_t code;
    અચલ અક્ષર *p = tmpline;
    end = TRUE;
    memcpy(tmpline, લીટી, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    જો(curlx_str_સંખ્યા(&p, &code, len == 5 ? 99999 : 999))
      પાછા FALSE;
    *resp = (પૂર્ણાંક)code;

    /* Make sure real server never sends internal value */
    જો(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  નહીંતર જો(લીટી[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    end = TRUE;
    *resp = 1;  /* Internal response code */
  }

  પાછા end;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
સ્થિર CURLcode smtp_get_message(સંરચના Curl_easy *data, સંરચના bufref *out)
{
  સંરચના smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  અક્ષર *message;
  size_t len;

  જો(!smtpc)
    પાછા CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  જો(len > 4) {
    /* Find the start of the message */
    len -= 4;
    માટે(message += 4; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    જ્યારે(len--)
      જો(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        તોડો;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ખાલી);
  }
  નહીંતર
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ખાલી);

  પાછા CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
સ્થિર શૂન્ય smtp_state(સંરચના Curl_easy *data,
                       સંરચના smtp_conn *smtpc,
                       smtpstate newstate)
{
#ifdef CURLVERBOSE
  /* for debug purposes */
  સ્થિર અચલ અક્ષર * અચલ names[] = {
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

  જો(smtpc->state != newstate)
    CURL_TRC_SMTP(data, "state change from %s to %s",
                  names[smtpc->state], names[newstate]);
#નહીંતર
  (શૂન્ય)data;
#endif

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
સ્થિર CURLcode smtp_perform_ehlo(સંરચના Curl_easy *data,
                                  સંરચના smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "EHLO %s", smtpc->domain);

  જો(!result)
    smtp_state(data, smtpc, SMTP_EHLO);

  પાછા result;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
સ્થિર CURLcode smtp_perform_helo(સંરચના Curl_easy *data,
                                  સંરચના smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "HELO %s", smtpc->domain);

  જો(!result)
    smtp_state(data, smtpc, SMTP_HELO);

  પાછા result;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
સ્થિર CURLcode smtp_perform_starttls(સંરચના Curl_easy *data,
                                      સંરચના smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "STARTTLS");

  જો(!result)
    smtp_state(data, smtpc, SMTP_STARTTLS);

  પાછા result;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
સ્થિર CURLcode smtp_perform_upgrade_tls(સંરચના Curl_easy *data,
                                         સંરચના smtp_conn *smtpc)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  સંરચના connectdata *conn = data->conn;
  CURLcode result;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  જો(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    જો(result)
      જાઓ out;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (પૂર્ણાંક)result, ssldone));
  જો(!result && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    result = smtp_perform_ehlo(data, smtpc);
  }
out:
  પાછા result;
#નહીંતર
  (શૂન્ય)data;
  (શૂન્ય)smtpc;
  પાછા CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
સ્થિર CURLcode smtp_perform_auth(સંરચના Curl_easy *data,
                                  અચલ અક્ષર *mech,
                                  અચલ સંરચના bufref *initresp)
{
  CURLcode result = CURLE_OK;
  સંરચના smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  અચલ અક્ષર *ir = Curl_bufref_ptr(initresp);

  જો(!smtpc)
    પાછા CURLE_FAILED_INIT;

  જો(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  નહીંતર {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s", mech);
  }

  પાછા result;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
સ્થિર CURLcode smtp_continue_auth(સંરચના Curl_easy *data,
                                   અચલ અક્ષર *mech,
                                   અચલ સંરચના bufref *resp)
{
  સંરચના smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (શૂન્ય)mech;
  જો(!smtpc)
    પાછા CURLE_FAILED_INIT;
  પાછા Curl_pp_sendf(data, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
સ્થિર CURLcode smtp_cancel_auth(સંરચના Curl_easy *data, અચલ અક્ષર *mech)
{
  સંરચના smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (શૂન્ય)mech;
  જો(!smtpc)
    પાછા CURLE_FAILED_INIT;
  પાછા Curl_pp_sendf(data, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
સ્થિર CURLcode smtp_perform_authentication(સંરચના Curl_easy *data,
                                            સંરચના smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  જો(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, data)) {
    smtp_state(data, smtpc, SMTP_STOP);
    પાછા result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&smtpc->sasl, data, FALSE, &progress);

  જો(!result) {
    જો(progress == SASL_INPROGRESS)
      smtp_state(data, smtpc, SMTP_AUTH);
    નહીંતર
      result = Curl_sasl_is_blocked(&smtpc->sasl, data);
  }

  પાછા result;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
સ્થિર CURLcode smtp_perform_command(સંરચના Curl_easy *data,
                                     સંરચના smtp_conn *smtpc,
                                     સંરચના SMTP *smtp)
{
  CURLcode result = CURLE_OK;

  જો(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    જો(!smtp->custom || !smtp->custom[0]) {
      અક્ષર *address = ખાલી;
      સંરચના hostname host = { ખાલી, ખાલી, ખાલી, ખાલી };
      અચલ અક્ષર *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, smtp->rcpt->data,
                                  &address, &host, &suffix);
      જો(result)
        પાછા result;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(address) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      result = Curl_pp_sendf(data, &smtpc->pp, "VRFY %s%s%s%s",
                             address,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(address);
    }
    નહીંતર {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !strcmp(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      result = Curl_pp_sendf(data, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->data,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  નહીંતર
    /* Send the non-recipient based command such as HELP */
    result = Curl_pp_sendf(data, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  જો(!result)
    smtp_state(data, smtpc, SMTP_COMMAND);

  પાછા result;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
સ્થિર CURLcode smtp_perform_mail(સંરચના Curl_easy *data,
                                  સંરચના smtp_conn *smtpc,
                                  સંરચના SMTP *smtp)
{
  અક્ષર *from = ખાલી;
  અક્ષર *auth = ખાલી;
  અક્ષર *size = ખાલી;
  અચલ અક્ષર *str;
  CURLcode result = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_FROM);
  જો(str) {
    અક્ષર *address = ખાલી;
    સંરચના hostname host = { ખાલી, ખાલી, ખાલી, ખાલી };
    અચલ અક્ષર *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    result = smtp_parse_address(data, str, &address, &host, &suffix);
    જો(result)
      જાઓ out;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(address) ||
            !Curl_is_ASCII_name(host.name));

    જો(host.name) {
      from = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    નહીંતર
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", address, suffix);

    curlx_free(address);
  }
  નહીંતર
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  જો(!from) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_AUTH);
  જો(str && smtpc->sasl.authused) {
    જો(str[0] != '\0') {
      અક્ષર *address = ખાલી;
      સંરચના hostname host = { ખાલી, ખાલી, ખાલી, ખાલી };
      અચલ અક્ષર *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, str, &address, &host, &suffix);
      જો(result)
        જાઓ out;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      જો(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(address) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      જો(host.name) {
        auth = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      નહીંતર
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", address, suffix);
      curlx_free(address);
    }
    નહીંતર
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    જો(!auth) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  જો(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned પૂર્ણાંક)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, ખાલી,
                                       ખાલી, MIMESTRATEGY_MAIL);

    જો(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    જો(!result)
      result = Curl_creader_set_mime(data, postp);
    જો(result)
      જાઓ out;
    data->state.infilesize = Curl_creader_total_length(data);
  }
  નહીંતર
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    જો(result)
      જાઓ out;
  }

  /* Calculate the optional SIZE parameter */
  જો(smtpc->size_supported && data->state.infilesize > 0) {
    size = curl_maprintf("%" FMT_OFF_T, data->state.infilesize);

    જો(!size) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  જો(smtpc->utf8_supported && !utf8) {
    સંરચના curl_slist *rcpt = smtp->rcpt;

    જ્યારે(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      જો(!Curl_is_ASCII_name(rcpt->data))
        utf8 = TRUE;

      rcpt = rcpt->next;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  result = cr_eob_add(data);
  જો(result)
    જાઓ out;

  /* Send the MAIL command */
  result = Curl_pp_sendf(data, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         size ? " SIZE=" : "", /* Optional on SIZE support  */
                         size ? size : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

out:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(size);

  જો(!result)
    smtp_state(data, smtpc, SMTP_MAIL);

  પાછા result;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
સ્થિર CURLcode smtp_perform_rcpt_to(સંરચના Curl_easy *data,
                                     સંરચના smtp_conn *smtpc,
                                     સંરચના SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  અક્ષર *address = ખાલી;
  સંરચના hostname host = { ખાલી, ખાલી, ખાલી, ખાલી };
  અચલ અક્ષર *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  result = smtp_parse_address(data, smtp->rcpt->data,
                              &address, &host, &suffix);
  જો(result)
    પાછા result;

  /* Send the RCPT TO command */
  જો(host.name)
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           address, host.name, suffix);
  નહીંતર
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s>%s",
                           address, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(address);

  જો(!result)
    smtp_state(data, smtpc, SMTP_RCPT);

  પાછા result;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
સ્થિર CURLcode smtp_perform_quit(સંરચના Curl_easy *data,
                                  સંરચના smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "QUIT");

  જો(!result)
    smtp_state(data, smtpc, SMTP_QUIT);

  પાછા result;
}

/* For the initial server greeting */
સ્થિર CURLcode smtp_state_servergreet_resp(સંરચના Curl_easy *data,
                                            સંરચના smtp_conn *smtpc,
                                            પૂર્ણાંક smtpcode,
                                            smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (શૂન્ય)instate;

  જો(smtpcode / 100 != 2) {
    failf(data, "Got unexpected smtp-server response: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  નહીંતર
    result = smtp_perform_ehlo(data, smtpc);

  પાછા result;
}

/* For STARTTLS responses */
સ્થિર CURLcode smtp_state_starttls_resp(સંરચના Curl_easy *data,
                                         સંરચના smtp_conn *smtpc,
                                         પૂર્ણાંક smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (શૂન્ય)instate;

  /* Pipelining in response is forbidden. */
  જો(smtpc->pp.overflow)
    પાછા CURLE_WEIRD_SERVER_REPLY;

  જો(smtpcode != 220) {
    જો(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied, code %d", smtpcode);
      result = CURLE_USE_SSL_FAILED;
    }
    નહીંતર
      result = smtp_perform_authentication(data, smtpc);
  }
  નહીંતર
    smtp_state(data, smtpc, SMTP_UPGRADETLS);

  પાછા result;
}

/* For EHLO responses */
સ્થિર CURLcode smtp_state_ehlo_resp(સંરચના Curl_easy *data,
                                     સંરચના smtp_conn *smtpc,
                                     પૂર્ણાંક smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  અચલ અક્ષર *લીટી = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (શૂન્ય)instate;

  જો(smtpcode / 100 != 2 && smtpcode != 1) {
    જો(data->set.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(data->conn, FIRSTSOCKET))
      result = smtp_perform_helo(data, smtpc);
    નહીંતર {
      failf(data, "Remote access denied: %d", smtpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  નહીંતર જો(len >= 4) {
    લીટી += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    જો(len >= 8 && curl_strnequal(લીટી, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    નહીંતર જો(len >= 4 && curl_strnequal(લીટી, "SIZE", 4))
      smtpc->size_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    નહીંતર જો(len >= 8 && curl_strnequal(લીટી, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    નહીંતર જો(len >= 5 && curl_strnequal(લીટી, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      લીટી += 5;
      len -= 5;

      /* Loop through the data line */
      માટે(;;) {
        size_t llen;
        size_t wordlen;
        unsigned short mechbit;

        જ્યારે(len && (ISBLANK(*લીટી) || ISNEWLINE(*લીટી))) {
          લીટી++;
          len--;
        }

        જો(!len)
          તોડો;

        /* Extract the word */
        માટે(wordlen = 0; wordlen < len && !ISBLANK(લીટી[wordlen]) &&
              !ISNEWLINE(લીટી[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(લીટી, wordlen, &llen);
        જો(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        લીટી += wordlen;
        len -= wordlen;
      }
    }

    જો(smtpcode != 1) {
      જો(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        જો(smtpc->tls_supported)
          /* Switch to TLS connection now */
          result = smtp_perform_starttls(data, smtpc);
        નહીંતર જો(data->set.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          result = smtp_perform_authentication(data, smtpc);
        નહીંતર {
          failf(data, "STARTTLS not supported.");
          result = CURLE_USE_SSL_FAILED;
        }
      }
      નહીંતર
        result = smtp_perform_authentication(data, smtpc);
    }
  }
  નહીંતર {
    failf(data, "Unexpectedly short EHLO response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  પાછા result;
}

/* For HELO responses */
સ્થિર CURLcode smtp_state_helo_resp(સંરચના Curl_easy *data,
                                     સંરચના smtp_conn *smtpc,
                                     પૂર્ણાંક smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (શૂન્ય)instate;

  જો(smtpcode / 100 != 2) {
    failf(data, "Remote access denied: %d", smtpcode);
    result = CURLE_REMOTE_ACCESS_DENIED;
  }
  નહીંતર
    /* End of connect phase */
    smtp_state(data, smtpc, SMTP_STOP);

  પાછા result;
}

/* For SASL authentication responses */
સ્થિર CURLcode smtp_state_auth_resp(સંરચના Curl_easy *data,
                                     સંરચના smtp_conn *smtpc,
                                     પૂર્ણાંક smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (શૂન્ય)instate;

  result = Curl_sasl_continue(&smtpc->sasl, data, smtpcode, &progress);
  જો(!result)
    પસંદગી(progress) {
    સ્થિતિ SASL_DONE:
      smtp_state(data, smtpc, SMTP_STOP);  /* Authenticated */
      તોડો;
    સ્થિતિ SASL_IDLE:            /* No mechanism left after cancellation */
      failf(data, "Authentication cancelled");
      result = CURLE_LOGIN_DENIED;
      તોડો;
    મૂળભૂત:
      તોડો;
    }

  પાછા result;
}

/* For command responses */
સ્થિર CURLcode smtp_state_command_resp(સંરચના Curl_easy *data,
                                        સંરચના smtp_conn *smtpc,
                                        સંરચના SMTP *smtp,
                                        પૂર્ણાંક smtpcode,
                                        smtpstate instate)
{
  CURLcode result = CURLE_OK;
  અચલ અક્ષર *લીટી = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (શૂન્ય)instate;

  જો((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(data, "Command failed: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  નહીંતર {
    જો(!data->req.no_body)
      result = Curl_client_write(data, CLIENTWRITE_BODY, લીટી, len);

    જો(!result && (smtpcode != 1)) {
      જો(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->next;

        જો(smtp->rcpt) {
          /* Send the next command */
          result = smtp_perform_command(data, smtpc, smtp);
        }
        નહીંતર
          /* End of DO phase */
          smtp_state(data, smtpc, SMTP_STOP);
      }
      નહીંતર
        /* End of DO phase */
        smtp_state(data, smtpc, SMTP_STOP);
    }
  }

  પાછા result;
}

/* For MAIL responses */
સ્થિર CURLcode smtp_state_mail_resp(સંરચના Curl_easy *data,
                                     સંરચના smtp_conn *smtpc,
                                     સંરચના SMTP *smtp,
                                     પૂર્ણાંક smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (શૂન્ય)instate;

  જો(smtpcode / 100 != 2) {
    failf(data, "MAIL failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  નહીંતર
    /* Start the RCPT TO command */
    result = smtp_perform_rcpt_to(data, smtpc, smtp);

  પાછા result;
}

/* For RCPT responses */
સ્થિર CURLcode smtp_state_rcpt_resp(સંરચના Curl_easy *data,
                                     સંરચના smtp_conn *smtpc,
                                     સંરચના SMTP *smtp,
                                     પૂર્ણાંક smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (શૂન્ય)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !data->set.mail_rcpt_allowfails);

  જો(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_last_error = smtpcode;

    જો(is_smtp_blocking_err) {
      failf(data, "RCPT failed: %d", smtpcode);
      result = CURLE_SEND_ERROR;
    }
  }
  નહીંતર {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  જો(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->next;

    જો(smtp->rcpt)
      /* Send the next RCPT TO command */
      result = smtp_perform_rcpt_to(data, smtpc, smtp);
    નહીંતર {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      જો(!smtp->rcpt_had_ok) {
        failf(data, "RCPT failed: %d (last error)", smtp->rcpt_last_error);
        result = CURLE_SEND_ERROR;
      }
      નહીંતર {
        /* Send the DATA command */
        result = Curl_pp_sendf(data, &smtpc->pp, "%s", "DATA");

        જો(!result)
          smtp_state(data, smtpc, SMTP_DATA);
      }
    }
  }

  પાછા result;
}

/* For DATA response */
સ્થિર CURLcode smtp_state_data_resp(સંરચના Curl_easy *data,
                                     સંરચના smtp_conn *smtpc,
                                     પૂર્ણાંક smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (શૂન્ય)instate;

  જો(smtpcode != 354) {
    failf(data, "DATA failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  નહીંતર {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* SMTP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(data, smtpc, SMTP_STOP);
  }

  પાછા result;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
સ્થિર CURLcode smtp_state_postdata_resp(સંરચના Curl_easy *data,
                                         સંરચના smtp_conn *smtpc,
                                         પૂર્ણાંક smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;

  (શૂન્ય)instate;

  જો(smtpcode != 250)
    result = CURLE_WEIRD_SERVER_REPLY;

  /* End of DONE phase */
  smtp_state(data, smtpc, SMTP_STOP);

  પાછા result;
}

સ્થિર CURLcode smtp_pp_statemachine(સંરચના Curl_easy *data,
                                     સંરચના connectdata *conn)
{
  CURLcode result = CURLE_OK;
  પૂર્ણાંક smtpcode;
  સંરચના smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  સંરચના SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  size_t nread = 0;

  જો(!smtpc || !smtp)
    પાછા CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  જો(smtpc->state == SMTP_UPGRADETLS) {
    result = smtp_perform_upgrade_tls(data, smtpc);
    જો(result || (smtpc->state == SMTP_UPGRADETLS))
      પાછા result;
  }

  /* Flush any data that needs to be sent */
  જો(smtpc->pp.sendleft)
    પાછા Curl_pp_flushsend(data, &smtpc->pp);

  કરો {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    જો(result)
      પાછા result;

    /* Store the latest response for later retrieval if necessary */
    જો(smtpc->state != SMTP_QUIT && smtpcode != 1)
      data->info.httpcode = smtpcode;

    જો(!smtpcode)
      તોડો;

    /* We have now received a full SMTP server response */
    પસંદગી(smtpc->state) {
    સ્થિતિ SMTP_SERVERGREET:
      result = smtp_state_servergreet_resp(data, smtpc,
                                           smtpcode, smtpc->state);
      તોડો;

    સ્થિતિ SMTP_EHLO:
      result = smtp_state_ehlo_resp(data, smtpc, smtpcode, smtpc->state);
      તોડો;

    સ્થિતિ SMTP_HELO:
      result = smtp_state_helo_resp(data, smtpc, smtpcode, smtpc->state);
      તોડો;

    સ્થિતિ SMTP_STARTTLS:
      result = smtp_state_starttls_resp(data, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      જો(!result && (smtpc->state == SMTP_UPGRADETLS))
        જાઓ upgrade_tls;
      તોડો;

    સ્થિતિ SMTP_AUTH:
      result = smtp_state_auth_resp(data, smtpc, smtpcode, smtpc->state);
      તોડો;

    સ્થિતિ SMTP_COMMAND:
      result = smtp_state_command_resp(data, smtpc, smtp,
                                       smtpcode, smtpc->state);
      તોડો;

    સ્થિતિ SMTP_MAIL:
      result = smtp_state_mail_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      તોડો;

    સ્થિતિ SMTP_RCPT:
      result = smtp_state_rcpt_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      તોડો;

    સ્થિતિ SMTP_DATA:
      result = smtp_state_data_resp(data, smtpc, smtpcode, smtpc->state);
      તોડો;

    સ્થિતિ SMTP_POSTDATA:
      result = smtp_state_postdata_resp(data, smtpc, smtpcode, smtpc->state);
      તોડો;

    સ્થિતિ SMTP_QUIT:
    મૂળભૂત:
      /* internal error */
      smtp_state(data, smtpc, SMTP_STOP);
      તોડો;
    }
  } જ્યારે(!result && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  પાછા result;
}

/* Called repeatedly until done from multi.c */
સ્થિર CURLcode smtp_multi_statemach(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  સંરચના smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  *done = FALSE;
  જો(!smtpc)
    પાછા CURLE_FAILED_INIT;

  result = Curl_pp_statemach(data, &smtpc->pp, FALSE, FALSE);
  *done = (smtpc->state == SMTP_STOP);
  પાછા result;
}

સ્થિર CURLcode smtp_block_statemach(સંરચના Curl_easy *data,
                                     સંરચના smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  જ્યારે(smtpc->state != SMTP_STOP && !result)
    result = Curl_pp_statemach(data, &smtpc->pp, TRUE, disconnecting);

  પાછા result;
}

/* For the SMTP "protocol connect" and "doing" phases only */
સ્થિર CURLcode smtp_pollset(સંરચના Curl_easy *data,
                             સંરચના easy_pollset *ps)
{
  સંરચના smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  પાછા smtpc ? Curl_pp_pollset(data, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
સ્થિર અચલ સંરચના SASLproto saslsmtp = {
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
સ્થિર CURLcode smtp_connect(સંરચના Curl_easy *data, bool *done)
{
  સંરચના smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  જો(!smtpc)
    પાછા CURLE_FAILED_INIT;

  PINGPONG_SETUP(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, data, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = smtp_parse_url_options(data->conn, smtpc);
  જો(result)
    પાછા result;

  /* Parse the URL path */
  result = smtp_parse_url_path(data, smtpc);
  જો(result)
    પાછા result;

  /* Start off waiting for the server greeting response */
  smtp_state(data, smtpc, SMTP_SERVERGREET);

  result = smtp_multi_statemach(data, done);

  પાછા result;
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
સ્થિર CURLcode smtp_done(સંરચના Curl_easy *data, CURLcode status,
                          bool premature)
{
  સંરચના smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);

  (શૂન્ય)premature;

  જો(!smtpc)
    પાછા CURLE_FAILED_INIT;
  જો(!smtp)
    પાછા CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  જો(status) {
    CURL_TRC_M(data, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  નહીંતર જો(!data->set.connect_only && data->set.mail_rcpt &&
          (data->state.upload || IS_MIME_POST(data))) {

    smtp_state(data, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    result = smtp_block_statemach(data, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(data, "smtp_done(status=%d, premature=%d) -> %d",
                (પૂર્ણાંક)status, premature, (પૂર્ણાંક)result);
  પાછા result;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
સ્થિર CURLcode smtp_perform(સંરચના Curl_easy *data,
                             સંરચના smtp_conn *smtpc,
                             સંરચના SMTP *smtp,
                             bool *connected,
                             bool *dophase_done)
{
  /* This is SMTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_SMTP(data, "smtp_perform(), start");

  જો(data->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = data->set.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_last_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  જો((data->state.upload || IS_MIME_POST(data)) && data->set.mail_rcpt)
    /* MAIL transfer */
    result = smtp_perform_mail(data, smtpc, smtp);
  નહીંતર
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    result = smtp_perform_command(data, smtpc, smtp);

  જો(result)
    જાઓ out;

  /* Run the state-machine */
  result = smtp_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

out:
  CURL_TRC_SMTP(data, "smtp_perform() -> %d, connected=%d, done=%d",
                (પૂર્ણાંક)result, *connected, *dophase_done);
  પાછા result;
}

/* Call this when the DO phase has completed */
સ્થિર CURLcode smtp_dophase_done(સંરચના Curl_easy *data,
                                  સંરચના SMTP *smtp,
                                  bool connected)
{
  (શૂન્ય)connected;

  જો(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  પાછા CURLE_OK;
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
સ્થિર CURLcode smtp_regular_transfer(સંરચના Curl_easy *data,
                                      સંરચના smtp_conn *smtpc,
                                      સંરચના SMTP *smtp,
                                      bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  data->req.size = -1;

  /* Set the progress data */
  Curl_pgrsReset(data);

  /* Carry out the perform */
  result = smtp_perform(data, smtpc, smtp, &connected, dophase_done);

  /* Perform post DO phase operations if necessary */
  જો(!result && *dophase_done)
    result = smtp_dophase_done(data, smtp, connected);

  CURL_TRC_SMTP(data, "smtp_regular_transfer() -> %d, done=%d",
                (પૂર્ણાંક)result, *dophase_done);
  પાછા result;
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
સ્થિર CURLcode smtp_do(સંરચના Curl_easy *data, bool *done)
{
  સંરચના smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  સંરચના SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  *done = FALSE; /* default to false */
  જો(!smtpc || !smtp)
    પાછા CURLE_FAILED_INIT;

  /* Parse the custom request */
  result = smtp_parse_custom_request(data, smtp);
  જો(result)
    પાછા result;

  result = smtp_regular_transfer(data, smtpc, smtp, done);
  CURL_TRC_SMTP(data, "smtp_do() -> %d, done=%d", (પૂર્ણાંક)result, *done);
  પાછા result;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
સ્થિર CURLcode smtp_disconnect(સંરચના Curl_easy *data,
                                સંરચના connectdata *conn,
                                bool dead_connection)
{
  સંરચના smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  જો(!smtpc)
    પાછા CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  જો(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &smtpc->pp) &&
     !smtp_perform_quit(data, smtpc))
    (શૂન્ય)smtp_block_statemach(data, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(data, "smtp_disconnect(), finished");
  પાછા CURLE_OK;
}

/* Called from multi.c while DOing */
સ્થિર CURLcode smtp_doing(સંરચના Curl_easy *data, bool *dophase_done)
{
  સંરચના SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result;

  જો(!smtp)
    પાછા CURLE_FAILED_INIT;
  result = smtp_multi_statemach(data, dophase_done);
  જો(result)
    DEBUGF(infof(data, "DO phase failed"));
  નહીંતર જો(*dophase_done) {
    result = smtp_dophase_done(data, smtp, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  CURL_TRC_SMTP(data, "smtp_doing() -> %d, done=%d", (પૂર્ણાંક)result,
                *dophase_done);
  પાછા result;
}

સ્થિર શૂન્ય smtp_easy_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના SMTP *smtp = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  curlx_free(smtp);
}

સ્થિર શૂન્ય smtp_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના smtp_conn *smtpc = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

સ્થિર CURLcode smtp_setup_connection(સંરચના Curl_easy *data,
                                      સંરચના connectdata *conn)
{
  સંરચના smtp_conn *smtpc;
  સંરચના SMTP *smtp;
  CURLcode result = CURLE_OK;

  smtpc = curlx_calloc(1, કદ(*smtpc));
  જો(!smtpc ||
     Curl_conn_meta_set(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  smtp = curlx_calloc(1, કદ(*smtp));
  જો(!smtp ||
     Curl_meta_set(data, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    result = CURLE_OUT_OF_MEMORY;

out:
  CURL_TRC_SMTP(data, "smtp_setup_connection() -> %d", (પૂર્ણાંક)result);
  પાછા result;
}

/*
 * SMTP protocol handler.
 */
અચલ સંરચના Curl_protocol Curl_protocol_smtp = {
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

#endif /* CURL_DISABLE_SMTP */
