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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "smtp.h"

#ifndef CURL_DISABLE_SMTP

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef __VMS
#অন্তর্ভাৱ <in.h>
#অন্তর্ভাৱ <inet.h>
#endif

#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "escape.h"
#অন্তর্ভাৱ "pingpong.h"
#অন্তর্ভাৱ "mime.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "curl_gethostname.h"
#অন্তর্ভাৱ "bufref.h"
#অন্তর্ভাৱ "curl_sasl.h"
#অন্তর্ভাৱ "idn.h"
#অন্তর্ভাৱ "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
প্রকারনির্দেশ গণনা {
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
সংরচনা smtp_conn {
  সংরচনা pingpong pp;
  সংরচনা SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  ৱর্ণ *domain;            /* Client address/name to send in the EHLO */
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
সংরচনা SMTP {
  curl_pp_transfer transfer;
  ৱর্ণ *custom;            /* Custom Request */
  সংরচনা curl_slist *rcpt; /* Recipient list */
  পূর্ণাঙ্ক rcpt_last_error;     /* The last error received for RCPT TO command */
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
স্থির CURLcode smtp_parse_url_options(সংরচনা connectdata *conn,
                                       সংরচনা smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *ptr = conn->options;

  যেতিয়ালৈ(!result && ptr && *ptr) {
    ধ্রুৱ ৱর্ণ *চাবি = ptr;
    ধ্রুৱ ৱর্ণ *মান;

    যেতিয়ালৈ(*ptr && *ptr != '=')
      ptr++;

    মান = ptr + 1;

    যেতিয়ালৈ(*ptr && *ptr != ';')
      ptr++;

    যদি(curl_strnequal(চাবি, "AUTH=", 5))
      result = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               মান, ptr - মান);
    অন্যথা
      result = CURLE_URL_MALFORMAT;

    যদি(*ptr == ';')
      ptr++;
  }

  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
স্থির CURLcode smtp_parse_url_path(সংরচনা Curl_easy *data,
                                    সংরচনা smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ধ্রুৱ ৱর্ণ *path = &data->state.up.path[1]; /* skip leading path */
  ৱর্ণ localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  যদি(!*path) {
    যদি(!Curl_gethostname(localhost, পরিমাণ(localhost)))
      path = localhost;
    অন্যথা
      path = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  ঘূৰাই Curl_urldecode(path, 0, &smtpc->domain, রিক্ত, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
স্থির CURLcode smtp_parse_custom_request(সংরচনা Curl_easy *data,
                                          সংরচনা SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  যদি(custom)
    result = Curl_urldecode(custom, 0, &smtp->custom, রিক্ত, REJECT_CTRL);

  ঘূৰাই result;
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
স্থির CURLcode smtp_parse_address(সংরচনা Curl_easy *data, ধ্রুৱ ৱর্ণ *fqma,
                                   ৱর্ণ **address, সংরচনা hostname *host,
                                   ধ্রুৱ ৱর্ণ **suffix)
{
  CURLcode result = CURLE_OK;
  size_t length;
  ৱর্ণ *addressend;
  ৱর্ণ *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  যদি(strpbrk(fqma, "\r\n")) {
    failf(data, "Refusing to send email address with a CR or LF");
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  যদি(!dup)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  যদি(fqma[0] != '<') {
    length = strlen(dup);
    যদি(length && dup[length - 1] == '>')
      dup[length - 1] = '\0';
  }
  অন্যথা {
    addressend = strrchr(dup, '>');
    যদি(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  যদি(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (শূন্য)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *address = dup;

  ঘূৰাই result;
}

সংরচনা cr_eob_ctx {
  সংরচনা Curl_creader super;
  সংরচনা bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

স্থির CURLcode cr_eob_init(সংরচনা Curl_easy *data,
                            সংরচনা Curl_creader *reader)
{
  সংরচনা cr_eob_ctx *ctx = reader->ctx;
  (শূন্য)data;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য cr_eob_close(সংরচনা Curl_easy *data, সংরচনা Curl_creader *reader)
{
  সংরচনা cr_eob_ctx *ctx = reader->ctx;
  (শূন্য)data;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#define SMTP_EOB          "\r\n.\r\n"
#define SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
স্থির CURLcode cr_eob_read(সংরচনা Curl_easy *data,
                            সংরচনা Curl_creader *reader,
                            ৱর্ণ *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  সংরচনা cr_eob_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, i, start, n;
  bool eos;

  যদি(!ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* Get more and convert it when needed */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(data, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (পূর্ণাঙ্ক)result, nread, eos);
    যদি(result)
      ঘূৰাই result;

    ctx->read_eos = eos;
    যদি(nread) {
      যদি(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        ঘূৰাই CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      বাবে(i = start = 0; i < nread; ++i) {
        যদি(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          যদি(result)
            ঘূৰাই result;
          result = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          যদি(result)
            ঘূৰাই result;
          ctx->n_eob = 0;
          start = i;
          যদি(data->state.infilesize > 0)
            data->state.infilesize++;
        }

        যদি(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        যদি(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      যদি(start < nread) {
        result = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        যদি(result)
          ঘূৰাই result;
      }
    }
  }

  *peos = FALSE;

  যদি(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ধ্রুৱ ৱর্ণ *eob = SMTP_EOB;
    CURL_TRC_SMTP(data, "auto-ending mail body with '\\r\\n.\\r\\n'");
    ৱিকল্প(ctx->n_eob) {
    পক্ষ 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      ভঙ্গ;
    পক্ষ 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      ভঙ্গ;
    স্ৱভাৱ:
      ভঙ্গ;
    }
    result = Curl_bufq_cwrite(&ctx->buf, eob, strlen(eob), &n);
    যদি(result)
      ঘূৰাই result;
    ctx->processed_eos = TRUE;
  }

  যদি(!Curl_bufq_is_empty(&ctx->buf)) {
    result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  অন্যথা
    *pnread = 0;

  যদি(ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(data, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(data, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (পূর্ণাঙ্ক)result, *pnread, *peos));
  ঘূৰাই result;
}

স্থির curl_off_t cr_eob_total_length(সংরচনা Curl_easy *data,
                                      সংরচনা Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (শূন্য)data;
  (শূন্য)reader;
  ঘূৰাই -1;
}

স্থির ধ্রুৱ সংরচনা Curl_crtype cr_eob = {
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
  পরিমাণ(সংরচনা cr_eob_ctx)
};

স্থির CURLcode cr_eob_add(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *reader = রিক্ত;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_eob, CURL_CR_CONTENT_ENCODE);
  যদি(!result)
    result = Curl_creader_add(data, reader);

  যদি(result && reader)
    Curl_creader_free(data, reader);
  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
স্থির bool smtp_endofresp(সংরচনা Curl_easy *data, সংরচনা connectdata *conn,
                           ধ্রুৱ ৱর্ণ *সাৰি, size_t len, পূর্ণাঙ্ক *resp)
{
  সংরচনা smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool end = FALSE;
  (শূন্য)data;

  DEBUGASSERT(smtpc);
  যদি(!smtpc)
    ঘূৰাই FALSE;

  /* Nothing for us */
  যদি(len < 4 || !ISDIGIT(সাৰি[0]) || !ISDIGIT(সাৰি[1]) || !ISDIGIT(সাৰি[2]))
    ঘূৰাই FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  যদি(সাৰি[3] == ' ' || len == 5) {
    ৱর্ণ tmpline[6];
    curl_off_t code;
    ধ্রুৱ ৱর্ণ *p = tmpline;
    end = TRUE;
    memcpy(tmpline, সাৰি, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    যদি(curlx_str_সংখ্যা(&p, &code, len == 5 ? 99999 : 999))
      ঘূৰাই FALSE;
    *resp = (পূর্ণাঙ্ক)code;

    /* Make sure real server never sends internal value */
    যদি(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  অন্যথা যদি(সাৰি[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    end = TRUE;
    *resp = 1;  /* Internal response code */
  }

  ঘূৰাই end;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
স্থির CURLcode smtp_get_message(সংরচনা Curl_easy *data, সংরচনা bufref *out)
{
  সংরচনা smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ৱর্ণ *message;
  size_t len;

  যদি(!smtpc)
    ঘূৰাই CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  যদি(len > 4) {
    /* Find the start of the message */
    len -= 4;
    বাবে(message += 4; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    যেতিয়ালৈ(len--)
      যদি(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        ভঙ্গ;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, রিক্ত);
  }
  অন্যথা
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, রিক্ত);

  ঘূৰাই CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
স্থির শূন্য smtp_state(সংরচনা Curl_easy *data,
                       সংরচনা smtp_conn *smtpc,
                       smtpstate newstate)
{
#ifdef CURLVERBOSE
  /* for debug purposes */
  স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ names[] = {
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

  যদি(smtpc->state != newstate)
    CURL_TRC_SMTP(data, "state change from %s to %s",
                  names[smtpc->state], names[newstate]);
#অন্যথা
  (শূন্য)data;
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
স্থির CURLcode smtp_perform_ehlo(সংরচনা Curl_easy *data,
                                  সংরচনা smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "EHLO %s", smtpc->domain);

  যদি(!result)
    smtp_state(data, smtpc, SMTP_EHLO);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
স্থির CURLcode smtp_perform_helo(সংরচনা Curl_easy *data,
                                  সংরচনা smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "HELO %s", smtpc->domain);

  যদি(!result)
    smtp_state(data, smtpc, SMTP_HELO);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
স্থির CURLcode smtp_perform_starttls(সংরচনা Curl_easy *data,
                                      সংরচনা smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "STARTTLS");

  যদি(!result)
    smtp_state(data, smtpc, SMTP_STARTTLS);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
স্থির CURLcode smtp_perform_upgrade_tls(সংরচনা Curl_easy *data,
                                         সংরচনা smtp_conn *smtpc)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  সংরচনা connectdata *conn = data->conn;
  CURLcode result;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  যদি(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    যদি(result)
      গচ্ছ out;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (পূর্ণাঙ্ক)result, ssldone));
  যদি(!result && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    result = smtp_perform_ehlo(data, smtpc);
  }
out:
  ঘূৰাই result;
#অন্যথা
  (শূন্য)data;
  (শূন্য)smtpc;
  ঘূৰাই CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
স্থির CURLcode smtp_perform_auth(সংরচনা Curl_easy *data,
                                  ধ্রুৱ ৱর্ণ *mech,
                                  ধ্রুৱ সংরচনা bufref *initresp)
{
  CURLcode result = CURLE_OK;
  সংরচনা smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ধ্রুৱ ৱর্ণ *ir = Curl_bufref_ptr(initresp);

  যদি(!smtpc)
    ঘূৰাই CURLE_FAILED_INIT;

  যদি(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  অন্যথা {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s", mech);
  }

  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
স্থির CURLcode smtp_continue_auth(সংরচনা Curl_easy *data,
                                   ধ্রুৱ ৱর্ণ *mech,
                                   ধ্রুৱ সংরচনা bufref *resp)
{
  সংরচনা smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (শূন্য)mech;
  যদি(!smtpc)
    ঘূৰাই CURLE_FAILED_INIT;
  ঘূৰাই Curl_pp_sendf(data, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
স্থির CURLcode smtp_cancel_auth(সংরচনা Curl_easy *data, ধ্রুৱ ৱর্ণ *mech)
{
  সংরচনা smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (শূন্য)mech;
  যদি(!smtpc)
    ঘূৰাই CURLE_FAILED_INIT;
  ঘূৰাই Curl_pp_sendf(data, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
স্থির CURLcode smtp_perform_authentication(সংরচনা Curl_easy *data,
                                            সংরচনা smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  যদি(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, data)) {
    smtp_state(data, smtpc, SMTP_STOP);
    ঘূৰাই result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&smtpc->sasl, data, FALSE, &progress);

  যদি(!result) {
    যদি(progress == SASL_INPROGRESS)
      smtp_state(data, smtpc, SMTP_AUTH);
    অন্যথা
      result = Curl_sasl_is_blocked(&smtpc->sasl, data);
  }

  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
স্থির CURLcode smtp_perform_command(সংরচনা Curl_easy *data,
                                     সংরচনা smtp_conn *smtpc,
                                     সংরচনা SMTP *smtp)
{
  CURLcode result = CURLE_OK;

  যদি(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    যদি(!smtp->custom || !smtp->custom[0]) {
      ৱর্ণ *address = রিক্ত;
      সংরচনা hostname host = { রিক্ত, রিক্ত, রিক্ত, রিক্ত };
      ধ্রুৱ ৱর্ণ *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, smtp->rcpt->data,
                                  &address, &host, &suffix);
      যদি(result)
        ঘূৰাই result;

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
    অন্যথা {
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
  অন্যথা
    /* Send the non-recipient based command such as HELP */
    result = Curl_pp_sendf(data, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  যদি(!result)
    smtp_state(data, smtpc, SMTP_COMMAND);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
স্থির CURLcode smtp_perform_mail(সংরচনা Curl_easy *data,
                                  সংরচনা smtp_conn *smtpc,
                                  সংরচনা SMTP *smtp)
{
  ৱর্ণ *from = রিক্ত;
  ৱর্ণ *auth = রিক্ত;
  ৱর্ণ *size = রিক্ত;
  ধ্রুৱ ৱর্ণ *str;
  CURLcode result = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_FROM);
  যদি(str) {
    ৱর্ণ *address = রিক্ত;
    সংরচনা hostname host = { রিক্ত, রিক্ত, রিক্ত, রিক্ত };
    ধ্রুৱ ৱর্ণ *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    result = smtp_parse_address(data, str, &address, &host, &suffix);
    যদি(result)
      গচ্ছ out;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(address) ||
            !Curl_is_ASCII_name(host.name));

    যদি(host.name) {
      from = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    অন্যথা
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", address, suffix);

    curlx_free(address);
  }
  অন্যথা
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  যদি(!from) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_AUTH);
  যদি(str && smtpc->sasl.authused) {
    যদি(str[0] != '\0') {
      ৱর্ণ *address = রিক্ত;
      সংরচনা hostname host = { রিক্ত, রিক্ত, রিক্ত, রিক্ত };
      ধ্রুৱ ৱর্ণ *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, str, &address, &host, &suffix);
      যদি(result)
        গচ্ছ out;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      যদি(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(address) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      যদি(host.name) {
        auth = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      অন্যথা
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", address, suffix);
      curlx_free(address);
    }
    অন্যথা
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    যদি(!auth) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  যদি(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned পূর্ণাঙ্ক)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, রিক্ত,
                                       রিক্ত, MIMESTRATEGY_MAIL);

    যদি(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    যদি(!result)
      result = Curl_creader_set_mime(data, postp);
    যদি(result)
      গচ্ছ out;
    data->state.infilesize = Curl_creader_total_length(data);
  }
  অন্যথা
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    যদি(result)
      গচ্ছ out;
  }

  /* Calculate the optional SIZE parameter */
  যদি(smtpc->size_supported && data->state.infilesize > 0) {
    size = curl_maprintf("%" FMT_OFF_T, data->state.infilesize);

    যদি(!size) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  যদি(smtpc->utf8_supported && !utf8) {
    সংরচনা curl_slist *rcpt = smtp->rcpt;

    যেতিয়ালৈ(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      যদি(!Curl_is_ASCII_name(rcpt->data))
        utf8 = TRUE;

      rcpt = rcpt->next;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  result = cr_eob_add(data);
  যদি(result)
    গচ্ছ out;

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

  যদি(!result)
    smtp_state(data, smtpc, SMTP_MAIL);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
স্থির CURLcode smtp_perform_rcpt_to(সংরচনা Curl_easy *data,
                                     সংরচনা smtp_conn *smtpc,
                                     সংরচনা SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  ৱর্ণ *address = রিক্ত;
  সংরচনা hostname host = { রিক্ত, রিক্ত, রিক্ত, রিক্ত };
  ধ্রুৱ ৱর্ণ *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  result = smtp_parse_address(data, smtp->rcpt->data,
                              &address, &host, &suffix);
  যদি(result)
    ঘূৰাই result;

  /* Send the RCPT TO command */
  যদি(host.name)
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           address, host.name, suffix);
  অন্যথা
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s>%s",
                           address, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(address);

  যদি(!result)
    smtp_state(data, smtpc, SMTP_RCPT);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
স্থির CURLcode smtp_perform_quit(সংরচনা Curl_easy *data,
                                  সংরচনা smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "QUIT");

  যদি(!result)
    smtp_state(data, smtpc, SMTP_QUIT);

  ঘূৰাই result;
}

/* For the initial server greeting */
স্থির CURLcode smtp_state_servergreet_resp(সংরচনা Curl_easy *data,
                                            সংরচনা smtp_conn *smtpc,
                                            পূর্ণাঙ্ক smtpcode,
                                            smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  যদি(smtpcode / 100 != 2) {
    failf(data, "Got unexpected smtp-server response: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  অন্যথা
    result = smtp_perform_ehlo(data, smtpc);

  ঘূৰাই result;
}

/* For STARTTLS responses */
স্থির CURLcode smtp_state_starttls_resp(সংরচনা Curl_easy *data,
                                         সংরচনা smtp_conn *smtpc,
                                         পূর্ণাঙ্ক smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  /* Pipelining in response is forbidden. */
  যদি(smtpc->pp.overflow)
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;

  যদি(smtpcode != 220) {
    যদি(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied, code %d", smtpcode);
      result = CURLE_USE_SSL_FAILED;
    }
    অন্যথা
      result = smtp_perform_authentication(data, smtpc);
  }
  অন্যথা
    smtp_state(data, smtpc, SMTP_UPGRADETLS);

  ঘূৰাই result;
}

/* For EHLO responses */
স্থির CURLcode smtp_state_ehlo_resp(সংরচনা Curl_easy *data,
                                     সংরচনা smtp_conn *smtpc,
                                     পূর্ণাঙ্ক smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *সাৰি = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (শূন্য)instate;

  যদি(smtpcode / 100 != 2 && smtpcode != 1) {
    যদি(data->set.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(data->conn, FIRSTSOCKET))
      result = smtp_perform_helo(data, smtpc);
    অন্যথা {
      failf(data, "Remote access denied: %d", smtpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  অন্যথা যদি(len >= 4) {
    সাৰি += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    যদি(len >= 8 && curl_strnequal(সাৰি, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    অন্যথা যদি(len >= 4 && curl_strnequal(সাৰি, "SIZE", 4))
      smtpc->size_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    অন্যথা যদি(len >= 8 && curl_strnequal(সাৰি, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    অন্যথা যদি(len >= 5 && curl_strnequal(সাৰি, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      সাৰি += 5;
      len -= 5;

      /* Loop through the data line */
      বাবে(;;) {
        size_t llen;
        size_t wordlen;
        unsigned short mechbit;

        যেতিয়ালৈ(len && (ISBLANK(*সাৰি) || ISNEWLINE(*সাৰি))) {
          সাৰি++;
          len--;
        }

        যদি(!len)
          ভঙ্গ;

        /* Extract the word */
        বাবে(wordlen = 0; wordlen < len && !ISBLANK(সাৰি[wordlen]) &&
              !ISNEWLINE(সাৰি[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(সাৰি, wordlen, &llen);
        যদি(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        সাৰি += wordlen;
        len -= wordlen;
      }
    }

    যদি(smtpcode != 1) {
      যদি(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        যদি(smtpc->tls_supported)
          /* Switch to TLS connection now */
          result = smtp_perform_starttls(data, smtpc);
        অন্যথা যদি(data->set.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          result = smtp_perform_authentication(data, smtpc);
        অন্যথা {
          failf(data, "STARTTLS not supported.");
          result = CURLE_USE_SSL_FAILED;
        }
      }
      অন্যথা
        result = smtp_perform_authentication(data, smtpc);
    }
  }
  অন্যথা {
    failf(data, "Unexpectedly short EHLO response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  ঘূৰাই result;
}

/* For HELO responses */
স্থির CURLcode smtp_state_helo_resp(সংরচনা Curl_easy *data,
                                     সংরচনা smtp_conn *smtpc,
                                     পূর্ণাঙ্ক smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  যদি(smtpcode / 100 != 2) {
    failf(data, "Remote access denied: %d", smtpcode);
    result = CURLE_REMOTE_ACCESS_DENIED;
  }
  অন্যথা
    /* End of connect phase */
    smtp_state(data, smtpc, SMTP_STOP);

  ঘূৰাই result;
}

/* For SASL authentication responses */
স্থির CURLcode smtp_state_auth_resp(সংরচনা Curl_easy *data,
                                     সংরচনা smtp_conn *smtpc,
                                     পূর্ণাঙ্ক smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (শূন্য)instate;

  result = Curl_sasl_continue(&smtpc->sasl, data, smtpcode, &progress);
  যদি(!result)
    ৱিকল্প(progress) {
    পক্ষ SASL_DONE:
      smtp_state(data, smtpc, SMTP_STOP);  /* Authenticated */
      ভঙ্গ;
    পক্ষ SASL_IDLE:            /* No mechanism left after cancellation */
      failf(data, "Authentication cancelled");
      result = CURLE_LOGIN_DENIED;
      ভঙ্গ;
    স্ৱভাৱ:
      ভঙ্গ;
    }

  ঘূৰাই result;
}

/* For command responses */
স্থির CURLcode smtp_state_command_resp(সংরচনা Curl_easy *data,
                                        সংরচনা smtp_conn *smtpc,
                                        সংরচনা SMTP *smtp,
                                        পূর্ণাঙ্ক smtpcode,
                                        smtpstate instate)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *সাৰি = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (শূন্য)instate;

  যদি((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(data, "Command failed: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  অন্যথা {
    যদি(!data->req.no_body)
      result = Curl_client_write(data, CLIENTWRITE_BODY, সাৰি, len);

    যদি(!result && (smtpcode != 1)) {
      যদি(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->next;

        যদি(smtp->rcpt) {
          /* Send the next command */
          result = smtp_perform_command(data, smtpc, smtp);
        }
        অন্যথা
          /* End of DO phase */
          smtp_state(data, smtpc, SMTP_STOP);
      }
      অন্যথা
        /* End of DO phase */
        smtp_state(data, smtpc, SMTP_STOP);
    }
  }

  ঘূৰাই result;
}

/* For MAIL responses */
স্থির CURLcode smtp_state_mail_resp(সংরচনা Curl_easy *data,
                                     সংরচনা smtp_conn *smtpc,
                                     সংরচনা SMTP *smtp,
                                     পূর্ণাঙ্ক smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  যদি(smtpcode / 100 != 2) {
    failf(data, "MAIL failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  অন্যথা
    /* Start the RCPT TO command */
    result = smtp_perform_rcpt_to(data, smtpc, smtp);

  ঘূৰাই result;
}

/* For RCPT responses */
স্থির CURLcode smtp_state_rcpt_resp(সংরচনা Curl_easy *data,
                                     সংরচনা smtp_conn *smtpc,
                                     সংরচনা SMTP *smtp,
                                     পূর্ণাঙ্ক smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (শূন্য)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !data->set.mail_rcpt_allowfails);

  যদি(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_last_error = smtpcode;

    যদি(is_smtp_blocking_err) {
      failf(data, "RCPT failed: %d", smtpcode);
      result = CURLE_SEND_ERROR;
    }
  }
  অন্যথা {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  যদি(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->next;

    যদি(smtp->rcpt)
      /* Send the next RCPT TO command */
      result = smtp_perform_rcpt_to(data, smtpc, smtp);
    অন্যথা {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      যদি(!smtp->rcpt_had_ok) {
        failf(data, "RCPT failed: %d (last error)", smtp->rcpt_last_error);
        result = CURLE_SEND_ERROR;
      }
      অন্যথা {
        /* Send the DATA command */
        result = Curl_pp_sendf(data, &smtpc->pp, "%s", "DATA");

        যদি(!result)
          smtp_state(data, smtpc, SMTP_DATA);
      }
    }
  }

  ঘূৰাই result;
}

/* For DATA response */
স্থির CURLcode smtp_state_data_resp(সংরচনা Curl_easy *data,
                                     সংরচনা smtp_conn *smtpc,
                                     পূর্ণাঙ্ক smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  যদি(smtpcode != 354) {
    failf(data, "DATA failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  অন্যথা {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* SMTP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(data, smtpc, SMTP_STOP);
  }

  ঘূৰাই result;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
স্থির CURLcode smtp_state_postdata_resp(সংরচনা Curl_easy *data,
                                         সংরচনা smtp_conn *smtpc,
                                         পূর্ণাঙ্ক smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;

  (শূন্য)instate;

  যদি(smtpcode != 250)
    result = CURLE_WEIRD_SERVER_REPLY;

  /* End of DONE phase */
  smtp_state(data, smtpc, SMTP_STOP);

  ঘূৰাই result;
}

স্থির CURLcode smtp_pp_statemachine(সংরচনা Curl_easy *data,
                                     সংরচনা connectdata *conn)
{
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক smtpcode;
  সংরচনা smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  সংরচনা SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  size_t nread = 0;

  যদি(!smtpc || !smtp)
    ঘূৰাই CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  যদি(smtpc->state == SMTP_UPGRADETLS) {
    result = smtp_perform_upgrade_tls(data, smtpc);
    যদি(result || (smtpc->state == SMTP_UPGRADETLS))
      ঘূৰাই result;
  }

  /* Flush any data that needs to be sent */
  যদি(smtpc->pp.sendleft)
    ঘূৰাই Curl_pp_flushsend(data, &smtpc->pp);

  কৰা {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    যদি(result)
      ঘূৰাই result;

    /* Store the latest response for later retrieval if necessary */
    যদি(smtpc->state != SMTP_QUIT && smtpcode != 1)
      data->info.httpcode = smtpcode;

    যদি(!smtpcode)
      ভঙ্গ;

    /* We have now received a full SMTP server response */
    ৱিকল্প(smtpc->state) {
    পক্ষ SMTP_SERVERGREET:
      result = smtp_state_servergreet_resp(data, smtpc,
                                           smtpcode, smtpc->state);
      ভঙ্গ;

    পক্ষ SMTP_EHLO:
      result = smtp_state_ehlo_resp(data, smtpc, smtpcode, smtpc->state);
      ভঙ্গ;

    পক্ষ SMTP_HELO:
      result = smtp_state_helo_resp(data, smtpc, smtpcode, smtpc->state);
      ভঙ্গ;

    পক্ষ SMTP_STARTTLS:
      result = smtp_state_starttls_resp(data, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      যদি(!result && (smtpc->state == SMTP_UPGRADETLS))
        গচ্ছ upgrade_tls;
      ভঙ্গ;

    পক্ষ SMTP_AUTH:
      result = smtp_state_auth_resp(data, smtpc, smtpcode, smtpc->state);
      ভঙ্গ;

    পক্ষ SMTP_COMMAND:
      result = smtp_state_command_resp(data, smtpc, smtp,
                                       smtpcode, smtpc->state);
      ভঙ্গ;

    পক্ষ SMTP_MAIL:
      result = smtp_state_mail_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      ভঙ্গ;

    পক্ষ SMTP_RCPT:
      result = smtp_state_rcpt_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      ভঙ্গ;

    পক্ষ SMTP_DATA:
      result = smtp_state_data_resp(data, smtpc, smtpcode, smtpc->state);
      ভঙ্গ;

    পক্ষ SMTP_POSTDATA:
      result = smtp_state_postdata_resp(data, smtpc, smtpcode, smtpc->state);
      ভঙ্গ;

    পক্ষ SMTP_QUIT:
    স্ৱভাৱ:
      /* internal error */
      smtp_state(data, smtpc, SMTP_STOP);
      ভঙ্গ;
    }
  } যেতিয়ালৈ(!result && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  ঘূৰাই result;
}

/* Called repeatedly until done from multi.c */
স্থির CURLcode smtp_multi_statemach(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  *done = FALSE;
  যদি(!smtpc)
    ঘূৰাই CURLE_FAILED_INIT;

  result = Curl_pp_statemach(data, &smtpc->pp, FALSE, FALSE);
  *done = (smtpc->state == SMTP_STOP);
  ঘূৰাই result;
}

স্থির CURLcode smtp_block_statemach(সংরচনা Curl_easy *data,
                                     সংরচনা smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  যেতিয়ালৈ(smtpc->state != SMTP_STOP && !result)
    result = Curl_pp_statemach(data, &smtpc->pp, TRUE, disconnecting);

  ঘূৰাই result;
}

/* For the SMTP "protocol connect" and "doing" phases only */
স্থির CURLcode smtp_pollset(সংরচনা Curl_easy *data,
                             সংরচনা easy_pollset *ps)
{
  সংরচনা smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ঘূৰাই smtpc ? Curl_pp_pollset(data, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
স্থির ধ্রুৱ সংরচনা SASLproto saslsmtp = {
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
স্থির CURLcode smtp_connect(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  যদি(!smtpc)
    ঘূৰাই CURLE_FAILED_INIT;

  PINGPONG_SETUP(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, data, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = smtp_parse_url_options(data->conn, smtpc);
  যদি(result)
    ঘূৰাই result;

  /* Parse the URL path */
  result = smtp_parse_url_path(data, smtpc);
  যদি(result)
    ঘূৰাই result;

  /* Start off waiting for the server greeting response */
  smtp_state(data, smtpc, SMTP_SERVERGREET);

  result = smtp_multi_statemach(data, done);

  ঘূৰাই result;
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
স্থির CURLcode smtp_done(সংরচনা Curl_easy *data, CURLcode status,
                          bool premature)
{
  সংরচনা smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);

  (শূন্য)premature;

  যদি(!smtpc)
    ঘূৰাই CURLE_FAILED_INIT;
  যদি(!smtp)
    ঘূৰাই CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  যদি(status) {
    CURL_TRC_M(data, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  অন্যথা যদি(!data->set.connect_only && data->set.mail_rcpt &&
          (data->state.upload || IS_MIME_POST(data))) {

    smtp_state(data, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    result = smtp_block_statemach(data, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(data, "smtp_done(status=%d, premature=%d) -> %d",
                (পূর্ণাঙ্ক)status, premature, (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
স্থির CURLcode smtp_perform(সংরচনা Curl_easy *data,
                             সংরচনা smtp_conn *smtpc,
                             সংরচনা SMTP *smtp,
                             bool *connected,
                             bool *dophase_done)
{
  /* This is SMTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_SMTP(data, "smtp_perform(), start");

  যদি(data->req.no_body) {
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
  যদি((data->state.upload || IS_MIME_POST(data)) && data->set.mail_rcpt)
    /* MAIL transfer */
    result = smtp_perform_mail(data, smtpc, smtp);
  অন্যথা
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    result = smtp_perform_command(data, smtpc, smtp);

  যদি(result)
    গচ্ছ out;

  /* Run the state-machine */
  result = smtp_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

out:
  CURL_TRC_SMTP(data, "smtp_perform() -> %d, connected=%d, done=%d",
                (পূর্ণাঙ্ক)result, *connected, *dophase_done);
  ঘূৰাই result;
}

/* Call this when the DO phase has completed */
স্থির CURLcode smtp_dophase_done(সংরচনা Curl_easy *data,
                                  সংরচনা SMTP *smtp,
                                  bool connected)
{
  (শূন্য)connected;

  যদি(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  ঘূৰাই CURLE_OK;
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
স্থির CURLcode smtp_regular_transfer(সংরচনা Curl_easy *data,
                                      সংরচনা smtp_conn *smtpc,
                                      সংরচনা SMTP *smtp,
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
  যদি(!result && *dophase_done)
    result = smtp_dophase_done(data, smtp, connected);

  CURL_TRC_SMTP(data, "smtp_regular_transfer() -> %d, done=%d",
                (পূর্ণাঙ্ক)result, *dophase_done);
  ঘূৰাই result;
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
স্থির CURLcode smtp_do(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  সংরচনা SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  *done = FALSE; /* default to false */
  যদি(!smtpc || !smtp)
    ঘূৰাই CURLE_FAILED_INIT;

  /* Parse the custom request */
  result = smtp_parse_custom_request(data, smtp);
  যদি(result)
    ঘূৰাই result;

  result = smtp_regular_transfer(data, smtpc, smtp, done);
  CURL_TRC_SMTP(data, "smtp_do() -> %d, done=%d", (পূর্ণাঙ্ক)result, *done);
  ঘূৰাই result;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
স্থির CURLcode smtp_disconnect(সংরচনা Curl_easy *data,
                                সংরচনা connectdata *conn,
                                bool dead_connection)
{
  সংরচনা smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  যদি(!smtpc)
    ঘূৰাই CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  যদি(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &smtpc->pp) &&
     !smtp_perform_quit(data, smtpc))
    (শূন্য)smtp_block_statemach(data, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(data, "smtp_disconnect(), finished");
  ঘূৰাই CURLE_OK;
}

/* Called from multi.c while DOing */
স্থির CURLcode smtp_doing(সংরচনা Curl_easy *data, bool *dophase_done)
{
  সংরচনা SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result;

  যদি(!smtp)
    ঘূৰাই CURLE_FAILED_INIT;
  result = smtp_multi_statemach(data, dophase_done);
  যদি(result)
    DEBUGF(infof(data, "DO phase failed"));
  অন্যথা যদি(*dophase_done) {
    result = smtp_dophase_done(data, smtp, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  CURL_TRC_SMTP(data, "smtp_doing() -> %d, done=%d", (পূর্ণাঙ্ক)result,
                *dophase_done);
  ঘূৰাই result;
}

স্থির শূন্য smtp_easy_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা SMTP *smtp = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_free(smtp);
}

স্থির শূন্য smtp_conn_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা smtp_conn *smtpc = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

স্থির CURLcode smtp_setup_connection(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn)
{
  সংরচনা smtp_conn *smtpc;
  সংরচনা SMTP *smtp;
  CURLcode result = CURLE_OK;

  smtpc = curlx_calloc(1, পরিমাণ(*smtpc));
  যদি(!smtpc ||
     Curl_conn_meta_set(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  smtp = curlx_calloc(1, পরিমাণ(*smtp));
  যদি(!smtp ||
     Curl_meta_set(data, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    result = CURLE_OUT_OF_MEMORY;

out:
  CURL_TRC_SMTP(data, "smtp_setup_connection() -> %d", (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

/*
 * SMTP protocol handler.
 */
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_smtp = {
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
