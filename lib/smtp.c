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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "smtp.h"

#ifndef CURL_DISABLE_SMTP

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "pingpong.h"
#அந்தர்பாவ "mime.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "curl_gethostname.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "curl_sasl.h"
#அந்தர்பாவ "idn.h"
#அந்தர்பாவ "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
வகைவரை எண்ணிக்கை {
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
கட்டமைப்பு smtp_conn {
  கட்டமைப்பு pingpong pp;
  கட்டமைப்பு SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  எழுத்து *domain;            /* Client address/name to send in the EHLO */
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
கட்டமைப்பு SMTP {
  curl_pp_transfer transfer;
  எழுத்து *custom;            /* Custom Request */
  கட்டமைப்பு curl_slist *rcpt; /* Recipient list */
  முழு rcpt_last_error;     /* The last error received for RCPT TO command */
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
நிலையான CURLcode smtp_parse_url_options(கட்டமைப்பு connectdata *conn,
                                       கட்டமைப்பு smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *ptr = conn->options;

  போது(!result && ptr && *ptr) {
    மாறிலி எழுத்து *சாவி = ptr;
    மாறிலி எழுத்து *மதிப்பு;

    போது(*ptr && *ptr != '=')
      ptr++;

    மதிப்பு = ptr + 1;

    போது(*ptr && *ptr != ';')
      ptr++;

    எனில்(curl_strnequal(சாவி, "AUTH=", 5))
      result = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               மதிப்பு, ptr - மதிப்பு);
    இல்லையெனில்
      result = CURLE_URL_MALFORMAT;

    எனில்(*ptr == ';')
      ptr++;
  }

  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
நிலையான CURLcode smtp_parse_url_path(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  மாறிலி எழுத்து *path = &data->state.up.path[1]; /* skip leading path */
  எழுத்து localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  எனில்(!*path) {
    எனில்(!Curl_gethostname(localhost, அளவு(localhost)))
      path = localhost;
    இல்லையெனில்
      path = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  திரும்பு Curl_urldecode(path, 0, &smtpc->domain, வெற்று, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
நிலையான CURLcode smtp_parse_custom_request(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  எனில்(custom)
    result = Curl_urldecode(custom, 0, &smtp->custom, வெற்று, REJECT_CTRL);

  திரும்பு result;
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
நிலையான CURLcode smtp_parse_address(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fqma,
                                   எழுத்து **address, கட்டமைப்பு hostname *host,
                                   மாறிலி எழுத்து **suffix)
{
  CURLcode result = CURLE_OK;
  size_t length;
  எழுத்து *addressend;
  எழுத்து *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  எனில்(strpbrk(fqma, "\r\n")) {
    failf(data, "Refusing to send email address with a CR or LF");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  எனில்(!dup)
    திரும்பு CURLE_OUT_OF_MEMORY;

  எனில்(fqma[0] != '<') {
    length = strlen(dup);
    எனில்(length && dup[length - 1] == '>')
      dup[length - 1] = '\0';
  }
  இல்லையெனில் {
    addressend = strrchr(dup, '>');
    எனில்(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  எனில்(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (வெறுமை)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *address = dup;

  திரும்பு result;
}

கட்டமைப்பு cr_eob_ctx {
  கட்டமைப்பு Curl_creader super;
  கட்டமைப்பு bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

நிலையான CURLcode cr_eob_init(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_eob_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cr_eob_close(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_eob_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#define SMTP_EOB          "\r\n.\r\n"
#define SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
நிலையான CURLcode cr_eob_read(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_creader *reader,
                            எழுத்து *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  கட்டமைப்பு cr_eob_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, i, start, n;
  bool eos;

  எனில்(!ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* Get more and convert it when needed */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(data, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (முழு)result, nread, eos);
    எனில்(result)
      திரும்பு result;

    ctx->read_eos = eos;
    எனில்(nread) {
      எனில்(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        திரும்பு CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      க்கு(i = start = 0; i < nread; ++i) {
        எனில்(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          எனில்(result)
            திரும்பு result;
          result = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          எனில்(result)
            திரும்பு result;
          ctx->n_eob = 0;
          start = i;
          எனில்(data->state.infilesize > 0)
            data->state.infilesize++;
        }

        எனில்(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        எனில்(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      எனில்(start < nread) {
        result = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        எனில்(result)
          திரும்பு result;
      }
    }
  }

  *peos = FALSE;

  எனில்(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    மாறிலி எழுத்து *eob = SMTP_EOB;
    CURL_TRC_SMTP(data, "auto-ending mail body with '\\r\\n.\\r\\n'");
    தேர்வு(ctx->n_eob) {
    நிலை 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      நிறுத்து;
    நிலை 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      நிறுத்து;
    இயல்பு:
      நிறுத்து;
    }
    result = Curl_bufq_cwrite(&ctx->buf, eob, strlen(eob), &n);
    எனில்(result)
      திரும்பு result;
    ctx->processed_eos = TRUE;
  }

  எனில்(!Curl_bufq_is_empty(&ctx->buf)) {
    result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  இல்லையெனில்
    *pnread = 0;

  எனில்(ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(data, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(data, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (முழு)result, *pnread, *peos));
  திரும்பு result;
}

நிலையான curl_off_t cr_eob_total_length(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (வெறுமை)data;
  (வெறுமை)reader;
  திரும்பு -1;
}

நிலையான மாறிலி கட்டமைப்பு Curl_crtype cr_eob = {
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
  அளவு(கட்டமைப்பு cr_eob_ctx)
};

நிலையான CURLcode cr_eob_add(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *reader = வெற்று;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_eob, CURL_CR_CONTENT_ENCODE);
  எனில்(!result)
    result = Curl_creader_add(data, reader);

  எனில்(result && reader)
    Curl_creader_free(data, reader);
  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
நிலையான bool smtp_endofresp(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn,
                           மாறிலி எழுத்து *வரி, size_t len, முழு *resp)
{
  கட்டமைப்பு smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool end = FALSE;
  (வெறுமை)data;

  DEBUGASSERT(smtpc);
  எனில்(!smtpc)
    திரும்பு FALSE;

  /* Nothing for us */
  எனில்(len < 4 || !ISDIGIT(வரி[0]) || !ISDIGIT(வரி[1]) || !ISDIGIT(வரி[2]))
    திரும்பு FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  எனில்(வரி[3] == ' ' || len == 5) {
    எழுத்து tmpline[6];
    curl_off_t code;
    மாறிலி எழுத்து *p = tmpline;
    end = TRUE;
    memcpy(tmpline, வரி, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    எனில்(curlx_str_எண்(&p, &code, len == 5 ? 99999 : 999))
      திரும்பு FALSE;
    *resp = (முழு)code;

    /* Make sure real server never sends internal value */
    எனில்(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  இல்லையெனில் எனில்(வரி[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    end = TRUE;
    *resp = 1;  /* Internal response code */
  }

  திரும்பு end;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
நிலையான CURLcode smtp_get_message(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு bufref *out)
{
  கட்டமைப்பு smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  எழுத்து *message;
  size_t len;

  எனில்(!smtpc)
    திரும்பு CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  எனில்(len > 4) {
    /* Find the start of the message */
    len -= 4;
    க்கு(message += 4; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    போது(len--)
      எனில்(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        நிறுத்து;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, வெற்று);
  }
  இல்லையெனில்
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, வெற்று);

  திரும்பு CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
நிலையான வெறுமை smtp_state(கட்டமைப்பு Curl_easy *data,
                       கட்டமைப்பு smtp_conn *smtpc,
                       smtpstate newstate)
{
#ifdef CURLVERBOSE
  /* for debug purposes */
  நிலையான மாறிலி எழுத்து * மாறிலி names[] = {
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

  எனில்(smtpc->state != newstate)
    CURL_TRC_SMTP(data, "state change from %s to %s",
                  names[smtpc->state], names[newstate]);
#இல்லையெனில்
  (வெறுமை)data;
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
நிலையான CURLcode smtp_perform_ehlo(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "EHLO %s", smtpc->domain);

  எனில்(!result)
    smtp_state(data, smtpc, SMTP_EHLO);

  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
நிலையான CURLcode smtp_perform_helo(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "HELO %s", smtpc->domain);

  எனில்(!result)
    smtp_state(data, smtpc, SMTP_HELO);

  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
நிலையான CURLcode smtp_perform_starttls(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "STARTTLS");

  எனில்(!result)
    smtp_state(data, smtpc, SMTP_STARTTLS);

  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
நிலையான CURLcode smtp_perform_upgrade_tls(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு smtp_conn *smtpc)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  கட்டமைப்பு connectdata *conn = data->conn;
  CURLcode result;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  எனில்(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    எனில்(result)
      செல் out;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (முழு)result, ssldone));
  எனில்(!result && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    result = smtp_perform_ehlo(data, smtpc);
  }
out:
  திரும்பு result;
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)smtpc;
  திரும்பு CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
நிலையான CURLcode smtp_perform_auth(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *mech,
                                  மாறிலி கட்டமைப்பு bufref *initresp)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  மாறிலி எழுத்து *ir = Curl_bufref_ptr(initresp);

  எனில்(!smtpc)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  இல்லையெனில் {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s", mech);
  }

  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
நிலையான CURLcode smtp_continue_auth(கட்டமைப்பு Curl_easy *data,
                                   மாறிலி எழுத்து *mech,
                                   மாறிலி கட்டமைப்பு bufref *resp)
{
  கட்டமைப்பு smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (வெறுமை)mech;
  எனில்(!smtpc)
    திரும்பு CURLE_FAILED_INIT;
  திரும்பு Curl_pp_sendf(data, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
நிலையான CURLcode smtp_cancel_auth(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *mech)
{
  கட்டமைப்பு smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (வெறுமை)mech;
  எனில்(!smtpc)
    திரும்பு CURLE_FAILED_INIT;
  திரும்பு Curl_pp_sendf(data, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
நிலையான CURLcode smtp_perform_authentication(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  எனில்(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, data)) {
    smtp_state(data, smtpc, SMTP_STOP);
    திரும்பு result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&smtpc->sasl, data, FALSE, &progress);

  எனில்(!result) {
    எனில்(progress == SASL_INPROGRESS)
      smtp_state(data, smtpc, SMTP_AUTH);
    இல்லையெனில்
      result = Curl_sasl_is_blocked(&smtpc->sasl, data);
  }

  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
நிலையான CURLcode smtp_perform_command(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு smtp_conn *smtpc,
                                     கட்டமைப்பு SMTP *smtp)
{
  CURLcode result = CURLE_OK;

  எனில்(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    எனில்(!smtp->custom || !smtp->custom[0]) {
      எழுத்து *address = வெற்று;
      கட்டமைப்பு hostname host = { வெற்று, வெற்று, வெற்று, வெற்று };
      மாறிலி எழுத்து *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, smtp->rcpt->data,
                                  &address, &host, &suffix);
      எனில்(result)
        திரும்பு result;

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
    இல்லையெனில் {
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
  இல்லையெனில்
    /* Send the non-recipient based command such as HELP */
    result = Curl_pp_sendf(data, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  எனில்(!result)
    smtp_state(data, smtpc, SMTP_COMMAND);

  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
நிலையான CURLcode smtp_perform_mail(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு smtp_conn *smtpc,
                                  கட்டமைப்பு SMTP *smtp)
{
  எழுத்து *from = வெற்று;
  எழுத்து *auth = வெற்று;
  எழுத்து *size = வெற்று;
  மாறிலி எழுத்து *str;
  CURLcode result = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_FROM);
  எனில்(str) {
    எழுத்து *address = வெற்று;
    கட்டமைப்பு hostname host = { வெற்று, வெற்று, வெற்று, வெற்று };
    மாறிலி எழுத்து *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    result = smtp_parse_address(data, str, &address, &host, &suffix);
    எனில்(result)
      செல் out;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(address) ||
            !Curl_is_ASCII_name(host.name));

    எனில்(host.name) {
      from = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    இல்லையெனில்
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", address, suffix);

    curlx_free(address);
  }
  இல்லையெனில்
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  எனில்(!from) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_AUTH);
  எனில்(str && smtpc->sasl.authused) {
    எனில்(str[0] != '\0') {
      எழுத்து *address = வெற்று;
      கட்டமைப்பு hostname host = { வெற்று, வெற்று, வெற்று, வெற்று };
      மாறிலி எழுத்து *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, str, &address, &host, &suffix);
      எனில்(result)
        செல் out;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      எனில்(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(address) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      எனில்(host.name) {
        auth = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      இல்லையெனில்
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", address, suffix);
      curlx_free(address);
    }
    இல்லையெனில்
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    எனில்(!auth) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  எனில்(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned முழு)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, வெற்று,
                                       வெற்று, MIMESTRATEGY_MAIL);

    எனில்(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    எனில்(!result)
      result = Curl_creader_set_mime(data, postp);
    எனில்(result)
      செல் out;
    data->state.infilesize = Curl_creader_total_length(data);
  }
  இல்லையெனில்
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    எனில்(result)
      செல் out;
  }

  /* Calculate the optional SIZE parameter */
  எனில்(smtpc->size_supported && data->state.infilesize > 0) {
    size = curl_maprintf("%" FMT_OFF_T, data->state.infilesize);

    எனில்(!size) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  எனில்(smtpc->utf8_supported && !utf8) {
    கட்டமைப்பு curl_slist *rcpt = smtp->rcpt;

    போது(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      எனில்(!Curl_is_ASCII_name(rcpt->data))
        utf8 = TRUE;

      rcpt = rcpt->next;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  result = cr_eob_add(data);
  எனில்(result)
    செல் out;

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

  எனில்(!result)
    smtp_state(data, smtpc, SMTP_MAIL);

  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
நிலையான CURLcode smtp_perform_rcpt_to(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு smtp_conn *smtpc,
                                     கட்டமைப்பு SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  எழுத்து *address = வெற்று;
  கட்டமைப்பு hostname host = { வெற்று, வெற்று, வெற்று, வெற்று };
  மாறிலி எழுத்து *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  result = smtp_parse_address(data, smtp->rcpt->data,
                              &address, &host, &suffix);
  எனில்(result)
    திரும்பு result;

  /* Send the RCPT TO command */
  எனில்(host.name)
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           address, host.name, suffix);
  இல்லையெனில்
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s>%s",
                           address, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(address);

  எனில்(!result)
    smtp_state(data, smtpc, SMTP_RCPT);

  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
நிலையான CURLcode smtp_perform_quit(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "QUIT");

  எனில்(!result)
    smtp_state(data, smtpc, SMTP_QUIT);

  திரும்பு result;
}

/* For the initial server greeting */
நிலையான CURLcode smtp_state_servergreet_resp(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு smtp_conn *smtpc,
                                            முழு smtpcode,
                                            smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  எனில்(smtpcode / 100 != 2) {
    failf(data, "Got unexpected smtp-server response: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  இல்லையெனில்
    result = smtp_perform_ehlo(data, smtpc);

  திரும்பு result;
}

/* For STARTTLS responses */
நிலையான CURLcode smtp_state_starttls_resp(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு smtp_conn *smtpc,
                                         முழு smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  /* Pipelining in response is forbidden. */
  எனில்(smtpc->pp.overflow)
    திரும்பு CURLE_WEIRD_SERVER_REPLY;

  எனில்(smtpcode != 220) {
    எனில்(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied, code %d", smtpcode);
      result = CURLE_USE_SSL_FAILED;
    }
    இல்லையெனில்
      result = smtp_perform_authentication(data, smtpc);
  }
  இல்லையெனில்
    smtp_state(data, smtpc, SMTP_UPGRADETLS);

  திரும்பு result;
}

/* For EHLO responses */
நிலையான CURLcode smtp_state_ehlo_resp(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு smtp_conn *smtpc,
                                     முழு smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *வரி = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (வெறுமை)instate;

  எனில்(smtpcode / 100 != 2 && smtpcode != 1) {
    எனில்(data->set.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(data->conn, FIRSTSOCKET))
      result = smtp_perform_helo(data, smtpc);
    இல்லையெனில் {
      failf(data, "Remote access denied: %d", smtpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  இல்லையெனில் எனில்(len >= 4) {
    வரி += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    எனில்(len >= 8 && curl_strnequal(வரி, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    இல்லையெனில் எனில்(len >= 4 && curl_strnequal(வரி, "SIZE", 4))
      smtpc->size_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    இல்லையெனில் எனில்(len >= 8 && curl_strnequal(வரி, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    இல்லையெனில் எனில்(len >= 5 && curl_strnequal(வரி, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      வரி += 5;
      len -= 5;

      /* Loop through the data line */
      க்கு(;;) {
        size_t llen;
        size_t wordlen;
        unsigned short mechbit;

        போது(len && (ISBLANK(*வரி) || ISNEWLINE(*வரி))) {
          வரி++;
          len--;
        }

        எனில்(!len)
          நிறுத்து;

        /* Extract the word */
        க்கு(wordlen = 0; wordlen < len && !ISBLANK(வரி[wordlen]) &&
              !ISNEWLINE(வரி[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(வரி, wordlen, &llen);
        எனில்(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        வரி += wordlen;
        len -= wordlen;
      }
    }

    எனில்(smtpcode != 1) {
      எனில்(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        எனில்(smtpc->tls_supported)
          /* Switch to TLS connection now */
          result = smtp_perform_starttls(data, smtpc);
        இல்லையெனில் எனில்(data->set.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          result = smtp_perform_authentication(data, smtpc);
        இல்லையெனில் {
          failf(data, "STARTTLS not supported.");
          result = CURLE_USE_SSL_FAILED;
        }
      }
      இல்லையெனில்
        result = smtp_perform_authentication(data, smtpc);
    }
  }
  இல்லையெனில் {
    failf(data, "Unexpectedly short EHLO response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  திரும்பு result;
}

/* For HELO responses */
நிலையான CURLcode smtp_state_helo_resp(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு smtp_conn *smtpc,
                                     முழு smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  எனில்(smtpcode / 100 != 2) {
    failf(data, "Remote access denied: %d", smtpcode);
    result = CURLE_REMOTE_ACCESS_DENIED;
  }
  இல்லையெனில்
    /* End of connect phase */
    smtp_state(data, smtpc, SMTP_STOP);

  திரும்பு result;
}

/* For SASL authentication responses */
நிலையான CURLcode smtp_state_auth_resp(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு smtp_conn *smtpc,
                                     முழு smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (வெறுமை)instate;

  result = Curl_sasl_continue(&smtpc->sasl, data, smtpcode, &progress);
  எனில்(!result)
    தேர்வு(progress) {
    நிலை SASL_DONE:
      smtp_state(data, smtpc, SMTP_STOP);  /* Authenticated */
      நிறுத்து;
    நிலை SASL_IDLE:            /* No mechanism left after cancellation */
      failf(data, "Authentication cancelled");
      result = CURLE_LOGIN_DENIED;
      நிறுத்து;
    இயல்பு:
      நிறுத்து;
    }

  திரும்பு result;
}

/* For command responses */
நிலையான CURLcode smtp_state_command_resp(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு smtp_conn *smtpc,
                                        கட்டமைப்பு SMTP *smtp,
                                        முழு smtpcode,
                                        smtpstate instate)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *வரி = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (வெறுமை)instate;

  எனில்((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(data, "Command failed: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  இல்லையெனில் {
    எனில்(!data->req.no_body)
      result = Curl_client_write(data, CLIENTWRITE_BODY, வரி, len);

    எனில்(!result && (smtpcode != 1)) {
      எனில்(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->next;

        எனில்(smtp->rcpt) {
          /* Send the next command */
          result = smtp_perform_command(data, smtpc, smtp);
        }
        இல்லையெனில்
          /* End of DO phase */
          smtp_state(data, smtpc, SMTP_STOP);
      }
      இல்லையெனில்
        /* End of DO phase */
        smtp_state(data, smtpc, SMTP_STOP);
    }
  }

  திரும்பு result;
}

/* For MAIL responses */
நிலையான CURLcode smtp_state_mail_resp(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு smtp_conn *smtpc,
                                     கட்டமைப்பு SMTP *smtp,
                                     முழு smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  எனில்(smtpcode / 100 != 2) {
    failf(data, "MAIL failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  இல்லையெனில்
    /* Start the RCPT TO command */
    result = smtp_perform_rcpt_to(data, smtpc, smtp);

  திரும்பு result;
}

/* For RCPT responses */
நிலையான CURLcode smtp_state_rcpt_resp(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு smtp_conn *smtpc,
                                     கட்டமைப்பு SMTP *smtp,
                                     முழு smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (வெறுமை)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !data->set.mail_rcpt_allowfails);

  எனில்(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_last_error = smtpcode;

    எனில்(is_smtp_blocking_err) {
      failf(data, "RCPT failed: %d", smtpcode);
      result = CURLE_SEND_ERROR;
    }
  }
  இல்லையெனில் {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  எனில்(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->next;

    எனில்(smtp->rcpt)
      /* Send the next RCPT TO command */
      result = smtp_perform_rcpt_to(data, smtpc, smtp);
    இல்லையெனில் {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      எனில்(!smtp->rcpt_had_ok) {
        failf(data, "RCPT failed: %d (last error)", smtp->rcpt_last_error);
        result = CURLE_SEND_ERROR;
      }
      இல்லையெனில் {
        /* Send the DATA command */
        result = Curl_pp_sendf(data, &smtpc->pp, "%s", "DATA");

        எனில்(!result)
          smtp_state(data, smtpc, SMTP_DATA);
      }
    }
  }

  திரும்பு result;
}

/* For DATA response */
நிலையான CURLcode smtp_state_data_resp(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு smtp_conn *smtpc,
                                     முழு smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  எனில்(smtpcode != 354) {
    failf(data, "DATA failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  இல்லையெனில் {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* SMTP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(data, smtpc, SMTP_STOP);
  }

  திரும்பு result;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
நிலையான CURLcode smtp_state_postdata_resp(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு smtp_conn *smtpc,
                                         முழு smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;

  (வெறுமை)instate;

  எனில்(smtpcode != 250)
    result = CURLE_WEIRD_SERVER_REPLY;

  /* End of DONE phase */
  smtp_state(data, smtpc, SMTP_STOP);

  திரும்பு result;
}

நிலையான CURLcode smtp_pp_statemachine(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு connectdata *conn)
{
  CURLcode result = CURLE_OK;
  முழு smtpcode;
  கட்டமைப்பு smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  கட்டமைப்பு SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  size_t nread = 0;

  எனில்(!smtpc || !smtp)
    திரும்பு CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  எனில்(smtpc->state == SMTP_UPGRADETLS) {
    result = smtp_perform_upgrade_tls(data, smtpc);
    எனில்(result || (smtpc->state == SMTP_UPGRADETLS))
      திரும்பு result;
  }

  /* Flush any data that needs to be sent */
  எனில்(smtpc->pp.sendleft)
    திரும்பு Curl_pp_flushsend(data, &smtpc->pp);

  செய் {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    எனில்(result)
      திரும்பு result;

    /* Store the latest response for later retrieval if necessary */
    எனில்(smtpc->state != SMTP_QUIT && smtpcode != 1)
      data->info.httpcode = smtpcode;

    எனில்(!smtpcode)
      நிறுத்து;

    /* We have now received a full SMTP server response */
    தேர்வு(smtpc->state) {
    நிலை SMTP_SERVERGREET:
      result = smtp_state_servergreet_resp(data, smtpc,
                                           smtpcode, smtpc->state);
      நிறுத்து;

    நிலை SMTP_EHLO:
      result = smtp_state_ehlo_resp(data, smtpc, smtpcode, smtpc->state);
      நிறுத்து;

    நிலை SMTP_HELO:
      result = smtp_state_helo_resp(data, smtpc, smtpcode, smtpc->state);
      நிறுத்து;

    நிலை SMTP_STARTTLS:
      result = smtp_state_starttls_resp(data, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      எனில்(!result && (smtpc->state == SMTP_UPGRADETLS))
        செல் upgrade_tls;
      நிறுத்து;

    நிலை SMTP_AUTH:
      result = smtp_state_auth_resp(data, smtpc, smtpcode, smtpc->state);
      நிறுத்து;

    நிலை SMTP_COMMAND:
      result = smtp_state_command_resp(data, smtpc, smtp,
                                       smtpcode, smtpc->state);
      நிறுத்து;

    நிலை SMTP_MAIL:
      result = smtp_state_mail_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      நிறுத்து;

    நிலை SMTP_RCPT:
      result = smtp_state_rcpt_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      நிறுத்து;

    நிலை SMTP_DATA:
      result = smtp_state_data_resp(data, smtpc, smtpcode, smtpc->state);
      நிறுத்து;

    நிலை SMTP_POSTDATA:
      result = smtp_state_postdata_resp(data, smtpc, smtpcode, smtpc->state);
      நிறுத்து;

    நிலை SMTP_QUIT:
    இயல்பு:
      /* internal error */
      smtp_state(data, smtpc, SMTP_STOP);
      நிறுத்து;
    }
  } போது(!result && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  திரும்பு result;
}

/* Called repeatedly until done from multi.c */
நிலையான CURLcode smtp_multi_statemach(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  *done = FALSE;
  எனில்(!smtpc)
    திரும்பு CURLE_FAILED_INIT;

  result = Curl_pp_statemach(data, &smtpc->pp, FALSE, FALSE);
  *done = (smtpc->state == SMTP_STOP);
  திரும்பு result;
}

நிலையான CURLcode smtp_block_statemach(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  போது(smtpc->state != SMTP_STOP && !result)
    result = Curl_pp_statemach(data, &smtpc->pp, TRUE, disconnecting);

  திரும்பு result;
}

/* For the SMTP "protocol connect" and "doing" phases only */
நிலையான CURLcode smtp_pollset(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  திரும்பு smtpc ? Curl_pp_pollset(data, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
நிலையான மாறிலி கட்டமைப்பு SASLproto saslsmtp = {
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
நிலையான CURLcode smtp_connect(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  எனில்(!smtpc)
    திரும்பு CURLE_FAILED_INIT;

  PINGPONG_SETUP(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, data, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = smtp_parse_url_options(data->conn, smtpc);
  எனில்(result)
    திரும்பு result;

  /* Parse the URL path */
  result = smtp_parse_url_path(data, smtpc);
  எனில்(result)
    திரும்பு result;

  /* Start off waiting for the server greeting response */
  smtp_state(data, smtpc, SMTP_SERVERGREET);

  result = smtp_multi_statemach(data, done);

  திரும்பு result;
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
நிலையான CURLcode smtp_done(கட்டமைப்பு Curl_easy *data, CURLcode status,
                          bool premature)
{
  கட்டமைப்பு smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);

  (வெறுமை)premature;

  எனில்(!smtpc)
    திரும்பு CURLE_FAILED_INIT;
  எனில்(!smtp)
    திரும்பு CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  எனில்(status) {
    CURL_TRC_M(data, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  இல்லையெனில் எனில்(!data->set.connect_only && data->set.mail_rcpt &&
          (data->state.upload || IS_MIME_POST(data))) {

    smtp_state(data, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    result = smtp_block_statemach(data, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(data, "smtp_done(status=%d, premature=%d) -> %d",
                (முழு)status, premature, (முழு)result);
  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
நிலையான CURLcode smtp_perform(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு smtp_conn *smtpc,
                             கட்டமைப்பு SMTP *smtp,
                             bool *connected,
                             bool *dophase_done)
{
  /* This is SMTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_SMTP(data, "smtp_perform(), start");

  எனில்(data->req.no_body) {
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
  எனில்((data->state.upload || IS_MIME_POST(data)) && data->set.mail_rcpt)
    /* MAIL transfer */
    result = smtp_perform_mail(data, smtpc, smtp);
  இல்லையெனில்
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    result = smtp_perform_command(data, smtpc, smtp);

  எனில்(result)
    செல் out;

  /* Run the state-machine */
  result = smtp_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

out:
  CURL_TRC_SMTP(data, "smtp_perform() -> %d, connected=%d, done=%d",
                (முழு)result, *connected, *dophase_done);
  திரும்பு result;
}

/* Call this when the DO phase has completed */
நிலையான CURLcode smtp_dophase_done(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு SMTP *smtp,
                                  bool connected)
{
  (வெறுமை)connected;

  எனில்(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  திரும்பு CURLE_OK;
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
நிலையான CURLcode smtp_regular_transfer(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு smtp_conn *smtpc,
                                      கட்டமைப்பு SMTP *smtp,
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
  எனில்(!result && *dophase_done)
    result = smtp_dophase_done(data, smtp, connected);

  CURL_TRC_SMTP(data, "smtp_regular_transfer() -> %d, done=%d",
                (முழு)result, *dophase_done);
  திரும்பு result;
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
நிலையான CURLcode smtp_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  கட்டமைப்பு SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  *done = FALSE; /* default to false */
  எனில்(!smtpc || !smtp)
    திரும்பு CURLE_FAILED_INIT;

  /* Parse the custom request */
  result = smtp_parse_custom_request(data, smtp);
  எனில்(result)
    திரும்பு result;

  result = smtp_regular_transfer(data, smtpc, smtp, done);
  CURL_TRC_SMTP(data, "smtp_do() -> %d, done=%d", (முழு)result, *done);
  திரும்பு result;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
நிலையான CURLcode smtp_disconnect(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn,
                                bool dead_connection)
{
  கட்டமைப்பு smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  எனில்(!smtpc)
    திரும்பு CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  எனில்(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &smtpc->pp) &&
     !smtp_perform_quit(data, smtpc))
    (வெறுமை)smtp_block_statemach(data, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(data, "smtp_disconnect(), finished");
  திரும்பு CURLE_OK;
}

/* Called from multi.c while DOing */
நிலையான CURLcode smtp_doing(கட்டமைப்பு Curl_easy *data, bool *dophase_done)
{
  கட்டமைப்பு SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result;

  எனில்(!smtp)
    திரும்பு CURLE_FAILED_INIT;
  result = smtp_multi_statemach(data, dophase_done);
  எனில்(result)
    DEBUGF(infof(data, "DO phase failed"));
  இல்லையெனில் எனில்(*dophase_done) {
    result = smtp_dophase_done(data, smtp, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  CURL_TRC_SMTP(data, "smtp_doing() -> %d, done=%d", (முழு)result,
                *dophase_done);
  திரும்பு result;
}

நிலையான வெறுமை smtp_easy_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு SMTP *smtp = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  curlx_free(smtp);
}

நிலையான வெறுமை smtp_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு smtp_conn *smtpc = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

நிலையான CURLcode smtp_setup_connection(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு smtp_conn *smtpc;
  கட்டமைப்பு SMTP *smtp;
  CURLcode result = CURLE_OK;

  smtpc = curlx_calloc(1, அளவு(*smtpc));
  எனில்(!smtpc ||
     Curl_conn_meta_set(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  smtp = curlx_calloc(1, அளவு(*smtp));
  எனில்(!smtp ||
     Curl_meta_set(data, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    result = CURLE_OUT_OF_MEMORY;

out:
  CURL_TRC_SMTP(data, "smtp_setup_connection() -> %d", (முழு)result);
  திரும்பு result;
}

/*
 * SMTP protocol handler.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_smtp = {
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
