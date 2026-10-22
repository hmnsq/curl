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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "smtp.h"

#ifndef CURL_DISABLE_SMTP

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "pingpong.h"
#अन्तर्भाव "mime.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "curl_gethostname.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curl_sasl.h"
#अन्तर्भाव "idn.h"
#अन्तर्भाव "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
प्रकारनिर्देश गणना {
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
संरचना smtp_conn {
  संरचना pingpong pp;
  संरचना SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  वर्ण *domain;            /* Client address/name to send in the EHLO */
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
संरचना SMTP {
  curl_pp_transfer transfer;
  वर्ण *custom;            /* Custom Request */
  संरचना curl_slist *rcpt; /* Recipient list */
  पूर्णाङ्क rcpt_last_error;     /* The last error received for RCPT TO command */
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
स्थिर CURLcode smtp_parse_url_options(संरचना connectdata *conn,
                                       संरचना smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *ptr = conn->options;

  जाव(!result && ptr && *ptr) {
    ध्रुव वर्ण *कुञ्चिका = ptr;
    ध्रुव वर्ण *मानम्;

    जाव(*ptr && *ptr != '=')
      ptr++;

    मानम् = ptr + 1;

    जाव(*ptr && *ptr != ';')
      ptr++;

    जइ(curl_strnequal(कुञ्चिका, "AUTH=", 5))
      result = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               मानम्, ptr - मानम्);
    अण्णहा
      result = CURLE_URL_MALFORMAT;

    जइ(*ptr == ';')
      ptr++;
  }

  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
स्थिर CURLcode smtp_parse_url_path(संरचना Curl_easy *data,
                                    संरचना smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ध्रुव वर्ण *path = &data->state.up.path[1]; /* skip leading path */
  वर्ण localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  जइ(!*path) {
    जइ(!Curl_gethostname(localhost, परिमाण(localhost)))
      path = localhost;
    अण्णहा
      path = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  प्रत्यागम Curl_urldecode(path, 0, &smtpc->domain, रिक्त, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
स्थिर CURLcode smtp_parse_custom_request(संरचना Curl_easy *data,
                                          संरचना SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  जइ(custom)
    result = Curl_urldecode(custom, 0, &smtp->custom, रिक्त, REJECT_CTRL);

  प्रत्यागम result;
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
स्थिर CURLcode smtp_parse_address(संरचना Curl_easy *data, ध्रुव वर्ण *fqma,
                                   वर्ण **address, संरचना hostname *host,
                                   ध्रुव वर्ण **suffix)
{
  CURLcode result = CURLE_OK;
  size_t length;
  वर्ण *addressend;
  वर्ण *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  जइ(strpbrk(fqma, "\r\n")) {
    failf(data, "Refusing to send email address with a CR or LF");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  जइ(!dup)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  जइ(fqma[0] != '<') {
    length = strlen(dup);
    जइ(length && dup[length - 1] == '>')
      dup[length - 1] = '\0';
  }
  अण्णहा {
    addressend = strrchr(dup, '>');
    जइ(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  जइ(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (शून्य)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *address = dup;

  प्रत्यागम result;
}

संरचना cr_eob_ctx {
  संरचना Curl_creader super;
  संरचना bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

स्थिर CURLcode cr_eob_init(संरचना Curl_easy *data,
                            संरचना Curl_creader *reader)
{
  संरचना cr_eob_ctx *ctx = reader->ctx;
  (शून्य)data;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cr_eob_close(संरचना Curl_easy *data, संरचना Curl_creader *reader)
{
  संरचना cr_eob_ctx *ctx = reader->ctx;
  (शून्य)data;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#define SMTP_EOB          "\r\n.\r\n"
#define SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
स्थिर CURLcode cr_eob_read(संरचना Curl_easy *data,
                            संरचना Curl_creader *reader,
                            वर्ण *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  संरचना cr_eob_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, i, start, n;
  bool eos;

  जइ(!ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* Get more and convert it when needed */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(data, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (पूर्णाङ्क)result, nread, eos);
    जइ(result)
      प्रत्यागम result;

    ctx->read_eos = eos;
    जइ(nread) {
      जइ(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        प्रत्यागम CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      प्रति(i = start = 0; i < nread; ++i) {
        जइ(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          जइ(result)
            प्रत्यागम result;
          result = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          जइ(result)
            प्रत्यागम result;
          ctx->n_eob = 0;
          start = i;
          जइ(data->state.infilesize > 0)
            data->state.infilesize++;
        }

        जइ(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        जइ(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      जइ(start < nread) {
        result = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        जइ(result)
          प्रत्यागम result;
      }
    }
  }

  *peos = FALSE;

  जइ(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ध्रुव वर्ण *eob = SMTP_EOB;
    CURL_TRC_SMTP(data, "auto-ending mail body with '\\r\\n.\\r\\n'");
    विकल्प(ctx->n_eob) {
    पक्ष 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      भङ्ग;
    पक्ष 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }
    result = Curl_bufq_cwrite(&ctx->buf, eob, strlen(eob), &n);
    जइ(result)
      प्रत्यागम result;
    ctx->processed_eos = TRUE;
  }

  जइ(!Curl_bufq_is_empty(&ctx->buf)) {
    result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  अण्णहा
    *pnread = 0;

  जइ(ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(data, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(data, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (पूर्णाङ्क)result, *pnread, *peos));
  प्रत्यागम result;
}

स्थिर curl_off_t cr_eob_total_length(संरचना Curl_easy *data,
                                      संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)data;
  (शून्य)reader;
  प्रत्यागम -1;
}

स्थिर ध्रुव संरचना Curl_crtype cr_eob = {
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
  परिमाण(संरचना cr_eob_ctx)
};

स्थिर CURLcode cr_eob_add(संरचना Curl_easy *data)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_eob, CURL_CR_CONTENT_ENCODE);
  जइ(!result)
    result = Curl_creader_add(data, reader);

  जइ(result && reader)
    Curl_creader_free(data, reader);
  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
स्थिर bool smtp_endofresp(संरचना Curl_easy *data, संरचना connectdata *conn,
                           ध्रुव वर्ण *पङ्क्तिः, size_t len, पूर्णाङ्क *resp)
{
  संरचना smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool end = FALSE;
  (शून्य)data;

  DEBUGASSERT(smtpc);
  जइ(!smtpc)
    प्रत्यागम FALSE;

  /* Nothing for us */
  जइ(len < 4 || !ISDIGIT(पङ्क्तिः[0]) || !ISDIGIT(पङ्क्तिः[1]) || !ISDIGIT(पङ्क्तिः[2]))
    प्रत्यागम FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  जइ(पङ्क्तिः[3] == ' ' || len == 5) {
    वर्ण tmpline[6];
    curl_off_t code;
    ध्रुव वर्ण *p = tmpline;
    end = TRUE;
    memcpy(tmpline, पङ्क्तिः, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    जइ(curlx_str_सङ्ख्या(&p, &code, len == 5 ? 99999 : 999))
      प्रत्यागम FALSE;
    *resp = (पूर्णाङ्क)code;

    /* Make sure real server never sends internal value */
    जइ(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  अण्णहा जइ(पङ्क्तिः[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    end = TRUE;
    *resp = 1;  /* Internal response code */
  }

  प्रत्यागम end;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
स्थिर CURLcode smtp_get_message(संरचना Curl_easy *data, संरचना bufref *out)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  वर्ण *message;
  size_t len;

  जइ(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  जइ(len > 4) {
    /* Find the start of the message */
    len -= 4;
    प्रति(message += 4; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    जाव(len--)
      जइ(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        भङ्ग;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, रिक्त);
  }
  अण्णहा
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, रिक्त);

  प्रत्यागम CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
स्थिर शून्य smtp_state(संरचना Curl_easy *data,
                       संरचना smtp_conn *smtpc,
                       smtpstate newstate)
{
#ifdef CURLVERBOSE
  /* for debug purposes */
  स्थिर ध्रुव वर्ण * ध्रुव names[] = {
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

  जइ(smtpc->state != newstate)
    CURL_TRC_SMTP(data, "state change from %s to %s",
                  names[smtpc->state], names[newstate]);
#अण्णहा
  (शून्य)data;
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
स्थिर CURLcode smtp_perform_ehlo(संरचना Curl_easy *data,
                                  संरचना smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "EHLO %s", smtpc->domain);

  जइ(!result)
    smtp_state(data, smtpc, SMTP_EHLO);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
स्थिर CURLcode smtp_perform_helo(संरचना Curl_easy *data,
                                  संरचना smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "HELO %s", smtpc->domain);

  जइ(!result)
    smtp_state(data, smtpc, SMTP_HELO);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
स्थिर CURLcode smtp_perform_starttls(संरचना Curl_easy *data,
                                      संरचना smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "STARTTLS");

  जइ(!result)
    smtp_state(data, smtpc, SMTP_STARTTLS);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
स्थिर CURLcode smtp_perform_upgrade_tls(संरचना Curl_easy *data,
                                         संरचना smtp_conn *smtpc)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  संरचना connectdata *conn = data->conn;
  CURLcode result;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  जइ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    जइ(result)
      गच्छ out;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (पूर्णाङ्क)result, ssldone));
  जइ(!result && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    result = smtp_perform_ehlo(data, smtpc);
  }
out:
  प्रत्यागम result;
#अण्णहा
  (शून्य)data;
  (शून्य)smtpc;
  प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
स्थिर CURLcode smtp_perform_auth(संरचना Curl_easy *data,
                                  ध्रुव वर्ण *mech,
                                  ध्रुव संरचना bufref *initresp)
{
  CURLcode result = CURLE_OK;
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ध्रुव वर्ण *ir = Curl_bufref_ptr(initresp);

  जइ(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;

  जइ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  अण्णहा {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s", mech);
  }

  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
स्थिर CURLcode smtp_continue_auth(संरचना Curl_easy *data,
                                   ध्रुव वर्ण *mech,
                                   ध्रुव संरचना bufref *resp)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (शून्य)mech;
  जइ(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_pp_sendf(data, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
स्थिर CURLcode smtp_cancel_auth(संरचना Curl_easy *data, ध्रुव वर्ण *mech)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (शून्य)mech;
  जइ(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_pp_sendf(data, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
स्थिर CURLcode smtp_perform_authentication(संरचना Curl_easy *data,
                                            संरचना smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  जइ(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, data)) {
    smtp_state(data, smtpc, SMTP_STOP);
    प्रत्यागम result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&smtpc->sasl, data, FALSE, &progress);

  जइ(!result) {
    जइ(progress == SASL_INPROGRESS)
      smtp_state(data, smtpc, SMTP_AUTH);
    अण्णहा
      result = Curl_sasl_is_blocked(&smtpc->sasl, data);
  }

  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
स्थिर CURLcode smtp_perform_command(संरचना Curl_easy *data,
                                     संरचना smtp_conn *smtpc,
                                     संरचना SMTP *smtp)
{
  CURLcode result = CURLE_OK;

  जइ(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    जइ(!smtp->custom || !smtp->custom[0]) {
      वर्ण *address = रिक्त;
      संरचना hostname host = { रिक्त, रिक्त, रिक्त, रिक्त };
      ध्रुव वर्ण *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, smtp->rcpt->data,
                                  &address, &host, &suffix);
      जइ(result)
        प्रत्यागम result;

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
    अण्णहा {
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
  अण्णहा
    /* Send the non-recipient based command such as HELP */
    result = Curl_pp_sendf(data, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  जइ(!result)
    smtp_state(data, smtpc, SMTP_COMMAND);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
स्थिर CURLcode smtp_perform_mail(संरचना Curl_easy *data,
                                  संरचना smtp_conn *smtpc,
                                  संरचना SMTP *smtp)
{
  वर्ण *from = रिक्त;
  वर्ण *auth = रिक्त;
  वर्ण *size = रिक्त;
  ध्रुव वर्ण *str;
  CURLcode result = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_FROM);
  जइ(str) {
    वर्ण *address = रिक्त;
    संरचना hostname host = { रिक्त, रिक्त, रिक्त, रिक्त };
    ध्रुव वर्ण *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    result = smtp_parse_address(data, str, &address, &host, &suffix);
    जइ(result)
      गच्छ out;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(address) ||
            !Curl_is_ASCII_name(host.name));

    जइ(host.name) {
      from = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    अण्णहा
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", address, suffix);

    curlx_free(address);
  }
  अण्णहा
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  जइ(!from) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_AUTH);
  जइ(str && smtpc->sasl.authused) {
    जइ(str[0] != '\0') {
      वर्ण *address = रिक्त;
      संरचना hostname host = { रिक्त, रिक्त, रिक्त, रिक्त };
      ध्रुव वर्ण *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, str, &address, &host, &suffix);
      जइ(result)
        गच्छ out;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      जइ(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(address) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      जइ(host.name) {
        auth = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      अण्णहा
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", address, suffix);
      curlx_free(address);
    }
    अण्णहा
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    जइ(!auth) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  जइ(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned पूर्णाङ्क)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, रिक्त,
                                       रिक्त, MIMESTRATEGY_MAIL);

    जइ(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    जइ(!result)
      result = Curl_creader_set_mime(data, postp);
    जइ(result)
      गच्छ out;
    data->state.infilesize = Curl_creader_total_length(data);
  }
  अण्णहा
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    जइ(result)
      गच्छ out;
  }

  /* Calculate the optional SIZE parameter */
  जइ(smtpc->size_supported && data->state.infilesize > 0) {
    size = curl_maprintf("%" FMT_OFF_T, data->state.infilesize);

    जइ(!size) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  जइ(smtpc->utf8_supported && !utf8) {
    संरचना curl_slist *rcpt = smtp->rcpt;

    जाव(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      जइ(!Curl_is_ASCII_name(rcpt->data))
        utf8 = TRUE;

      rcpt = rcpt->next;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  result = cr_eob_add(data);
  जइ(result)
    गच्छ out;

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

  जइ(!result)
    smtp_state(data, smtpc, SMTP_MAIL);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
स्थिर CURLcode smtp_perform_rcpt_to(संरचना Curl_easy *data,
                                     संरचना smtp_conn *smtpc,
                                     संरचना SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  वर्ण *address = रिक्त;
  संरचना hostname host = { रिक्त, रिक्त, रिक्त, रिक्त };
  ध्रुव वर्ण *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  result = smtp_parse_address(data, smtp->rcpt->data,
                              &address, &host, &suffix);
  जइ(result)
    प्रत्यागम result;

  /* Send the RCPT TO command */
  जइ(host.name)
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           address, host.name, suffix);
  अण्णहा
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s>%s",
                           address, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(address);

  जइ(!result)
    smtp_state(data, smtpc, SMTP_RCPT);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
स्थिर CURLcode smtp_perform_quit(संरचना Curl_easy *data,
                                  संरचना smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "QUIT");

  जइ(!result)
    smtp_state(data, smtpc, SMTP_QUIT);

  प्रत्यागम result;
}

/* For the initial server greeting */
स्थिर CURLcode smtp_state_servergreet_resp(संरचना Curl_easy *data,
                                            संरचना smtp_conn *smtpc,
                                            पूर्णाङ्क smtpcode,
                                            smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  जइ(smtpcode / 100 != 2) {
    failf(data, "Got unexpected smtp-server response: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  अण्णहा
    result = smtp_perform_ehlo(data, smtpc);

  प्रत्यागम result;
}

/* For STARTTLS responses */
स्थिर CURLcode smtp_state_starttls_resp(संरचना Curl_easy *data,
                                         संरचना smtp_conn *smtpc,
                                         पूर्णाङ्क smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  /* Pipelining in response is forbidden. */
  जइ(smtpc->pp.overflow)
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;

  जइ(smtpcode != 220) {
    जइ(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied, code %d", smtpcode);
      result = CURLE_USE_SSL_FAILED;
    }
    अण्णहा
      result = smtp_perform_authentication(data, smtpc);
  }
  अण्णहा
    smtp_state(data, smtpc, SMTP_UPGRADETLS);

  प्रत्यागम result;
}

/* For EHLO responses */
स्थिर CURLcode smtp_state_ehlo_resp(संरचना Curl_easy *data,
                                     संरचना smtp_conn *smtpc,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (शून्य)instate;

  जइ(smtpcode / 100 != 2 && smtpcode != 1) {
    जइ(data->set.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(data->conn, FIRSTSOCKET))
      result = smtp_perform_helo(data, smtpc);
    अण्णहा {
      failf(data, "Remote access denied: %d", smtpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  अण्णहा जइ(len >= 4) {
    पङ्क्तिः += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    जइ(len >= 8 && curl_strnequal(पङ्क्तिः, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    अण्णहा जइ(len >= 4 && curl_strnequal(पङ्क्तिः, "SIZE", 4))
      smtpc->size_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    अण्णहा जइ(len >= 8 && curl_strnequal(पङ्क्तिः, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    अण्णहा जइ(len >= 5 && curl_strnequal(पङ्क्तिः, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      पङ्क्तिः += 5;
      len -= 5;

      /* Loop through the data line */
      प्रति(;;) {
        size_t llen;
        size_t wordlen;
        unsigned short mechbit;

        जाव(len && (ISBLANK(*पङ्क्तिः) || ISNEWLINE(*पङ्क्तिः))) {
          पङ्क्तिः++;
          len--;
        }

        जइ(!len)
          भङ्ग;

        /* Extract the word */
        प्रति(wordlen = 0; wordlen < len && !ISBLANK(पङ्क्तिः[wordlen]) &&
              !ISNEWLINE(पङ्क्तिः[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(पङ्क्तिः, wordlen, &llen);
        जइ(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        पङ्क्तिः += wordlen;
        len -= wordlen;
      }
    }

    जइ(smtpcode != 1) {
      जइ(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        जइ(smtpc->tls_supported)
          /* Switch to TLS connection now */
          result = smtp_perform_starttls(data, smtpc);
        अण्णहा जइ(data->set.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          result = smtp_perform_authentication(data, smtpc);
        अण्णहा {
          failf(data, "STARTTLS not supported.");
          result = CURLE_USE_SSL_FAILED;
        }
      }
      अण्णहा
        result = smtp_perform_authentication(data, smtpc);
    }
  }
  अण्णहा {
    failf(data, "Unexpectedly short EHLO response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  प्रत्यागम result;
}

/* For HELO responses */
स्थिर CURLcode smtp_state_helo_resp(संरचना Curl_easy *data,
                                     संरचना smtp_conn *smtpc,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  जइ(smtpcode / 100 != 2) {
    failf(data, "Remote access denied: %d", smtpcode);
    result = CURLE_REMOTE_ACCESS_DENIED;
  }
  अण्णहा
    /* End of connect phase */
    smtp_state(data, smtpc, SMTP_STOP);

  प्रत्यागम result;
}

/* For SASL authentication responses */
स्थिर CURLcode smtp_state_auth_resp(संरचना Curl_easy *data,
                                     संरचना smtp_conn *smtpc,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (शून्य)instate;

  result = Curl_sasl_continue(&smtpc->sasl, data, smtpcode, &progress);
  जइ(!result)
    विकल्प(progress) {
    पक्ष SASL_DONE:
      smtp_state(data, smtpc, SMTP_STOP);  /* Authenticated */
      भङ्ग;
    पक्ष SASL_IDLE:            /* No mechanism left after cancellation */
      failf(data, "Authentication cancelled");
      result = CURLE_LOGIN_DENIED;
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }

  प्रत्यागम result;
}

/* For command responses */
स्थिर CURLcode smtp_state_command_resp(संरचना Curl_easy *data,
                                        संरचना smtp_conn *smtpc,
                                        संरचना SMTP *smtp,
                                        पूर्णाङ्क smtpcode,
                                        smtpstate instate)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (शून्य)instate;

  जइ((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(data, "Command failed: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  अण्णहा {
    जइ(!data->req.no_body)
      result = Curl_client_write(data, CLIENTWRITE_BODY, पङ्क्तिः, len);

    जइ(!result && (smtpcode != 1)) {
      जइ(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->next;

        जइ(smtp->rcpt) {
          /* Send the next command */
          result = smtp_perform_command(data, smtpc, smtp);
        }
        अण्णहा
          /* End of DO phase */
          smtp_state(data, smtpc, SMTP_STOP);
      }
      अण्णहा
        /* End of DO phase */
        smtp_state(data, smtpc, SMTP_STOP);
    }
  }

  प्रत्यागम result;
}

/* For MAIL responses */
स्थिर CURLcode smtp_state_mail_resp(संरचना Curl_easy *data,
                                     संरचना smtp_conn *smtpc,
                                     संरचना SMTP *smtp,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  जइ(smtpcode / 100 != 2) {
    failf(data, "MAIL failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  अण्णहा
    /* Start the RCPT TO command */
    result = smtp_perform_rcpt_to(data, smtpc, smtp);

  प्रत्यागम result;
}

/* For RCPT responses */
स्थिर CURLcode smtp_state_rcpt_resp(संरचना Curl_easy *data,
                                     संरचना smtp_conn *smtpc,
                                     संरचना SMTP *smtp,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (शून्य)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !data->set.mail_rcpt_allowfails);

  जइ(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_last_error = smtpcode;

    जइ(is_smtp_blocking_err) {
      failf(data, "RCPT failed: %d", smtpcode);
      result = CURLE_SEND_ERROR;
    }
  }
  अण्णहा {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  जइ(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->next;

    जइ(smtp->rcpt)
      /* Send the next RCPT TO command */
      result = smtp_perform_rcpt_to(data, smtpc, smtp);
    अण्णहा {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      जइ(!smtp->rcpt_had_ok) {
        failf(data, "RCPT failed: %d (last error)", smtp->rcpt_last_error);
        result = CURLE_SEND_ERROR;
      }
      अण्णहा {
        /* Send the DATA command */
        result = Curl_pp_sendf(data, &smtpc->pp, "%s", "DATA");

        जइ(!result)
          smtp_state(data, smtpc, SMTP_DATA);
      }
    }
  }

  प्रत्यागम result;
}

/* For DATA response */
स्थिर CURLcode smtp_state_data_resp(संरचना Curl_easy *data,
                                     संरचना smtp_conn *smtpc,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  जइ(smtpcode != 354) {
    failf(data, "DATA failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  अण्णहा {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* SMTP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(data, smtpc, SMTP_STOP);
  }

  प्रत्यागम result;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
स्थिर CURLcode smtp_state_postdata_resp(संरचना Curl_easy *data,
                                         संरचना smtp_conn *smtpc,
                                         पूर्णाङ्क smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;

  (शून्य)instate;

  जइ(smtpcode != 250)
    result = CURLE_WEIRD_SERVER_REPLY;

  /* End of DONE phase */
  smtp_state(data, smtpc, SMTP_STOP);

  प्रत्यागम result;
}

स्थिर CURLcode smtp_pp_statemachine(संरचना Curl_easy *data,
                                     संरचना connectdata *conn)
{
  CURLcode result = CURLE_OK;
  पूर्णाङ्क smtpcode;
  संरचना smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  संरचना SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  size_t nread = 0;

  जइ(!smtpc || !smtp)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  जइ(smtpc->state == SMTP_UPGRADETLS) {
    result = smtp_perform_upgrade_tls(data, smtpc);
    जइ(result || (smtpc->state == SMTP_UPGRADETLS))
      प्रत्यागम result;
  }

  /* Flush any data that needs to be sent */
  जइ(smtpc->pp.sendleft)
    प्रत्यागम Curl_pp_flushsend(data, &smtpc->pp);

  करेहि {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    जइ(result)
      प्रत्यागम result;

    /* Store the latest response for later retrieval if necessary */
    जइ(smtpc->state != SMTP_QUIT && smtpcode != 1)
      data->info.httpcode = smtpcode;

    जइ(!smtpcode)
      भङ्ग;

    /* We have now received a full SMTP server response */
    विकल्प(smtpc->state) {
    पक्ष SMTP_SERVERGREET:
      result = smtp_state_servergreet_resp(data, smtpc,
                                           smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_EHLO:
      result = smtp_state_ehlo_resp(data, smtpc, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_HELO:
      result = smtp_state_helo_resp(data, smtpc, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_STARTTLS:
      result = smtp_state_starttls_resp(data, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      जइ(!result && (smtpc->state == SMTP_UPGRADETLS))
        गच्छ upgrade_tls;
      भङ्ग;

    पक्ष SMTP_AUTH:
      result = smtp_state_auth_resp(data, smtpc, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_COMMAND:
      result = smtp_state_command_resp(data, smtpc, smtp,
                                       smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_MAIL:
      result = smtp_state_mail_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_RCPT:
      result = smtp_state_rcpt_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_DATA:
      result = smtp_state_data_resp(data, smtpc, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_POSTDATA:
      result = smtp_state_postdata_resp(data, smtpc, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_QUIT:
    स्वभाव:
      /* internal error */
      smtp_state(data, smtpc, SMTP_STOP);
      भङ्ग;
    }
  } जाव(!result && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  प्रत्यागम result;
}

/* Called repeatedly until done from multi.c */
स्थिर CURLcode smtp_multi_statemach(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  *done = FALSE;
  जइ(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;

  result = Curl_pp_statemach(data, &smtpc->pp, FALSE, FALSE);
  *done = (smtpc->state == SMTP_STOP);
  प्रत्यागम result;
}

स्थिर CURLcode smtp_block_statemach(संरचना Curl_easy *data,
                                     संरचना smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  जाव(smtpc->state != SMTP_STOP && !result)
    result = Curl_pp_statemach(data, &smtpc->pp, TRUE, disconnecting);

  प्रत्यागम result;
}

/* For the SMTP "protocol connect" and "doing" phases only */
स्थिर CURLcode smtp_pollset(संरचना Curl_easy *data,
                             संरचना easy_pollset *ps)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  प्रत्यागम smtpc ? Curl_pp_pollset(data, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
स्थिर ध्रुव संरचना SASLproto saslsmtp = {
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
स्थिर CURLcode smtp_connect(संरचना Curl_easy *data, bool *done)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  जइ(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;

  PINGPONG_SETUP(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, data, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = smtp_parse_url_options(data->conn, smtpc);
  जइ(result)
    प्रत्यागम result;

  /* Parse the URL path */
  result = smtp_parse_url_path(data, smtpc);
  जइ(result)
    प्रत्यागम result;

  /* Start off waiting for the server greeting response */
  smtp_state(data, smtpc, SMTP_SERVERGREET);

  result = smtp_multi_statemach(data, done);

  प्रत्यागम result;
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
स्थिर CURLcode smtp_done(संरचना Curl_easy *data, CURLcode status,
                          bool premature)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);

  (शून्य)premature;

  जइ(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;
  जइ(!smtp)
    प्रत्यागम CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  जइ(status) {
    CURL_TRC_M(data, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  अण्णहा जइ(!data->set.connect_only && data->set.mail_rcpt &&
          (data->state.upload || IS_MIME_POST(data))) {

    smtp_state(data, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    result = smtp_block_statemach(data, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(data, "smtp_done(status=%d, premature=%d) -> %d",
                (पूर्णाङ्क)status, premature, (पूर्णाङ्क)result);
  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
स्थिर CURLcode smtp_perform(संरचना Curl_easy *data,
                             संरचना smtp_conn *smtpc,
                             संरचना SMTP *smtp,
                             bool *connected,
                             bool *dophase_done)
{
  /* This is SMTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_SMTP(data, "smtp_perform(), start");

  जइ(data->req.no_body) {
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
  जइ((data->state.upload || IS_MIME_POST(data)) && data->set.mail_rcpt)
    /* MAIL transfer */
    result = smtp_perform_mail(data, smtpc, smtp);
  अण्णहा
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    result = smtp_perform_command(data, smtpc, smtp);

  जइ(result)
    गच्छ out;

  /* Run the state-machine */
  result = smtp_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

out:
  CURL_TRC_SMTP(data, "smtp_perform() -> %d, connected=%d, done=%d",
                (पूर्णाङ्क)result, *connected, *dophase_done);
  प्रत्यागम result;
}

/* Call this when the DO phase has completed */
स्थिर CURLcode smtp_dophase_done(संरचना Curl_easy *data,
                                  संरचना SMTP *smtp,
                                  bool connected)
{
  (शून्य)connected;

  जइ(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  प्रत्यागम CURLE_OK;
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
स्थिर CURLcode smtp_regular_transfer(संरचना Curl_easy *data,
                                      संरचना smtp_conn *smtpc,
                                      संरचना SMTP *smtp,
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
  जइ(!result && *dophase_done)
    result = smtp_dophase_done(data, smtp, connected);

  CURL_TRC_SMTP(data, "smtp_regular_transfer() -> %d, done=%d",
                (पूर्णाङ्क)result, *dophase_done);
  प्रत्यागम result;
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
स्थिर CURLcode smtp_do(संरचना Curl_easy *data, bool *done)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  संरचना SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  *done = FALSE; /* default to false */
  जइ(!smtpc || !smtp)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Parse the custom request */
  result = smtp_parse_custom_request(data, smtp);
  जइ(result)
    प्रत्यागम result;

  result = smtp_regular_transfer(data, smtpc, smtp, done);
  CURL_TRC_SMTP(data, "smtp_do() -> %d, done=%d", (पूर्णाङ्क)result, *done);
  प्रत्यागम result;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
स्थिर CURLcode smtp_disconnect(संरचना Curl_easy *data,
                                संरचना connectdata *conn,
                                bool dead_connection)
{
  संरचना smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  जइ(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  जइ(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &smtpc->pp) &&
     !smtp_perform_quit(data, smtpc))
    (शून्य)smtp_block_statemach(data, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(data, "smtp_disconnect(), finished");
  प्रत्यागम CURLE_OK;
}

/* Called from multi.c while DOing */
स्थिर CURLcode smtp_doing(संरचना Curl_easy *data, bool *dophase_done)
{
  संरचना SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result;

  जइ(!smtp)
    प्रत्यागम CURLE_FAILED_INIT;
  result = smtp_multi_statemach(data, dophase_done);
  जइ(result)
    DEBUGF(infof(data, "DO phase failed"));
  अण्णहा जइ(*dophase_done) {
    result = smtp_dophase_done(data, smtp, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  CURL_TRC_SMTP(data, "smtp_doing() -> %d, done=%d", (पूर्णाङ्क)result,
                *dophase_done);
  प्रत्यागम result;
}

स्थिर शून्य smtp_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना SMTP *smtp = entry;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_free(smtp);
}

स्थिर शून्य smtp_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना smtp_conn *smtpc = entry;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

स्थिर CURLcode smtp_setup_connection(संरचना Curl_easy *data,
                                      संरचना connectdata *conn)
{
  संरचना smtp_conn *smtpc;
  संरचना SMTP *smtp;
  CURLcode result = CURLE_OK;

  smtpc = curlx_calloc(1, परिमाण(*smtpc));
  जइ(!smtpc ||
     Curl_conn_meta_set(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  smtp = curlx_calloc(1, परिमाण(*smtp));
  जइ(!smtp ||
     Curl_meta_set(data, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    result = CURLE_OUT_OF_MEMORY;

out:
  CURL_TRC_SMTP(data, "smtp_setup_connection() -> %d", (पूर्णाङ्क)result);
  प्रत्यागम result;
}

/*
 * SMTP protocol handler.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_smtp = {
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
