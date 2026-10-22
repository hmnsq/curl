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

#اگر_تعریف_نشده CURL_DISABLE_SMTP

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

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
#تعریف_کن CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کن CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

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
  SMTP_داده,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_آخرین         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
بنية smtp_conn {
  بنية pingpong pp;
  بنية SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  حرف *domain;            /* Client address/name to send in the EHLO */
  بیت(ssldone);            /* Is connect() over SSL done? */
  بیت(tls_supported);      /* StartTLS capability supported by server */
  بیت(اندازه_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  بیت(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  بیت(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
بنية SMTP {
  curl_pp_transfer transfer;
  حرف *custom;            /* Custom Request */
  بنية curl_slist *rcpt; /* Recipient list */
  صحيح rcpt_آخرین_error;     /* The last error received for RCPT TO command */
  size_t eob;              /* Number of bytes of the EOB (End Of Body) that
                              have been received so far */
  بیت(rcpt_had_ok);        /* Whether any of RCPT TO commands (depends on
                              total number of recipients) succeeded so far */
  بیت(trailing_crlf);      /* Specifies if the trailing CRLF is present */
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
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *ptr = conn->options;

  طالما(!نتیجه && ptr && *ptr) {
    ثابت حرف *کلید = ptr;
    ثابت حرف *مقدار;

    طالما(*ptr && *ptr != '=')
      ptr++;

    مقدار = ptr + 1;

    طالما(*ptr && *ptr != ';')
      ptr++;

    إذا(curl_strnequal(کلید, "AUTH=", 5))
      نتیجه = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               مقدار, ptr - مقدار);
    وإلا
      نتیجه = CURLE_URL_MALFORMAT;

    إذا(*ptr == ';')
      ptr++;
  }

  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ساكن CURLcode smtp_parse_url_مسیر(بنية Curl_easy *داده,
                                    بنية smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ثابت حرف *مسیر = &داده->state.به_بالا.مسیر[1]; /* skip leading path */
  حرف localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  إذا(!*مسیر) {
    إذا(!Curl_gethostname(localhost, حجم(localhost)))
      مسیر = localhost;
    وإلا
      مسیر = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  أرجع Curl_urldecode(مسیر, 0, &smtpc->domain, عدم, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
ساكن CURLcode smtp_parse_custom_request(بنية Curl_easy *داده,
                                          بنية SMTP *smtp)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *custom = CURL_EASY_STR(داده, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  إذا(custom)
    نتیجه = Curl_urldecode(custom, 0, &smtp->custom, عدم, REJECT_CTRL);

  أرجع نتیجه;
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
ساكن CURLcode smtp_parse_آدرس(بنية Curl_easy *داده, ثابت حرف *fqma,
                                   حرف **آدرس, بنية hostname *host,
                                   ثابت حرف **suffix)
{
  CURLcode نتیجه = CURLE_OK;
  size_t درازی;
  حرف *addressend;
  حرف *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  إذا(strpbrk(fqma, "\r\n")) {
    failf(داده, "Refusing to send email address with a CR or LF");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  إذا(!dup)
    أرجع CURLE_بیرون_از_حافظه;

  إذا(fqma[0] != '<') {
    درازی = طول_رشته(dup);
    إذا(درازی && dup[درازی - 1] == '>')
      dup[درازی - 1] = '\0';
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
  *آدرس = dup;

  أرجع نتیجه;
}

بنية cr_eob_ctx {
  بنية Curl_creader super;
  بنية bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  بیت(read_eos);  /* we read an EOS from the next reader */
  بیت(processed_eos);  /* we read and processed an EOS */
  بیت(eos);       /* we have returned an EOS */
};

ساكن CURLcode cr_eob_init(بنية Curl_easy *داده,
                            بنية Curl_creader *reader)
{
  بنية cr_eob_ctx *ctx = reader->ctx;
  (فراغ)داده;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_حد);
  أرجع CURLE_OK;
}

ساكن فراغ cr_eob_close(بنية Curl_easy *داده, بنية Curl_creader *reader)
{
  بنية cr_eob_ctx *ctx = reader->ctx;
  (فراغ)داده;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#تعریف_کن SMTP_EOB          "\r\n.\r\n"
#تعریف_کن SMTP_EOB_پیدا_کن_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
ساكن CURLcode cr_eob_read(بنية Curl_easy *داده,
                            بنية Curl_creader *reader,
                            حرف *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  بنية cr_eob_ctx *ctx = reader->ctx;
  CURLcode نتیجه = CURLE_OK;
  size_t nread, i, شروع, n;
  bool eos;

  إذا(!ctx->read_eos && Curl_bufq_is_خالی(&ctx->buf)) {
    /* Get more and convert it when needed */
    نتیجه = Curl_creader_read(داده, reader->بعدی, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(داده, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (صحيح)نتیجه, nread, eos);
    إذا(نتیجه)
      أرجع نتیجه;

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
        إذا(ctx->n_eob >= SMTP_EOB_پیدا_کن_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          نتیجه = Curl_bufq_cwrite(&ctx->buf, buf + شروع, i - شروع, &n);
          إذا(نتیجه)
            أرجع نتیجه;
          نتیجه = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          إذا(نتیجه)
            أرجع نتیجه;
          ctx->n_eob = 0;
          شروع = i;
          إذا(داده->state.infilesize > 0)
            داده->state.infilesize++;
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
        نتیجه = Curl_bufq_cwrite(&ctx->buf, buf + شروع, nread - شروع, &n);
        إذا(نتیجه)
          أرجع نتیجه;
      }
    }
  }

  *peos = FALSE;

  إذا(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ثابت حرف *eob = SMTP_EOB;
    CURL_TRC_SMTP(داده, "auto-ending mail body with '\\r\\n.\\r\\n'");
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
    نتیجه = Curl_bufq_cwrite(&ctx->buf, eob, طول_رشته(eob), &n);
    إذا(نتیجه)
      أرجع نتیجه;
    ctx->processed_eos = TRUE;
  }

  إذا(!Curl_bufq_is_خالی(&ctx->buf)) {
    نتیجه = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  وإلا
    *pnread = 0;

  إذا(ctx->read_eos && Curl_bufq_is_خالی(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(داده, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(داده, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (صحيح)نتیجه, *pnread, *peos));
  أرجع نتیجه;
}

ساكن curl_off_t cr_eob_مجموع_درازی(بنية Curl_easy *داده,
                                      بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)داده;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_مجموع_درازی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_انجامشده,
  حجم(بنية cr_eob_ctx)
};

ساكن CURLcode cr_eob_جمع_کن(بنية Curl_easy *داده)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتیجه;

  نتیجه = Curl_creader_create(&reader, داده, &cr_eob, CURL_CR_CONTENT_ENCODE);
  إذا(!نتیجه)
    نتیجه = Curl_creader_جمع_کن(داده, reader);

  إذا(نتیجه && reader)
    Curl_creader_free(داده, reader);
  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
ساكن bool smtp_endofresp(بنية Curl_easy *داده, بنية connectdata *conn,
                           ثابت حرف *سطر, size_t len, صحيح *resp)
{
  بنية smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool انتها = FALSE;
  (فراغ)داده;

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
    curl_off_t کد;
    ثابت حرف *p = tmpline;
    انتها = TRUE;
    کپی_حافظه(tmpline, سطر, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    إذا(curlx_str_عدد(&p, &کد, len == 5 ? 99999 : 999))
      أرجع FALSE;
    *resp = (صحيح)کد;

    /* Make sure real server never sends internal value */
    إذا(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  وإلا إذا(سطر[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    انتها = TRUE;
    *resp = 1;  /* Internal response code */
  }

  أرجع انتها;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ساكن CURLcode smtp_get_پیام(بنية Curl_easy *داده, بنية bufref *بیرون)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(داده->conn, CURL_META_SMTP_CONN);
  حرف *پیام_2;
  size_t len;

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  پیام_2 = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  إذا(len > 4) {
    /* Find the start of the message */
    len -= 4;
    لكل(پیام_2 += 4; ISBLANK(*پیام_2); پیام_2++, len--)
      ;

    /* Find the end of the message */
    طالما(len--)
      إذا(!ISNEWLINE(پیام_2[len]) && !ISBLANK(پیام_2[len]))
        اكسر;

    /* Terminate the message */
    پیام_2[++len] = '\0';
    Curl_bufref_مجموعه(بیرون, پیام_2, len, عدم);
  }
  وإلا
    /* junk input => zero length output */
    Curl_bufref_مجموعه(بیرون, "", 0, عدم);

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
ساكن فراغ smtp_state(بنية Curl_easy *داده,
                       بنية smtp_conn *smtpc,
                       smtpstate newstate)
{
#اگر_تعریف_شده CURLVERBOSE
  /* for debug purposes */
  ساكن ثابت حرف * ثابت نامها[] = {
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
    /*آخرین*/
  };

  إذا(smtpc->state != newstate)
    CURL_TRC_SMTP(داده, "state change from %s to %s",
                  نامها[smtpc->state], نامها[newstate]);
#وإلا
  (فراغ)داده;
#پایان_اگر

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
ساكن CURLcode smtp_perform_ehlo(بنية Curl_easy *داده,
                                  بنية smtp_conn *smtpc)
{
  CURLcode نتیجه = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "EHLO %s", smtpc->domain);

  إذا(!نتیجه)
    smtp_state(داده, smtpc, SMTP_EHLO);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
ساكن CURLcode smtp_perform_helo(بنية Curl_easy *داده,
                                  بنية smtp_conn *smtpc)
{
  CURLcode نتیجه = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "HELO %s", smtpc->domain);

  إذا(!نتیجه)
    smtp_state(داده, smtpc, SMTP_HELO);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ساكن CURLcode smtp_perform_starttls(بنية Curl_easy *داده,
                                      بنية smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "%s", "STARTTLS");

  إذا(!نتیجه)
    smtp_state(داده, smtpc, SMTP_STARTTLS);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساكن CURLcode smtp_perform_upgrade_tls(بنية Curl_easy *داده,
                                         بنية smtp_conn *smtpc)
{
#اگر_تعریف_شده USE_SSL
  /* Start the SSL connection */
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجه = Curl_ssl_cfilter_جمع_کن(
      داده, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    إذا(نتیجه)
      اذهب بیرون;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  نتیجه = Curl_conn_connect(داده, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(داده, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (صحيح)نتیجه, ssldone));
  إذا(!نتیجه && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    نتیجه = smtp_perform_ehlo(داده, smtpc);
  }
بیرون:
  أرجع نتیجه;
#وإلا
  (فراغ)داده;
  (فراغ)smtpc;
  أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ساكن CURLcode smtp_perform_auth(بنية Curl_easy *داده,
                                  ثابت حرف *mech,
                                  ثابت بنية bufref *initresp)
{
  CURLcode نتیجه = CURLE_OK;
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(داده->conn, CURL_META_SMTP_CONN);
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  إذا(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  وإلا {
    /* Send the AUTH command */
    نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "AUTH %s", mech);
  }

  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
ساكن CURLcode smtp_continue_auth(بنية Curl_easy *داده,
                                   ثابت حرف *mech,
                                   ثابت بنية bufref *resp)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(داده->conn, CURL_META_SMTP_CONN);

  (فراغ)mech;
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_pp_sendf(داده, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ساكن CURLcode smtp_cancel_auth(بنية Curl_easy *داده, ثابت حرف *mech)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(داده->conn, CURL_META_SMTP_CONN);

  (فراغ)mech;
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_pp_sendf(داده, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
ساكن CURLcode smtp_perform_authentication(بنية Curl_easy *داده,
                                            بنية smtp_conn *smtpc)
{
  CURLcode نتیجه = CURLE_OK;
  saslprogress پیشرفت;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  إذا(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, داده)) {
    smtp_state(داده, smtpc, SMTP_STOP);
    أرجع نتیجه;
  }

  /* Calculate the SASL login details */
  نتیجه = Curl_sasl_شروع(&smtpc->sasl, داده, FALSE, &پیشرفت);

  إذا(!نتیجه) {
    إذا(پیشرفت == SASL_INPROGRESS)
      smtp_state(داده, smtpc, SMTP_AUTH);
    وإلا
      نتیجه = Curl_sasl_is_blocked(&smtpc->sasl, داده);
  }

  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
ساكن CURLcode smtp_perform_command(بنية Curl_easy *داده,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    إذا(!smtp->custom || !smtp->custom[0]) {
      حرف *آدرس = عدم;
      بنية hostname host = { عدم, عدم, عدم, عدم };
      ثابت حرف *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      نتیجه = smtp_parse_آدرس(داده, smtp->rcpt->داده,
                                  &آدرس, &host, &suffix);
      إذا(نتیجه)
        أرجع نتیجه;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(آدرس) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "VRFY %s%s%s%s",
                             آدرس,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(آدرس);
    }
    وإلا {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !strcmp(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      نتیجه = Curl_pp_sendf(داده, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->داده,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  وإلا
    /* Send the non-recipient based command such as HELP */
    نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  إذا(!نتیجه)
    smtp_state(داده, smtpc, SMTP_COMMAND);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
ساكن CURLcode smtp_perform_mail(بنية Curl_easy *داده,
                                  بنية smtp_conn *smtpc,
                                  بنية SMTP *smtp)
{
  حرف *from = عدم;
  حرف *auth = عدم;
  حرف *اندازه = عدم;
  ثابت حرف *str;
  CURLcode نتیجه = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(داده, STRING_MAIL_FROM);
  إذا(str) {
    حرف *آدرس = عدم;
    بنية hostname host = { عدم, عدم, عدم, عدم };
    ثابت حرف *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    نتیجه = smtp_parse_آدرس(داده, str, &آدرس, &host, &suffix);
    إذا(نتیجه)
      اذهب بیرون;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(آدرس) ||
            !Curl_is_ASCII_name(host.name));

    إذا(host.name) {
      from = curl_maprintf("<%s@%s>%s", آدرس, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    وإلا
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", آدرس, suffix);

    curlx_free(آدرس);
  }
  وإلا
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  إذا(!from) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(داده, STRING_MAIL_AUTH);
  إذا(str && smtpc->sasl.authused) {
    إذا(str[0] != '\0') {
      حرف *آدرس = عدم;
      بنية hostname host = { عدم, عدم, عدم, عدم };
      ثابت حرف *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      نتیجه = smtp_parse_آدرس(داده, str, &آدرس, &host, &suffix);
      إذا(نتیجه)
        اذهب بیرون;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      إذا(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(آدرس) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      إذا(host.name) {
        auth = curl_maprintf("<%s@%s>%s", آدرس, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      وإلا
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", آدرس, suffix);
      curlx_free(آدرس);
    }
    وإلا
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    إذا(!auth) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }

#اگر_تعریف_نشده CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  إذا(IS_MIME_POST(داده)) {
    curl_mimepart *postp = داده->مجموعه.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(بیعلامت صحيح)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_سرآیندها(postp, داده->مجموعه.سرآیندها, 0);
    نتیجه = Curl_mime_prepare_سرآیندها(داده, postp, عدم,
                                       عدم, MIMESTRATEGY_MAIL);

    إذا(!نتیجه && !Curl_checkheaders(داده, STRCONST("Mime-Version")))
      نتیجه = Curl_mime_جمع_کن_سرآیند(&postp->curlheaders, "Mime-Version: 1.0");
    إذا(!نتیجه)
      نتیجه = Curl_creader_مجموعه_mime(داده, postp);
    إذا(نتیجه)
      اذهب بیرون;
    داده->state.infilesize = Curl_creader_مجموع_درازی(داده);
  }
  وإلا
#پایان_اگر
  {
    نتیجه = Curl_creader_مجموعه_fread(داده, داده->state.infilesize);
    إذا(نتیجه)
      اذهب بیرون;
  }

  /* Calculate the optional SIZE parameter */
  إذا(smtpc->اندازه_supported && داده->state.infilesize > 0) {
    اندازه = curl_maprintf("%" FMT_OFF_T, داده->state.infilesize);

    إذا(!اندازه) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
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
      إذا(!Curl_is_ASCII_name(rcpt->داده))
        utf8 = TRUE;

      rcpt = rcpt->بعدی;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  نتیجه = cr_eob_جمع_کن(داده);
  إذا(نتیجه)
    اذهب بیرون;

  /* Send the MAIL command */
  نتیجه = Curl_pp_sendf(داده, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         اندازه ? " SIZE=" : "", /* Optional on SIZE support  */
                         اندازه ? اندازه : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

بیرون:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(اندازه);

  إذا(!نتیجه)
    smtp_state(داده, smtpc, SMTP_MAIL);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
ساكن CURLcode smtp_perform_rcpt_مقصد(بنية Curl_easy *داده,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp)
{
  CURLcode نتیجه = CURLE_OK;
  حرف *آدرس = عدم;
  بنية hostname host = { عدم, عدم, عدم, عدم };
  ثابت حرف *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  نتیجه = smtp_parse_آدرس(داده, smtp->rcpt->داده,
                              &آدرس, &host, &suffix);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Send the RCPT TO command */
  إذا(host.name)
    نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           آدرس, host.name, suffix);
  وإلا
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "RCPT TO:<%s>%s",
                           آدرس, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(آدرس);

  إذا(!نتیجه)
    smtp_state(داده, smtpc, SMTP_RCPT);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
ساكن CURLcode smtp_perform_quit(بنية Curl_easy *داده,
                                  بنية smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "%s", "QUIT");

  إذا(!نتیجه)
    smtp_state(داده, smtpc, SMTP_QUIT);

  أرجع نتیجه;
}

/* For the initial server greeting */
ساكن CURLcode smtp_state_servergreet_resp(بنية Curl_easy *داده,
                                            بنية smtp_conn *smtpc,
                                            صحيح smtpcode,
                                            smtpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode / 100 != 2) {
    failf(داده, "Got unexpected smtp-server response: %d", smtpcode);
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;
  }
  وإلا
    نتیجه = smtp_perform_ehlo(داده, smtpc);

  أرجع نتیجه;
}

/* For STARTTLS responses */
ساكن CURLcode smtp_state_starttls_resp(بنية Curl_easy *داده,
                                         بنية smtp_conn *smtpc,
                                         صحيح smtpcode,
                                         smtpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  /* Pipelining in response is forbidden. */
  إذا(smtpc->pp.سرریز)
    أرجع CURLE_WEIRD_SERVER_پاسخ;

  إذا(smtpcode != 220) {
    إذا(داده->مجموعه.use_ssl != CURLUSESSL_TRY) {
      failf(داده, "STARTTLS denied, code %d", smtpcode);
      نتیجه = CURLE_USE_SSL_FAILED;
    }
    وإلا
      نتیجه = smtp_perform_authentication(داده, smtpc);
  }
  وإلا
    smtp_state(داده, smtpc, SMTP_UPGRADETLS);

  أرجع نتیجه;
}

/* For EHLO responses */
ساكن CURLcode smtp_state_ehlo_resp(بنية Curl_easy *داده,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (فراغ)instate;

  إذا(smtpcode / 100 != 2 && smtpcode != 1) {
    إذا(داده->مجموعه.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(داده->conn, FIRSTSOCKET))
      نتیجه = smtp_perform_helo(داده, smtpc);
    وإلا {
      failf(داده, "Remote access denied: %d", smtpcode);
      نتیجه = CURLE_REMOTE_ACCESS_DENIED;
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
      smtpc->اندازه_supported = TRUE;

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
        بیعلامت کوتاه mechbit;

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
      إذا(داده->مجموعه.use_ssl && !Curl_conn_is_ssl(داده->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        إذا(smtpc->tls_supported)
          /* Switch to TLS connection now */
          نتیجه = smtp_perform_starttls(داده, smtpc);
        وإلا إذا(داده->مجموعه.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          نتیجه = smtp_perform_authentication(داده, smtpc);
        وإلا {
          failf(داده, "STARTTLS not supported.");
          نتیجه = CURLE_USE_SSL_FAILED;
        }
      }
      وإلا
        نتیجه = smtp_perform_authentication(داده, smtpc);
    }
  }
  وإلا {
    failf(داده, "Unexpectedly short EHLO response");
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;
  }

  أرجع نتیجه;
}

/* For HELO responses */
ساكن CURLcode smtp_state_helo_resp(بنية Curl_easy *داده,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode / 100 != 2) {
    failf(داده, "Remote access denied: %d", smtpcode);
    نتیجه = CURLE_REMOTE_ACCESS_DENIED;
  }
  وإلا
    /* End of connect phase */
    smtp_state(داده, smtpc, SMTP_STOP);

  أرجع نتیجه;
}

/* For SASL authentication responses */
ساكن CURLcode smtp_state_auth_resp(بنية Curl_easy *داده,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  saslprogress پیشرفت;

  (فراغ)instate;

  نتیجه = Curl_sasl_continue(&smtpc->sasl, داده, smtpcode, &پیشرفت);
  إذا(!نتیجه)
    بدل(پیشرفت) {
    حالة SASL_انجامشده:
      smtp_state(داده, smtpc, SMTP_STOP);  /* Authenticated */
      اكسر;
    حالة SASL_IDLE:            /* No mechanism left after cancellation */
      failf(داده, "Authentication cancelled");
      نتیجه = CURLE_LOGIN_DENIED;
      اكسر;
    افتراضي:
      اكسر;
    }

  أرجع نتیجه;
}

/* For command responses */
ساكن CURLcode smtp_state_command_resp(بنية Curl_easy *داده,
                                        بنية smtp_conn *smtpc,
                                        بنية SMTP *smtp,
                                        صحيح smtpcode,
                                        smtpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (فراغ)instate;

  إذا((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(داده, "Command failed: %d", smtpcode);
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;
  }
  وإلا {
    إذا(!داده->req.no_body)
      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, سطر, len);

    إذا(!نتیجه && (smtpcode != 1)) {
      إذا(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->بعدی;

        إذا(smtp->rcpt) {
          /* Send the next command */
          نتیجه = smtp_perform_command(داده, smtpc, smtp);
        }
        وإلا
          /* End of DO phase */
          smtp_state(داده, smtpc, SMTP_STOP);
      }
      وإلا
        /* End of DO phase */
        smtp_state(داده, smtpc, SMTP_STOP);
    }
  }

  أرجع نتیجه;
}

/* For MAIL responses */
ساكن CURLcode smtp_state_mail_resp(بنية Curl_easy *داده,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode / 100 != 2) {
    failf(داده, "MAIL failed: %d", smtpcode);
    نتیجه = CURLE_SEND_ERROR;
  }
  وإلا
    /* Start the RCPT TO command */
    نتیجه = smtp_perform_rcpt_مقصد(داده, smtpc, smtp);

  أرجع نتیجه;
}

/* For RCPT responses */
ساكن CURLcode smtp_state_rcpt_resp(بنية Curl_easy *داده,
                                     بنية smtp_conn *smtpc,
                                     بنية SMTP *smtp,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (فراغ)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !داده->مجموعه.mail_rcpt_allowfails);

  إذا(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_آخرین_error = smtpcode;

    إذا(is_smtp_blocking_err) {
      failf(داده, "RCPT failed: %d", smtpcode);
      نتیجه = CURLE_SEND_ERROR;
    }
  }
  وإلا {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  إذا(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->بعدی;

    إذا(smtp->rcpt)
      /* Send the next RCPT TO command */
      نتیجه = smtp_perform_rcpt_مقصد(داده, smtpc, smtp);
    وإلا {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      إذا(!smtp->rcpt_had_ok) {
        failf(داده, "RCPT failed: %d (last error)", smtp->rcpt_آخرین_error);
        نتیجه = CURLE_SEND_ERROR;
      }
      وإلا {
        /* Send the DATA command */
        نتیجه = Curl_pp_sendf(داده, &smtpc->pp, "%s", "DATA");

        إذا(!نتیجه)
          smtp_state(داده, smtpc, SMTP_داده);
      }
    }
  }

  أرجع نتیجه;
}

/* For DATA response */
ساكن CURLcode smtp_state_داده_resp(بنية Curl_easy *داده,
                                     بنية smtp_conn *smtpc,
                                     صحيح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  إذا(smtpcode != 354) {
    failf(داده, "DATA failed: %d", smtpcode);
    نتیجه = CURLE_SEND_ERROR;
  }
  وإلا {
    /* Set the progress upload size */
    Curl_pgrs_مجموعه_Upload_اندازه(داده, داده->state.infilesize);

    /* SMTP upload */
    Curl_xfer_راهاندازی_send(داده, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(داده, smtpc, SMTP_STOP);
  }

  أرجع نتیجه;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
ساكن CURLcode smtp_state_postdata_resp(بنية Curl_easy *داده,
                                         بنية smtp_conn *smtpc,
                                         صحيح smtpcode,
                                         smtpstate instate)
{
  CURLcode نتیجه = CURLE_OK;

  (فراغ)instate;

  إذا(smtpcode != 250)
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;

  /* End of DONE phase */
  smtp_state(داده, smtpc, SMTP_STOP);

  أرجع نتیجه;
}

ساكن CURLcode smtp_pp_statemachine(بنية Curl_easy *داده,
                                     بنية connectdata *conn)
{
  CURLcode نتیجه = CURLE_OK;
  صحيح smtpcode;
  بنية smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  بنية SMTP *smtp = Curl_meta_get(داده, CURL_META_SMTP_EASY);
  size_t nread = 0;

  إذا(!smtpc || !smtp)
    أرجع CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  إذا(smtpc->state == SMTP_UPGRADETLS) {
    نتیجه = smtp_perform_upgrade_tls(داده, smtpc);
    إذا(نتیجه || (smtpc->state == SMTP_UPGRADETLS))
      أرجع نتیجه;
  }

  /* Flush any data that needs to be sent */
  إذا(smtpc->pp.sendleft)
    أرجع Curl_pp_flushsend(داده, &smtpc->pp);

  افعل {
    /* Read the response from the server */
    نتیجه = Curl_pp_readresp(داده, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    إذا(نتیجه)
      أرجع نتیجه;

    /* Store the latest response for later retrieval if necessary */
    إذا(smtpc->state != SMTP_QUIT && smtpcode != 1)
      داده->info.httpcode = smtpcode;

    إذا(!smtpcode)
      اكسر;

    /* We have now received a full SMTP server response */
    بدل(smtpc->state) {
    حالة SMTP_SERVERGREET:
      نتیجه = smtp_state_servergreet_resp(داده, smtpc,
                                           smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_EHLO:
      نتیجه = smtp_state_ehlo_resp(داده, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_HELO:
      نتیجه = smtp_state_helo_resp(داده, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_STARTTLS:
      نتیجه = smtp_state_starttls_resp(داده, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      إذا(!نتیجه && (smtpc->state == SMTP_UPGRADETLS))
        اذهب upgrade_tls;
      اكسر;

    حالة SMTP_AUTH:
      نتیجه = smtp_state_auth_resp(داده, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_COMMAND:
      نتیجه = smtp_state_command_resp(داده, smtpc, smtp,
                                       smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_MAIL:
      نتیجه = smtp_state_mail_resp(داده, smtpc, smtp, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_RCPT:
      نتیجه = smtp_state_rcpt_resp(داده, smtpc, smtp, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_داده:
      نتیجه = smtp_state_داده_resp(داده, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_POSTDATA:
      نتیجه = smtp_state_postdata_resp(داده, smtpc, smtpcode, smtpc->state);
      اكسر;

    حالة SMTP_QUIT:
    افتراضي:
      /* internal error */
      smtp_state(داده, smtpc, SMTP_STOP);
      اكسر;
    }
  } طالما(!نتیجه && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  أرجع نتیجه;
}

/* Called repeatedly until done from multi.c */
ساكن CURLcode smtp_multi_statemach(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(داده->conn, CURL_META_SMTP_CONN);

  *انجامشده = FALSE;
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  نتیجه = Curl_pp_statemach(داده, &smtpc->pp, FALSE, FALSE);
  *انجامشده = (smtpc->state == SMTP_STOP);
  أرجع نتیجه;
}

ساكن CURLcode smtp_بلوک_statemach(بنية Curl_easy *داده,
                                     بنية smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode نتیجه = CURLE_OK;

  طالما(smtpc->state != SMTP_STOP && !نتیجه)
    نتیجه = Curl_pp_statemach(داده, &smtpc->pp, TRUE, disconnecting);

  أرجع نتیجه;
}

/* For the SMTP "protocol connect" and "doing" phases only */
ساكن CURLcode smtp_pollset(بنية Curl_easy *داده,
                             بنية easy_pollset *ps)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(داده->conn, CURL_META_SMTP_CONN);
  أرجع smtpc ? Curl_pp_pollset(داده, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
ساكن ثابت بنية SASLproto saslsmtp = {
  "smtp",               /* The service name */
  smtp_perform_auth,    /* Send authentication command */
  smtp_continue_auth,   /* Send authentication continuation */
  smtp_cancel_auth,     /* Cancel authentication */
  smtp_get_پیام,     /* Get SASL response message */
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
ساكن CURLcode smtp_connect(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(داده->conn, CURL_META_SMTP_CONN);
  CURLcode نتیجه = CURLE_OK;

  *انجامشده = FALSE; /* default to not done yet */
  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  PINGPONG_راهاندازی(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, داده, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(داده));

  /* Parse the URL options */
  نتیجه = smtp_parse_url_options(داده->conn, smtpc);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Parse the URL path */
  نتیجه = smtp_parse_url_مسیر(داده, smtpc);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Start off waiting for the server greeting response */
  smtp_state(داده, smtpc, SMTP_SERVERGREET);

  نتیجه = smtp_multi_statemach(داده, انجامشده);

  أرجع نتیجه;
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
ساكن CURLcode smtp_انجامشده(بنية Curl_easy *داده, CURLcode وضعیت,
                          bool premature)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(داده->conn, CURL_META_SMTP_CONN);
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية SMTP *smtp = Curl_meta_get(داده, CURL_META_SMTP_EASY);

  (فراغ)premature;

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;
  إذا(!smtp)
    أرجع CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  إذا(وضعیت) {
    CURL_TRC_M(داده, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    نتیجه = وضعیت;         /* use the already set error code */
  }
  وإلا إذا(!داده->مجموعه.connect_only && داده->مجموعه.mail_rcpt &&
          (داده->state.upload || IS_MIME_POST(داده))) {

    smtp_state(داده, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    نتیجه = smtp_بلوک_statemach(داده, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(داده, "smtp_done(status=%d, premature=%d) -> %d",
                (صحيح)وضعیت, premature, (صحيح)نتیجه);
  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
ساكن CURLcode smtp_perform(بنية Curl_easy *داده,
                             بنية smtp_conn *smtpc,
                             بنية SMTP *smtp,
                             bool *connected,
                             bool *dophase_انجامشده)
{
  /* This is SMTP and no proxy */
  CURLcode نتیجه = CURLE_OK;

  CURL_TRC_SMTP(داده, "smtp_perform(), start");

  إذا(داده->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_انجامشده = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = داده->مجموعه.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_آخرین_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  إذا((داده->state.upload || IS_MIME_POST(داده)) && داده->مجموعه.mail_rcpt)
    /* MAIL transfer */
    نتیجه = smtp_perform_mail(داده, smtpc, smtp);
  وإلا
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    نتیجه = smtp_perform_command(داده, smtpc, smtp);

  إذا(نتیجه)
    اذهب بیرون;

  /* Run the state-machine */
  نتیجه = smtp_multi_statemach(داده, dophase_انجامشده);

  *connected = Curl_conn_is_connected(داده->conn, FIRSTSOCKET);

بیرون:
  CURL_TRC_SMTP(داده, "smtp_perform() -> %d, connected=%d, done=%d",
                (صحيح)نتیجه, *connected, *dophase_انجامشده);
  أرجع نتیجه;
}

/* Call this when the DO phase has completed */
ساكن CURLcode smtp_dophase_انجامشده(بنية Curl_easy *داده,
                                  بنية SMTP *smtp,
                                  bool connected)
{
  (فراغ)connected;

  إذا(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_راهاندازی_nop(داده);

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
ساكن CURLcode smtp_regular_transfer(بنية Curl_easy *داده,
                                      بنية smtp_conn *smtpc,
                                      بنية SMTP *smtp,
                                      bool *dophase_انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  داده->req.اندازه = -1;

  /* Set the progress data */
  Curl_pgrsReset(داده);

  /* Carry out the perform */
  نتیجه = smtp_perform(داده, smtpc, smtp, &connected, dophase_انجامشده);

  /* Perform post DO phase operations if necessary */
  إذا(!نتیجه && *dophase_انجامشده)
    نتیجه = smtp_dophase_انجامشده(داده, smtp, connected);

  CURL_TRC_SMTP(داده, "smtp_regular_transfer() -> %d, done=%d",
                (صحيح)نتیجه, *dophase_انجامشده);
  أرجع نتیجه;
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
ساكن CURLcode smtp_do(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية smtp_conn *smtpc =
    Curl_conn_meta_get(داده->conn, CURL_META_SMTP_CONN);
  بنية SMTP *smtp = Curl_meta_get(داده, CURL_META_SMTP_EASY);
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);
  *انجامشده = FALSE; /* default to false */
  إذا(!smtpc || !smtp)
    أرجع CURLE_FAILED_INIT;

  /* Parse the custom request */
  نتیجه = smtp_parse_custom_request(داده, smtp);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = smtp_regular_transfer(داده, smtpc, smtp, انجامشده);
  CURL_TRC_SMTP(داده, "smtp_do() -> %d, done=%d", (صحيح)نتیجه, *انجامشده);
  أرجع نتیجه;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode smtp_disconnect(بنية Curl_easy *داده,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  بنية smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  إذا(!smtpc)
    أرجع CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  إذا(!dead_connection && conn->بیتها.protoconnstart &&
     !Curl_pp_needs_flush(داده, &smtpc->pp) &&
     !smtp_perform_quit(داده, smtpc))
    (فراغ)smtp_بلوک_statemach(داده, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(داده, "smtp_disconnect(), finished");
  أرجع CURLE_OK;
}

/* Called from multi.c while DOing */
ساكن CURLcode smtp_doing(بنية Curl_easy *داده, bool *dophase_انجامشده)
{
  بنية SMTP *smtp = Curl_meta_get(داده, CURL_META_SMTP_EASY);
  CURLcode نتیجه;

  إذا(!smtp)
    أرجع CURLE_FAILED_INIT;
  نتیجه = smtp_multi_statemach(داده, dophase_انجامشده);
  إذا(نتیجه)
    DEBUGF(infof(داده, "DO phase failed"));
  وإلا إذا(*dophase_انجامشده) {
    نتیجه = smtp_dophase_انجامشده(داده, smtp, FALSE /* not connected */);

    DEBUGF(infof(داده, "DO phase is complete"));
  }

  CURL_TRC_SMTP(داده, "smtp_doing() -> %d, done=%d", (صحيح)نتیجه,
                *dophase_انجامشده);
  أرجع نتیجه;
}

ساكن فراغ smtp_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية SMTP *smtp = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_free(smtp);
}

ساكن فراغ smtp_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية smtp_conn *smtpc = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

ساكن CURLcode smtp_راهاندازی_connection(بنية Curl_easy *داده,
                                      بنية connectdata *conn)
{
  بنية smtp_conn *smtpc;
  بنية SMTP *smtp;
  CURLcode نتیجه = CURLE_OK;

  smtpc = curlx_calloc(1, حجم(*smtpc));
  إذا(!smtpc ||
     Curl_conn_meta_مجموعه(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  smtp = curlx_calloc(1, حجم(*smtp));
  إذا(!smtp ||
     Curl_meta_مجموعه(داده, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    نتیجه = CURLE_بیرون_از_حافظه;

بیرون:
  CURL_TRC_SMTP(داده, "smtp_setup_connection() -> %d", (صحيح)نتیجه);
  أرجع نتیجه;
}

/*
 * SMTP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_smtp = {
  smtp_راهاندازی_connection,            /* setup_connection */
  smtp_do,                          /* do_it */
  smtp_انجامشده,                        /* done */
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

#پایان_اگر /* CURL_DISABLE_SMTP */
