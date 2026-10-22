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

#جے_بنا_تعریف CURL_DISABLE_SMTP

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

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
#تعریف_کرو CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کرو CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

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
  SMTP_انکڑے,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_آخری         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
بنية smtp_conn {
  بنية pingpong pp;
  بنية SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  حرف *domain;            /* Client address/name to send in the EHLO */
  ثنائی_ہندسہ(ssldone);            /* Is connect() over SSL done? */
  ثنائی_ہندسہ(tls_supported);      /* StartTLS capability supported by server */
  ثنائی_ہندسہ(ناپ_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  ثنائی_ہندسہ(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  ثنائی_ہندسہ(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
بنية SMTP {
  curl_pp_transfer transfer;
  حرف *custom;            /* Custom Request */
  بنية curl_slist *rcpt; /* Recipient list */
  صحيح rcpt_آخری_error;     /* The last error received for RCPT TO command */
  size_t eob;              /* Number of bytes of the EOB (End Of Body) that
                              have been received so far */
  ثنائی_ہندسہ(rcpt_had_ok);        /* Whether any of RCPT TO commands (depends on
                              total number of recipients) succeeded so far */
  ثنائی_ہندسہ(trailing_crlf);      /* Specifies if the trailing CRLF is present */
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
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ptr = conn->options;

  طالما(!نتیجہ && ptr && *ptr) {
    ثابت حرف *کلید = ptr;
    ثابت حرف *قدر;

    طالما(*ptr && *ptr != '=')
      ptr++;

    قدر = ptr + 1;

    طالما(*ptr && *ptr != ';')
      ptr++;

    إذا(curl_strnequal(کلید, "AUTH=", 5))
      نتیجہ = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               قدر, ptr - قدر);
    وإلا
      نتیجہ = CURLE_URL_MALFORMAT;

    إذا(*ptr == ';')
      ptr++;
  }

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ساكن CURLcode smtp_parse_url_رستہ(بنية Curl_easy *انکڑے,
                                    بنية smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ثابت حرف *رستہ = &انکڑے->state.اتے.رستہ[1]; /* skip leading path */
  حرف localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  إذا(!*رستہ) {
    إذا(!Curl_gethostname(localhost, حجم(localhost)))
      رستہ = localhost;
    وإلا
      رستہ = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  أرجع Curl_urldecode(رستہ, 0, &smtpc->domain, عدم, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
ساكن CURLcode smtp_parse_custom_request(بنية Curl_easy *انکڑے,
                                          بنية SMTP *smtp)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *custom = CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  إذا(custom)
    نتیجہ = Curl_urldecode(custom, 0, &smtp->custom, عدم, REJECT_CTRL);

  أرجع نتیجہ;
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
ساكن CURLcode smtp_parse_پتہ(بنية Curl_easy *انکڑے, ثابت حرف *fqma,
                                   حرف **پتہ, بنية hostname *host,
                                   ثابت حرف **suffix)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t لمائی;
  حرف *addressend;
  حرف *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  إذا(strpbrk(fqma, "\r\n")) {
    failf(انکڑے, "Refusing to send email address with a CR or LF");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  إذا(!dup)
    أرجع CURLE_باہرلا_دا_حافظہ;

  إذا(fqma[0] != '<') {
    لمائی = لڑی_لمبائی(dup);
    إذا(لمائی && dup[لمائی - 1] == '>')
      dup[لمائی - 1] = '\0';
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
  *پتہ = dup;

  أرجع نتیجہ;
}

بنية cr_eob_ctx {
  بنية Curl_creader super;
  بنية bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  ثنائی_ہندسہ(read_eos);  /* we read an EOS from the next reader */
  ثنائی_ہندسہ(processed_eos);  /* we read and processed an EOS */
  ثنائی_ہندسہ(eos);       /* we have returned an EOS */
};

ساكن CURLcode cr_eob_init(بنية Curl_easy *انکڑے,
                            بنية Curl_creader *reader)
{
  بنية cr_eob_ctx *ctx = reader->ctx;
  (فراغ)انکڑے;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_حد_بندی);
  أرجع CURLE_OK;
}

ساكن فراغ cr_eob_close(بنية Curl_easy *انکڑے, بنية Curl_creader *reader)
{
  بنية cr_eob_ctx *ctx = reader->ctx;
  (فراغ)انکڑے;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#تعریف_کرو SMTP_EOB          "\r\n.\r\n"
#تعریف_کرو SMTP_EOB_لبھو_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
ساكن CURLcode cr_eob_read(بنية Curl_easy *انکڑے,
                            بنية Curl_creader *reader,
                            حرف *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  بنية cr_eob_ctx *ctx = reader->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread, i, شروع, n;
  bool eos;

  إذا(!ctx->read_eos && Curl_bufq_is_خالی_تھاواں(&ctx->buf)) {
    /* Get more and convert it when needed */
    نتیجہ = Curl_creader_read(انکڑے, reader->اگلا, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(انکڑے, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (صحيح)نتیجہ, nread, eos);
    إذا(نتیجہ)
      أرجع نتیجہ;

    ctx->read_eos = eos;
    إذا(nread) {
      إذا(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        أرجع CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      لكل(i = شروع = 0; i < nread; ++i) {
        إذا(ctx->n_eob >= SMTP_EOB_لبھو_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          نتیجہ = Curl_bufq_cwrite(&ctx->buf, buf + شروع, i - شروع, &n);
          إذا(نتیجہ)
            أرجع نتیجہ;
          نتیجہ = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          إذا(نتیجہ)
            أرجع نتیجہ;
          ctx->n_eob = 0;
          شروع = i;
          إذا(انکڑے->state.infilesize > 0)
            انکڑے->state.infilesize++;
        }

        إذا(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        إذا(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      إذا(شروع < nread) {
        نتیجہ = Curl_bufq_cwrite(&ctx->buf, buf + شروع, nread - شروع, &n);
        إذا(نتیجہ)
          أرجع نتیجہ;
      }
    }
  }

  *peos = FALSE;

  إذا(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ثابت حرف *eob = SMTP_EOB;
    CURL_TRC_SMTP(انکڑے, "auto-ending mail body with '\\r\\n.\\r\\n'");
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
    نتیجہ = Curl_bufq_cwrite(&ctx->buf, eob, لڑی_لمبائی(eob), &n);
    إذا(نتیجہ)
      أرجع نتیجہ;
    ctx->processed_eos = TRUE;
  }

  إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->buf)) {
    نتیجہ = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  وإلا
    *pnread = 0;

  إذا(ctx->read_eos && Curl_bufq_is_خالی_تھاواں(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(انکڑے, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(انکڑے, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (صحيح)نتیجہ, *pnread, *peos));
  أرجع نتیجہ;
}

ساكن curl_off_t cr_eob_کل_لمائی(بنية Curl_easy *انکڑے,
                                      بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)انکڑے;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_کل_لمائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورا,
  حجم(بنية cr_eob_ctx)
};

ساكن CURLcode cr_eob_جوڑو(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&reader, انکڑے, &cr_eob, CURL_CR_CONTENT_ENCODE);
  إذا(!نتیجہ)
    نتیجہ = Curl_creader_جوڑو(انکڑے, reader);

  إذا(نتیجہ && reader)
    Curl_creader_free(انکڑے, reader);
  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
ساكن bool smtp_endofresp(بنية Curl_easy *انکڑے, بنية connectdata *conn,
                           ثابت حرف *سطر, size_t len, صحيح *resp)
{
  بنية smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool سرا = FALSE;
  (فراغ)انکڑے;

  DEBUGASSERT(smtpc);
  إذا(!smtpc)
    أرجع FALSE;

  /* Nothing for us */
  إذا(len < 4 || !ISDIGIT(سطر[0]) || !ISDIGIT(سطر[1]) || !ISDIGIT(سطر[2]))
    أرجع FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  إذا(سطر[3] == ' ' || len == 5) {
    حرف tmpline[6];
    curl_off_t کوڈ;
    ثابت حرف *p = tmpline;
    سرا = TRUE;
    حافظہ_نقل(tmpline, سطر, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    إذا(curlx_str_نمبر(&p, &کوڈ, len == 5 ? 99999 : 999))
      أرجع FALSE;
    *resp = (صحيح)کوڈ;

    /* Make sure real server never sends internal value */
    إذا(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  وإلا إذا(سطر[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    سرا = TRUE;
    *resp = 1;  /* Internal response code */
  }

  أرجع سرا;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ساكن CURLcode smtp_get_سنیہا(بنية Curl_easy *انکڑے, بنية bufref *باہرلا)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_SMTP_CONN);
  حرف *سنیہا;
  size_t len;

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  سنیہا = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  إذا(len > 4) {
    /* Find the start of the message */
    len -= 4;
    لكل(سنیہا += 4; ISBLANK(*سنیہا); سنیہا++, len--)
      ;

    /* Find the end of the message */
    طالما(len--)
      إذا(!ISNEWLINE(سنیہا[len]) && !ISBLANK(سنیہا[len]))
        اكسر;

    /* Terminate the message */
    سنیہا[++len] = '\0';
    Curl_bufref_سیٹ(باہرلا, سنیہا, len, عدم);
  }
  وإلا
    /* junk input => zero length output */
    Curl_bufref_سیٹ(باہرلا, "", 0, عدم);

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
ساكن فراغ smtp_state(بنية Curl_easy *انکڑے,
                       بنية smtp_conn *smtpc,
                       smtpstate newstate)
{
#جے_تعریف_شدہ CURLVERBOSE
  /* for debug purposes */
  ساكن ثابت حرف * ثابت ناں[] = {
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
    /*آخری*/
  };

  إذا(smtpc->state != newstate)
    CURL_TRC_SMTP(انکڑے, "state change from %s to %s",
                  ناں[smtpc->state], ناں[newstate]);
#وإلا
  (فراغ)انکڑے;
#جے_انت

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
ساكن CURLcode smtp_perform_ehlo(بنية Curl_easy *انکڑے,
                                  بنية smtp_conn *smtpc)
{
  CURLcode نتیجہ = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "EHLO %s", smtpc->domain);

  إذا(!نتیجہ)
    smtp_state(انکڑے, smtpc, SMTP_EHLO);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
ساكن CURLcode smtp_perform_helo(بنية Curl_easy *انکڑے,
                                  بنية smtp_conn *smtpc)
{
  CURLcode نتیجہ = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "HELO %s", smtpc->domain);

  إذا(!نتیجہ)
    smtp_state(انکڑے, smtpc, SMTP_HELO);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ساكن CURLcode smtp_perform_starttls(بنية Curl_easy *انکڑے,
                                      بنية smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "%s", "STARTTLS");

  إذا(!نتیجہ)
    smtp_state(انکڑے, smtpc, SMTP_STARTTLS);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساكن CURLcode smtp_perform_upgrade_tls(بنية Curl_easy *انکڑے,
                                         بنية smtp_conn *smtpc)
{
#جے_تعریف_شدہ USE_SSL
  /* Start the SSL connection */
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجہ = Curl_ssl_cfilter_جوڑو(
      انکڑے, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    إذا(نتیجہ)
      اذهب باہرلا;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  نتیجہ = Curl_conn_connect(انکڑے, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(انکڑے, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (صحيح)نتیجہ, ssldone));
  إذا(!نتیجہ && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    نتیجہ = smtp_perform_ehlo(انکڑے, smtpc);
  }
باہرلا:
  أرجع نتیجہ;
#وإلا
  (فراغ)انکڑے;
  (فراغ)smtpc;
  أرجع CURLE_NOT_BUILT_وچ;
#جے_انت
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ساكن CURLcode smtp_perform_auth(بنية Curl_easy *انکڑے,
                                  ثابت حرف *mech,
                                  ثابت بنية bufref *initresp)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_SMTP_CONN);
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  إذا(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  وإلا {
    /* Send the AUTH command */
    نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "AUTH %s", mech);
  }

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
ساكن CURLcode smtp_continue_auth(بنية Curl_easy *انکڑے,
                                   ثابت حرف *mech,
                                   ثابت بنية bufref *resp)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_SMTP_CONN);

  (فراغ)mech;
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_pp_sendf(انکڑے, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ساكن CURLcode smtp_cancel_auth(بنية Curl_easy *انکڑے, ثابت حرف *mech)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_SMTP_CONN);

  (فراغ)mech;
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_pp_sendf(انکڑے, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
ساكن CURLcode smtp_perform_authentication(بنية Curl_easy *انکڑے,
                                            بنية smtp_conn *smtpc)
{
  CURLcode نتیجہ = CURLE_OK;
  saslprogress ترقی;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  إذا(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, انکڑے)) {
    smtp_state(انکڑے, smtpc, SMTP_STOP);
    أرجع نتیجہ;
  }

  /* Calculate the SASL login details */
  نتیجہ = Curl_sasl_شروع(&smtpc->sasl, انکڑے, FALSE, &ترقی);

  إذا(!نتیجہ) {
    إذا(ترقی == SASL_INPROGRESS)
      smtp_state(انکڑے, smtpc, SMTP_AUTH);
    وإلا
      نتیجہ = Curl_sasl_is_blocked(&smtpc->sasl, انکڑے);
  }

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
ساكن CURLcode smtp_perform_command(بنية Curl_easy *انکڑے,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    إذا(!smtp->custom || !smtp->custom[0]) {
      حرف *پتہ = عدم;
      بنية hostname host = { عدم, عدم, عدم, عدم };
      ثابت حرف *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      نتیجہ = smtp_parse_پتہ(انکڑے, smtp->rcpt->انکڑے,
                                  &پتہ, &host, &suffix);
      إذا(نتیجہ)
        أرجع نتیجہ;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(پتہ) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "VRFY %s%s%s%s",
                             پتہ,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(پتہ);
    }
    وإلا {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !strcmp(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->انکڑے,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  وإلا
    /* Send the non-recipient based command such as HELP */
    نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  إذا(!نتیجہ)
    smtp_state(انکڑے, smtpc, SMTP_COMMAND);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
ساكن CURLcode smtp_perform_mail(بنية Curl_easy *انکڑے,
                                  بنية smtp_conn *smtpc,
                                  بنية SMTP *smtp)
{
  حرف *from = عدم;
  حرف *auth = عدم;
  حرف *ناپ = عدم;
  ثابت حرف *str;
  CURLcode نتیجہ = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(انکڑے, STRING_MAIL_FROM);
  إذا(str) {
    حرف *پتہ = عدم;
    بنية hostname host = { عدم, عدم, عدم, عدم };
    ثابت حرف *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    نتیجہ = smtp_parse_پتہ(انکڑے, str, &پتہ, &host, &suffix);
    إذا(نتیجہ)
      اذهب باہرلا;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(پتہ) ||
            !Curl_is_ASCII_name(host.name));

    إذا(host.name) {
      from = curl_maprintf("<%s@%s>%s", پتہ, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    وإلا
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", پتہ, suffix);

    curlx_free(پتہ);
  }
  وإلا
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  إذا(!from) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(انکڑے, STRING_MAIL_AUTH);
  إذا(str && smtpc->sasl.authused) {
    إذا(str[0] != '\0') {
      حرف *پتہ = عدم;
      بنية hostname host = { عدم, عدم, عدم, عدم };
      ثابت حرف *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      نتیجہ = smtp_parse_پتہ(انکڑے, str, &پتہ, &host, &suffix);
      إذا(نتیجہ)
        اذهب باہرلا;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      إذا(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(پتہ) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      إذا(host.name) {
        auth = curl_maprintf("<%s@%s>%s", پتہ, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      وإلا
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", پتہ, suffix);
      curlx_free(پتہ);
    }
    وإلا
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    إذا(!auth) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }

#جے_بنا_تعریف CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  إذا(IS_MIME_POST(انکڑے)) {
    curl_mimepart *postp = انکڑے->سیٹ.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(بنانشانی صحيح)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_سرخیاں(postp, انکڑے->سیٹ.سرخیاں, 0);
    نتیجہ = Curl_mime_prepare_سرخیاں(انکڑے, postp, عدم,
                                       عدم, MIMESTRATEGY_MAIL);

    إذا(!نتیجہ && !Curl_checkheaders(انکڑے, STRCONST("Mime-Version")))
      نتیجہ = Curl_mime_جوڑو_سرخی(&postp->curlheaders, "Mime-Version: 1.0");
    إذا(!نتیجہ)
      نتیجہ = Curl_creader_سیٹ_mime(انکڑے, postp);
    إذا(نتیجہ)
      اذهب باہرلا;
    انکڑے->state.infilesize = Curl_creader_کل_لمائی(انکڑے);
  }
  وإلا
#جے_انت
  {
    نتیجہ = Curl_creader_سیٹ_fread(انکڑے, انکڑے->state.infilesize);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  /* Calculate the optional SIZE parameter */
  إذا(smtpc->ناپ_supported && انکڑے->state.infilesize > 0) {
    ناپ = curl_maprintf("%" FMT_OFF_T, انکڑے->state.infilesize);

    إذا(!ناپ) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
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
      إذا(!Curl_is_ASCII_name(rcpt->انکڑے))
        utf8 = TRUE;

      rcpt = rcpt->اگلا;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  نتیجہ = cr_eob_جوڑو(انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* Send the MAIL command */
  نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         ناپ ? " SIZE=" : "", /* Optional on SIZE support  */
                         ناپ ? ناپ : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

باہرلا:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(ناپ);

  إذا(!نتیجہ)
    smtp_state(انکڑے, smtpc, SMTP_MAIL);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
ساكن CURLcode smtp_perform_rcpt_منزل(بنية Curl_easy *انکڑے,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *پتہ = عدم;
  بنية hostname host = { عدم, عدم, عدم, عدم };
  ثابت حرف *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  نتیجہ = smtp_parse_پتہ(انکڑے, smtp->rcpt->انکڑے,
                              &پتہ, &host, &suffix);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Send the RCPT TO command */
  إذا(host.name)
    نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           پتہ, host.name, suffix);
  وإلا
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "RCPT TO:<%s>%s",
                           پتہ, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(پتہ);

  إذا(!نتیجہ)
    smtp_state(انکڑے, smtpc, SMTP_RCPT);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
ساكن CURLcode smtp_perform_quit(بنية Curl_easy *انکڑے,
                                  بنية smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "%s", "QUIT");

  إذا(!نتیجہ)
    smtp_state(انکڑے, smtpc, SMTP_QUIT);

  أرجع نتیجہ;
}

/* For the initial server greeting */
ساكن CURLcode smtp_state_servergreet_resp(بنية Curl_easy *انکڑے,
                                            بنية smtp_conn *smtpc,
                                            صحيح smtpcode,
                                            smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode / 100 != 2) {
    failf(انکڑے, "Got unexpected smtp-server response: %d", smtpcode);
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }
  وإلا
    نتیجہ = smtp_perform_ehlo(انکڑے, smtpc);

  أرجع نتیجہ;
}

/* For STARTTLS responses */
ساكن CURLcode smtp_state_starttls_resp(بنية Curl_easy *انکڑے,
                                         بنية smtp_conn *smtpc,
                                         صحيح smtpcode,
                                         smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)instate;

  /* Pipelining in response is forbidden. */
  إذا(smtpc->pp.حد_توں_ودھ)
    أرجع CURLE_WEIRD_SERVER_جواب;

  إذا(smtpcode != 220) {
    إذا(انکڑے->سیٹ.use_ssl != CURLUSESSL_TRY) {
      failf(انکڑے, "STARTTLS denied, code %d", smtpcode);
      نتیجہ = CURLE_USE_SSL_FAILED;
    }
    وإلا
      نتیجہ = smtp_perform_authentication(انکڑے, smtpc);
  }
  وإلا
    smtp_state(انکڑے, smtpc, SMTP_UPGRADETLS);

  أرجع نتیجہ;
}

/* For EHLO responses */
ساكن CURLcode smtp_state_ehlo_resp(بنية Curl_easy *انکڑے,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (فراغ)instate;

  إذا(smtpcode / 100 != 2 && smtpcode != 1) {
    إذا(انکڑے->سیٹ.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(انکڑے->conn, FIRSTSOCKET))
      نتیجہ = smtp_perform_helo(انکڑے, smtpc);
    وإلا {
      failf(انکڑے, "Remote access denied: %d", smtpcode);
      نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  وإلا إذا(len >= 4) {
    سطر += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    إذا(len >= 8 && curl_strnequal(سطر, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    وإلا إذا(len >= 4 && curl_strnequal(سطر, "SIZE", 4))
      smtpc->ناپ_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    وإلا إذا(len >= 8 && curl_strnequal(سطر, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    وإلا إذا(len >= 5 && curl_strnequal(سطر, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      سطر += 5;
      len -= 5;

      /* Loop through the data line */
      لكل(;;) {
        size_t llen;
        size_t wordlen;
        بنانشانی چھوٹا mechbit;

        طالما(len && (ISBLANK(*سطر) || ISNEWLINE(*سطر))) {
          سطر++;
          len--;
        }

        إذا(!len)
          اكسر;

        /* Extract the word */
        لكل(wordlen = 0; wordlen < len && !ISBLANK(سطر[wordlen]) &&
              !ISNEWLINE(سطر[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(سطر, wordlen, &llen);
        إذا(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        سطر += wordlen;
        len -= wordlen;
      }
    }

    إذا(smtpcode != 1) {
      إذا(انکڑے->سیٹ.use_ssl && !Curl_conn_is_ssl(انکڑے->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        إذا(smtpc->tls_supported)
          /* Switch to TLS connection now */
          نتیجہ = smtp_perform_starttls(انکڑے, smtpc);
        وإلا إذا(انکڑے->سیٹ.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          نتیجہ = smtp_perform_authentication(انکڑے, smtpc);
        وإلا {
          failf(انکڑے, "STARTTLS not supported.");
          نتیجہ = CURLE_USE_SSL_FAILED;
        }
      }
      وإلا
        نتیجہ = smtp_perform_authentication(انکڑے, smtpc);
    }
  }
  وإلا {
    failf(انکڑے, "Unexpectedly short EHLO response");
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }

  أرجع نتیجہ;
}

/* For HELO responses */
ساكن CURLcode smtp_state_helo_resp(بنية Curl_easy *انکڑے,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode / 100 != 2) {
    failf(انکڑے, "Remote access denied: %d", smtpcode);
    نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
  }
  وإلا
    /* End of connect phase */
    smtp_state(انکڑے, smtpc, SMTP_STOP);

  أرجع نتیجہ;
}

/* For SASL authentication responses */
ساكن CURLcode smtp_state_auth_resp(بنية Curl_easy *انکڑے,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  saslprogress ترقی;

  (فراغ)instate;

  نتیجہ = Curl_sasl_continue(&smtpc->sasl, انکڑے, smtpcode, &ترقی);
  إذا(!نتیجہ)
    بدل(ترقی) {
    حالة SASL_پورا:
      smtp_state(انکڑے, smtpc, SMTP_STOP);  /* Authenticated */
      اكسر;
    حالة SASL_IDLE:            /* No mechanism left after cancellation */
      failf(انکڑے, "Authentication cancelled");
      نتیجہ = CURLE_LOGIN_DENIED;
      اكسر;
    افتراضي:
      اكسر;
    }

  أرجع نتیجہ;
}

/* For command responses */
ساكن CURLcode smtp_state_command_resp(بنية Curl_easy *انکڑے,
                                        بنية smtp_conn *smtpc,
                                        بنية SMTP *smtp,
                                        صحيح smtpcode,
                                        smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (فراغ)instate;

  إذا((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(انکڑے, "Command failed: %d", smtpcode);
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }
  وإلا {
    إذا(!انکڑے->req.no_body)
      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, سطر, len);

    إذا(!نتیجہ && (smtpcode != 1)) {
      إذا(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->اگلا;

        إذا(smtp->rcpt) {
          /* Send the next command */
          نتیجہ = smtp_perform_command(انکڑے, smtpc, smtp);
        }
        وإلا
          /* End of DO phase */
          smtp_state(انکڑے, smtpc, SMTP_STOP);
      }
      وإلا
        /* End of DO phase */
        smtp_state(انکڑے, smtpc, SMTP_STOP);
    }
  }

  أرجع نتیجہ;
}

/* For MAIL responses */
ساكن CURLcode smtp_state_mail_resp(بنية Curl_easy *انکڑے,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode / 100 != 2) {
    failf(انکڑے, "MAIL failed: %d", smtpcode);
    نتیجہ = CURLE_SEND_ERROR;
  }
  وإلا
    /* Start the RCPT TO command */
    نتیجہ = smtp_perform_rcpt_منزل(انکڑے, smtpc, smtp);

  أرجع نتیجہ;
}

/* For RCPT responses */
ساكن CURLcode smtp_state_rcpt_resp(بنية Curl_easy *انکڑے,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (فراغ)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !انکڑے->سیٹ.mail_rcpt_allowfails);

  إذا(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_آخری_error = smtpcode;

    إذا(is_smtp_blocking_err) {
      failf(انکڑے, "RCPT failed: %d", smtpcode);
      نتیجہ = CURLE_SEND_ERROR;
    }
  }
  وإلا {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  إذا(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->اگلا;

    إذا(smtp->rcpt)
      /* Send the next RCPT TO command */
      نتیجہ = smtp_perform_rcpt_منزل(انکڑے, smtpc, smtp);
    وإلا {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      إذا(!smtp->rcpt_had_ok) {
        failf(انکڑے, "RCPT failed: %d (last error)", smtp->rcpt_آخری_error);
        نتیجہ = CURLE_SEND_ERROR;
      }
      وإلا {
        /* Send the DATA command */
        نتیجہ = Curl_pp_sendf(انکڑے, &smtpc->pp, "%s", "DATA");

        إذا(!نتیجہ)
          smtp_state(انکڑے, smtpc, SMTP_انکڑے);
      }
    }
  }

  أرجع نتیجہ;
}

/* For DATA response */
ساكن CURLcode smtp_state_انکڑے_resp(بنية Curl_easy *انکڑے,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode != 354) {
    failf(انکڑے, "DATA failed: %d", smtpcode);
    نتیجہ = CURLE_SEND_ERROR;
  }
  وإلا {
    /* Set the progress upload size */
    Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, انکڑے->state.infilesize);

    /* SMTP upload */
    Curl_xfer_تیاری_send(انکڑے, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(انکڑے, smtpc, SMTP_STOP);
  }

  أرجع نتیجہ;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
ساكن CURLcode smtp_state_postdata_resp(بنية Curl_easy *انکڑے,
                                         بنية smtp_conn *smtpc,
                                         صحيح smtpcode,
                                         smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)instate;

  إذا(smtpcode != 250)
    نتیجہ = CURLE_WEIRD_SERVER_جواب;

  /* End of DONE phase */
  smtp_state(انکڑے, smtpc, SMTP_STOP);

  أرجع نتیجہ;
}

ساكن CURLcode smtp_pp_statemachine(بنية Curl_easy *انکڑے,
                                     بنية connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  صحيح smtpcode;
  بنية smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  بنية SMTP *smtp = Curl_meta_get(انکڑے, CURL_META_SMTP_EASY);
  size_t nread = 0;

  إذا(!smtpc || !smtp)
    أرجع CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  إذا(smtpc->state == SMTP_UPGRADETLS) {
    نتیجہ = smtp_perform_upgrade_tls(انکڑے, smtpc);
    إذا(نتیجہ || (smtpc->state == SMTP_UPGRADETLS))
      أرجع نتیجہ;
  }

  /* Flush any data that needs to be sent */
  إذا(smtpc->pp.sendleft)
    أرجع Curl_pp_flushsend(انکڑے, &smtpc->pp);

  افعل {
    /* Read the response from the server */
    نتیجہ = Curl_pp_readresp(انکڑے, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    إذا(نتیجہ)
      أرجع نتیجہ;

    /* Store the latest response for later retrieval if necessary */
    إذا(smtpc->state != SMTP_QUIT && smtpcode != 1)
      انکڑے->info.httpcode = smtpcode;

    إذا(!smtpcode)
      اكسر;

    /* We have now received a full SMTP server response */
    بدل(smtpc->state) {
    حالة SMTP_SERVERGREET:
      نتیجہ = smtp_state_servergreet_resp(انکڑے, smtpc,
                                           smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_EHLO:
      نتیجہ = smtp_state_ehlo_resp(انکڑے, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_HELO:
      نتیجہ = smtp_state_helo_resp(انکڑے, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_STARTTLS:
      نتیجہ = smtp_state_starttls_resp(انکڑے, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      إذا(!نتیجہ && (smtpc->state == SMTP_UPGRADETLS))
        اذهب upgrade_tls;
      اكسر;

    حالة SMTP_AUTH:
      نتیجہ = smtp_state_auth_resp(انکڑے, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_COMMAND:
      نتیجہ = smtp_state_command_resp(انکڑے, smtpc, smtp,
                                       smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_MAIL:
      نتیجہ = smtp_state_mail_resp(انکڑے, smtpc, smtp, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_RCPT:
      نتیجہ = smtp_state_rcpt_resp(انکڑے, smtpc, smtp, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_انکڑے:
      نتیجہ = smtp_state_انکڑے_resp(انکڑے, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_POSTDATA:
      نتیجہ = smtp_state_postdata_resp(انکڑے, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_QUIT:
    افتراضي:
      /* internal error */
      smtp_state(انکڑے, smtpc, SMTP_STOP);
      اكسر;
    }
  } طالما(!نتیجہ && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  أرجع نتیجہ;
}

/* Called repeatedly until done from multi.c */
ساكن CURLcode smtp_multi_statemach(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_SMTP_CONN);

  *پورا = FALSE;
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  نتیجہ = Curl_pp_statemach(انکڑے, &smtpc->pp, FALSE, FALSE);
  *پورا = (smtpc->state == SMTP_STOP);
  أرجع نتیجہ;
}

ساكن CURLcode smtp_بلاک_statemach(بنية Curl_easy *انکڑے,
                                     بنية smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode نتیجہ = CURLE_OK;

  طالما(smtpc->state != SMTP_STOP && !نتیجہ)
    نتیجہ = Curl_pp_statemach(انکڑے, &smtpc->pp, TRUE, disconnecting);

  أرجع نتیجہ;
}

/* For the SMTP "protocol connect" and "doing" phases only */
ساكن CURLcode smtp_pollset(بنية Curl_easy *انکڑے,
                             بنية easy_pollset *ps)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_SMTP_CONN);
  أرجع smtpc ? Curl_pp_pollset(انکڑے, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
ساكن ثابت بنية SASLproto saslsmtp = {
  "smtp",               /* The service name */
  smtp_perform_auth,    /* Send authentication command */
  smtp_continue_auth,   /* Send authentication continuation */
  smtp_cancel_auth,     /* Cancel authentication */
  smtp_get_سنیہا,     /* Get SASL response message */
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
ساكن CURLcode smtp_connect(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_SMTP_CONN);
  CURLcode نتیجہ = CURLE_OK;

  *پورا = FALSE; /* default to not done yet */
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  PINGPONG_تیاری(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, انکڑے, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(انکڑے));

  /* Parse the URL options */
  نتیجہ = smtp_parse_url_options(انکڑے->conn, smtpc);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Parse the URL path */
  نتیجہ = smtp_parse_url_رستہ(انکڑے, smtpc);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Start off waiting for the server greeting response */
  smtp_state(انکڑے, smtpc, SMTP_SERVERGREET);

  نتیجہ = smtp_multi_statemach(انکڑے, پورا);

  أرجع نتیجہ;
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
ساكن CURLcode smtp_پورا(بنية Curl_easy *انکڑے, CURLcode حالت,
                          bool premature)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_SMTP_CONN);
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  بنية SMTP *smtp = Curl_meta_get(انکڑے, CURL_META_SMTP_EASY);

  (فراغ)premature;

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;
  إذا(!smtp)
    أرجع CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  إذا(حالت) {
    CURL_TRC_M(انکڑے, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    نتیجہ = حالت;         /* use the already set error code */
  }
  وإلا إذا(!انکڑے->سیٹ.connect_only && انکڑے->سیٹ.mail_rcpt &&
          (انکڑے->state.upload || IS_MIME_POST(انکڑے))) {

    smtp_state(انکڑے, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    نتیجہ = smtp_بلاک_statemach(انکڑے, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(انکڑے, "smtp_done(status=%d, premature=%d) -> %d",
                (صحيح)حالت, premature, (صحيح)نتیجہ);
  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
ساكن CURLcode smtp_perform(بنية Curl_easy *انکڑے,
                             بنية smtp_conn *smtpc,
                             بنية SMTP *smtp,
                             bool *connected,
                             bool *dophase_پورا)
{
  /* This is SMTP and no proxy */
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_SMTP(انکڑے, "smtp_perform(), start");

  إذا(انکڑے->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_پورا = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = انکڑے->سیٹ.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_آخری_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  إذا((انکڑے->state.upload || IS_MIME_POST(انکڑے)) && انکڑے->سیٹ.mail_rcpt)
    /* MAIL transfer */
    نتیجہ = smtp_perform_mail(انکڑے, smtpc, smtp);
  وإلا
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    نتیجہ = smtp_perform_command(انکڑے, smtpc, smtp);

  إذا(نتیجہ)
    اذهب باہرلا;

  /* Run the state-machine */
  نتیجہ = smtp_multi_statemach(انکڑے, dophase_پورا);

  *connected = Curl_conn_is_connected(انکڑے->conn, FIRSTSOCKET);

باہرلا:
  CURL_TRC_SMTP(انکڑے, "smtp_perform() -> %d, connected=%d, done=%d",
                (صحيح)نتیجہ, *connected, *dophase_پورا);
  أرجع نتیجہ;
}

/* Call this when the DO phase has completed */
ساكن CURLcode smtp_dophase_پورا(بنية Curl_easy *انکڑے,
                                  بنية SMTP *smtp,
                                  bool connected)
{
  (فراغ)connected;

  إذا(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_تیاری_nop(انکڑے);

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
ساكن CURLcode smtp_regular_transfer(بنية Curl_easy *انکڑے,
                                      بنية smtp_conn *smtpc,
                                      بنية SMTP *smtp,
                                      bool *dophase_پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  انکڑے->req.ناپ = -1;

  /* Set the progress data */
  Curl_pgrsReset(انکڑے);

  /* Carry out the perform */
  نتیجہ = smtp_perform(انکڑے, smtpc, smtp, &connected, dophase_پورا);

  /* Perform post DO phase operations if necessary */
  إذا(!نتیجہ && *dophase_پورا)
    نتیجہ = smtp_dophase_پورا(انکڑے, smtp, connected);

  CURL_TRC_SMTP(انکڑے, "smtp_regular_transfer() -> %d, done=%d",
                (صحيح)نتیجہ, *dophase_پورا);
  أرجع نتیجہ;
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
ساكن CURLcode smtp_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_SMTP_CONN);
  بنية SMTP *smtp = Curl_meta_get(انکڑے, CURL_META_SMTP_EASY);
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(انکڑے);
  DEBUGASSERT(انکڑے->conn);
  *پورا = FALSE; /* default to false */
  إذا(!smtpc || !smtp)
    أرجع CURLE_FAILED_INIT;

  /* Parse the custom request */
  نتیجہ = smtp_parse_custom_request(انکڑے, smtp);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = smtp_regular_transfer(انکڑے, smtpc, smtp, پورا);
  CURL_TRC_SMTP(انکڑے, "smtp_do() -> %d, done=%d", (صحيح)نتیجہ, *پورا);
  أرجع نتیجہ;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode smtp_disconnect(بنية Curl_easy *انکڑے,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  بنية smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  إذا(!dead_connection && conn->بٹ.protoconnstart &&
     !Curl_pp_needs_flush(انکڑے, &smtpc->pp) &&
     !smtp_perform_quit(انکڑے, smtpc))
    (فراغ)smtp_بلاک_statemach(انکڑے, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(انکڑے, "smtp_disconnect(), finished");
  أرجع CURLE_OK;
}

/* Called from multi.c while DOing */
ساكن CURLcode smtp_doing(بنية Curl_easy *انکڑے, bool *dophase_پورا)
{
  بنية SMTP *smtp = Curl_meta_get(انکڑے, CURL_META_SMTP_EASY);
  CURLcode نتیجہ;

  إذا(!smtp)
    أرجع CURLE_FAILED_INIT;
  نتیجہ = smtp_multi_statemach(انکڑے, dophase_پورا);
  إذا(نتیجہ)
    DEBUGF(infof(انکڑے, "DO phase failed"));
  وإلا إذا(*dophase_پورا) {
    نتیجہ = smtp_dophase_پورا(انکڑے, smtp, FALSE /* not connected */);

    DEBUGF(infof(انکڑے, "DO phase is complete"));
  }

  CURL_TRC_SMTP(انکڑے, "smtp_doing() -> %d, done=%d", (صحيح)نتیجہ,
                *dophase_پورا);
  أرجع نتیجہ;
}

ساكن فراغ smtp_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية SMTP *smtp = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_free(smtp);
}

ساكن فراغ smtp_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية smtp_conn *smtpc = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

ساكن CURLcode smtp_تیاری_connection(بنية Curl_easy *انکڑے,
                                      بنية connectdata *conn)
{
  بنية smtp_conn *smtpc;
  بنية SMTP *smtp;
  CURLcode نتیجہ = CURLE_OK;

  smtpc = curlx_calloc(1, حجم(*smtpc));
  إذا(!smtpc ||
     Curl_conn_meta_سیٹ(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  smtp = curlx_calloc(1, حجم(*smtp));
  إذا(!smtp ||
     Curl_meta_سیٹ(انکڑے, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    نتیجہ = CURLE_باہرلا_دا_حافظہ;

باہرلا:
  CURL_TRC_SMTP(انکڑے, "smtp_setup_connection() -> %d", (صحيح)نتیجہ);
  أرجع نتیجہ;
}

/*
 * SMTP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_smtp = {
  smtp_تیاری_connection,            /* setup_connection */
  smtp_do,                          /* do_it */
  smtp_پورا,                        /* done */
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

#جے_انت /* CURL_DISABLE_SMTP */
