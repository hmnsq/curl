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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "smtp.h"

#ifndef CURL_DISABLE_SMTP

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "pingpong.h"
#അന്തര്ഭാവ "mime.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "curl_gethostname.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "curl_sasl.h"
#അന്തര്ഭാവ "idn.h"
#അന്തര്ഭാവ "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
തരനിർവചനം എണ്ണൽ {
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
ഘടന smtp_conn {
  ഘടന pingpong pp;
  ഘടന SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  അക്ഷരം *domain;            /* Client address/name to send in the EHLO */
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
ഘടന SMTP {
  curl_pp_transfer transfer;
  അക്ഷരം *custom;            /* Custom Request */
  ഘടന curl_slist *rcpt; /* Recipient list */
  പൂർണ്ണസംഖ്യ rcpt_last_error;     /* The last error received for RCPT TO command */
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
സ്ഥിതം CURLcode smtp_parse_url_options(ഘടന connectdata *conn,
                                       ഘടന smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *ptr = conn->options;

  ആകുമ്പോൾ(!result && ptr && *ptr) {
    സ്ഥിരം അക്ഷരം *താക്കോൽ = ptr;
    സ്ഥിരം അക്ഷരം *മൂല്യം;

    ആകുമ്പോൾ(*ptr && *ptr != '=')
      ptr++;

    മൂല്യം = ptr + 1;

    ആകുമ്പോൾ(*ptr && *ptr != ';')
      ptr++;

    എങ്കിൽ(curl_strnequal(താക്കോൽ, "AUTH=", 5))
      result = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               മൂല്യം, ptr - മൂല്യം);
    അല്ലെങ്കിൽ
      result = CURLE_URL_MALFORMAT;

    എങ്കിൽ(*ptr == ';')
      ptr++;
  }

  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
സ്ഥിതം CURLcode smtp_parse_url_path(ഘടന Curl_easy *data,
                                    ഘടന smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  സ്ഥിരം അക്ഷരം *path = &data->state.up.path[1]; /* skip leading path */
  അക്ഷരം localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  എങ്കിൽ(!*path) {
    എങ്കിൽ(!Curl_gethostname(localhost, വലുപ്പം(localhost)))
      path = localhost;
    അല്ലെങ്കിൽ
      path = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  തിരികെ Curl_urldecode(path, 0, &smtpc->domain, ശൂന്യ, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
സ്ഥിതം CURLcode smtp_parse_custom_request(ഘടന Curl_easy *data,
                                          ഘടന SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  എങ്കിൽ(custom)
    result = Curl_urldecode(custom, 0, &smtp->custom, ശൂന്യ, REJECT_CTRL);

  തിരികെ result;
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
സ്ഥിതം CURLcode smtp_parse_address(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fqma,
                                   അക്ഷരം **address, ഘടന hostname *host,
                                   സ്ഥിരം അക്ഷരം **suffix)
{
  CURLcode result = CURLE_OK;
  size_t length;
  അക്ഷരം *addressend;
  അക്ഷരം *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  എങ്കിൽ(strpbrk(fqma, "\r\n")) {
    failf(data, "Refusing to send email address with a CR or LF");
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  എങ്കിൽ(!dup)
    തിരികെ CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(fqma[0] != '<') {
    length = strlen(dup);
    എങ്കിൽ(length && dup[length - 1] == '>')
      dup[length - 1] = '\0';
  }
  അല്ലെങ്കിൽ {
    addressend = strrchr(dup, '>');
    എങ്കിൽ(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  എങ്കിൽ(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (ശൂന്യം)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *address = dup;

  തിരികെ result;
}

ഘടന cr_eob_ctx {
  ഘടന Curl_creader super;
  ഘടന bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

സ്ഥിതം CURLcode cr_eob_init(ഘടന Curl_easy *data,
                            ഘടന Curl_creader *reader)
{
  ഘടന cr_eob_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cr_eob_close(ഘടന Curl_easy *data, ഘടന Curl_creader *reader)
{
  ഘടന cr_eob_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#define SMTP_EOB          "\r\n.\r\n"
#define SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
സ്ഥിതം CURLcode cr_eob_read(ഘടന Curl_easy *data,
                            ഘടന Curl_creader *reader,
                            അക്ഷരം *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ഘടന cr_eob_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, i, start, n;
  bool eos;

  എങ്കിൽ(!ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* Get more and convert it when needed */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(data, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (പൂർണ്ണസംഖ്യ)result, nread, eos);
    എങ്കിൽ(result)
      തിരികെ result;

    ctx->read_eos = eos;
    എങ്കിൽ(nread) {
      എങ്കിൽ(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        തിരികെ CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      വേണ്ടി(i = start = 0; i < nread; ++i) {
        എങ്കിൽ(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          എങ്കിൽ(result)
            തിരികെ result;
          result = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          എങ്കിൽ(result)
            തിരികെ result;
          ctx->n_eob = 0;
          start = i;
          എങ്കിൽ(data->state.infilesize > 0)
            data->state.infilesize++;
        }

        എങ്കിൽ(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        എങ്കിൽ(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      എങ്കിൽ(start < nread) {
        result = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        എങ്കിൽ(result)
          തിരികെ result;
      }
    }
  }

  *peos = FALSE;

  എങ്കിൽ(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    സ്ഥിരം അക്ഷരം *eob = SMTP_EOB;
    CURL_TRC_SMTP(data, "auto-ending mail body with '\\r\\n.\\r\\n'");
    തിരഞ്ഞെടുപ്പ്(ctx->n_eob) {
    അവസ്ഥ 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      നിർത്തുക;
    അവസ്ഥ 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
    result = Curl_bufq_cwrite(&ctx->buf, eob, strlen(eob), &n);
    എങ്കിൽ(result)
      തിരികെ result;
    ctx->processed_eos = TRUE;
  }

  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->buf)) {
    result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  അല്ലെങ്കിൽ
    *pnread = 0;

  എങ്കിൽ(ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(data, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(data, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (പൂർണ്ണസംഖ്യ)result, *pnread, *peos));
  തിരികെ result;
}

സ്ഥിതം curl_off_t cr_eob_total_length(ഘടന Curl_easy *data,
                                      ഘടന Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  തിരികെ -1;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_crtype cr_eob = {
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
  വലുപ്പം(ഘടന cr_eob_ctx)
};

സ്ഥിതം CURLcode cr_eob_add(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *reader = ശൂന്യ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_eob, CURL_CR_CONTENT_ENCODE);
  എങ്കിൽ(!result)
    result = Curl_creader_add(data, reader);

  എങ്കിൽ(result && reader)
    Curl_creader_free(data, reader);
  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
സ്ഥിതം bool smtp_endofresp(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                           സ്ഥിരം അക്ഷരം *വരി, size_t len, പൂർണ്ണസംഖ്യ *resp)
{
  ഘടന smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool end = FALSE;
  (ശൂന്യം)data;

  DEBUGASSERT(smtpc);
  എങ്കിൽ(!smtpc)
    തിരികെ FALSE;

  /* Nothing for us */
  എങ്കിൽ(len < 4 || !ISDIGIT(വരി[0]) || !ISDIGIT(വരി[1]) || !ISDIGIT(വരി[2]))
    തിരികെ FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  എങ്കിൽ(വരി[3] == ' ' || len == 5) {
    അക്ഷരം tmpline[6];
    curl_off_t code;
    സ്ഥിരം അക്ഷരം *p = tmpline;
    end = TRUE;
    memcpy(tmpline, വരി, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    എങ്കിൽ(curlx_str_സംഖ്യ(&p, &code, len == 5 ? 99999 : 999))
      തിരികെ FALSE;
    *resp = (പൂർണ്ണസംഖ്യ)code;

    /* Make sure real server never sends internal value */
    എങ്കിൽ(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  അല്ലെങ്കിൽ എങ്കിൽ(വരി[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    end = TRUE;
    *resp = 1;  /* Internal response code */
  }

  തിരികെ end;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
സ്ഥിതം CURLcode smtp_get_message(ഘടന Curl_easy *data, ഘടന bufref *out)
{
  ഘടന smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  അക്ഷരം *message;
  size_t len;

  എങ്കിൽ(!smtpc)
    തിരികെ CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  എങ്കിൽ(len > 4) {
    /* Find the start of the message */
    len -= 4;
    വേണ്ടി(message += 4; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    ആകുമ്പോൾ(len--)
      എങ്കിൽ(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        നിർത്തുക;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ശൂന്യ);
  }
  അല്ലെങ്കിൽ
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ശൂന്യ);

  തിരികെ CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
സ്ഥിതം ശൂന്യം smtp_state(ഘടന Curl_easy *data,
                       ഘടന smtp_conn *smtpc,
                       smtpstate newstate)
{
#ifdef CURLVERBOSE
  /* for debug purposes */
  സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം names[] = {
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

  എങ്കിൽ(smtpc->state != newstate)
    CURL_TRC_SMTP(data, "state change from %s to %s",
                  names[smtpc->state], names[newstate]);
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
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
സ്ഥിതം CURLcode smtp_perform_ehlo(ഘടന Curl_easy *data,
                                  ഘടന smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "EHLO %s", smtpc->domain);

  എങ്കിൽ(!result)
    smtp_state(data, smtpc, SMTP_EHLO);

  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
സ്ഥിതം CURLcode smtp_perform_helo(ഘടന Curl_easy *data,
                                  ഘടന smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "HELO %s", smtpc->domain);

  എങ്കിൽ(!result)
    smtp_state(data, smtpc, SMTP_HELO);

  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
സ്ഥിതം CURLcode smtp_perform_starttls(ഘടന Curl_easy *data,
                                      ഘടന smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "STARTTLS");

  എങ്കിൽ(!result)
    smtp_state(data, smtpc, SMTP_STARTTLS);

  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
സ്ഥിതം CURLcode smtp_perform_upgrade_tls(ഘടന Curl_easy *data,
                                         ഘടന smtp_conn *smtpc)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  ഘടന connectdata *conn = data->conn;
  CURLcode result;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  എങ്കിൽ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    എങ്കിൽ(result)
      പോകുക out;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (പൂർണ്ണസംഖ്യ)result, ssldone));
  എങ്കിൽ(!result && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    result = smtp_perform_ehlo(data, smtpc);
  }
out:
  തിരികെ result;
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)smtpc;
  തിരികെ CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
സ്ഥിതം CURLcode smtp_perform_auth(ഘടന Curl_easy *data,
                                  സ്ഥിരം അക്ഷരം *mech,
                                  സ്ഥിരം ഘടന bufref *initresp)
{
  CURLcode result = CURLE_OK;
  ഘടന smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  സ്ഥിരം അക്ഷരം *ir = Curl_bufref_ptr(initresp);

  എങ്കിൽ(!smtpc)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  അല്ലെങ്കിൽ {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s", mech);
  }

  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
സ്ഥിതം CURLcode smtp_continue_auth(ഘടന Curl_easy *data,
                                   സ്ഥിരം അക്ഷരം *mech,
                                   സ്ഥിരം ഘടന bufref *resp)
{
  ഘടന smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (ശൂന്യം)mech;
  എങ്കിൽ(!smtpc)
    തിരികെ CURLE_FAILED_INIT;
  തിരികെ Curl_pp_sendf(data, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
സ്ഥിതം CURLcode smtp_cancel_auth(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *mech)
{
  ഘടന smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (ശൂന്യം)mech;
  എങ്കിൽ(!smtpc)
    തിരികെ CURLE_FAILED_INIT;
  തിരികെ Curl_pp_sendf(data, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
സ്ഥിതം CURLcode smtp_perform_authentication(ഘടന Curl_easy *data,
                                            ഘടന smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  എങ്കിൽ(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, data)) {
    smtp_state(data, smtpc, SMTP_STOP);
    തിരികെ result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&smtpc->sasl, data, FALSE, &progress);

  എങ്കിൽ(!result) {
    എങ്കിൽ(progress == SASL_INPROGRESS)
      smtp_state(data, smtpc, SMTP_AUTH);
    അല്ലെങ്കിൽ
      result = Curl_sasl_is_blocked(&smtpc->sasl, data);
  }

  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
സ്ഥിതം CURLcode smtp_perform_command(ഘടന Curl_easy *data,
                                     ഘടന smtp_conn *smtpc,
                                     ഘടന SMTP *smtp)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    എങ്കിൽ(!smtp->custom || !smtp->custom[0]) {
      അക്ഷരം *address = ശൂന്യ;
      ഘടന hostname host = { ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ };
      സ്ഥിരം അക്ഷരം *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, smtp->rcpt->data,
                                  &address, &host, &suffix);
      എങ്കിൽ(result)
        തിരികെ result;

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
    അല്ലെങ്കിൽ {
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
  അല്ലെങ്കിൽ
    /* Send the non-recipient based command such as HELP */
    result = Curl_pp_sendf(data, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  എങ്കിൽ(!result)
    smtp_state(data, smtpc, SMTP_COMMAND);

  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
സ്ഥിതം CURLcode smtp_perform_mail(ഘടന Curl_easy *data,
                                  ഘടന smtp_conn *smtpc,
                                  ഘടന SMTP *smtp)
{
  അക്ഷരം *from = ശൂന്യ;
  അക്ഷരം *auth = ശൂന്യ;
  അക്ഷരം *size = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *str;
  CURLcode result = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_FROM);
  എങ്കിൽ(str) {
    അക്ഷരം *address = ശൂന്യ;
    ഘടന hostname host = { ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ };
    സ്ഥിരം അക്ഷരം *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    result = smtp_parse_address(data, str, &address, &host, &suffix);
    എങ്കിൽ(result)
      പോകുക out;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(address) ||
            !Curl_is_ASCII_name(host.name));

    എങ്കിൽ(host.name) {
      from = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    അല്ലെങ്കിൽ
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", address, suffix);

    curlx_free(address);
  }
  അല്ലെങ്കിൽ
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  എങ്കിൽ(!from) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_AUTH);
  എങ്കിൽ(str && smtpc->sasl.authused) {
    എങ്കിൽ(str[0] != '\0') {
      അക്ഷരം *address = ശൂന്യ;
      ഘടന hostname host = { ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ };
      സ്ഥിരം അക്ഷരം *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, str, &address, &host, &suffix);
      എങ്കിൽ(result)
        പോകുക out;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      എങ്കിൽ(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(address) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      എങ്കിൽ(host.name) {
        auth = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      അല്ലെങ്കിൽ
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", address, suffix);
      curlx_free(address);
    }
    അല്ലെങ്കിൽ
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    എങ്കിൽ(!auth) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  എങ്കിൽ(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned പൂർണ്ണസംഖ്യ)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, ശൂന്യ,
                                       ശൂന്യ, MIMESTRATEGY_MAIL);

    എങ്കിൽ(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    എങ്കിൽ(!result)
      result = Curl_creader_set_mime(data, postp);
    എങ്കിൽ(result)
      പോകുക out;
    data->state.infilesize = Curl_creader_total_length(data);
  }
  അല്ലെങ്കിൽ
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    എങ്കിൽ(result)
      പോകുക out;
  }

  /* Calculate the optional SIZE parameter */
  എങ്കിൽ(smtpc->size_supported && data->state.infilesize > 0) {
    size = curl_maprintf("%" FMT_OFF_T, data->state.infilesize);

    എങ്കിൽ(!size) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  എങ്കിൽ(smtpc->utf8_supported && !utf8) {
    ഘടന curl_slist *rcpt = smtp->rcpt;

    ആകുമ്പോൾ(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      എങ്കിൽ(!Curl_is_ASCII_name(rcpt->data))
        utf8 = TRUE;

      rcpt = rcpt->next;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  result = cr_eob_add(data);
  എങ്കിൽ(result)
    പോകുക out;

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

  എങ്കിൽ(!result)
    smtp_state(data, smtpc, SMTP_MAIL);

  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
സ്ഥിതം CURLcode smtp_perform_rcpt_to(ഘടന Curl_easy *data,
                                     ഘടന smtp_conn *smtpc,
                                     ഘടന SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  അക്ഷരം *address = ശൂന്യ;
  ഘടന hostname host = { ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ };
  സ്ഥിരം അക്ഷരം *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  result = smtp_parse_address(data, smtp->rcpt->data,
                              &address, &host, &suffix);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Send the RCPT TO command */
  എങ്കിൽ(host.name)
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           address, host.name, suffix);
  അല്ലെങ്കിൽ
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s>%s",
                           address, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(address);

  എങ്കിൽ(!result)
    smtp_state(data, smtpc, SMTP_RCPT);

  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
സ്ഥിതം CURLcode smtp_perform_quit(ഘടന Curl_easy *data,
                                  ഘടന smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "QUIT");

  എങ്കിൽ(!result)
    smtp_state(data, smtpc, SMTP_QUIT);

  തിരികെ result;
}

/* For the initial server greeting */
സ്ഥിതം CURLcode smtp_state_servergreet_resp(ഘടന Curl_easy *data,
                                            ഘടന smtp_conn *smtpc,
                                            പൂർണ്ണസംഖ്യ smtpcode,
                                            smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  എങ്കിൽ(smtpcode / 100 != 2) {
    failf(data, "Got unexpected smtp-server response: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  അല്ലെങ്കിൽ
    result = smtp_perform_ehlo(data, smtpc);

  തിരികെ result;
}

/* For STARTTLS responses */
സ്ഥിതം CURLcode smtp_state_starttls_resp(ഘടന Curl_easy *data,
                                         ഘടന smtp_conn *smtpc,
                                         പൂർണ്ണസംഖ്യ smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  /* Pipelining in response is forbidden. */
  എങ്കിൽ(smtpc->pp.overflow)
    തിരികെ CURLE_WEIRD_SERVER_REPLY;

  എങ്കിൽ(smtpcode != 220) {
    എങ്കിൽ(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied, code %d", smtpcode);
      result = CURLE_USE_SSL_FAILED;
    }
    അല്ലെങ്കിൽ
      result = smtp_perform_authentication(data, smtpc);
  }
  അല്ലെങ്കിൽ
    smtp_state(data, smtpc, SMTP_UPGRADETLS);

  തിരികെ result;
}

/* For EHLO responses */
സ്ഥിതം CURLcode smtp_state_ehlo_resp(ഘടന Curl_easy *data,
                                     ഘടന smtp_conn *smtpc,
                                     പൂർണ്ണസംഖ്യ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *വരി = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (ശൂന്യം)instate;

  എങ്കിൽ(smtpcode / 100 != 2 && smtpcode != 1) {
    എങ്കിൽ(data->set.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(data->conn, FIRSTSOCKET))
      result = smtp_perform_helo(data, smtpc);
    അല്ലെങ്കിൽ {
      failf(data, "Remote access denied: %d", smtpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(len >= 4) {
    വരി += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    എങ്കിൽ(len >= 8 && curl_strnequal(വരി, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    അല്ലെങ്കിൽ എങ്കിൽ(len >= 4 && curl_strnequal(വരി, "SIZE", 4))
      smtpc->size_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    അല്ലെങ്കിൽ എങ്കിൽ(len >= 8 && curl_strnequal(വരി, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    അല്ലെങ്കിൽ എങ്കിൽ(len >= 5 && curl_strnequal(വരി, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      വരി += 5;
      len -= 5;

      /* Loop through the data line */
      വേണ്ടി(;;) {
        size_t llen;
        size_t wordlen;
        unsigned short mechbit;

        ആകുമ്പോൾ(len && (ISBLANK(*വരി) || ISNEWLINE(*വരി))) {
          വരി++;
          len--;
        }

        എങ്കിൽ(!len)
          നിർത്തുക;

        /* Extract the word */
        വേണ്ടി(wordlen = 0; wordlen < len && !ISBLANK(വരി[wordlen]) &&
              !ISNEWLINE(വരി[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(വരി, wordlen, &llen);
        എങ്കിൽ(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        വരി += wordlen;
        len -= wordlen;
      }
    }

    എങ്കിൽ(smtpcode != 1) {
      എങ്കിൽ(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        എങ്കിൽ(smtpc->tls_supported)
          /* Switch to TLS connection now */
          result = smtp_perform_starttls(data, smtpc);
        അല്ലെങ്കിൽ എങ്കിൽ(data->set.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          result = smtp_perform_authentication(data, smtpc);
        അല്ലെങ്കിൽ {
          failf(data, "STARTTLS not supported.");
          result = CURLE_USE_SSL_FAILED;
        }
      }
      അല്ലെങ്കിൽ
        result = smtp_perform_authentication(data, smtpc);
    }
  }
  അല്ലെങ്കിൽ {
    failf(data, "Unexpectedly short EHLO response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  തിരികെ result;
}

/* For HELO responses */
സ്ഥിതം CURLcode smtp_state_helo_resp(ഘടന Curl_easy *data,
                                     ഘടന smtp_conn *smtpc,
                                     പൂർണ്ണസംഖ്യ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  എങ്കിൽ(smtpcode / 100 != 2) {
    failf(data, "Remote access denied: %d", smtpcode);
    result = CURLE_REMOTE_ACCESS_DENIED;
  }
  അല്ലെങ്കിൽ
    /* End of connect phase */
    smtp_state(data, smtpc, SMTP_STOP);

  തിരികെ result;
}

/* For SASL authentication responses */
സ്ഥിതം CURLcode smtp_state_auth_resp(ഘടന Curl_easy *data,
                                     ഘടന smtp_conn *smtpc,
                                     പൂർണ്ണസംഖ്യ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (ശൂന്യം)instate;

  result = Curl_sasl_continue(&smtpc->sasl, data, smtpcode, &progress);
  എങ്കിൽ(!result)
    തിരഞ്ഞെടുപ്പ്(progress) {
    അവസ്ഥ SASL_DONE:
      smtp_state(data, smtpc, SMTP_STOP);  /* Authenticated */
      നിർത്തുക;
    അവസ്ഥ SASL_IDLE:            /* No mechanism left after cancellation */
      failf(data, "Authentication cancelled");
      result = CURLE_LOGIN_DENIED;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }

  തിരികെ result;
}

/* For command responses */
സ്ഥിതം CURLcode smtp_state_command_resp(ഘടന Curl_easy *data,
                                        ഘടന smtp_conn *smtpc,
                                        ഘടന SMTP *smtp,
                                        പൂർണ്ണസംഖ്യ smtpcode,
                                        smtpstate instate)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *വരി = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (ശൂന്യം)instate;

  എങ്കിൽ((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(data, "Command failed: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(!data->req.no_body)
      result = Curl_client_write(data, CLIENTWRITE_BODY, വരി, len);

    എങ്കിൽ(!result && (smtpcode != 1)) {
      എങ്കിൽ(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->next;

        എങ്കിൽ(smtp->rcpt) {
          /* Send the next command */
          result = smtp_perform_command(data, smtpc, smtp);
        }
        അല്ലെങ്കിൽ
          /* End of DO phase */
          smtp_state(data, smtpc, SMTP_STOP);
      }
      അല്ലെങ്കിൽ
        /* End of DO phase */
        smtp_state(data, smtpc, SMTP_STOP);
    }
  }

  തിരികെ result;
}

/* For MAIL responses */
സ്ഥിതം CURLcode smtp_state_mail_resp(ഘടന Curl_easy *data,
                                     ഘടന smtp_conn *smtpc,
                                     ഘടന SMTP *smtp,
                                     പൂർണ്ണസംഖ്യ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  എങ്കിൽ(smtpcode / 100 != 2) {
    failf(data, "MAIL failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  അല്ലെങ്കിൽ
    /* Start the RCPT TO command */
    result = smtp_perform_rcpt_to(data, smtpc, smtp);

  തിരികെ result;
}

/* For RCPT responses */
സ്ഥിതം CURLcode smtp_state_rcpt_resp(ഘടന Curl_easy *data,
                                     ഘടന smtp_conn *smtpc,
                                     ഘടന SMTP *smtp,
                                     പൂർണ്ണസംഖ്യ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (ശൂന്യം)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !data->set.mail_rcpt_allowfails);

  എങ്കിൽ(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_last_error = smtpcode;

    എങ്കിൽ(is_smtp_blocking_err) {
      failf(data, "RCPT failed: %d", smtpcode);
      result = CURLE_SEND_ERROR;
    }
  }
  അല്ലെങ്കിൽ {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  എങ്കിൽ(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->next;

    എങ്കിൽ(smtp->rcpt)
      /* Send the next RCPT TO command */
      result = smtp_perform_rcpt_to(data, smtpc, smtp);
    അല്ലെങ്കിൽ {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      എങ്കിൽ(!smtp->rcpt_had_ok) {
        failf(data, "RCPT failed: %d (last error)", smtp->rcpt_last_error);
        result = CURLE_SEND_ERROR;
      }
      അല്ലെങ്കിൽ {
        /* Send the DATA command */
        result = Curl_pp_sendf(data, &smtpc->pp, "%s", "DATA");

        എങ്കിൽ(!result)
          smtp_state(data, smtpc, SMTP_DATA);
      }
    }
  }

  തിരികെ result;
}

/* For DATA response */
സ്ഥിതം CURLcode smtp_state_data_resp(ഘടന Curl_easy *data,
                                     ഘടന smtp_conn *smtpc,
                                     പൂർണ്ണസംഖ്യ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  എങ്കിൽ(smtpcode != 354) {
    failf(data, "DATA failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  അല്ലെങ്കിൽ {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* SMTP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(data, smtpc, SMTP_STOP);
  }

  തിരികെ result;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
സ്ഥിതം CURLcode smtp_state_postdata_resp(ഘടന Curl_easy *data,
                                         ഘടന smtp_conn *smtpc,
                                         പൂർണ്ണസംഖ്യ smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;

  (ശൂന്യം)instate;

  എങ്കിൽ(smtpcode != 250)
    result = CURLE_WEIRD_SERVER_REPLY;

  /* End of DONE phase */
  smtp_state(data, smtpc, SMTP_STOP);

  തിരികെ result;
}

സ്ഥിതം CURLcode smtp_pp_statemachine(ഘടന Curl_easy *data,
                                     ഘടന connectdata *conn)
{
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ smtpcode;
  ഘടന smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  ഘടന SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  size_t nread = 0;

  എങ്കിൽ(!smtpc || !smtp)
    തിരികെ CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  എങ്കിൽ(smtpc->state == SMTP_UPGRADETLS) {
    result = smtp_perform_upgrade_tls(data, smtpc);
    എങ്കിൽ(result || (smtpc->state == SMTP_UPGRADETLS))
      തിരികെ result;
  }

  /* Flush any data that needs to be sent */
  എങ്കിൽ(smtpc->pp.sendleft)
    തിരികെ Curl_pp_flushsend(data, &smtpc->pp);

  ചെയ്യുക {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    എങ്കിൽ(result)
      തിരികെ result;

    /* Store the latest response for later retrieval if necessary */
    എങ്കിൽ(smtpc->state != SMTP_QUIT && smtpcode != 1)
      data->info.httpcode = smtpcode;

    എങ്കിൽ(!smtpcode)
      നിർത്തുക;

    /* We have now received a full SMTP server response */
    തിരഞ്ഞെടുപ്പ്(smtpc->state) {
    അവസ്ഥ SMTP_SERVERGREET:
      result = smtp_state_servergreet_resp(data, smtpc,
                                           smtpcode, smtpc->state);
      നിർത്തുക;

    അവസ്ഥ SMTP_EHLO:
      result = smtp_state_ehlo_resp(data, smtpc, smtpcode, smtpc->state);
      നിർത്തുക;

    അവസ്ഥ SMTP_HELO:
      result = smtp_state_helo_resp(data, smtpc, smtpcode, smtpc->state);
      നിർത്തുക;

    അവസ്ഥ SMTP_STARTTLS:
      result = smtp_state_starttls_resp(data, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      എങ്കിൽ(!result && (smtpc->state == SMTP_UPGRADETLS))
        പോകുക upgrade_tls;
      നിർത്തുക;

    അവസ്ഥ SMTP_AUTH:
      result = smtp_state_auth_resp(data, smtpc, smtpcode, smtpc->state);
      നിർത്തുക;

    അവസ്ഥ SMTP_COMMAND:
      result = smtp_state_command_resp(data, smtpc, smtp,
                                       smtpcode, smtpc->state);
      നിർത്തുക;

    അവസ്ഥ SMTP_MAIL:
      result = smtp_state_mail_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      നിർത്തുക;

    അവസ്ഥ SMTP_RCPT:
      result = smtp_state_rcpt_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      നിർത്തുക;

    അവസ്ഥ SMTP_DATA:
      result = smtp_state_data_resp(data, smtpc, smtpcode, smtpc->state);
      നിർത്തുക;

    അവസ്ഥ SMTP_POSTDATA:
      result = smtp_state_postdata_resp(data, smtpc, smtpcode, smtpc->state);
      നിർത്തുക;

    അവസ്ഥ SMTP_QUIT:
    സ്ഥിരസ്ഥിതി:
      /* internal error */
      smtp_state(data, smtpc, SMTP_STOP);
      നിർത്തുക;
    }
  } ആകുമ്പോൾ(!result && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  തിരികെ result;
}

/* Called repeatedly until done from multi.c */
സ്ഥിതം CURLcode smtp_multi_statemach(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ഘടന smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  *done = FALSE;
  എങ്കിൽ(!smtpc)
    തിരികെ CURLE_FAILED_INIT;

  result = Curl_pp_statemach(data, &smtpc->pp, FALSE, FALSE);
  *done = (smtpc->state == SMTP_STOP);
  തിരികെ result;
}

സ്ഥിതം CURLcode smtp_block_statemach(ഘടന Curl_easy *data,
                                     ഘടന smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  ആകുമ്പോൾ(smtpc->state != SMTP_STOP && !result)
    result = Curl_pp_statemach(data, &smtpc->pp, TRUE, disconnecting);

  തിരികെ result;
}

/* For the SMTP "protocol connect" and "doing" phases only */
സ്ഥിതം CURLcode smtp_pollset(ഘടന Curl_easy *data,
                             ഘടന easy_pollset *ps)
{
  ഘടന smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  തിരികെ smtpc ? Curl_pp_pollset(data, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
സ്ഥിതം സ്ഥിരം ഘടന SASLproto saslsmtp = {
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
സ്ഥിതം CURLcode smtp_connect(ഘടന Curl_easy *data, bool *done)
{
  ഘടന smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  എങ്കിൽ(!smtpc)
    തിരികെ CURLE_FAILED_INIT;

  PINGPONG_SETUP(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, data, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = smtp_parse_url_options(data->conn, smtpc);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Parse the URL path */
  result = smtp_parse_url_path(data, smtpc);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Start off waiting for the server greeting response */
  smtp_state(data, smtpc, SMTP_SERVERGREET);

  result = smtp_multi_statemach(data, done);

  തിരികെ result;
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
സ്ഥിതം CURLcode smtp_done(ഘടന Curl_easy *data, CURLcode status,
                          bool premature)
{
  ഘടന smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);

  (ശൂന്യം)premature;

  എങ്കിൽ(!smtpc)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(!smtp)
    തിരികെ CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  എങ്കിൽ(status) {
    CURL_TRC_M(data, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!data->set.connect_only && data->set.mail_rcpt &&
          (data->state.upload || IS_MIME_POST(data))) {

    smtp_state(data, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    result = smtp_block_statemach(data, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(data, "smtp_done(status=%d, premature=%d) -> %d",
                (പൂർണ്ണസംഖ്യ)status, premature, (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
സ്ഥിതം CURLcode smtp_perform(ഘടന Curl_easy *data,
                             ഘടന smtp_conn *smtpc,
                             ഘടന SMTP *smtp,
                             bool *connected,
                             bool *dophase_done)
{
  /* This is SMTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_SMTP(data, "smtp_perform(), start");

  എങ്കിൽ(data->req.no_body) {
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
  എങ്കിൽ((data->state.upload || IS_MIME_POST(data)) && data->set.mail_rcpt)
    /* MAIL transfer */
    result = smtp_perform_mail(data, smtpc, smtp);
  അല്ലെങ്കിൽ
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    result = smtp_perform_command(data, smtpc, smtp);

  എങ്കിൽ(result)
    പോകുക out;

  /* Run the state-machine */
  result = smtp_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

out:
  CURL_TRC_SMTP(data, "smtp_perform() -> %d, connected=%d, done=%d",
                (പൂർണ്ണസംഖ്യ)result, *connected, *dophase_done);
  തിരികെ result;
}

/* Call this when the DO phase has completed */
സ്ഥിതം CURLcode smtp_dophase_done(ഘടന Curl_easy *data,
                                  ഘടന SMTP *smtp,
                                  bool connected)
{
  (ശൂന്യം)connected;

  എങ്കിൽ(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  തിരികെ CURLE_OK;
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
സ്ഥിതം CURLcode smtp_regular_transfer(ഘടന Curl_easy *data,
                                      ഘടന smtp_conn *smtpc,
                                      ഘടന SMTP *smtp,
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
  എങ്കിൽ(!result && *dophase_done)
    result = smtp_dophase_done(data, smtp, connected);

  CURL_TRC_SMTP(data, "smtp_regular_transfer() -> %d, done=%d",
                (പൂർണ്ണസംഖ്യ)result, *dophase_done);
  തിരികെ result;
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
സ്ഥിതം CURLcode smtp_do(ഘടന Curl_easy *data, bool *done)
{
  ഘടന smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ഘടന SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  *done = FALSE; /* default to false */
  എങ്കിൽ(!smtpc || !smtp)
    തിരികെ CURLE_FAILED_INIT;

  /* Parse the custom request */
  result = smtp_parse_custom_request(data, smtp);
  എങ്കിൽ(result)
    തിരികെ result;

  result = smtp_regular_transfer(data, smtpc, smtp, done);
  CURL_TRC_SMTP(data, "smtp_do() -> %d, done=%d", (പൂർണ്ണസംഖ്യ)result, *done);
  തിരികെ result;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
സ്ഥിതം CURLcode smtp_disconnect(ഘടന Curl_easy *data,
                                ഘടന connectdata *conn,
                                bool dead_connection)
{
  ഘടന smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  എങ്കിൽ(!smtpc)
    തിരികെ CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  എങ്കിൽ(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &smtpc->pp) &&
     !smtp_perform_quit(data, smtpc))
    (ശൂന്യം)smtp_block_statemach(data, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(data, "smtp_disconnect(), finished");
  തിരികെ CURLE_OK;
}

/* Called from multi.c while DOing */
സ്ഥിതം CURLcode smtp_doing(ഘടന Curl_easy *data, bool *dophase_done)
{
  ഘടന SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result;

  എങ്കിൽ(!smtp)
    തിരികെ CURLE_FAILED_INIT;
  result = smtp_multi_statemach(data, dophase_done);
  എങ്കിൽ(result)
    DEBUGF(infof(data, "DO phase failed"));
  അല്ലെങ്കിൽ എങ്കിൽ(*dophase_done) {
    result = smtp_dophase_done(data, smtp, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  CURL_TRC_SMTP(data, "smtp_doing() -> %d, done=%d", (പൂർണ്ണസംഖ്യ)result,
                *dophase_done);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം smtp_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന SMTP *smtp = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  curlx_free(smtp);
}

സ്ഥിതം ശൂന്യം smtp_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന smtp_conn *smtpc = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

സ്ഥിതം CURLcode smtp_setup_connection(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn)
{
  ഘടന smtp_conn *smtpc;
  ഘടന SMTP *smtp;
  CURLcode result = CURLE_OK;

  smtpc = curlx_calloc(1, വലുപ്പം(*smtpc));
  എങ്കിൽ(!smtpc ||
     Curl_conn_meta_set(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  smtp = curlx_calloc(1, വലുപ്പം(*smtp));
  എങ്കിൽ(!smtp ||
     Curl_meta_set(data, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    result = CURLE_OUT_OF_MEMORY;

out:
  CURL_TRC_SMTP(data, "smtp_setup_connection() -> %d", (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

/*
 * SMTP protocol handler.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_smtp = {
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
