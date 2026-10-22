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
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "smtp.h"

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SMTP

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جيڪڏهن_پورو

#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "transfer.h"
#ضمن "escape.h"
#ضمن "pingpong.h"
#ضمن "mime.h"
#ضمن "vtls/vtls.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "url.h"
#ضمن "curl_gethostname.h"
#ضمن "bufref.h"
#ضمن "curl_sasl.h"
#ضمن "idn.h"
#ضمن "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#وصف_ڪريو CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#وصف_ڪريو CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
تعريفنوع تعداد {
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
  SMTP_معلومات,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_آخري         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
بنية smtp_conn {
  بنية pingpong pp;
  بنية SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  حرف *domain;            /* Client address/name to send in the EHLO */
  ٻٽو_انگ(ssldone);            /* Is connect() over SSL done? */
  ٻٽو_انگ(tls_supported);      /* StartTLS capability supported by server */
  ٻٽو_انگ(سائيز_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  ٻٽو_انگ(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  ٻٽو_انگ(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
بنية SMTP {
  curl_pp_transfer transfer;
  حرف *custom;            /* Custom Request */
  بنية curl_slist *rcpt; /* Recipient list */
  صحيح rcpt_آخري_error;     /* The last error received for RCPT TO command */
  size_t eob;              /* Number of bytes of the EOB (End Of Body) that
                              have been received so far */
  ٻٽو_انگ(rcpt_had_ok);        /* Whether any of RCPT TO commands (depends on
                              total number of recipients) succeeded so far */
  ٻٽو_انگ(trailing_crlf);      /* Specifies if the trailing CRLF is present */
};

/***********************************************************************
 *
 * smtp_parse_url_options()
 *
 * Parse the URL login options.
 */
ساكن CURLcode smtp_parse_url_options(بنية connectdata *conn,
                                       بنية smtp_conn *smtpc)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *ptr = conn->options;

  طالما(!نتيجو && ptr && *ptr) {
    ثابت حرف *ڪنجي = ptr;
    ثابت حرف *قدر;

    طالما(*ptr && *ptr != '=')
      ptr++;

    قدر = ptr + 1;

    طالما(*ptr && *ptr != ';')
      ptr++;

    إذا(curl_strnequal(ڪنجي, "AUTH=", 5))
      نتيجو = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               قدر, ptr - قدر);
    وإلا
      نتيجو = CURLE_URL_MALFORMAT;

    إذا(*ptr == ';')
      ptr++;
  }

  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ساكن CURLcode smtp_parse_url_رستو(بنية Curl_easy *معلومات,
                                    بنية smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ثابت حرف *رستو = &معلومات->state.مٿي.رستو[1]; /* skip leading path */
  حرف localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  إذا(!*رستو) {
    إذا(!Curl_gethostname(localhost, حجم(localhost)))
      رستو = localhost;
    وإلا
      رستو = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  أرجع Curl_urldecode(رستو, 0, &smtpc->domain, عدم, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
ساكن CURLcode smtp_parse_custom_request(بنية Curl_easy *معلومات,
                                          بنية SMTP *smtp)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *custom = CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  إذا(custom)
    نتيجو = Curl_urldecode(custom, 0, &smtp->custom, عدم, REJECT_CTRL);

  أرجع نتيجو;
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
ساكن CURLcode smtp_parse_پتو(بنية Curl_easy *معلومات, ثابت حرف *fqma,
                                   حرف **پتو, بنية hostname *host,
                                   ثابت حرف **suffix)
{
  CURLcode نتيجو = CURLE_OK;
  size_t ڊيگهائي;
  حرف *addressend;
  حرف *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  إذا(strpbrk(fqma, "\r\n")) {
    failf(معلومات, "Refusing to send email address with a CR or LF");
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  إذا(!dup)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  إذا(fqma[0] != '<') {
    ڊيگهائي = لڙي_ڊيگهه(dup);
    إذا(ڊيگهائي && dup[ڊيگهائي - 1] == '>')
      dup[ڊيگهائي - 1] = '\0';
  }
  وإلا {
    addressend = strrchr(dup, '>');
    إذا(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  إذا(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (فراغ)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *پتو = dup;

  أرجع نتيجو;
}

بنية cr_eob_ctx {
  بنية Curl_creader super;
  بنية bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  ٻٽو_انگ(read_eos);  /* we read an EOS from the next reader */
  ٻٽو_انگ(processed_eos);  /* we read and processed an EOS */
  ٻٽو_انگ(eos);       /* we have returned an EOS */
};

ساكن CURLcode cr_eob_init(بنية Curl_easy *معلومات,
                            بنية Curl_creader *reader)
{
  بنية cr_eob_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_حد_بندي);
  أرجع CURLE_OK;
}

ساكن فراغ cr_eob_close(بنية Curl_easy *معلومات, بنية Curl_creader *reader)
{
  بنية cr_eob_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#وصف_ڪريو SMTP_EOB          "\r\n.\r\n"
#وصف_ڪريو SMTP_EOB_ڳوليو_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
ساكن CURLcode cr_eob_read(بنية Curl_easy *معلومات,
                            بنية Curl_creader *reader,
                            حرف *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  بنية cr_eob_ctx *ctx = reader->ctx;
  CURLcode نتيجو = CURLE_OK;
  size_t nread, i, شروعات, n;
  bool eos;

  إذا(!ctx->read_eos && Curl_bufq_is_خالي_جايون(&ctx->buf)) {
    /* Get more and convert it when needed */
    نتيجو = Curl_creader_read(معلومات, reader->ايندڙ, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(معلومات, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (صحيح)نتيجو, nread, eos);
    إذا(نتيجو)
      أرجع نتيجو;

    ctx->read_eos = eos;
    إذا(nread) {
      إذا(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        أرجع CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      لكل(i = شروعات = 0; i < nread; ++i) {
        إذا(ctx->n_eob >= SMTP_EOB_ڳوليو_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          نتيجو = Curl_bufq_cwrite(&ctx->buf, buf + شروعات, i - شروعات, &n);
          إذا(نتيجو)
            أرجع نتيجو;
          نتيجو = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          إذا(نتيجو)
            أرجع نتيجو;
          ctx->n_eob = 0;
          شروعات = i;
          إذا(معلومات->state.infilesize > 0)
            معلومات->state.infilesize++;
        }

        إذا(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        إذا(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      إذا(شروعات < nread) {
        نتيجو = Curl_bufq_cwrite(&ctx->buf, buf + شروعات, nread - شروعات, &n);
        إذا(نتيجو)
          أرجع نتيجو;
      }
    }
  }

  *peos = FALSE;

  إذا(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ثابت حرف *eob = SMTP_EOB;
    CURL_TRC_SMTP(معلومات, "auto-ending mail body with '\\r\\n.\\r\\n'");
    بدل(ctx->n_eob) {
    حالة 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      اكسر;
    حالة 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      اكسر;
    افتراضي:
      اكسر;
    }
    نتيجو = Curl_bufq_cwrite(&ctx->buf, eob, لڙي_ڊيگهه(eob), &n);
    إذا(نتيجو)
      أرجع نتيجو;
    ctx->processed_eos = TRUE;
  }

  إذا(!Curl_bufq_is_خالي_جايون(&ctx->buf)) {
    نتيجو = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  وإلا
    *pnread = 0;

  إذا(ctx->read_eos && Curl_bufq_is_خالي_جايون(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(معلومات, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(معلومات, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (صحيح)نتيجو, *pnread, *peos));
  أرجع نتيجو;
}

ساكن curl_off_t cr_eob_ڪل_ڊيگهائي(بنية Curl_easy *معلومات,
                                      بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)معلومات;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_ڪل_ڊيگهائي,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورو,
  حجم(بنية cr_eob_ctx)
};

ساكن CURLcode cr_eob_جوڙيو(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتيجو;

  نتيجو = Curl_creader_create(&reader, معلومات, &cr_eob, CURL_CR_CONTENT_ENCODE);
  إذا(!نتيجو)
    نتيجو = Curl_creader_جوڙيو(معلومات, reader);

  إذا(نتيجو && reader)
    Curl_creader_free(معلومات, reader);
  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
ساكن bool smtp_endofresp(بنية Curl_easy *معلومات, بنية connectdata *conn,
                           ثابت حرف *سٽ, size_t len, صحيح *resp)
{
  بنية smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool پڇاڙي = FALSE;
  (فراغ)معلومات;

  DEBUGASSERT(smtpc);
  إذا(!smtpc)
    أرجع FALSE;

  /* Nothing for us */
  إذا(len < 4 || !ISDIGIT(سٽ[0]) || !ISDIGIT(سٽ[1]) || !ISDIGIT(سٽ[2]))
    أرجع FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  إذا(سٽ[3] == ' ' || len == 5) {
    حرف tmpline[6];
    curl_off_t ڪوڊ;
    ثابت حرف *p = tmpline;
    پڇاڙي = TRUE;
    يادگيري_نقل(tmpline, سٽ, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    إذا(curlx_str_عدد(&p, &ڪوڊ, len == 5 ? 99999 : 999))
      أرجع FALSE;
    *resp = (صحيح)ڪوڊ;

    /* Make sure real server never sends internal value */
    إذا(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  وإلا إذا(سٽ[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    پڇاڙي = TRUE;
    *resp = 1;  /* Internal response code */
  }

  أرجع پڇاڙي;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ساكن CURLcode smtp_get_پيغام(بنية Curl_easy *معلومات, بنية bufref *ٻاهريون)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_SMTP_CONN);
  حرف *پيغام;
  size_t len;

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  پيغام = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  إذا(len > 4) {
    /* Find the start of the message */
    len -= 4;
    لكل(پيغام += 4; ISBLANK(*پيغام); پيغام++, len--)
      ;

    /* Find the end of the message */
    طالما(len--)
      إذا(!ISNEWLINE(پيغام[len]) && !ISBLANK(پيغام[len]))
        اكسر;

    /* Terminate the message */
    پيغام[++len] = '\0';
    Curl_bufref_سيٽ(ٻاهريون, پيغام, len, عدم);
  }
  وإلا
    /* junk input => zero length output */
    Curl_bufref_سيٽ(ٻاهريون, "", 0, عدم);

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
ساكن فراغ smtp_state(بنية Curl_easy *معلومات,
                       بنية smtp_conn *smtpc,
                       smtpstate newstate)
{
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  /* for debug purposes */
  ساكن ثابت حرف * ثابت نالا[] = {
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
    /*آخري*/
  };

  إذا(smtpc->state != newstate)
    CURL_TRC_SMTP(معلومات, "state change from %s to %s",
                  نالا[smtpc->state], نالا[newstate]);
#وإلا
  (فراغ)معلومات;
#جيڪڏهن_پورو

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
ساكن CURLcode smtp_perform_ehlo(بنية Curl_easy *معلومات,
                                  بنية smtp_conn *smtpc)
{
  CURLcode نتيجو = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "EHLO %s", smtpc->domain);

  إذا(!نتيجو)
    smtp_state(معلومات, smtpc, SMTP_EHLO);

  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
ساكن CURLcode smtp_perform_helo(بنية Curl_easy *معلومات,
                                  بنية smtp_conn *smtpc)
{
  CURLcode نتيجو = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "HELO %s", smtpc->domain);

  إذا(!نتيجو)
    smtp_state(معلومات, smtpc, SMTP_HELO);

  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ساكن CURLcode smtp_perform_starttls(بنية Curl_easy *معلومات,
                                      بنية smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "%s", "STARTTLS");

  إذا(!نتيجو)
    smtp_state(معلومات, smtpc, SMTP_STARTTLS);

  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساكن CURLcode smtp_perform_upgrade_tls(بنية Curl_easy *معلومات,
                                         بنية smtp_conn *smtpc)
{
#جيڪڏهن_وصف_ٿيل USE_SSL
  /* Start the SSL connection */
  بنية connectdata *conn = معلومات->conn;
  CURLcode نتيجو;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتيجو = Curl_ssl_cfilter_جوڙيو(
      معلومات, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    إذا(نتيجو)
      اذهب ٻاهريون;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  نتيجو = Curl_conn_connect(معلومات, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(معلومات, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (صحيح)نتيجو, ssldone));
  إذا(!نتيجو && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    نتيجو = smtp_perform_ehlo(معلومات, smtpc);
  }
ٻاهريون:
  أرجع نتيجو;
#وإلا
  (فراغ)معلومات;
  (فراغ)smtpc;
  أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ساكن CURLcode smtp_perform_auth(بنية Curl_easy *معلومات,
                                  ثابت حرف *mech,
                                  ثابت بنية bufref *initresp)
{
  CURLcode نتيجو = CURLE_OK;
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_SMTP_CONN);
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  إذا(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  وإلا {
    /* Send the AUTH command */
    نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "AUTH %s", mech);
  }

  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
ساكن CURLcode smtp_continue_auth(بنية Curl_easy *معلومات,
                                   ثابت حرف *mech,
                                   ثابت بنية bufref *resp)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_SMTP_CONN);

  (فراغ)mech;
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_pp_sendf(معلومات, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ساكن CURLcode smtp_cancel_auth(بنية Curl_easy *معلومات, ثابت حرف *mech)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_SMTP_CONN);

  (فراغ)mech;
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_pp_sendf(معلومات, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
ساكن CURLcode smtp_perform_authentication(بنية Curl_easy *معلومات,
                                            بنية smtp_conn *smtpc)
{
  CURLcode نتيجو = CURLE_OK;
  saslprogress ترقي;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  إذا(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, معلومات)) {
    smtp_state(معلومات, smtpc, SMTP_STOP);
    أرجع نتيجو;
  }

  /* Calculate the SASL login details */
  نتيجو = Curl_sasl_شروعات(&smtpc->sasl, معلومات, FALSE, &ترقي);

  إذا(!نتيجو) {
    إذا(ترقي == SASL_INPROGRESS)
      smtp_state(معلومات, smtpc, SMTP_AUTH);
    وإلا
      نتيجو = Curl_sasl_is_blocked(&smtpc->sasl, معلومات);
  }

  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
ساكن CURLcode smtp_perform_command(بنية Curl_easy *معلومات,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp)
{
  CURLcode نتيجو = CURLE_OK;

  إذا(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    إذا(!smtp->custom || !smtp->custom[0]) {
      حرف *پتو = عدم;
      بنية hostname host = { عدم, عدم, عدم, عدم };
      ثابت حرف *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      نتيجو = smtp_parse_پتو(معلومات, smtp->rcpt->معلومات,
                                  &پتو, &host, &suffix);
      إذا(نتيجو)
        أرجع نتيجو;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(پتو) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "VRFY %s%s%s%s",
                             پتو,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(پتو);
    }
    وإلا {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !strcmp(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->معلومات,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  وإلا
    /* Send the non-recipient based command such as HELP */
    نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  إذا(!نتيجو)
    smtp_state(معلومات, smtpc, SMTP_COMMAND);

  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
ساكن CURLcode smtp_perform_mail(بنية Curl_easy *معلومات,
                                  بنية smtp_conn *smtpc,
                                  بنية SMTP *smtp)
{
  حرف *from = عدم;
  حرف *auth = عدم;
  حرف *سائيز = عدم;
  ثابت حرف *str;
  CURLcode نتيجو = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(معلومات, STRING_MAIL_FROM);
  إذا(str) {
    حرف *پتو = عدم;
    بنية hostname host = { عدم, عدم, عدم, عدم };
    ثابت حرف *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    نتيجو = smtp_parse_پتو(معلومات, str, &پتو, &host, &suffix);
    إذا(نتيجو)
      اذهب ٻاهريون;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(پتو) ||
            !Curl_is_ASCII_name(host.name));

    إذا(host.name) {
      from = curl_maprintf("<%s@%s>%s", پتو, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    وإلا
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", پتو, suffix);

    curlx_free(پتو);
  }
  وإلا
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  إذا(!from) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(معلومات, STRING_MAIL_AUTH);
  إذا(str && smtpc->sasl.authused) {
    إذا(str[0] != '\0') {
      حرف *پتو = عدم;
      بنية hostname host = { عدم, عدم, عدم, عدم };
      ثابت حرف *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      نتيجو = smtp_parse_پتو(معلومات, str, &پتو, &host, &suffix);
      إذا(نتيجو)
        اذهب ٻاهريون;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      إذا(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(پتو) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      إذا(host.name) {
        auth = curl_maprintf("<%s@%s>%s", پتو, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      وإلا
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", پتو, suffix);
      curlx_free(پتو);
    }
    وإلا
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    إذا(!auth) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  إذا(IS_MIME_POST(معلومات)) {
    curl_mimepart *postp = معلومات->سيٽ.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(بنانشان صحيح)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_سرا(postp, معلومات->سيٽ.سرا, 0);
    نتيجو = Curl_mime_prepare_سرا(معلومات, postp, عدم,
                                       عدم, MIMESTRATEGY_MAIL);

    إذا(!نتيجو && !Curl_checkheaders(معلومات, STRCONST("Mime-Version")))
      نتيجو = Curl_mime_جوڙيو_سرو(&postp->curlheaders, "Mime-Version: 1.0");
    إذا(!نتيجو)
      نتيجو = Curl_creader_سيٽ_mime(معلومات, postp);
    إذا(نتيجو)
      اذهب ٻاهريون;
    معلومات->state.infilesize = Curl_creader_ڪل_ڊيگهائي(معلومات);
  }
  وإلا
#جيڪڏهن_پورو
  {
    نتيجو = Curl_creader_سيٽ_fread(معلومات, معلومات->state.infilesize);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  /* Calculate the optional SIZE parameter */
  إذا(smtpc->سائيز_supported && معلومات->state.infilesize > 0) {
    سائيز = curl_maprintf("%" FMT_OFF_T, معلومات->state.infilesize);

    إذا(!سائيز) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  إذا(smtpc->utf8_supported && !utf8) {
    بنية curl_slist *rcpt = smtp->rcpt;

    طالما(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      إذا(!Curl_is_ASCII_name(rcpt->معلومات))
        utf8 = TRUE;

      rcpt = rcpt->ايندڙ;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  نتيجو = cr_eob_جوڙيو(معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  /* Send the MAIL command */
  نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         سائيز ? " SIZE=" : "", /* Optional on SIZE support  */
                         سائيز ? سائيز : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

ٻاهريون:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(سائيز);

  إذا(!نتيجو)
    smtp_state(معلومات, smtpc, SMTP_MAIL);

  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
ساكن CURLcode smtp_perform_rcpt_منزل(بنية Curl_easy *معلومات,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp)
{
  CURLcode نتيجو = CURLE_OK;
  حرف *پتو = عدم;
  بنية hostname host = { عدم, عدم, عدم, عدم };
  ثابت حرف *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  نتيجو = smtp_parse_پتو(معلومات, smtp->rcpt->معلومات,
                              &پتو, &host, &suffix);
  إذا(نتيجو)
    أرجع نتيجو;

  /* Send the RCPT TO command */
  إذا(host.name)
    نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           پتو, host.name, suffix);
  وإلا
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "RCPT TO:<%s>%s",
                           پتو, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(پتو);

  إذا(!نتيجو)
    smtp_state(معلومات, smtpc, SMTP_RCPT);

  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
ساكن CURLcode smtp_perform_quit(بنية Curl_easy *معلومات,
                                  بنية smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "%s", "QUIT");

  إذا(!نتيجو)
    smtp_state(معلومات, smtpc, SMTP_QUIT);

  أرجع نتيجو;
}

/* For the initial server greeting */
ساكن CURLcode smtp_state_servergreet_resp(بنية Curl_easy *معلومات,
                                            بنية smtp_conn *smtpc,
                                            صحيح smtpcode,
                                            smtpstate instate)
{
  CURLcode نتيجو = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode / 100 != 2) {
    failf(معلومات, "Got unexpected smtp-server response: %d", smtpcode);
    نتيجو = CURLE_WEIRD_SERVER_جواب;
  }
  وإلا
    نتيجو = smtp_perform_ehlo(معلومات, smtpc);

  أرجع نتيجو;
}

/* For STARTTLS responses */
ساكن CURLcode smtp_state_starttls_resp(بنية Curl_easy *معلومات,
                                         بنية smtp_conn *smtpc,
                                         صحيح smtpcode,
                                         smtpstate instate)
{
  CURLcode نتيجو = CURLE_OK;
  (فراغ)instate;

  /* Pipelining in response is forbidden. */
  إذا(smtpc->pp.حد_کان_مٿي)
    أرجع CURLE_WEIRD_SERVER_جواب;

  إذا(smtpcode != 220) {
    إذا(معلومات->سيٽ.use_ssl != CURLUSESSL_TRY) {
      failf(معلومات, "STARTTLS denied, code %d", smtpcode);
      نتيجو = CURLE_USE_SSL_FAILED;
    }
    وإلا
      نتيجو = smtp_perform_authentication(معلومات, smtpc);
  }
  وإلا
    smtp_state(معلومات, smtpc, SMTP_UPGRADETLS);

  أرجع نتيجو;
}

/* For EHLO responses */
ساكن CURLcode smtp_state_ehlo_resp(بنية Curl_easy *معلومات,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *سٽ = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (فراغ)instate;

  إذا(smtpcode / 100 != 2 && smtpcode != 1) {
    إذا(معلومات->سيٽ.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(معلومات->conn, FIRSTSOCKET))
      نتيجو = smtp_perform_helo(معلومات, smtpc);
    وإلا {
      failf(معلومات, "Remote access denied: %d", smtpcode);
      نتيجو = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  وإلا إذا(len >= 4) {
    سٽ += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    إذا(len >= 8 && curl_strnequal(سٽ, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    وإلا إذا(len >= 4 && curl_strnequal(سٽ, "SIZE", 4))
      smtpc->سائيز_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    وإلا إذا(len >= 8 && curl_strnequal(سٽ, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    وإلا إذا(len >= 5 && curl_strnequal(سٽ, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      سٽ += 5;
      len -= 5;

      /* Loop through the data line */
      لكل(;;) {
        size_t llen;
        size_t wordlen;
        بنانشان ننڍو mechbit;

        طالما(len && (ISBLANK(*سٽ) || ISNEWLINE(*سٽ))) {
          سٽ++;
          len--;
        }

        إذا(!len)
          اكسر;

        /* Extract the word */
        لكل(wordlen = 0; wordlen < len && !ISBLANK(سٽ[wordlen]) &&
              !ISNEWLINE(سٽ[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(سٽ, wordlen, &llen);
        إذا(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        سٽ += wordlen;
        len -= wordlen;
      }
    }

    إذا(smtpcode != 1) {
      إذا(معلومات->سيٽ.use_ssl && !Curl_conn_is_ssl(معلومات->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        إذا(smtpc->tls_supported)
          /* Switch to TLS connection now */
          نتيجو = smtp_perform_starttls(معلومات, smtpc);
        وإلا إذا(معلومات->سيٽ.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          نتيجو = smtp_perform_authentication(معلومات, smtpc);
        وإلا {
          failf(معلومات, "STARTTLS not supported.");
          نتيجو = CURLE_USE_SSL_FAILED;
        }
      }
      وإلا
        نتيجو = smtp_perform_authentication(معلومات, smtpc);
    }
  }
  وإلا {
    failf(معلومات, "Unexpectedly short EHLO response");
    نتيجو = CURLE_WEIRD_SERVER_جواب;
  }

  أرجع نتيجو;
}

/* For HELO responses */
ساكن CURLcode smtp_state_helo_resp(بنية Curl_easy *معلومات,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتيجو = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode / 100 != 2) {
    failf(معلومات, "Remote access denied: %d", smtpcode);
    نتيجو = CURLE_REMOTE_ACCESS_DENIED;
  }
  وإلا
    /* End of connect phase */
    smtp_state(معلومات, smtpc, SMTP_STOP);

  أرجع نتيجو;
}

/* For SASL authentication responses */
ساكن CURLcode smtp_state_auth_resp(بنية Curl_easy *معلومات,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتيجو = CURLE_OK;
  saslprogress ترقي;

  (فراغ)instate;

  نتيجو = Curl_sasl_continue(&smtpc->sasl, معلومات, smtpcode, &ترقي);
  إذا(!نتيجو)
    بدل(ترقي) {
    حالة SASL_پورو:
      smtp_state(معلومات, smtpc, SMTP_STOP);  /* Authenticated */
      اكسر;
    حالة SASL_IDLE:            /* No mechanism left after cancellation */
      failf(معلومات, "Authentication cancelled");
      نتيجو = CURLE_LOGIN_DENIED;
      اكسر;
    افتراضي:
      اكسر;
    }

  أرجع نتيجو;
}

/* For command responses */
ساكن CURLcode smtp_state_command_resp(بنية Curl_easy *معلومات,
                                        بنية smtp_conn *smtpc,
                                        بنية SMTP *smtp,
                                        صحيح smtpcode,
                                        smtpstate instate)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *سٽ = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (فراغ)instate;

  إذا((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(معلومات, "Command failed: %d", smtpcode);
    نتيجو = CURLE_WEIRD_SERVER_جواب;
  }
  وإلا {
    إذا(!معلومات->req.no_body)
      نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, سٽ, len);

    إذا(!نتيجو && (smtpcode != 1)) {
      إذا(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->ايندڙ;

        إذا(smtp->rcpt) {
          /* Send the next command */
          نتيجو = smtp_perform_command(معلومات, smtpc, smtp);
        }
        وإلا
          /* End of DO phase */
          smtp_state(معلومات, smtpc, SMTP_STOP);
      }
      وإلا
        /* End of DO phase */
        smtp_state(معلومات, smtpc, SMTP_STOP);
    }
  }

  أرجع نتيجو;
}

/* For MAIL responses */
ساكن CURLcode smtp_state_mail_resp(بنية Curl_easy *معلومات,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتيجو = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode / 100 != 2) {
    failf(معلومات, "MAIL failed: %d", smtpcode);
    نتيجو = CURLE_SEND_ERROR;
  }
  وإلا
    /* Start the RCPT TO command */
    نتيجو = smtp_perform_rcpt_منزل(معلومات, smtpc, smtp);

  أرجع نتيجو;
}

/* For RCPT responses */
ساكن CURLcode smtp_state_rcpt_resp(بنية Curl_easy *معلومات,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتيجو = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (فراغ)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !معلومات->سيٽ.mail_rcpt_allowfails);

  إذا(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_آخري_error = smtpcode;

    إذا(is_smtp_blocking_err) {
      failf(معلومات, "RCPT failed: %d", smtpcode);
      نتيجو = CURLE_SEND_ERROR;
    }
  }
  وإلا {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  إذا(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->ايندڙ;

    إذا(smtp->rcpt)
      /* Send the next RCPT TO command */
      نتيجو = smtp_perform_rcpt_منزل(معلومات, smtpc, smtp);
    وإلا {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      إذا(!smtp->rcpt_had_ok) {
        failf(معلومات, "RCPT failed: %d (last error)", smtp->rcpt_آخري_error);
        نتيجو = CURLE_SEND_ERROR;
      }
      وإلا {
        /* Send the DATA command */
        نتيجو = Curl_pp_sendf(معلومات, &smtpc->pp, "%s", "DATA");

        إذا(!نتيجو)
          smtp_state(معلومات, smtpc, SMTP_معلومات);
      }
    }
  }

  أرجع نتيجو;
}

/* For DATA response */
ساكن CURLcode smtp_state_معلومات_resp(بنية Curl_easy *معلومات,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتيجو = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode != 354) {
    failf(معلومات, "DATA failed: %d", smtpcode);
    نتيجو = CURLE_SEND_ERROR;
  }
  وإلا {
    /* Set the progress upload size */
    Curl_pgrs_سيٽ_Upload_سائيز(معلومات, معلومات->state.infilesize);

    /* SMTP upload */
    Curl_xfer_تياري_send(معلومات, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(معلومات, smtpc, SMTP_STOP);
  }

  أرجع نتيجو;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
ساكن CURLcode smtp_state_postdata_resp(بنية Curl_easy *معلومات,
                                         بنية smtp_conn *smtpc,
                                         صحيح smtpcode,
                                         smtpstate instate)
{
  CURLcode نتيجو = CURLE_OK;

  (فراغ)instate;

  إذا(smtpcode != 250)
    نتيجو = CURLE_WEIRD_SERVER_جواب;

  /* End of DONE phase */
  smtp_state(معلومات, smtpc, SMTP_STOP);

  أرجع نتيجو;
}

ساكن CURLcode smtp_pp_statemachine(بنية Curl_easy *معلومات,
                                     بنية connectdata *conn)
{
  CURLcode نتيجو = CURLE_OK;
  صحيح smtpcode;
  بنية smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  بنية SMTP *smtp = Curl_meta_get(معلومات, CURL_META_SMTP_EASY);
  size_t nread = 0;

  إذا(!smtpc || !smtp)
    أرجع CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  إذا(smtpc->state == SMTP_UPGRADETLS) {
    نتيجو = smtp_perform_upgrade_tls(معلومات, smtpc);
    إذا(نتيجو || (smtpc->state == SMTP_UPGRADETLS))
      أرجع نتيجو;
  }

  /* Flush any data that needs to be sent */
  إذا(smtpc->pp.sendleft)
    أرجع Curl_pp_flushsend(معلومات, &smtpc->pp);

  افعل {
    /* Read the response from the server */
    نتيجو = Curl_pp_readresp(معلومات, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    إذا(نتيجو)
      أرجع نتيجو;

    /* Store the latest response for later retrieval if necessary */
    إذا(smtpc->state != SMTP_QUIT && smtpcode != 1)
      معلومات->info.httpcode = smtpcode;

    إذا(!smtpcode)
      اكسر;

    /* We have now received a full SMTP server response */
    بدل(smtpc->state) {
    حالة SMTP_SERVERGREET:
      نتيجو = smtp_state_servergreet_resp(معلومات, smtpc,
                                           smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_EHLO:
      نتيجو = smtp_state_ehlo_resp(معلومات, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_HELO:
      نتيجو = smtp_state_helo_resp(معلومات, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_STARTTLS:
      نتيجو = smtp_state_starttls_resp(معلومات, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      إذا(!نتيجو && (smtpc->state == SMTP_UPGRADETLS))
        اذهب upgrade_tls;
      اكسر;

    حالة SMTP_AUTH:
      نتيجو = smtp_state_auth_resp(معلومات, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_COMMAND:
      نتيجو = smtp_state_command_resp(معلومات, smtpc, smtp,
                                       smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_MAIL:
      نتيجو = smtp_state_mail_resp(معلومات, smtpc, smtp, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_RCPT:
      نتيجو = smtp_state_rcpt_resp(معلومات, smtpc, smtp, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_معلومات:
      نتيجو = smtp_state_معلومات_resp(معلومات, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_POSTDATA:
      نتيجو = smtp_state_postdata_resp(معلومات, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_QUIT:
    افتراضي:
      /* internal error */
      smtp_state(معلومات, smtpc, SMTP_STOP);
      اكسر;
    }
  } طالما(!نتيجو && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  أرجع نتيجو;
}

/* Called repeatedly until done from multi.c */
ساكن CURLcode smtp_multi_statemach(بنية Curl_easy *معلومات, bool *پورو)
{
  CURLcode نتيجو = CURLE_OK;
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_SMTP_CONN);

  *پورو = FALSE;
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  نتيجو = Curl_pp_statemach(معلومات, &smtpc->pp, FALSE, FALSE);
  *پورو = (smtpc->state == SMTP_STOP);
  أرجع نتيجو;
}

ساكن CURLcode smtp_بلاڪ_statemach(بنية Curl_easy *معلومات,
                                     بنية smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode نتيجو = CURLE_OK;

  طالما(smtpc->state != SMTP_STOP && !نتيجو)
    نتيجو = Curl_pp_statemach(معلومات, &smtpc->pp, TRUE, disconnecting);

  أرجع نتيجو;
}

/* For the SMTP "protocol connect" and "doing" phases only */
ساكن CURLcode smtp_pollset(بنية Curl_easy *معلومات,
                             بنية easy_pollset *ps)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_SMTP_CONN);
  أرجع smtpc ? Curl_pp_pollset(معلومات, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
ساكن ثابت بنية SASLproto saslsmtp = {
  "smtp",               /* The service name */
  smtp_perform_auth,    /* Send authentication command */
  smtp_continue_auth,   /* Send authentication continuation */
  smtp_cancel_auth,     /* Cancel authentication */
  smtp_get_پيغام,     /* Get SASL response message */
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
ساكن CURLcode smtp_connect(بنية Curl_easy *معلومات, bool *پورو)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_SMTP_CONN);
  CURLcode نتيجو = CURLE_OK;

  *پورو = FALSE; /* default to not done yet */
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  PINGPONG_تياري(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, معلومات, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(معلومات));

  /* Parse the URL options */
  نتيجو = smtp_parse_url_options(معلومات->conn, smtpc);
  إذا(نتيجو)
    أرجع نتيجو;

  /* Parse the URL path */
  نتيجو = smtp_parse_url_رستو(معلومات, smtpc);
  إذا(نتيجو)
    أرجع نتيجو;

  /* Start off waiting for the server greeting response */
  smtp_state(معلومات, smtpc, SMTP_SERVERGREET);

  نتيجو = smtp_multi_statemach(معلومات, پورو);

  أرجع نتيجو;
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
ساكن CURLcode smtp_پورو(بنية Curl_easy *معلومات, CURLcode حالت,
                          bool premature)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_SMTP_CONN);
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية SMTP *smtp = Curl_meta_get(معلومات, CURL_META_SMTP_EASY);

  (فراغ)premature;

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;
  إذا(!smtp)
    أرجع CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  إذا(حالت) {
    CURL_TRC_M(معلومات, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    نتيجو = حالت;         /* use the already set error code */
  }
  وإلا إذا(!معلومات->سيٽ.connect_only && معلومات->سيٽ.mail_rcpt &&
          (معلومات->state.upload || IS_MIME_POST(معلومات))) {

    smtp_state(معلومات, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    نتيجو = smtp_بلاڪ_statemach(معلومات, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(معلومات, "smtp_done(status=%d, premature=%d) -> %d",
                (صحيح)حالت, premature, (صحيح)نتيجو);
  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
ساكن CURLcode smtp_perform(بنية Curl_easy *معلومات,
                             بنية smtp_conn *smtpc,
                             بنية SMTP *smtp,
                             bool *connected,
                             bool *dophase_پورو)
{
  /* This is SMTP and no proxy */
  CURLcode نتيجو = CURLE_OK;

  CURL_TRC_SMTP(معلومات, "smtp_perform(), start");

  إذا(معلومات->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_پورو = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = معلومات->سيٽ.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_آخري_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  إذا((معلومات->state.upload || IS_MIME_POST(معلومات)) && معلومات->سيٽ.mail_rcpt)
    /* MAIL transfer */
    نتيجو = smtp_perform_mail(معلومات, smtpc, smtp);
  وإلا
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    نتيجو = smtp_perform_command(معلومات, smtpc, smtp);

  إذا(نتيجو)
    اذهب ٻاهريون;

  /* Run the state-machine */
  نتيجو = smtp_multi_statemach(معلومات, dophase_پورو);

  *connected = Curl_conn_is_connected(معلومات->conn, FIRSTSOCKET);

ٻاهريون:
  CURL_TRC_SMTP(معلومات, "smtp_perform() -> %d, connected=%d, done=%d",
                (صحيح)نتيجو, *connected, *dophase_پورو);
  أرجع نتيجو;
}

/* Call this when the DO phase has completed */
ساكن CURLcode smtp_dophase_پورو(بنية Curl_easy *معلومات,
                                  بنية SMTP *smtp,
                                  bool connected)
{
  (فراغ)connected;

  إذا(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_تياري_nop(معلومات);

  أرجع CURLE_OK;
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
ساكن CURLcode smtp_regular_transfer(بنية Curl_easy *معلومات,
                                      بنية smtp_conn *smtpc,
                                      بنية SMTP *smtp,
                                      bool *dophase_پورو)
{
  CURLcode نتيجو = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  معلومات->req.سائيز = -1;

  /* Set the progress data */
  Curl_pgrsReset(معلومات);

  /* Carry out the perform */
  نتيجو = smtp_perform(معلومات, smtpc, smtp, &connected, dophase_پورو);

  /* Perform post DO phase operations if necessary */
  إذا(!نتيجو && *dophase_پورو)
    نتيجو = smtp_dophase_پورو(معلومات, smtp, connected);

  CURL_TRC_SMTP(معلومات, "smtp_regular_transfer() -> %d, done=%d",
                (صحيح)نتيجو, *dophase_پورو);
  أرجع نتيجو;
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
ساكن CURLcode smtp_do(بنية Curl_easy *معلومات, bool *پورو)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_SMTP_CONN);
  بنية SMTP *smtp = Curl_meta_get(معلومات, CURL_META_SMTP_EASY);
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);
  *پورو = FALSE; /* default to false */
  إذا(!smtpc || !smtp)
    أرجع CURLE_FAILED_INIT;

  /* Parse the custom request */
  نتيجو = smtp_parse_custom_request(معلومات, smtp);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = smtp_regular_transfer(معلومات, smtpc, smtp, پورو);
  CURL_TRC_SMTP(معلومات, "smtp_do() -> %d, done=%d", (صحيح)نتيجو, *پورو);
  أرجع نتيجو;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode smtp_disconnect(بنية Curl_easy *معلومات,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  بنية smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  إذا(!dead_connection && conn->بٽ.protoconnstart &&
     !Curl_pp_needs_flush(معلومات, &smtpc->pp) &&
     !smtp_perform_quit(معلومات, smtpc))
    (فراغ)smtp_بلاڪ_statemach(معلومات, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(معلومات, "smtp_disconnect(), finished");
  أرجع CURLE_OK;
}

/* Called from multi.c while DOing */
ساكن CURLcode smtp_doing(بنية Curl_easy *معلومات, bool *dophase_پورو)
{
  بنية SMTP *smtp = Curl_meta_get(معلومات, CURL_META_SMTP_EASY);
  CURLcode نتيجو;

  إذا(!smtp)
    أرجع CURLE_FAILED_INIT;
  نتيجو = smtp_multi_statemach(معلومات, dophase_پورو);
  إذا(نتيجو)
    DEBUGF(infof(معلومات, "DO phase failed"));
  وإلا إذا(*dophase_پورو) {
    نتيجو = smtp_dophase_پورو(معلومات, smtp, FALSE /* not connected */);

    DEBUGF(infof(معلومات, "DO phase is complete"));
  }

  CURL_TRC_SMTP(معلومات, "smtp_doing() -> %d, done=%d", (صحيح)نتيجو,
                *dophase_پورو);
  أرجع نتيجو;
}

ساكن فراغ smtp_easy_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  بنية SMTP *smtp = داخلا_2;
  (فراغ)ڪنجي;
  (فراغ)klen;
  curlx_free(smtp);
}

ساكن فراغ smtp_conn_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  بنية smtp_conn *smtpc = داخلا_2;
  (فراغ)ڪنجي;
  (فراغ)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

ساكن CURLcode smtp_تياري_connection(بنية Curl_easy *معلومات,
                                      بنية connectdata *conn)
{
  بنية smtp_conn *smtpc;
  بنية SMTP *smtp;
  CURLcode نتيجو = CURLE_OK;

  smtpc = curlx_calloc(1, حجم(*smtpc));
  إذا(!smtpc ||
     Curl_conn_meta_سيٽ(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  smtp = curlx_calloc(1, حجم(*smtp));
  إذا(!smtp ||
     Curl_meta_سيٽ(معلومات, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;

ٻاهريون:
  CURL_TRC_SMTP(معلومات, "smtp_setup_connection() -> %d", (صحيح)نتيجو);
  أرجع نتيجو;
}

/*
 * SMTP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_smtp = {
  smtp_تياري_connection,            /* setup_connection */
  smtp_do,                          /* do_it */
  smtp_پورو,                        /* done */
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

#جيڪڏهن_پورو /* CURL_DISABLE_SMTP */
