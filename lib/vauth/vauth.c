/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
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
 ***************************************************************************/
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "vauth/vauth.h"
#અન્તર્ભાવ "creds.h"
#અન્તર્ભાવ "curlx/multibyte.h"
#અન્તર્ભાવ "url.h"

/*
 * Curl_auth_build_spn()
 *
 * This is used to build an SPN string in the following formats:
 *
 * service/host@realm (Not currently used)
 * service/host       (Not used by GSS-API)
 * service@realm      (Not used by Windows SSPI)
 *
 * Parameters:
 *
 * service  [in] - The service type such as http, smtp, pop or imap.
 * host     [in] - The hostname.
 * realm    [in] - The realm.
 *
 * Returns a pointer to the newly allocated SPN.
 */
#ifndef USE_WINDOWS_SSPI
અક્ષર *Curl_auth_build_spn(અચલ અક્ષર *service, અચલ અક્ષર *host,
                          અચલ અક્ષર *realm)
{
  અક્ષર *spn = ખાલી;

  /* Generate our SPN */
  જો(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  નહીંતર જો(host)
    spn = curl_maprintf("%s/%s", service, host);
  નહીંતર જો(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  પાછા spn;
}
#નહીંતર
TCHAR *Curl_auth_build_spn(અચલ અક્ષર *service, અચલ અક્ષર *host,
                           અચલ અક્ષર *realm)
{
  અક્ષર *utf8_spn = ખાલી;
  TCHAR *tchar_spn = ખાલી;

  (શૂન્ય)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  જો(!utf8_spn)
    પાછા ખાલી;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  પાછા tchar_spn;
}
#endif /* USE_WINDOWS_SSPI */

/*
 * Curl_auth_user_contains_domain()
 *
 * This is used to test if the specified user contains a Windows domain name as
 * follows:
 *
 * Domain\User (Down-level Logon Name)
 * Domain/User (curl Down-level format - for compatibility with existing code)
 * User@Domain (User Principal Name)
 *
 * Note: The username may be empty when using a GSS-API library or Windows
 * SSPI as the user and domain are either obtained from the credentials cache
 * when using GSS-API or via the currently logged in user's credentials when
 * using Windows SSPI.
 *
 * Parameters:
 *
 * user  [in] - The username.
 *
 * Returns TRUE on success; otherwise FALSE.
 */
bool Curl_auth_user_contains_domain(સંરચના Curl_creds *creds)
{
  bool valid = FALSE;

  જો(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    અચલ અક્ષર *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + strlen(creds->user) - 1));
  }
#જો defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  નહીંતર
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#endif

  પાછા valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(સંરચના Curl_easy *data)
{
  પાછા Curl_auth_allowed_to_origin(data, data->state.origin);
}

bool Curl_auth_allowed_to_origin(સંરચના Curl_easy *data,
                                 સંરચના Curl_peer *origin)
{
  પાછા data->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(data->state.initial_origin, origin);
}

#ifdef USE_NTLM
સ્થિર શૂન્ય ntlm_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના ntlmdata *ntlm = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

સંરચના ntlmdata *Curl_auth_ntlm_get(સંરચના connectdata *conn, bool proxy)
{
  અચલ અક્ષર *ચાવી = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  સંરચના ntlmdata *ntlm = Curl_conn_meta_get(conn, ચાવી);
  જો(!ntlm) {
    ntlm = curlx_calloc(1, કદ(*ntlm));
    જો(!ntlm || Curl_conn_meta_set(conn, ચાવી, ntlm, ntlm_conn_dtor))
      પાછા ખાલી;
  }
  પાછા ntlm;
}

શૂન્ય Curl_auth_ntlm_remove(સંરચના connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#endif /* USE_NTLM */

#ifdef USE_KERBEROS5
સ્થિર શૂન્ય krb5_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના kerberos5data *krb5 = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

સંરચના kerberos5data *Curl_auth_krb5_get(સંરચના connectdata *conn)
{
  સંરચના kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  જો(!krb5) {
    krb5 = curlx_calloc(1, કદ(*krb5));
    જો(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      પાછા ખાલી;
  }
  પાછા krb5;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
સ્થિર શૂન્ય gsasl_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના gsasldata *gsasl = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

સંરચના gsasldata *Curl_auth_gsasl_get(સંરચના connectdata *conn)
{
  સંરચના gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  જો(!gsasl) {
    gsasl = curlx_calloc(1, કદ(*gsasl));
    જો(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      પાછા ખાલી;
  }
  પાછા gsasl;
}
#endif /* USE_GSASL */

#ifdef USE_SPNEGO
સ્થિર શૂન્ય nego_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના negotiatedata *nego = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

સંરચના negotiatedata *Curl_auth_nego_get(સંરચના connectdata *conn, bool proxy)
{
  અચલ અક્ષર *ચાવી = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  સંરચના negotiatedata *nego = Curl_conn_meta_get(conn, ચાવી);
  જો(!nego) {
    nego = curlx_calloc(1, કદ(*nego));
    જો(!nego || Curl_conn_meta_set(conn, ચાવી, nego, nego_conn_dtor))
      પાછા ખાલી;
  }
  પાછા nego;
}
#endif /* USE_SPNEGO */
