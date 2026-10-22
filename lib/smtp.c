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
#include "curl_setup.h"
#include "urldata.h"
#include "smtp.h"

#ifndef CURL_DISABLE_SMTP

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include "sendf.h"
#include "curl_trc.h"
#include "progress.h"
#include "transfer.h"
#include "escape.h"
#include "pingpong.h"
#include "mime.h"
#include "vtls/vtls.h"
#include "cfilters.h"
#include "connect.h"
#include "select.h"
#include "url.h"
#include "curl_gethostname.h"
#include "bufref.h"
#include "curl_sasl.h"
#include "idn.h"
#include "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
typedef enum {
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
struct smtp_conn {
  struct pingpong pp;
  struct SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  char *domain;            /* Client address/name to send in the EHLO */
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
struct SMTP {
  curl_pp_transfer transfer;
  char *custom;            /* Custom Request */
  struct curl_slist *rcpt; /* Recipient list */
  整数 rcpt_last_error;     /* The last error received for RCPT TO command */
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
static CURLcode smtp_parse_url_options(struct connectdata *conn,
                                       struct smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  const char *ptr = conn->options;

  当(!result && ptr && *ptr) {
    const char *key = ptr;
    const char *value;

    当(*ptr && *ptr != '=')
      ptr++;

    value = ptr + 1;

    当(*ptr && *ptr != ';')
      ptr++;

    如果(curl_strnequal(key, "AUTH=", 5))
      result = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               value, ptr - value);
    否则
      result = CURLE_URL_MALFORMAT;

    如果(*ptr == ';')
      ptr++;
  }

  返回 result;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
static CURLcode smtp_parse_url_path(struct Curl_easy *data,
                                    struct smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  const char *path = &data->state.up.path[1]; /* skip leading path */
  char localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  如果(!*path) {
    如果(!Curl_gethostname(localhost, sizeof(localhost)))
      path = localhost;
    否则
      path = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  返回 Curl_urldecode(path, 0, &smtpc->domain, NULL, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
static CURLcode smtp_parse_custom_request(struct Curl_easy *data,
                                          struct SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  const char *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  如果(custom)
    result = Curl_urldecode(custom, 0, &smtp->custom, NULL, REJECT_CTRL);

  返回 result;
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
static CURLcode smtp_parse_address(struct Curl_easy *data, const char *fqma,
                                   char **address, struct hostname *host,
                                   const char **suffix)
{
  CURLcode result = CURLE_OK;
  size_t length;
  char *addressend;
  char *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  如果(strpbrk(fqma, "\r\n")) {
    failf(data, "Refusing to send email address with a CR or LF");
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  如果(!dup)
    返回 CURLE_OUT_OF_MEMORY;

  如果(fqma[0] != '<') {
    length = strlen(dup);
    如果(length && dup[length - 1] == '>')
      dup[length - 1] = '\0';
  }
  否则 {
    addressend = strrchr(dup, '>');
    如果(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  如果(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (void)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *address = dup;

  返回 result;
}

struct cr_eob_ctx {
  struct Curl_creader super;
  struct bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

static CURLcode cr_eob_init(struct Curl_easy *data,
                            struct Curl_creader *reader)
{
  struct cr_eob_ctx *ctx = reader->ctx;
  (void)data;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  返回 CURLE_OK;
}

static void cr_eob_close(struct Curl_easy *data, struct Curl_creader *reader)
{
  struct cr_eob_ctx *ctx = reader->ctx;
  (void)data;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#define SMTP_EOB          "\r\n.\r\n"
#define SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
static CURLcode cr_eob_read(struct Curl_easy *data,
                            struct Curl_creader *reader,
                            char *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  struct cr_eob_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, i, start, n;
  bool eos;

  如果(!ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* Get more and convert it when needed */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(data, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (整数)result, nread, eos);
    如果(result)
      返回 result;

    ctx->read_eos = eos;
    如果(nread) {
      如果(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        返回 CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      为(i = start = 0; i < nread; ++i) {
        如果(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          如果(result)
            返回 result;
          result = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          如果(result)
            返回 result;
          ctx->n_eob = 0;
          start = i;
          如果(data->state.infilesize > 0)
            data->state.infilesize++;
        }

        如果(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        如果(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      如果(start < nread) {
        result = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        如果(result)
          返回 result;
      }
    }
  }

  *peos = FALSE;

  如果(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    const char *eob = SMTP_EOB;
    CURL_TRC_SMTP(data, "auto-ending mail body with '\\r\\n.\\r\\n'");
    switch(ctx->n_eob) {
    case 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      break;
    case 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      break;
    default:
      break;
    }
    result = Curl_bufq_cwrite(&ctx->buf, eob, strlen(eob), &n);
    如果(result)
      返回 result;
    ctx->processed_eos = TRUE;
  }

  如果(!Curl_bufq_is_empty(&ctx->buf)) {
    result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  否则
    *pnread = 0;

  如果(ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(data, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(data, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (整数)result, *pnread, *peos));
  返回 result;
}

static curl_off_t cr_eob_total_length(struct Curl_easy *data,
                                      struct Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (void)data;
  (void)reader;
  返回 -1;
}

static const struct Curl_crtype cr_eob = {
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
  sizeof(struct cr_eob_ctx)
};

static CURLcode cr_eob_add(struct Curl_easy *data)
{
  struct Curl_creader *reader = NULL;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_eob, CURL_CR_CONTENT_ENCODE);
  如果(!result)
    result = Curl_creader_add(data, reader);

  如果(result && reader)
    Curl_creader_free(data, reader);
  返回 result;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
static bool smtp_endofresp(struct Curl_easy *data, struct connectdata *conn,
                           const char *line, size_t len, 整数 *resp)
{
  struct smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool end = FALSE;
  (void)data;

  DEBUGASSERT(smtpc);
  如果(!smtpc)
    返回 FALSE;

  /* Nothing for us */
  如果(len < 4 || !ISDIGIT(line[0]) || !ISDIGIT(line[1]) || !ISDIGIT(line[2]))
    返回 FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  如果(line[3] == ' ' || len == 5) {
    char tmpline[6];
    curl_off_t code;
    const char *p = tmpline;
    end = TRUE;
    memcpy(tmpline, line, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    如果(curlx_str_number(&p, &code, len == 5 ? 99999 : 999))
      返回 FALSE;
    *resp = (整数)code;

    /* Make sure real server never sends internal value */
    如果(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  否则 如果(line[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    end = TRUE;
    *resp = 1;  /* Internal response code */
  }

  返回 end;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
static CURLcode smtp_get_message(struct Curl_easy *data, struct bufref *out)
{
  struct smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  char *message;
  size_t len;

  如果(!smtpc)
    返回 CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  如果(len > 4) {
    /* Find the start of the message */
    len -= 4;
    为(message += 4; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    当(len--)
      如果(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        break;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, NULL);
  }
  否则
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, NULL);

  返回 CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
static void smtp_state(struct Curl_easy *data,
                       struct smtp_conn *smtpc,
                       smtpstate newstate)
{
#ifdef CURLVERBOSE
  /* for debug purposes */
  static const char * const names[] = {
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

  如果(smtpc->state != newstate)
    CURL_TRC_SMTP(data, "state change from %s to %s",
                  names[smtpc->state], names[newstate]);
#否则
  (void)data;
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
static CURLcode smtp_perform_ehlo(struct Curl_easy *data,
                                  struct smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "EHLO %s", smtpc->domain);

  如果(!result)
    smtp_state(data, smtpc, SMTP_EHLO);

  返回 result;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
static CURLcode smtp_perform_helo(struct Curl_easy *data,
                                  struct smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "HELO %s", smtpc->domain);

  如果(!result)
    smtp_state(data, smtpc, SMTP_HELO);

  返回 result;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
static CURLcode smtp_perform_starttls(struct Curl_easy *data,
                                      struct smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "STARTTLS");

  如果(!result)
    smtp_state(data, smtpc, SMTP_STARTTLS);

  返回 result;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
static CURLcode smtp_perform_upgrade_tls(struct Curl_easy *data,
                                         struct smtp_conn *smtpc)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  struct connectdata *conn = data->conn;
  CURLcode result;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  如果(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    如果(result)
      goto out;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (整数)result, ssldone));
  如果(!result && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    result = smtp_perform_ehlo(data, smtpc);
  }
out:
  返回 result;
#否则
  (void)data;
  (void)smtpc;
  返回 CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
static CURLcode smtp_perform_auth(struct Curl_easy *data,
                                  const char *mech,
                                  const struct bufref *initresp)
{
  CURLcode result = CURLE_OK;
  struct smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  const char *ir = Curl_bufref_ptr(initresp);

  如果(!smtpc)
    返回 CURLE_FAILED_INIT;

  如果(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  否则 {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s", mech);
  }

  返回 result;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
static CURLcode smtp_continue_auth(struct Curl_easy *data,
                                   const char *mech,
                                   const struct bufref *resp)
{
  struct smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (void)mech;
  如果(!smtpc)
    返回 CURLE_FAILED_INIT;
  返回 Curl_pp_sendf(data, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
static CURLcode smtp_cancel_auth(struct Curl_easy *data, const char *mech)
{
  struct smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (void)mech;
  如果(!smtpc)
    返回 CURLE_FAILED_INIT;
  返回 Curl_pp_sendf(data, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
static CURLcode smtp_perform_authentication(struct Curl_easy *data,
                                            struct smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  如果(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, data)) {
    smtp_state(data, smtpc, SMTP_STOP);
    返回 result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&smtpc->sasl, data, FALSE, &progress);

  如果(!result) {
    如果(progress == SASL_INPROGRESS)
      smtp_state(data, smtpc, SMTP_AUTH);
    否则
      result = Curl_sasl_is_blocked(&smtpc->sasl, data);
  }

  返回 result;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
static CURLcode smtp_perform_command(struct Curl_easy *data,
                                     struct smtp_conn *smtpc,
                                     struct SMTP *smtp)
{
  CURLcode result = CURLE_OK;

  如果(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    如果(!smtp->custom || !smtp->custom[0]) {
      char *address = NULL;
      struct hostname host = { NULL, NULL, NULL, NULL };
      const char *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, smtp->rcpt->data,
                                  &address, &host, &suffix);
      如果(result)
        返回 result;

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
    否则 {
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
  否则
    /* Send the non-recipient based command such as HELP */
    result = Curl_pp_sendf(data, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  如果(!result)
    smtp_state(data, smtpc, SMTP_COMMAND);

  返回 result;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
static CURLcode smtp_perform_mail(struct Curl_easy *data,
                                  struct smtp_conn *smtpc,
                                  struct SMTP *smtp)
{
  char *from = NULL;
  char *auth = NULL;
  char *size = NULL;
  const char *str;
  CURLcode result = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_FROM);
  如果(str) {
    char *address = NULL;
    struct hostname host = { NULL, NULL, NULL, NULL };
    const char *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    result = smtp_parse_address(data, str, &address, &host, &suffix);
    如果(result)
      goto out;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(address) ||
            !Curl_is_ASCII_name(host.name));

    如果(host.name) {
      from = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    否则
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", address, suffix);

    curlx_free(address);
  }
  否则
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  如果(!from) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_AUTH);
  如果(str && smtpc->sasl.authused) {
    如果(str[0] != '\0') {
      char *address = NULL;
      struct hostname host = { NULL, NULL, NULL, NULL };
      const char *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, str, &address, &host, &suffix);
      如果(result)
        goto out;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      如果(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(address) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      如果(host.name) {
        auth = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      否则
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", address, suffix);
      curlx_free(address);
    }
    否则
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    如果(!auth) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  如果(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned 整数)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, NULL,
                                       NULL, MIMESTRATEGY_MAIL);

    如果(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    如果(!result)
      result = Curl_creader_set_mime(data, postp);
    如果(result)
      goto out;
    data->state.infilesize = Curl_creader_total_length(data);
  }
  否则
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    如果(result)
      goto out;
  }

  /* Calculate the optional SIZE parameter */
  如果(smtpc->size_supported && data->state.infilesize > 0) {
    size = curl_maprintf("%" FMT_OFF_T, data->state.infilesize);

    如果(!size) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  如果(smtpc->utf8_supported && !utf8) {
    struct curl_slist *rcpt = smtp->rcpt;

    当(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      如果(!Curl_is_ASCII_name(rcpt->data))
        utf8 = TRUE;

      rcpt = rcpt->next;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  result = cr_eob_add(data);
  如果(result)
    goto out;

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

  如果(!result)
    smtp_state(data, smtpc, SMTP_MAIL);

  返回 result;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
static CURLcode smtp_perform_rcpt_to(struct Curl_easy *data,
                                     struct smtp_conn *smtpc,
                                     struct SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  char *address = NULL;
  struct hostname host = { NULL, NULL, NULL, NULL };
  const char *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  result = smtp_parse_address(data, smtp->rcpt->data,
                              &address, &host, &suffix);
  如果(result)
    返回 result;

  /* Send the RCPT TO command */
  如果(host.name)
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           address, host.name, suffix);
  否则
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s>%s",
                           address, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(address);

  如果(!result)
    smtp_state(data, smtpc, SMTP_RCPT);

  返回 result;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
static CURLcode smtp_perform_quit(struct Curl_easy *data,
                                  struct smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "QUIT");

  如果(!result)
    smtp_state(data, smtpc, SMTP_QUIT);

  返回 result;
}

/* For the initial server greeting */
static CURLcode smtp_state_servergreet_resp(struct Curl_easy *data,
                                            struct smtp_conn *smtpc,
                                            整数 smtpcode,
                                            smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  如果(smtpcode / 100 != 2) {
    failf(data, "Got unexpected smtp-server response: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  否则
    result = smtp_perform_ehlo(data, smtpc);

  返回 result;
}

/* For STARTTLS responses */
static CURLcode smtp_state_starttls_resp(struct Curl_easy *data,
                                         struct smtp_conn *smtpc,
                                         整数 smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  /* Pipelining in response is forbidden. */
  如果(smtpc->pp.overflow)
    返回 CURLE_WEIRD_SERVER_REPLY;

  如果(smtpcode != 220) {
    如果(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied, code %d", smtpcode);
      result = CURLE_USE_SSL_FAILED;
    }
    否则
      result = smtp_perform_authentication(data, smtpc);
  }
  否则
    smtp_state(data, smtpc, SMTP_UPGRADETLS);

  返回 result;
}

/* For EHLO responses */
static CURLcode smtp_state_ehlo_resp(struct Curl_easy *data,
                                     struct smtp_conn *smtpc,
                                     整数 smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  const char *line = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (void)instate;

  如果(smtpcode / 100 != 2 && smtpcode != 1) {
    如果(data->set.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(data->conn, FIRSTSOCKET))
      result = smtp_perform_helo(data, smtpc);
    否则 {
      failf(data, "Remote access denied: %d", smtpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  否则 如果(len >= 4) {
    line += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    如果(len >= 8 && curl_strnequal(line, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    否则 如果(len >= 4 && curl_strnequal(line, "SIZE", 4))
      smtpc->size_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    否则 如果(len >= 8 && curl_strnequal(line, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    否则 如果(len >= 5 && curl_strnequal(line, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      line += 5;
      len -= 5;

      /* Loop through the data line */
      为(;;) {
        size_t llen;
        size_t wordlen;
        unsigned short mechbit;

        当(len && (ISBLANK(*line) || ISNEWLINE(*line))) {
          line++;
          len--;
        }

        如果(!len)
          break;

        /* Extract the word */
        为(wordlen = 0; wordlen < len && !ISBLANK(line[wordlen]) &&
              !ISNEWLINE(line[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(line, wordlen, &llen);
        如果(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        line += wordlen;
        len -= wordlen;
      }
    }

    如果(smtpcode != 1) {
      如果(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        如果(smtpc->tls_supported)
          /* Switch to TLS connection now */
          result = smtp_perform_starttls(data, smtpc);
        否则 如果(data->set.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          result = smtp_perform_authentication(data, smtpc);
        否则 {
          failf(data, "STARTTLS not supported.");
          result = CURLE_USE_SSL_FAILED;
        }
      }
      否则
        result = smtp_perform_authentication(data, smtpc);
    }
  }
  否则 {
    failf(data, "Unexpectedly short EHLO response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  返回 result;
}

/* For HELO responses */
static CURLcode smtp_state_helo_resp(struct Curl_easy *data,
                                     struct smtp_conn *smtpc,
                                     整数 smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  如果(smtpcode / 100 != 2) {
    failf(data, "Remote access denied: %d", smtpcode);
    result = CURLE_REMOTE_ACCESS_DENIED;
  }
  否则
    /* End of connect phase */
    smtp_state(data, smtpc, SMTP_STOP);

  返回 result;
}

/* For SASL authentication responses */
static CURLcode smtp_state_auth_resp(struct Curl_easy *data,
                                     struct smtp_conn *smtpc,
                                     整数 smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (void)instate;

  result = Curl_sasl_continue(&smtpc->sasl, data, smtpcode, &progress);
  如果(!result)
    switch(progress) {
    case SASL_DONE:
      smtp_state(data, smtpc, SMTP_STOP);  /* Authenticated */
      break;
    case SASL_IDLE:            /* No mechanism left after cancellation */
      failf(data, "Authentication cancelled");
      result = CURLE_LOGIN_DENIED;
      break;
    default:
      break;
    }

  返回 result;
}

/* For command responses */
static CURLcode smtp_state_command_resp(struct Curl_easy *data,
                                        struct smtp_conn *smtpc,
                                        struct SMTP *smtp,
                                        整数 smtpcode,
                                        smtpstate instate)
{
  CURLcode result = CURLE_OK;
  const char *line = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (void)instate;

  如果((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(data, "Command failed: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  否则 {
    如果(!data->req.no_body)
      result = Curl_client_write(data, CLIENTWRITE_BODY, line, len);

    如果(!result && (smtpcode != 1)) {
      如果(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->next;

        如果(smtp->rcpt) {
          /* Send the next command */
          result = smtp_perform_command(data, smtpc, smtp);
        }
        否则
          /* End of DO phase */
          smtp_state(data, smtpc, SMTP_STOP);
      }
      否则
        /* End of DO phase */
        smtp_state(data, smtpc, SMTP_STOP);
    }
  }

  返回 result;
}

/* For MAIL responses */
static CURLcode smtp_state_mail_resp(struct Curl_easy *data,
                                     struct smtp_conn *smtpc,
                                     struct SMTP *smtp,
                                     整数 smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  如果(smtpcode / 100 != 2) {
    failf(data, "MAIL failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  否则
    /* Start the RCPT TO command */
    result = smtp_perform_rcpt_to(data, smtpc, smtp);

  返回 result;
}

/* For RCPT responses */
static CURLcode smtp_state_rcpt_resp(struct Curl_easy *data,
                                     struct smtp_conn *smtpc,
                                     struct SMTP *smtp,
                                     整数 smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (void)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !data->set.mail_rcpt_allowfails);

  如果(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_last_error = smtpcode;

    如果(is_smtp_blocking_err) {
      failf(data, "RCPT failed: %d", smtpcode);
      result = CURLE_SEND_ERROR;
    }
  }
  否则 {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  如果(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->next;

    如果(smtp->rcpt)
      /* Send the next RCPT TO command */
      result = smtp_perform_rcpt_to(data, smtpc, smtp);
    否则 {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      如果(!smtp->rcpt_had_ok) {
        failf(data, "RCPT failed: %d (last error)", smtp->rcpt_last_error);
        result = CURLE_SEND_ERROR;
      }
      否则 {
        /* Send the DATA command */
        result = Curl_pp_sendf(data, &smtpc->pp, "%s", "DATA");

        如果(!result)
          smtp_state(data, smtpc, SMTP_DATA);
      }
    }
  }

  返回 result;
}

/* For DATA response */
static CURLcode smtp_state_data_resp(struct Curl_easy *data,
                                     struct smtp_conn *smtpc,
                                     整数 smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  如果(smtpcode != 354) {
    failf(data, "DATA failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  否则 {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* SMTP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(data, smtpc, SMTP_STOP);
  }

  返回 result;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
static CURLcode smtp_state_postdata_resp(struct Curl_easy *data,
                                         struct smtp_conn *smtpc,
                                         整数 smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;

  (void)instate;

  如果(smtpcode != 250)
    result = CURLE_WEIRD_SERVER_REPLY;

  /* End of DONE phase */
  smtp_state(data, smtpc, SMTP_STOP);

  返回 result;
}

static CURLcode smtp_pp_statemachine(struct Curl_easy *data,
                                     struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  整数 smtpcode;
  struct smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  struct SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  size_t nread = 0;

  如果(!smtpc || !smtp)
    返回 CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  如果(smtpc->state == SMTP_UPGRADETLS) {
    result = smtp_perform_upgrade_tls(data, smtpc);
    如果(result || (smtpc->state == SMTP_UPGRADETLS))
      返回 result;
  }

  /* Flush any data that needs to be sent */
  如果(smtpc->pp.sendleft)
    返回 Curl_pp_flushsend(data, &smtpc->pp);

  do {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    如果(result)
      返回 result;

    /* Store the latest response for later retrieval if necessary */
    如果(smtpc->state != SMTP_QUIT && smtpcode != 1)
      data->info.httpcode = smtpcode;

    如果(!smtpcode)
      break;

    /* We have now received a full SMTP server response */
    switch(smtpc->state) {
    case SMTP_SERVERGREET:
      result = smtp_state_servergreet_resp(data, smtpc,
                                           smtpcode, smtpc->state);
      break;

    case SMTP_EHLO:
      result = smtp_state_ehlo_resp(data, smtpc, smtpcode, smtpc->state);
      break;

    case SMTP_HELO:
      result = smtp_state_helo_resp(data, smtpc, smtpcode, smtpc->state);
      break;

    case SMTP_STARTTLS:
      result = smtp_state_starttls_resp(data, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      如果(!result && (smtpc->state == SMTP_UPGRADETLS))
        goto upgrade_tls;
      break;

    case SMTP_AUTH:
      result = smtp_state_auth_resp(data, smtpc, smtpcode, smtpc->state);
      break;

    case SMTP_COMMAND:
      result = smtp_state_command_resp(data, smtpc, smtp,
                                       smtpcode, smtpc->state);
      break;

    case SMTP_MAIL:
      result = smtp_state_mail_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      break;

    case SMTP_RCPT:
      result = smtp_state_rcpt_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      break;

    case SMTP_DATA:
      result = smtp_state_data_resp(data, smtpc, smtpcode, smtpc->state);
      break;

    case SMTP_POSTDATA:
      result = smtp_state_postdata_resp(data, smtpc, smtpcode, smtpc->state);
      break;

    case SMTP_QUIT:
    default:
      /* internal error */
      smtp_state(data, smtpc, SMTP_STOP);
      break;
    }
  } 当(!result && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  返回 result;
}

/* Called repeatedly until done from multi.c */
static CURLcode smtp_multi_statemach(struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  struct smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  *done = FALSE;
  如果(!smtpc)
    返回 CURLE_FAILED_INIT;

  result = Curl_pp_statemach(data, &smtpc->pp, FALSE, FALSE);
  *done = (smtpc->state == SMTP_STOP);
  返回 result;
}

static CURLcode smtp_block_statemach(struct Curl_easy *data,
                                     struct smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  当(smtpc->state != SMTP_STOP && !result)
    result = Curl_pp_statemach(data, &smtpc->pp, TRUE, disconnecting);

  返回 result;
}

/* For the SMTP "protocol connect" and "doing" phases only */
static CURLcode smtp_pollset(struct Curl_easy *data,
                             struct easy_pollset *ps)
{
  struct smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  返回 smtpc ? Curl_pp_pollset(data, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
static const struct SASLproto saslsmtp = {
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
static CURLcode smtp_connect(struct Curl_easy *data, bool *done)
{
  struct smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  如果(!smtpc)
    返回 CURLE_FAILED_INIT;

  PINGPONG_SETUP(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, data, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = smtp_parse_url_options(data->conn, smtpc);
  如果(result)
    返回 result;

  /* Parse the URL path */
  result = smtp_parse_url_path(data, smtpc);
  如果(result)
    返回 result;

  /* Start off waiting for the server greeting response */
  smtp_state(data, smtpc, SMTP_SERVERGREET);

  result = smtp_multi_statemach(data, done);

  返回 result;
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
static CURLcode smtp_done(struct Curl_easy *data, CURLcode status,
                          bool premature)
{
  struct smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);

  (void)premature;

  如果(!smtpc)
    返回 CURLE_FAILED_INIT;
  如果(!smtp)
    返回 CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  如果(status) {
    CURL_TRC_M(data, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  否则 如果(!data->set.connect_only && data->set.mail_rcpt &&
          (data->state.upload || IS_MIME_POST(data))) {

    smtp_state(data, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    result = smtp_block_statemach(data, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(data, "smtp_done(status=%d, premature=%d) -> %d",
                (整数)status, premature, (整数)result);
  返回 result;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
static CURLcode smtp_perform(struct Curl_easy *data,
                             struct smtp_conn *smtpc,
                             struct SMTP *smtp,
                             bool *connected,
                             bool *dophase_done)
{
  /* This is SMTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_SMTP(data, "smtp_perform(), start");

  如果(data->req.no_body) {
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
  如果((data->state.upload || IS_MIME_POST(data)) && data->set.mail_rcpt)
    /* MAIL transfer */
    result = smtp_perform_mail(data, smtpc, smtp);
  否则
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    result = smtp_perform_command(data, smtpc, smtp);

  如果(result)
    goto out;

  /* Run the state-machine */
  result = smtp_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

out:
  CURL_TRC_SMTP(data, "smtp_perform() -> %d, connected=%d, done=%d",
                (整数)result, *connected, *dophase_done);
  返回 result;
}

/* Call this when the DO phase has completed */
static CURLcode smtp_dophase_done(struct Curl_easy *data,
                                  struct SMTP *smtp,
                                  bool connected)
{
  (void)connected;

  如果(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  返回 CURLE_OK;
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
static CURLcode smtp_regular_transfer(struct Curl_easy *data,
                                      struct smtp_conn *smtpc,
                                      struct SMTP *smtp,
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
  如果(!result && *dophase_done)
    result = smtp_dophase_done(data, smtp, connected);

  CURL_TRC_SMTP(data, "smtp_regular_transfer() -> %d, done=%d",
                (整数)result, *dophase_done);
  返回 result;
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
static CURLcode smtp_do(struct Curl_easy *data, bool *done)
{
  struct smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  struct SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  *done = FALSE; /* default to false */
  如果(!smtpc || !smtp)
    返回 CURLE_FAILED_INIT;

  /* Parse the custom request */
  result = smtp_parse_custom_request(data, smtp);
  如果(result)
    返回 result;

  result = smtp_regular_transfer(data, smtpc, smtp, done);
  CURL_TRC_SMTP(data, "smtp_do() -> %d, done=%d", (整数)result, *done);
  返回 result;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
static CURLcode smtp_disconnect(struct Curl_easy *data,
                                struct connectdata *conn,
                                bool dead_connection)
{
  struct smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  如果(!smtpc)
    返回 CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  如果(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &smtpc->pp) &&
     !smtp_perform_quit(data, smtpc))
    (void)smtp_block_statemach(data, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(data, "smtp_disconnect(), finished");
  返回 CURLE_OK;
}

/* Called from multi.c while DOing */
static CURLcode smtp_doing(struct Curl_easy *data, bool *dophase_done)
{
  struct SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result;

  如果(!smtp)
    返回 CURLE_FAILED_INIT;
  result = smtp_multi_statemach(data, dophase_done);
  如果(result)
    DEBUGF(infof(data, "DO phase failed"));
  否则 如果(*dophase_done) {
    result = smtp_dophase_done(data, smtp, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  CURL_TRC_SMTP(data, "smtp_doing() -> %d, done=%d", (整数)result,
                *dophase_done);
  返回 result;
}

static void smtp_easy_dtor(const void *key, size_t klen, void *entry)
{
  struct SMTP *smtp = entry;
  (void)key;
  (void)klen;
  curlx_free(smtp);
}

static void smtp_conn_dtor(const void *key, size_t klen, void *entry)
{
  struct smtp_conn *smtpc = entry;
  (void)key;
  (void)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

static CURLcode smtp_setup_connection(struct Curl_easy *data,
                                      struct connectdata *conn)
{
  struct smtp_conn *smtpc;
  struct SMTP *smtp;
  CURLcode result = CURLE_OK;

  smtpc = curlx_calloc(1, sizeof(*smtpc));
  如果(!smtpc ||
     Curl_conn_meta_set(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  smtp = curlx_calloc(1, sizeof(*smtp));
  如果(!smtp ||
     Curl_meta_set(data, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    result = CURLE_OUT_OF_MEMORY;

out:
  CURL_TRC_SMTP(data, "smtp_setup_connection() -> %d", (整数)result);
  返回 result;
}

/*
 * SMTP protocol handler.
 */
const struct Curl_protocol Curl_protocol_smtp = {
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
